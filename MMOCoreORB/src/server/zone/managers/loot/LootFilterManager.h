#ifndef LOOTFILTERMANAGER_H_
#define LOOTFILTERMANAGER_H_

#include "engine/engine.h"
#include "server/zone/objects/player/variables/LootFilterRule.h"

class TangibleObject;
class PlayerObject;

/**
 * Server-wide loot filter configuration + classification utilities.
 *
 * Lazy-initialized singleton. First instance() call loads
 * scripts/managers/loot_filter.lua, then memoizes the result. The matcher
 * is read-only after load; mutations to player filter state live on
 * PlayerObject.
 */
class LootFilterManager : public Singleton<LootFilterManager>, public Object, public ReadWriteLock {
private:
	bool initialized;
	bool systemEnabled;
	int maxRulesPerPlayer;
	int maxStatModsPerRule;

	Vector<String> specialDropTemplates;
	Vector<String> bloodChargeTemplates;
	Vector<LootFilterRule> defaultRules;

	void ensureInitialized();
	bool loadLuaConfig();
	void loadStringList(class LuaObject& tbl, Vector<String>& out);
	void loadDefaultRulesTable(class LuaObject& tbl);

public:
	LootFilterManager();

	bool isSystemEnabled() {
		ensureInitialized();
		return systemEnabled;
	}

	int getMaxRulesPerPlayer() {
		ensureInitialized();
		return maxRulesPerPlayer;
	}

	int getMaxStatModsPerRule() {
		ensureInitialized();
		return maxStatModsPerRule;
	}

	const Vector<LootFilterRule>& getDefaultRules() {
		ensureInitialized();
		return defaultRules;
	}

	// ---- Item classification ----------------------------------------

	// Returns a bitmask of LootFilterRule::CAT_* values applicable to obj.
	int classifyItem(TangibleObject* obj);

	// Detects rarity tier from custom-object-name suffix.
	int getItemRarityTier(TangibleObject* obj);

	// True if obj matches the given rule (category overlap, rarity >= min,
	// all stat-mod minimums hold). Returns false if rule->enabled is false.
	bool ruleMatchesItem(const LootFilterRule& rule, TangibleObject* obj);

	// True if obj matches any enabled rule on the given player's filter.
	bool filterMatchesItem(PlayerObject* ghost, TangibleObject* obj);

	// Helper exposed for tests / commands: does any template prefix in
	// the given list match obj's full template string?
	static bool templateMatchesAny(TangibleObject* obj, const Vector<String>& prefixes);
};

#endif /* LOOTFILTERMANAGER_H_ */
