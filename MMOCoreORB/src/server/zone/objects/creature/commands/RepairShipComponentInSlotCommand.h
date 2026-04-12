/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef REPAIRSHIPCOMPONENTINSLOTCOMMAND_H_
#define REPAIRSHIPCOMPONENTINSLOTCOMMAND_H_

#include "server/zone/objects/ship/ShipObject.h"
#include "server/zone/objects/transaction/TransactionLog.h"

class RepairShipComponentInSlotCommand : public QueueCommand {
public:

	RepairShipComponentInSlotCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		auto ghost = creature->getPlayerObject().get();

		if (ghost == nullptr || ghost->isTeleporting()) {
			return GENERALERROR;
		}

		StringTokenizer tokenizer(arguments.toString());

		if (!tokenizer.hasMoreTokens())
			return GENERALERROR;

		uint64 shipID = tokenizer.getLongToken();
		int slot = tokenizer.hasMoreTokens() ? tokenizer.getIntToken() : -1;

		if (shipID == 0)
			return GENERALERROR;

		ZoneServer* zoneServer = creature->getZoneServer();

		if (zoneServer == nullptr)
			return GENERALERROR;

		ManagedReference<SceneObject*> shipSceneO = zoneServer->getObject(shipID);

		if (shipSceneO == nullptr || !shipSceneO->isShipObject())
			return GENERALERROR;

		auto ship = shipSceneO->asShipObject();

		if (ship == nullptr || ship->getOwner().get() != creature)
			return GENERALERROR;

		Locker locker(ship, creature);

		float totalDamage = ship->getTotalShipDamage();

		if (totalDamage <= 0.f)
			return SUCCESS;

		int repairCost = (int)totalDamage;

		if (ghost->hasGodMode())
			repairCost = 0;

		if (repairCost > 0 && creature->getCashCredits() < repairCost) {
			creature->sendSystemMessage("@error_message:insufficient_funds_cash");
			return GENERALERROR;
		}

		ship->repairShip(1.0f, false);

		if (repairCost > 0) {
			TransactionLog trx(creature, ship, TrxCode::SHIPPINGSYSTEM, repairCost, true);
			creature->subtractCashCredits(repairCost);

			StringIdChatParameter params("@base_player:prose_pay_success_no_target");
			params.setDI(repairCost);
			creature->sendSystemMessage(params);
		}

		return SUCCESS;
	}

};

#endif //REPAIRSHIPCOMPONENTINSLOTCOMMAND_H_
