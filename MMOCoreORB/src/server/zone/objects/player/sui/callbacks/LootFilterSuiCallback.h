#ifndef LOOTFILTERSUICALLBACK_H_
#define LOOTFILTERSUICALLBACK_H_

#include "server/zone/objects/player/sui/SuiCallback.h"
#include "server/zone/objects/player/sui/SuiWindowType.h"
#include "server/zone/objects/player/sui/listbox/SuiListBox.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/player/variables/LootFilterRule.h"
#include "server/zone/managers/loot/LootFilterManager.h"

/**
 * Main SUI for the loot filter. A single SuiListBox with rows for:
 *   row 0          : filter on/off toggle
 *   row 1..N       : each rule, click to toggle its enabled flag
 *   row N+1        : "Reload default rules"
 *   row N+2        : "Clear all rules"
 *
 * Click → action → re-open the SUI with refreshed state.
 */
class LootFilterSuiCallback : public SuiCallback {
public:
	static void openMainSui(CreatureObject* creature) {
		if (creature == nullptr || !creature->isPlayerCreature())
			return;

		auto ghost = creature->getSlottedObject("ghost").castTo<PlayerObject*>();
		if (ghost == nullptr)
			return;

		ManagedReference<SuiListBox*> box = new SuiListBox(creature, SuiWindowType::NONE);
		box->setUsingObject(creature);
		box->setCallback(new LootFilterSuiCallback(creature->getZoneServer()));
		box->setPromptTitle("Loot Filter");

		StringBuffer prompt;
		prompt << "Filter is currently " << (ghost->isLootFilterEnabled() ? "\\#00FF00ENABLED\\#." : "\\#FF6347DISABLED\\#.")
			<< "\nRules: " << ghost->getLootFilterRuleCount()
			<< "\n\nClick the top entry to toggle the filter. Click any rule to toggle whether that rule contributes.";
		box->setPromptText(prompt.toString());

		// Row 0 — toggle filter
		StringBuffer toggleLine;
		toggleLine << "[" << (ghost->isLootFilterEnabled() ? "ON " : "off") << "] Loot filter master switch";
		box->addMenuItem(toggleLine.toString());

		// Rows 1..N — each rule
		int ruleCount = ghost->getLootFilterRuleCount();
		for (int i = 0; i < ruleCount; ++i) {
			LootFilterRule rule = ghost->getLootFilterRule(i);
			StringBuffer line;
			line << "  [" << (rule.isEnabled() ? "ON " : "off") << "] " << rule.getName();
			int sm = rule.getStatModCount();
			if (sm > 0) {
				line << " (";
				for (int j = 0; j < sm; ++j) {
					if (j > 0) line << ", ";
					line << rule.getStatModName(j) << " " << rule.getStatModValue(j) << "+";
				}
				line << ")";
			}
			box->addMenuItem(line.toString());
		}

		// Row N+1
		box->addMenuItem("→ Reload default rules");
		// Row N+2
		box->addMenuItem("→ Clear all rules");

		box->setCancelButton(true, "@cancel");
		box->setOkButton(true, "@ok");

		ghost->addSuiBox(box);
		creature->sendMessage(box->generateMessage());
	}

	LootFilterSuiCallback(ZoneServer* serv) : SuiCallback(serv) {}

	void run(CreatureObject* creature, SuiBox* sui, uint32 eventIndex, Vector<UnicodeString>* args) {
		if (!sui->isListBox() || eventIndex == 1)
			return;

		if (creature == nullptr || !creature->isPlayerCreature())
			return;

		auto ghost = creature->getSlottedObject("ghost").castTo<PlayerObject*>();
		if (ghost == nullptr)
			return;

		if (args == nullptr || args->size() == 0)
			return;

		int index = Integer::valueOf(args->get(0).toString());
		int ruleCount = ghost->getLootFilterRuleCount();

		Locker locker(creature);

		if (index == 0) {
			// Toggle master switch
			ghost->setLootFilterEnabled(!ghost->isLootFilterEnabled());
		} else if (index >= 1 && index <= ruleCount) {
			// Toggle a rule
			int ruleIndex = index - 1;
			LootFilterRule rule = ghost->getLootFilterRule(ruleIndex);
			ghost->setLootFilterRuleEnabled(ruleIndex, !rule.isEnabled());
		} else if (index == ruleCount + 1) {
			// Reload defaults
			LootFilterManager* mgr = LootFilterManager::instance();
			ghost->clearLootFilterRules();
			const Vector<LootFilterRule>& defaults = mgr->getDefaultRules();
			int n = defaults.size();
			for (int i = 0; i < n; ++i) {
				LootFilterRule copy = defaults.get(i);
				ghost->addLootFilterRule(copy);
			}
			StringBuffer msg;
			msg << "Loaded " << n << " default rules.";
			creature->sendSystemMessage(msg.toString());
		} else if (index == ruleCount + 2) {
			// Clear all
			ghost->clearLootFilterRules();
			creature->sendSystemMessage("All filter rules cleared.");
		} else {
			return;
		}

		locker.release();

		// Re-open with refreshed state
		openMainSui(creature);
	}
};

#endif /* LOOTFILTERSUICALLBACK_H_ */
