JediManager = require("managers.jedi.jedi_manager")
local ObjectManager = require("managers.object.object_manager")
JediTrials = require("screenplays.jedi.jedi_trials")

jediManagerName = "HologrindJediManager"

NUMBEROFPROFESSIONSTOMASTER = 1
MAXIMUMNUMBEROFPROFESSIONSTOSHOWWITHHOLOCRON = NUMBEROFPROFESSIONSTOMASTER

HologrindJediManager = JediManager:new {
	screenplayName = jediManagerName,
	jediManagerName = jediManagerName,
	jediProgressionType = HOLOGRINDJEDIPROGRESSION,
	startingEvent = nil,
}

-- Return a list of all professions and their badge number that are available for the hologrind
-- @return a list of professions and their badge numbers.
function HologrindJediManager:getGrindableProfessionList()
	local grindableProfessions = {
		-- String Id, badge number, profession name
		--{ "pilot_rebel_navy_corellia", 	PILOT_REBEL_NAVY_CORELLIA },
		--{ "pilot_imperial_navy_corellia", 	PILOT_IMPERIAL_NAVY_CORELLIA },
		--{ "pilot_neutral_corellia", 		PILOT_CORELLIA },
		--{ "pilot_rebel_navy_tatooine", 	PILOT_REBEL_NAVY_TATOOINE },
		--{ "pilot_imperial_navy_naboo", 	PILOT_IMPERIAL_NAVY_NABOO },
		--{ "crafting_architect_master", 		CRAFTING_ARCHITECT_MASTER  },
		--{ "crafting_armorsmith_master", 	CRAFTING_ARMORSMITH_MASTER  },
		--{ "crafting_artisan_master", 		CRAFTING_ARTISAN_MASTER  },
		--{ "outdoors_bio_engineer_master", 	OUTDOORS_BIOENGINEER_MASTER  },
		--{ "combat_bountyhunter_master", 	COMBAT_BOUNTYHUNTER_MASTER  },
		{ "combat_brawler_master", 		COMBAT_BRAWLER_MASTER  },
		--{ "combat_carbine_master", 		COMBAT_CARBINE_MASTER  },
		--{ "crafting_chef_master", 		CRAFTING_CHEF_MASTER  },
		--{ "science_combatmedic_master", 	SCIENCE_COMBATMEDIC_MASTER  },
		--{ "combat_commando_master", 		COMBAT_COMMANDO_MASTER  },
		--{ "outdoors_creaturehandler_master", 	OUTDOORS_CREATUREHANDLER_MASTER  },
		--{ "social_dancer_master", 		SOCIAL_DANCER_MASTER  },
		--{ "science_doctor_master", 		SCIENCE_DOCTOR_MASTER  },
		--{ "crafting_droidengineer_master", 	CRAFTING_DROIDENGINEER_MASTER  },
		--{ "social_entertainer_master", 		SOCIAL_ENTERTAINER_MASTER  },
		--{ "combat_1hsword_master", 		COMBAT_1HSWORD_MASTER  },
		--{ "social_imagedesigner_master", 	SOCIAL_IMAGEDESIGNER_MASTER  },
		{ "combat_marksman_master", 		COMBAT_MARKSMAN_MASTER  },
		--{ "science_medic_master", 		SCIENCE_MEDIC_MASTER  },
		--{ "crafting_merchant_master", 		CRAFTING_MERCHANT_MASTER  },
		--{ "social_musician_master", 		SOCIAL_MUSICIAN_MASTER  },
		--{ "combat_polearm_master", 		COMBAT_POLEARM_MASTER  },
		--{ "combat_pistol_master", 		COMBAT_PISTOL_MASTER  },
		--{ "social_politician_master", 	SOCIAL_POLITICIAN_MASTER  },
		--{ "outdoors_ranger_master", 		OUTDOORS_RANGER_MASTER  },
		--{ "combat_rifleman_master", 		COMBAT_RIFLEMAN_MASTER  },
		--{ "outdoors_scout_master", 		OUTDOORS_SCOUT_MASTER  },
		--{ "crafting_shipwright", 		CRAFTING_SHIPWRIGHT },
		--{ "combat_smuggler_master", 		COMBAT_SMUGGLER_MASTER  },
		--{ "outdoors_squadleader_master", 	OUTDOORS_SQUADLEADER_MASTER  },
		--{ "combat_2hsword_master", 		COMBAT_2HSWORD_MASTER  },
		--{ "crafting_tailor_master", 		CRAFTING_TAILOR_MASTER  },
		--{ "crafting_weaponsmith_master", 	CRAFTING_WEAPONSMITH_MASTER  },
		--{ "pilot_neutral_naboo", 		PILOT_NEUTRAL_NABOO },
		--{ "pilot_neutral_tatooine", 		PILOT_TATOOINE },
		--{ "pilot_imperial_navy_tatooine", 	PILOT_IMPERIAL_NAVY_TATOOINE },
		{ "combat_unarmed_master", 		COMBAT_UNARMED_MASTER  },
	--{ "pilot_rebel_navy_naboo", 		PILOT_REBEL_NAVY_NABOO }
	}
	return grindableProfessions
