#include "LootFilterManager.h"

#include "engine/lua/Lua.h"
#include "engine/lua/LuaObject.h"

#include "templates/SharedObjectTemplate.h"
#include "templates/params/OptionBitmask.h"

#include "server/zone/objects/tangible/TangibleObject.h"
#include "server/zone/objects/tangible/attachment/Attachment.h"
#include "server/zone/objects/tangible/wearables/WearableObject.h"
#include "server/zone/objects/player/PlayerObject.h"

LootFilterManager::LootFilterManager() {
	initialized = false;
	systemEnabled = true;
	maxRulesPerPlayer = 32;
	maxStatModsPerRule = 16;
}

void LootFilterManager::ensureInitialized() {
	if (initialized)
		return;

	Locker locker(this);

	if (initialized)
		return;

	if (!loadLuaConfig()) {
		Logger logger("LootFilterManager");
		logger.error() << "Failed to load scripts/managers/loot_filter.lua — using built-in defaults.";
	}

	initialized = true;
}

bool LootFilterManager::loadLuaConfig() {
	Lua lua;
	lua.init();

	if (!lua.runFile("scripts/managers/loot_filter.lua"))
		return false;

	systemEnabled = (lua.getGlobalByte("lootFilterSystemEnabled") != 0);

	int mrpp = lua.getGlobalInt("maxRulesPerPlayer");
	if (mrpp > 0)
		maxRulesPerPlayer = mrpp;

	int msmpr = lua.getGlobalInt("maxStatModsPerRule");
	if (msmpr > 0)
		maxStatModsPerRule = msmpr;

	LuaObject sdt = lua.getGlobalObject("specialDropTemplates");
	loadStringList(sdt, specialDropTemplates);
	sdt.pop();

	LuaObject bct = lua.getGlobalObject("bloodChargeTemplates");
	loadStringList(bct, bloodChargeTemplates);
	bct.pop();

	LuaObject dr = lua.getGlobalObject("defaultRules");
	loadDefaultRulesTable(dr);
	dr.pop();

	Logger logger("LootFilterManager");
	logger.info(true) << "Loaded — enabled=" << systemEnabled
		<< " maxRulesPerPlayer=" << maxRulesPerPlayer
		<< " specialDropPrefixes=" << specialDropTemplates.size()
		<< " bloodChargePrefixes=" << bloodChargeTemplates.size()
		<< " defaultRules=" << defaultRules.size();

	return true;
}

void LootFilterManager::loadStringList(LuaObject& tbl, Vector<String>& out) {
	out.removeAll();
	if (!tbl.isValidTable())
		return;

	int n = tbl.getTableSize();
	for (int i = 1; i <= n; ++i) {
		String s = tbl.getStringAt(i);
		if (s.length() > 0)
			out.add(s);
	}
}

void LootFilterManager::loadDefaultRulesTable(LuaObject& tbl) {
	defaultRules.removeAll();
	if (!tbl.isValidTable())
		return;

	int n = tbl.getTableSize();
	for (int i = 1; i <= n; ++i) {
		LuaObject row = tbl.getObjectAt(i);

		if (!row.isValidTable()) {
			row.pop();
			continue;
		}

		LootFilterRule rule;
		rule.setName(row.getStringField("name"));
		rule.setCategories(row.getIntField("categories"));
		rule.setMinRarity(row.getIntField("minRarity"));
		rule.setEnabled(true);

		LuaObject stats = row.getObjectField("minStats");
		if (stats.isValidTable()) {
			lua_State* L = stats.getLuaState();
			lua_pushnil(L);
			while (lua_next(L, -2) != 0) {
				if (lua_type(L, -2) == LUA_TSTRING && lua_type(L, -1) == LUA_TNUMBER) {
					String modName = lua_tolstring(L, -2, 0);
					int modValue = (int) lua_tointeger(L, -1);
					rule.setMinStatMod(modName, modValue);
				}
				lua_pop(L, 1);
			}
		}
		stats.pop();

		defaultRules.add(rule);
		row.pop();
	}
}

