#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"
#include "../Entity.h"

class rvWeaponFloorLauncher : public rvWeapon {
public:

	CLASS_PROTOTYPE( rvWeaponFloorLauncher );

	rvWeaponFloorLauncher ( void );

	virtual void			Spawn				( void );
	void					PreSave				( void );
	void					PostSave			( void );
	void					SpawnFloor			(const idVec3& position);

#ifdef _XENON
	virtual bool		AllowAutoAim			( void ) const { return false; }
#endif

private:

	stateResult_t		State_Idle		( const stateParms_t& parms );
	stateResult_t		State_Fire		( const stateParms_t& parms );
	stateResult_t		State_Reload	( const stateParms_t& parms );

	const char*			GetFireAnim() const { return (!AmmoInClip()) ? "fire_empty" : "fire"; }
	const char*			GetIdleAnim() const { return (!AmmoInClip()) ? "idle_empty" : "idle"; }
	
	CLASS_STATES_PROTOTYPE ( rvWeaponFloorLauncher );
};

CLASS_DECLARATION( rvWeapon, rvWeaponFloorLauncher )
END_CLASS

/*
================
rvWeaponFloorLauncher::rvWeaponFloorLauncher
================
*/
rvWeaponFloorLauncher::rvWeaponFloorLauncher ( void ) {
}

/*
================
rvWeaponFloorLauncher::Spawn
================
*/
void rvWeaponFloorLauncher::Spawn ( void ) {
	SetState ( "Raise", 0 );	
}

/*
================
rvWeaponFloorLauncher::PreSave
================
*/
void rvWeaponFloorLauncher::PreSave ( void ) {
}

/*
================
rvWeaponFloorLauncher::PostSave
================
*/
void rvWeaponFloorLauncher::PostSave ( void ) {
}

/*
===============================================================================

	States 

===============================================================================
*/

CLASS_STATES_DECLARATION ( rvWeaponFloorLauncher )
	STATE ( "Idle",		rvWeaponFloorLauncher::State_Idle)
	STATE ( "Fire",		rvWeaponFloorLauncher::State_Fire )
	STATE ( "Reload",	rvWeaponFloorLauncher::State_Reload )
END_CLASS_STATES

/*
================
rvWeaponFloorLauncher::State_Idle
================
*/
stateResult_t rvWeaponFloorLauncher::State_Idle( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( !AmmoAvailable ( ) ) {
				SetStatus ( WP_OUTOFAMMO );
			} else {
				SetStatus ( WP_READY );
			}
		
			PlayCycle( ANIMCHANNEL_ALL, GetIdleAnim(), parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );
		
		case STAGE_WAIT:			
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}		
			if ( !clipSize ) {
				if ( wsfl.attack && AmmoAvailable ( ) ) {
					SetState ( "Fire", 0 );
					return SRESULT_DONE;
				}
			} else { 
				if ( gameLocal.time > nextAttackTime && wsfl.attack && AmmoInClip ( ) ) {
					SetState ( "Fire", 0 );
					return SRESULT_DONE;
				}  
						
				if ( wsfl.attack && AutoReload() && !AmmoInClip ( ) && AmmoAvailable () ) {
					SetState ( "Reload", 4 );
					return SRESULT_DONE;			
				}
				if ( wsfl.netReload || (wsfl.reload && AmmoInClip() < ClipSize() && AmmoAvailable()>AmmoInClip()) ) {
					SetState ( "Reload", 4 );
					return SRESULT_DONE;			
				}
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponFloorLauncher::SpawnFloor
================
*/
void rvWeaponFloorLauncher::SpawnFloor(const idVec3& position) {
	idDict args;
	args.Set("classname", "floor_entity");
	args.SetVector("origin", position);
	args.Set("health", "100");
	gameLocal.SpawnEntityDef(args);
}

/*
================
rvWeaponFloorLauncher::State_Fire
================
*/
stateResult_t rvWeaponFloorLauncher::State_Fire(const stateParms_t& parms) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};
	switch (parms.stage) {
	case STAGE_INIT: {
		nextAttackTime = gameLocal.time + (fireRate * owner->PowerUpModifier(PMOD_FIRERATE));
		idVec3 spawnPos = owner->GetEyePosition() + owner->viewAxis[0] * 100.0f; 
		SpawnFloor(spawnPos);
		PlayAnim(ANIMCHANNEL_ALL, GetFireAnim(), 0);
		return SRESULT_STAGE(STAGE_WAIT);
	}
	case STAGE_WAIT:
		if (wsfl.attack && gameLocal.time >= nextAttackTime && AmmoInClip() && !wsfl.lowerWeapon) {
			SetState("Fire", 0);
			return SRESULT_DONE;
		}
		if (AnimDone(ANIMCHANNEL_ALL, 0)) {
			SetState("Idle", 0);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponFloorLauncher::State_Reload
================
*/
stateResult_t rvWeaponFloorLauncher::State_Reload ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( wsfl.netReload ) {
				wsfl.netReload = false;
			} else {
				NetReload ( );
			}
			
			SetStatus ( WP_RELOAD );
			PlayAnim ( ANIMCHANNEL_ALL, "reload", parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );
			
		case STAGE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				AddToClip ( ClipSize() );
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}
		