end

-- Handling of the onPlayerCreated event.
-- Hologrind professions will be generated for the player.
-- @param pCreatureObject pointer to the creature object of the created player.
function HologrindJediManager:onPlayerCreated(pCreatureObject)
	local skillList = self:getGrindableProfessionList()

	local pGhost = CreatureObject(pCreatureObject):getPlayerObject()

	if (pGhost == nil) then
		return
	end

	for i = 1, NUMBEROFPROFESSIONSTOMASTER, 1 do
		local numberOfSkillsInList = #skillList
		local skillNumber = getRandomNumber(1, numberOfSkillsInList)
		PlayerObject(pGhost):addHologrindProfession(skillList[skillNumber][2])
		table.remove(skillList, skillNumber)
	end
end

-- Check and count the number of mastered hologrind professions.
-- @param pCreatureObject pointer to the creature object of the player which should get its number of mastered professions counted.
-- @return the number of mastered hologrind professions.
function HologrindJediManager:getNumberOfMasteredProfessions(pCreatureObject)
	local pGhost = CreatureObject(pCreatureObject):getPlayerObject()

	if (pGhost == nil) then
		return 0
	end

	local professions = PlayerObject(pGhost):getHologrindProfessions()
	local masteredNumberOfProfessions = 0
	for i = 1, #professions, 1 do
		if PlayerObject(pGhost):hasBadge(professions[i]) then
			masteredNumberOfProfessions = masteredNumberOfProfessions + 1
		end
	end
	return masteredNumberOfProfessions
end

-- Check if the player is jedi.
-- @param pCreatureObject pointer to the creature object of the player to check if he is jedi.
-- @return returns if the player is jedi or not.
function HologrindJediManager:isJedi(pCreatureObject)
	local pGhost = CreatureObject(pCreatureObject):getPlayerObject()

	if (pGhost == nil) then
		return false
	end

	return PlayerObject(pGhost):getJediState() >= 2
end

-- Sui window ok pressed callback function.
function HologrindJediManager:notifyOkPressed()
-- Do nothing.
end

-- Send a sui window to the player about unlocking jedi and award jedi status and force sensitive skill.
-- @param pCreatureObject pointer to the creature object of the player who unlocked jedi.
function HologrindJediManager:sendSuiWindow(pCreatureObject)
	local suiManager = LuaSuiManager()
	suiManager:sendMessageBox(pCreatureObject, pCreatureObject, "@quest/force_sensitive/intro:force_sensitive", "Perhaps you should meditate somewhere alone...", "@ok", "HologrindJediManager", "notifyOkPressed")
	
	-- Create waypoint to nearest force shrine
	self:createForceShrineWaypoint(pCreatureObject)
end

