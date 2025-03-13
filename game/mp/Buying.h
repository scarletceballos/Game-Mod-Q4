//----------------------------------------------------------------
// Buying.h
//
// Copyright 2005 Ritual Entertainment
//
// This file essentially serves as an extension to the Game DLL
// source files Multiplayer.h and Player.h, in an attempt
// to isolate, as much as possible, these changes from the main
// body of code (for merge simplification, etc).
//----------------------------------------------------------------

#ifndef __BUYING_H__
#define __BUYING_H__

#include "../Game_local.h"
#include "../MultiplayerGame.h"


class riBuyingManager
{
private:
	const idDeclEntityDef* _buyingGameBalanceConstants;
	int						opponentKillCashAward;	// latch
	int						opponentKillFragCount;

public:
	riBuyingManager();
	~riBuyingManager();

	void Init();

	int GetIntValueForKey(const char* keyName, int defaultValue);
	int GetOpponentKillCashAward(void);

	void Reset(void) { opponentKillFragCount = -1; }
	bool canBuyWeapon(const idDict& inventory, const char* weaponName);
	void BuyWeapon(idPlayer* player, const char* weaponName);

	int GetWeaponReq(const char* weaponName);

};


#endif // __BUYING_H__
