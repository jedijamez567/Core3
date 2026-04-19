/*
 * MissionTerminalImplementation.cpp
 *
 *  Created on: 03/05/11
 *      Author: polonel
 */

#include "server/zone/objects/tangible/terminal/mission/MissionTerminal.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/objects/region/CityRegion.h"
#include "server/zone/managers/city/CityManager.h"
#include "server/zone/managers/city/CityRemoveAmenityTask.h"
#include "server/zone/managers/mission/MissionManager.h"
#include "server/zone/objects/player/sessions/SlicingSession.h"
#include "server/zone/ZoneServer.h"

void MissionTerminalImplementation::fillObjectMenuResponse(ObjectMenuResponse* menuResponse, CreatureObject* player) {
	TerminalImplementation::fillObjectMenuResponse(menuResponse, player);

	ManagedReference<CityRegion*> city = player->getCityRegion().get();

	if (city != nullptr && city->isMayor(player->getObjectID()) && getParent().get() == nullptr) {

		menuResponse->addRadialMenuItem(72, 3, "@city/city:mt_remove"); // Remove

		menuResponse->addRadialMenuItem(73, 3, "@city/city:align"); // Align
		menuResponse->addRadialMenuItemToRadialID(73, 74, 3, "@city/city:north"); // North
		menuResponse->addRadialMenuItemToRadialID(73, 75, 3, "@city/city:east"); // East
		menuResponse->addRadialMenuItemToRadialID(73, 76, 3, "@city/city:south"); // South
		menuResponse->addRadialMenuItemToRadialID(73, 77, 3, "@city/city:west"); // West
	}

	MissionManager* missionManager = getZoneServer()->getMissionManager();
	if (missionManager != nullptr && missionManager->isMissionDirectionFilterEnabled()) {
		int currentDir = missionManager->getPlayerDirectionFilter(player->getObjectID());
		String label = "Direction Filter";
		if (currentDir >= 0 && currentDir <= 7) {
			static const char* shortNames[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
			label = label + " [" + shortNames[currentDir] + "]";
		}

		menuResponse->addRadialMenuItem(80, 3, label);
		menuResponse->addRadialMenuItemToRadialID(80, 81, 3, "Clear filter");
		menuResponse->addRadialMenuItemToRadialID(80, 82, 3, "North");
		menuResponse->addRadialMenuItemToRadialID(80, 83, 3, "Northeast");
		menuResponse->addRadialMenuItemToRadialID(80, 84, 3, "East");
		menuResponse->addRadialMenuItemToRadialID(80, 85, 3, "Southeast");
		menuResponse->addRadialMenuItemToRadialID(80, 86, 3, "South");
		menuResponse->addRadialMenuItemToRadialID(80, 87, 3, "Southwest");
		menuResponse->addRadialMenuItemToRadialID(80, 88, 3, "West");
		menuResponse->addRadialMenuItemToRadialID(80, 89, 3, "Northwest");
	}
}

int MissionTerminalImplementation::handleObjectMenuSelect(CreatureObject* player, byte selectedID) {
	ManagedReference<CityRegion*> city = player->getCityRegion().get();

	if (selectedID == 69 && player->hasSkill("combat_smuggler_slicing_01")) {
		if (isBountyTerminal())
			return 0;

		if (city != nullptr && !city->isClientRegion() && city->isBanned(player->getObjectID())) {
			player->sendSystemMessage("@city/city:banned_services"); // You are banned from using this city's services.
			return 0;
		}

		if (player->containsActiveSession(SessionFacadeType::SLICING)) {
			player->sendSystemMessage("@slicing/slicing:already_slicing");
			return 0;
		}

		if (!player->checkCooldownRecovery("slicing.terminal")) {
			StringIdChatParameter message;
			message.setStringId("@slicing/slicing:not_yet"); // You will be able to hack the network again in %DI seconds.
			message.setDI(player->getCooldownTime("slicing.terminal")->getTime() - Time().getTime());
			player->sendSystemMessage(message);
			return 0;
		}

		//Create Session
		ManagedReference<SlicingSession*> session = new SlicingSession(player);
		session->initalizeSlicingMenu(player, _this.getReferenceUnsafeStaticCast());

		return 0;

	} else if (selectedID == 72) {

		if (city != nullptr && city->isMayor(player->getObjectID())) {
			CityRemoveAmenityTask* task = new CityRemoveAmenityTask(_this.getReferenceUnsafeStaticCast(), city);
			task->execute();

			player->sendSystemMessage("@city/city:mt_removed"); // The object has been removed from the city.
		}

		return 0;

	} else if (selectedID == 74 || selectedID == 75 || selectedID == 76 || selectedID == 77) {

		CityManager* cityManager = getZoneServer()->getCityManager();
		cityManager->alignAmenity(city, player, _this.getReferenceUnsafeStaticCast(), selectedID - 74);

		return 0;
	} else if (selectedID == 81) {
		MissionManager* missionManager = getZoneServer()->getMissionManager();
		if (missionManager != nullptr && missionManager->isMissionDirectionFilterEnabled()) {
			missionManager->clearPlayerDirectionFilter(player->getObjectID());
			player->sendSystemMessage("Mission direction filter cleared.");
		}
		return 0;
	} else if (selectedID >= 82 && selectedID <= 89) {
		MissionManager* missionManager = getZoneServer()->getMissionManager();
		if (missionManager != nullptr && missionManager->isMissionDirectionFilterEnabled()) {
			int dir = selectedID - 82;
			static const char* names[] = {"north", "northeast", "east", "southeast", "south", "southwest", "west", "northwest"};
			missionManager->setPlayerDirectionFilter(player->getObjectID(), dir);
			player->sendSystemMessage(String("Mission direction filter set to ") + names[dir] + ".");
		}
		return 0;
	}

	return TangibleObjectImplementation::handleObjectMenuSelect(player, selectedID);
}

String MissionTerminalImplementation::getTerminalName() {
	String name = "@terminal_name:terminal_mission";

	if (terminalType == "artisan" || terminalType == "entertainer" || terminalType == "bounty" || terminalType == "imperial" || terminalType == "rebel" || terminalType == "scout")
		name = name + "_" + terminalType;

	return name;
}