-- Create a waypoint to the nearest force shrine for the player.
-- @param pCreatureObject pointer to the creature object of the player.
function HologrindJediManager:createForceShrineWaypoint(pCreatureObject)
	local pGhost = CreatureObject(pCreatureObject):getPlayerObject()
	if (pGhost == nil) then
		return
	end
	
	-- Force shrine IDs by planet
	local forceShrineIds = {
		corellia = { 7345554, 7345568, 7345533, 7345540, 7345561 },
		dantooine = { 6045480, 6045501, 6045522, 6045494, 6045508 },
		dathomir = { 7325403, 7325410, 7325417, 7325424, 7325438 },
		endor = { 7255422, 7255429, 7255436, 7255443, 7255457 },
		lok = { 6625591, 6625598, 6625605, 6625619, 6625626 },
		naboo = { 7525395, 7525400, 7525447, 7525454 },
		rori = { 6885431, 6885438, 6885445, 6885452, 6885459 },
		talus = { 6535834, 6535841, 6535869, 6535891 },
		tatooine = { 5996497, 5996504, 5996539, 5996546, 5996560 },
		yavin4 = { 6845418, 6845425, 7665623, 7665630 }
	}
	
	local planet = SceneObject(pCreatureObject):getZoneName()
	local shrineList = forceShrineIds[planet]
	
	-- If not on a planet with shrines, default to Naboo
	if (shrineList == nil) then
		planet = "naboo"
		shrineList = forceShrineIds[planet]
	end
	
	-- Find nearest shrine on current planet
	local pClosestShrine = nil
	local lastDist = 128000
	
	for i = 1, #shrineList, 1 do
		local pShrine = getSceneObject(shrineList[i])
		if (pShrine ~= nil) then
			local curDist = SceneObject(pCreatureObject):getDistanceTo(pShrine)
			if (lastDist > curDist) then
				lastDist = curDist
				pClosestShrine = pShrine
			end
		end
	end
	
	-- Create waypoint if shrine was found
	if (pClosestShrine ~= nil) then
		local zoneName = SceneObject(pClosestShrine):getZoneName()
		local capitalizedZone = zoneName:gsub("^%l", string.upper)
		PlayerObject(pGhost):addWaypoint(zoneName, capitalizedZone .. " Force Shrine", "", 
			SceneObject(pClosestShrine):getWorldPositionX(), 
			SceneObject(pClosestShrine):getWorldPositionY(), 
			WAYPOINTYELLOW, true, true, 0)
		CreatureObject(pCreatureObject):sendSystemMessage("A waypoint to a nearby Force Shrine has been added to your datapad.")
	else
		CreatureObject(pCreatureObject):sendSystemMessage("Unable to locate a Force Shrine. Please seek one out manually.")
	end
end

