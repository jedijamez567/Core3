#ifndef LOOTFILTERRULE_H_
#define LOOTFILTERRULE_H_

#include "engine/engine.h"
#include "engine/util/json_utils.h"

/**
 * Per-player loot filter rule. A player owns a Vector<LootFilterRule> on
 * their PlayerObject. An item matches the filter if it matches ANY enabled
 * rule (OR across rules, AND within a rule's conditions).
 *
 * Category mask values mirror constants in scripts/managers/loot_filter.lua.
 * Keep both sides aligned if you renumber.
 */
class LootFilterRule : public Serializable {
public:
	// Category bitmask. Keep in sync with mods/loot-filter/server/conf/loot_filter.lua.
	static const int CAT_ARMOR_ATTACHMENT    = 0x0001;
	static const int CAT_CLOTHING_ATTACHMENT = 0x0002;
	static const int CAT_WEAPON              = 0x0004;
	static const int CAT_ARMOR               = 0x0008;
	static const int CAT_WEARABLE_OTHER      = 0x0010;
	static const int CAT_BLOOD_CHARGE        = 0x0020;
	static const int CAT_SPECIAL_DROP        = 0x0040;

	static const int RARITY_ANY         = 0;
	static const int RARITY_YELLOW      = 1;
	static const int RARITY_EXCEPTIONAL = 2;
	static const int RARITY_LEGENDARY   = 3;

private:
	String name;
	int categories;
	int minRarity;
	bool enabled;
	VectorMap<String, int> minStatMods;

public:
	LootFilterRule() : Object(), Serializable() {
		categories = 0;
		minRarity = RARITY_ANY;
		enabled = true;
		minStatMods.setAllowOverwriteInsertPlan();
		minStatMods.setNullValue(0);
		addSerializableVariables();
	}

	LootFilterRule(const LootFilterRule& o) : Object(), Serializable() {
		name = o.name;
		categories = o.categories;
		minRarity = o.minRarity;
		enabled = o.enabled;
		minStatMods.setAllowOverwriteInsertPlan();
		minStatMods.setNullValue(0);
		minStatMods = o.minStatMods;
		addSerializableVariables();
	}

	LootFilterRule& operator=(const LootFilterRule& o) {
		if (this == &o)
			return *this;
		name = o.name;
		categories = o.categories;
		minRarity = o.minRarity;
		enabled = o.enabled;
		minStatMods = o.minStatMods;
		return *this;
	}

	void addSerializableVariables() {
		addSerializableVariable("name", &name);
		addSerializableVariable("categories", &categories);
		addSerializableVariable("minRarity", &minRarity);
		addSerializableVariable("enabled", &enabled);
		addSerializableVariable("minStatMods", &minStatMods);
	}

	const String& getName() const { return name; }
	void setName(const String& n) { name = n; }

	int getCategories() const { return categories; }
	void setCategories(int c) { categories = c; }
	bool hasCategory(int cat) const { return (categories & cat) != 0; }

	int getMinRarity() const { return minRarity; }
	void setMinRarity(int r) { minRarity = r; }

	bool isEnabled() const { return enabled; }
	void setEnabled(bool e) { enabled = e; }

	int getStatModCount() const { return minStatMods.size(); }
	const String& getStatModName(int idx) const { return minStatMods.elementAt(idx).getKey(); }
	int getStatModValue(int idx) const { return minStatMods.elementAt(idx).getValue(); }

	int getMinStatModValue(const String& mod) const {
		if (!minStatMods.contains(mod))
			return 0;
		return minStatMods.get(mod);
	}

	void setMinStatMod(const String& mod, int value) {
		if (value <= 0) {
			minStatMods.drop(mod);
		} else {
			minStatMods.put(mod, value);
		}
	}

	void clearStatMods() {
		minStatMods.removeAll();
	}

	friend void to_json(nlohmann::json& j, const LootFilterRule& r) {
		j["name"] = r.name;
		j["categories"] = r.categories;
		j["minRarity"] = r.minRarity;
		j["enabled"] = r.enabled;
		j["minStatMods"] = r.minStatMods.getMapUnsafe();
	}
};

#endif /* LOOTFILTERRULE_H_ */
