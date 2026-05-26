#ifndef LOOTFILTERCOMMAND_H_
#define LOOTFILTERCOMMAND_H_

#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/player/variables/LootFilterRule.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/managers/loot/LootFilterManager.h"
#include "server/zone/objects/player/sui/callbacks/LootFilterSuiCallback.h"

class LootFilterCommand : public QueueCommand {
public:
	LootFilterCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {
	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!creature->isPlayerCreature())
			return GENERALERROR;

		auto ghost = creature->getSlottedObject("ghost").castTo<PlayerObject*>();
		if (ghost == nullptr)
			return GENERALERROR;

		LootFilterManager* mgr = LootFilterManager::instance();

		if (!mgr->isSystemEnabled()) {
			creature->sendSystemMessage("Loot filter system is disabled by the server administrator.");
			return GENERALERROR;
		}

		String args = arguments.toString().toLowerCase().trim();
		StringTokenizer tok(args);
		String sub;
		if (tok.hasMoreTokens())
			tok.getStringToken(sub);

		if (sub.length() == 0 || sub == "ui" || sub == "open")
			return openSui(creature, ghost, mgr);

		if (sub == "on" || sub == "enable")
			return setEnabled(creature, ghost, true);

		if (sub == "off" || sub == "disable")
			return setEnabled(creature, ghost, false);

		if (sub == "status")
			return showStatus(creature, ghost, mgr);

		if (sub == "list")
			return listRules(creature, ghost);

		if (sub == "reset")
			return resetRules(creature, ghost, mgr);

		if (sub == "clear")
			return clearRules(creature, ghost);

		if (sub == "help")
			return showHelp(creature);

		creature->sendSystemMessage("Unknown subcommand. Try /lootfilter help.");
		return GENERALERROR;
	}

private:
	int openSui(CreatureObject* creature, PlayerObject* ghost, LootFilterManager* mgr) const {
		LootFilterSuiCallback::openMainSui(creature);
		return SUCCESS;
	}

	int setEnabled(CreatureObject* creature, PlayerObject* ghost, bool on) const {
		ghost->setLootFilterEnabled(on);
		if (on) {
			creature->sendSystemMessage("Loot filter ENABLED. /loot all will now only collect matching items.");
			if (ghost->getLootFilterRuleCount() == 0)
				creature->sendSystemMessage("(You have no rules. Run /lootfilter reset to load the default rule set.)");
		} else {
			creature->sendSystemMessage("Loot filter DISABLED. /loot all behaves normally.");
		}
		return SUCCESS;
	}

	int showStatus(CreatureObject* creature, PlayerObject* ghost, LootFilterManager* mgr) const {
		StringBuffer msg;
		msg << "Loot filter: " << (ghost->isLootFilterEnabled() ? "ON" : "OFF")
			<< " (" << ghost->getLootFilterRuleCount() << " rules)";
		creature->sendSystemMessage(msg.toString());
		return SUCCESS;
	}

	int listRules(CreatureObject* creature, PlayerObject* ghost) const {
		int n = ghost->getLootFilterRuleCount();
		if (n == 0) {
			creature->sendSystemMessage("No filter rules. /lootfilter reset to load defaults.");
			return SUCCESS;
		}

		for (int i = 0; i < n; ++i) {
			LootFilterRule rule = ghost->getLootFilterRule(i);
			StringBuffer line;
			line << "[" << i << "] " << (rule.isEnabled() ? "ON " : "off ")
				<< rule.getName() << " — cats=0x";
			line << String::hexvalueOf(rule.getCategories());
			line << " minRarity=" << rule.getMinRarity();

			int sm = rule.getStatModCount();
			if (sm > 0) {
				line << " stats=[";
				for (int j = 0; j < sm; ++j) {
					if (j > 0) line << ",";
					line << rule.getStatModName(j) << ">=" << rule.getStatModValue(j);
				}
				line << "]";
			}
			creature->sendSystemMessage(line.toString());
		}
		return SUCCESS;
	}

	int resetRules(CreatureObject* creature, PlayerObject* ghost, LootFilterManager* mgr) const {
		ghost->clearLootFilterRules();
		const Vector<LootFilterRule>& defaults = mgr->getDefaultRules();
		int n = defaults.size();
		for (int i = 0; i < n; ++i) {
			LootFilterRule copy = defaults.get(i);
			ghost->addLootFilterRule(copy);
		}
		StringBuffer msg;
		msg << "Loot filter reset — " << n << " default rules loaded.";
		creature->sendSystemMessage(msg.toString());
		return SUCCESS;
	}

	int clearRules(CreatureObject* creature, PlayerObject* ghost) const {
		ghost->clearLootFilterRules();
		creature->sendSystemMessage("All loot filter rules cleared.");
		return SUCCESS;
	}

	int showHelp(CreatureObject* creature) const {
		creature->sendSystemMessage("Loot filter commands:");
		creature->sendSystemMessage("  /lootfilter on | off          - toggle filter");
		creature->sendSystemMessage("  /lootfilter status            - show current state");
		creature->sendSystemMessage("  /lootfilter list              - print rules to chat");
		creature->sendSystemMessage("  /lootfilter reset             - load default rules");
		creature->sendSystemMessage("  /lootfilter clear             - remove all rules");
		creature->sendSystemMessage("  /lootfilter                   - open the SUI window (toggle filter / rules)");
		return SUCCESS;
	}
};

#endif /* LOOTFILTERCOMMAND_H_ */