int LootFilterManager::classifyItem(TangibleObject* obj) {
	if (obj == nullptr)
		return 0;

	ensureInitialized();

	int mask = 0;

	if (obj->isArmorAttachment())
		mask |= LootFilterRule::CAT_ARMOR_ATTACHMENT;

	if (obj->isClothingAttachment())
		mask |= LootFilterRule::CAT_CLOTHING_ATTACHMENT;

	if (obj->isWeaponObject())
		mask |= LootFilterRule::CAT_WEAPON;

	if (obj->isArmorObject()) {
		mask |= LootFilterRule::CAT_ARMOR;
	} else if (obj->isWearableObject() && !obj->isAttachment() && !obj->isWeaponObject()) {
		mask |= LootFilterRule::CAT_WEARABLE_OTHER;
	}

	if (templateMatchesAny(obj, bloodChargeTemplates))
		mask |= LootFilterRule::CAT_BLOOD_CHARGE;

	if (templateMatchesAny(obj, specialDropTemplates))
		mask |= LootFilterRule::CAT_SPECIAL_DROP;

	return mask;
}

int LootFilterManager::getItemRarityTier(TangibleObject* obj) {
	if (obj == nullptr)
		return LootFilterRule::RARITY_ANY;

	String name = obj->getCustomObjectName().toString();
	if (name.contains("(Legendary)"))
		return LootFilterRule::RARITY_LEGENDARY;
	if (name.contains("(Exceptional)"))
		return LootFilterRule::RARITY_EXCEPTIONAL;
	if ((obj->getOptionsBitmask() & OptionBitmask::YELLOW) != 0)
		return LootFilterRule::RARITY_YELLOW;

	return LootFilterRule::RARITY_ANY;
}

bool LootFilterManager::ruleMatchesItem(const LootFilterRule& rule, TangibleObject* obj) {
	if (!rule.isEnabled() || obj == nullptr)
		return false;

	int itemCats = classifyItem(obj);
	if ((rule.getCategories() & itemCats) == 0)
		return false;

	int tier = getItemRarityTier(obj);
	if (tier < rule.getMinRarity())
		return false;

	int statCount = rule.getStatModCount();
	if (statCount == 0)
		return true;

	// Resolve the item's stat-mod map via the right accessor for its type.
	const VectorMap<String, int>* itemMods = nullptr;

	if (obj->isAttachment()) {
		Attachment* att = cast<Attachment*>(obj);
		if (att != nullptr)
			itemMods = att->getSkillMods();
	} else if (obj->isWearableObject()) {
		WearableObject* wear = cast<WearableObject*>(obj);
		if (wear != nullptr)
			itemMods = wear->getWearableSkillMods();
	}

	if (itemMods == nullptr)
		return false;

	for (int i = 0; i < statCount; ++i) {
		const String& modName = rule.getStatModName(i);
		int required = rule.getStatModValue(i);

		int actual = itemMods->contains(modName) ? itemMods->get(modName) : 0;

		if (actual < required)
			return false;
	}

	return true;
}

bool LootFilterManager::filterMatchesItem(PlayerObject* ghost, TangibleObject* obj) {
	if (ghost == nullptr || obj == nullptr)
		return false;

	ensureInitialized();

	if (!systemEnabled)
		return false;

	if (!ghost->isLootFilterEnabled())
		return false;

	int n = ghost->getLootFilterRuleCount();
	for (int i = 0; i < n; ++i) {
		LootFilterRule rule = ghost->getLootFilterRule(i);
		if (ruleMatchesItem(rule, obj))
			return true;
	}

	return false;
}

bool LootFilterManager::templateMatchesAny(TangibleObject* obj, const Vector<String>& prefixes) {
	if (obj == nullptr || prefixes.size() == 0)
		return false;

	SharedObjectTemplate* tmpl = obj->getObjectTemplate();
	if (tmpl == nullptr)
		return false;

	const String& path = tmpl->getFullTemplateString();

	int n = prefixes.size();
	for (int i = 0; i < n; ++i) {
		if (path.beginsWith(prefixes.get(i)))
			return true;
	}

	return false;
}