-- Award skill and jedi status to the player (called after meditation at Force Shrine).
-- @param pCreatureObject pointer to the creature object of the player who unlocked jedi.
function HologrindJediManager:awardJediStatusAndSkill(pCreatureObject)
	local pGhost = CreatureObject(pCreatureObject):getPlayerObject()

	if (pGhost == nil) then
		return
	end

	awardSkill(pCreatureObject, "force_title_jedi_novice")
	awardSkill(pCreatureObject, "force_title_jedi_rank_01") -- Initiate rank
	awardSkill(pCreatureObject, "force_title_jedi_rank_02") -- Padawan rank
	--awardSkill(pCreatureObject, "force_title_jedi_rank_03") -- Knight rank (grants full access to Jedi skills)
	PlayerObject(pGhost):setJediState(2)
	
	-- Grant all force sensitive skill trees
	local forceSensitiveBranches = {
		"force_sensitive_combat_prowess_ranged_accuracy",
		"force_sensitive_combat_prowess_ranged_speed",
		"force_sensitive_combat_prowess_melee_accuracy",
		"force_sensitive_combat_prowess_melee_speed",
		"force_sensitive_enhanced_reflexes_ranged_defense",
		"force_sensitive_enhanced_reflexes_melee_defense",
		"force_sensitive_enhanced_reflexes_vehicle_control",
		"force_sensitive_enhanced_reflexes_survival",
		"force_sensitive_crafting_mastery_experimentation",
		"force_sensitive_crafting_mastery_assembly",
		"force_sensitive_crafting_mastery_repair",
		"force_sensitive_crafting_mastery_technique",
		"force_sensitive_heightened_senses_healing",
		"force_sensitive_heightened_senses_surveying",
		"force_sensitive_heightened_senses_persuasion",
		"force_sensitive_heightened_senses_luck"
	}
	
	-- Unlock and grant each force sensitive branch (all 4 skill levels per branch)
	for i = 1, #forceSensitiveBranches, 1 do
		local branch = forceSensitiveBranches[i]
		
		-- Grant all 4 skill levels in the branch
		awardSkill(pCreatureObject, branch .. "_01")
		awardSkill(pCreatureObject, branch .. "_02")
		awardSkill(pCreatureObject, branch .. "_03")
		awardSkill(pCreatureObject, branch .. "_04")
	end
	
	-- Grant the 4 parent tree novice skills
	awardSkill(pCreatureObject, "force_sensitive_combat_prowess_novice")
	awardSkill(pCreatureObject, "force_sensitive_enhanced_reflexes_novice")
	awardSkill(pCreatureObject, "force_sensitive_crafting_mastery_novice")
	awardSkill(pCreatureObject, "force_sensitive_heightened_senses_novice")
	
	-- Grant the 4 parent tree master skills
	awardSkill(pCreatureObject, "force_sensitive_combat_prowess_master")
	awardSkill(pCreatureObject, "force_sensitive_enhanced_reflexes_master")
	awardSkill(pCreatureObject, "force_sensitive_crafting_mastery_master")
	awardSkill(pCreatureObject, "force_sensitive_heightened_senses_master")
	
	CreatureObject(pCreatureObject):sendSystemMessage("All Force Sensitive skills have been granted! You now have full access to all Jedi skill trees.")
	
	-- Give the Jedi Starter Kit
	self:giveJediStarterKit(pCreatureObject)
end

-- Check if the player has mastered all hologrind professions and show meditation message.
-- Skills and items are awarded after meditation at a Force Shrine.
-- @param pCreatureObject pointer to the creature object of the player to check the jedi progression on.
function HologrindJediManager:checkIfProgressedToJedi(pCreatureObject)
	local pGhost = CreatureObject(pCreatureObject):getPlayerObject()
	
	if (pGhost == nil) then
		return
	end
	
	local jediState = PlayerObject(pGhost):getJediState()
	
	-- If player has mastered professions and hasn't started Jedi unlock yet
	if self:getNumberOfMasteredProfessions(pCreatureObject) >= NUMBEROFPROFESSIONSTOMASTER and jediState == 0 then
		-- Show meditation message and create waypoint
		self:sendSuiWindow(pCreatureObject)
		-- Set Jedi state to 1 (awaiting meditation)
		PlayerObject(pGhost):setJediState(1)
	end
end

-- Event handler for the BADGEAWARDED event.
-- @param pCreatureObject pointer to the creature object of the player who was awarded with a badge.
-- @param pCreatureObject2 pointer to the creature object of the player who was awarded with a badge.
-- @param badgeNumber the badge number that was awarded.
-- @return 0 to keep the observer active.
function HologrindJediManager:badgeAwardedEventHandler(pCreatureObject, pCreatureObject2, badgeNumber)
	if (pCreatureObject == nil) then
		return 0
	end

	self:checkIfProgressedToJedi(pCreatureObject)

	return 0
end

-- Register observer on the player for observing badge awards.
-- @param pCreatureObject pointer to the creature object of the player to register observers on.
function HologrindJediManager:registerObservers(pCreatureObject)
	createObserver(BADGEAWARDED, "HologrindJediManager", "badgeAwardedEventHandler", pCreatureObject)
end

