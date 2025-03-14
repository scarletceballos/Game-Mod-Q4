//----------------------------------------------------------------
// Buying.cpp
//
// Copyright 2005 Ritual Entertainment
//
// This file essentially serves as an extension to the Game DLL
// source files Multiplayer.cpp and Player.cpp, in an attempt
// to isolate, as much as possible, these changes from the main
// body of code (for merge simplification, etc).
//----------------------------------------------------------------

#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "Buying.h"

riBuyingManager::riBuyingManager() {
	Init(); 
}

void riBuyingManager::Init() {
	_buyingGameBalanceConstants = nullptr;
	opponentKillCashAward = 0;
	opponentKillFragCount = -1;
}

riBuyingManager::~riBuyingManager() {}

int riBuyingManager::GetIntValueForKey(const char* keyName, int defaultValue) {
	if (!keyName)
	{
		return defaultValue;
	}

	if (!_buyingGameBalanceConstants)
	{
		_buyingGameBalanceConstants = static_cast<const idDeclEntityDef*>(declManager->FindType(DECL_ENTITYDEF, "BuyingGameBalanceConstants", false));

		if (!_buyingGameBalanceConstants)
		{
			return defaultValue;
		}
	}

	for (int i = 0; i < _buyingGameBalanceConstants->dict.GetNumKeyVals(); i++)
	{
		const idKeyValue* keyValuePair = _buyingGameBalanceConstants->dict.GetKeyVal(i);
		if (!keyValuePair->GetKey().Icmp(keyName))
		{
			return atoi(keyValuePair->GetValue());
		}
	}

	return defaultValue;
}

int riBuyingManager::GetWeaponReq(const char* weaponName) {
	if (idStr::Icmp(weaponName, "blaster") == 0) {
		return 1; // get blaster first
	}
	return GetIntValueForKey(va("%s_cost", weaponName), 0);
}

int riBuyingManager::GetOpponentKillCashAward(void) {
	int targetFragCount = gameLocal.serverInfo.GetInt("si_fragLimit");
	if (opponentKillFragCount != targetFragCount) {
		opponentKillFragCount = targetFragCount;
		if (idStr::Icmp(gameLocal.serverInfo.GetString("si_gameType"), "DM") && idStr::Icmp(gameLocal.serverInfo.GetString("si_gameType"), "Team DM")) {
			// only do frag reward scaling in DM/TDM
			opponentKillCashAward = GetIntValueForKey("playerCashAward_killingOpponent", 600);
		}
		else {
			targetFragCount = idMath::ClampInt(GetIntValueForKey("killingOpponent_minFragAdjust", 10), GetIntValueForKey("killingOpponent_maxFragAdjust", 50), targetFragCount);
			int baseVal = GetIntValueForKey("playerCashAward_killingOpponent", 600);
			int fragTarget = GetIntValueForKey("killingOpponent_bestFragCount", 25);
			opponentKillCashAward = (baseVal * fragTarget) / targetFragCount;
		}
	}
	return opponentKillCashAward;
}

bool riBuyingManager::canBuyWeapon(const idDict& inventory, const char* weaponName) {
	// Get weapon requirements
	int neededWood = GetIntValueForKey(va("%s_wood", weaponName), 0);
	int neededStone = GetIntValueForKey(va("%s_stone", weaponName), 0);
	int neededMetal = GetIntValueForKey(va("%s_metal", weaponName), 0);
	int neededSilverOre = GetIntValueForKey(va("%s_silverOre", weaponName), 0);
	int neededSleekMechParts = GetIntValueForKey(va("%s_sleekMechParts", weaponName), 0);

	return inventory.GetInt("wood") >= neededWood &&
		inventory.GetInt("stone") >= neededStone &&
		inventory.GetInt("metal") >= neededMetal &&
		inventory.GetInt("silverOre") >= neededSilverOre &&
		inventory.GetInt("sleekMechParts") >= neededSleekMechParts;
}

void riBuyingManager::BuyWeapon(idPlayer* player, const char* weaponName) {
	idInventory& inventory = player->inventory;
	idDict inventoryDict;

	// Convert idInventory to idDict
	inventoryDict.SetInt("wood", inventory.wood);
	inventoryDict.SetInt("stone", inventory.stone);
	inventoryDict.SetInt("metal", inventory.metal);
	inventoryDict.SetInt("silverOre", inventory.silverOre);
	inventoryDict.SetInt("sleekMechParts", inventory.sleekMechParts);

	if (canBuyWeapon(inventoryDict, weaponName)) {
		int neededWood = GetIntValueForKey(va("%s_wood", weaponName), 0);
		int neededStone = GetIntValueForKey(va("%s_stone", weaponName), 0);
		int neededMetal = GetIntValueForKey(va("%s_metal", weaponName), 0);
		int neededSilverOre = GetIntValueForKey(va("%s_silverOre", weaponName), 0);
		int neededSleekMechParts = GetIntValueForKey(va("%s_sleekMechParts", weaponName), 0);

		inventoryDict.SetInt("wood", inventoryDict.GetInt("wood") - neededWood);
		inventoryDict.SetInt("stone", inventoryDict.GetInt("stone") - neededStone);
		inventoryDict.SetInt("metal", inventoryDict.GetInt("metal") - neededMetal);
		inventoryDict.SetInt("silverOre", inventoryDict.GetInt("silverOre") - neededSilverOre);
		inventoryDict.SetInt("sleekMechParts", inventoryDict.GetInt("sleekMechParts") - neededSleekMechParts);

		// Update idInventory from idDict
		inventory.wood = inventoryDict.GetInt("wood");
		inventory.stone = inventoryDict.GetInt("stone");
		inventory.metal = inventoryDict.GetInt("metal");
		inventory.silverOre = inventoryDict.GetInt("silverOre");
		inventory.sleekMechParts = inventoryDict.GetInt("sleekMechParts");

		player->GiveWeapon(weaponName);
	}
}


	