-- Handling of the onPlayerLoggedIn event. The progression of the player will be checked and observers will be registered.
-- @param pCreatureObject pointer to the creature object of the player who logged in.
function HologrindJediManager:onPlayerLoggedIn(pCreatureObject)
	if (pCreatureObject == nil) then
		return
	end

	self:checkIfProgressedToJedi(pCreatureObject)
	self:registerObservers(pCreatureObject)
end

-- Get the profession name from the badge number.
-- @param badgeNumber the badge number to find the profession name for.
-- @return the profession name associated with the badge number, Unknown profession returned if the badge number isn't found.
function HologrindJediManager:getProfessionStringIdFromBadgeNumber(badgeNumber)
	local skillList = self:getGrindableProfessionList()
	for i = 1, #skillList, 1 do
		if skillList[i][2] == badgeNumber then
			return skillList[i][1]
		end
	end
	return "Unknown profession"
end

-- Find out and send the response from the holocron to the player
-- @param pCreatureObject pointer to the creature object of the player who used the holocron.
function HologrindJediManager:sendHolocronMessage(pCreatureObject)
	if self:getNumberOfMasteredProfessions(pCreatureObject) >= MAXIMUMNUMBEROFPROFESSIONSTOSHOWWITHHOLOCRON then
		-- The Holocron is quiet. The ancients' knowledge of the Force will no longer assist you on your journey. You must continue seeking on your own.
		CreatureObject(pCreatureObject):sendSystemMessage("@jedi_spam:holocron_quiet")
		return true
	else
		local pGhost = CreatureObject(pCreatureObject):getPlayerObject()

		if (pGhost == nil) then
			return false
		end

		local professions = PlayerObject(pGhost):getHologrindProfessions()
		for i = 1, #professions, 1 do
			if not PlayerObject(pGhost):hasBadge(professions[i]) then
				local professionText = self:getProfessionStringIdFromBadgeNumber(professions[i])
				CreatureObject(pCreatureObject):sendSystemMessageWithTO("@jedi_spam:holocron_light_information", "@skl_n:" .. professionText)
			end
		end

		return false
	end
end

-- Handling of the useItem event.
-- @param pSceneObject pointer to the item object.
-- @param itemType the type of item that is used.
-- @param pCreatureObject pointer to the creature object that used the item.
function HologrindJediManager:useItem(pSceneObject, itemType, pCreatureObject)
	if (pCreatureObject == nil or pSceneObject == nil) then
		return
	end

	if itemType == ITEMHOLOCRON then
		local isSilent = self:sendHolocronMessage(pCreatureObject)
		if isSilent then
			return
		else
			SceneObject(pSceneObject):destroyObjectFromWorld()
			SceneObject(pSceneObject):destroyObjectFromDatabase()
		end
	end
end

function HologrindJediManager:canLearnSkill(pPlayer, skillName)
	return true
end

-- Give the Jedi Starter Kit to the player.
-- @param pCreatureObject pointer to the creature object of the player.
function HologrindJediManager:giveJediStarterKit(pCreatureObject)
	local pInventory = SceneObject(pCreatureObject):getSlottedObject("inventory")
	if (pInventory == nil) then
		CreatureObject(pCreatureObject):sendSystemMessage("Error: Unable to access inventory.")
		return
	end
	
	if SceneObject(pInventory):isContainerFullRecursive() then
		CreatureObject(pCreatureObject):sendSystemMessage("@jedi_spam:inventory_full_jedi_robe")
		return
	end
	
	-- Give Jedi Crafting Tool
	local pItem = giveItem(pInventory, "object/tangible/crafting/station/jedi_tool.iff", -1)
	
	-- Give Padawan Robe
	if (pItem ~= nil) then
		pItem = giveItem(pInventory, "object/tangible/wearables/robe/robe_jedi_padawan.iff", -1)
	end
	
	CreatureObject(pCreatureObject):sendSystemMessage("You have received a Jedi Starter Kit!")
end

registerScreenPlay("HologrindJediManager", true)

return HologrindJediManager
