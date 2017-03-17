#ifndef WEAPON_GHOST_H
#define WEAPON_GHOST_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_neobase.h"


#if defined( CLIENT_DLL )
	#define CWeaponGhost C_WeaponGhost
#endif

class CWeaponGhost : public CWeaponNEOBase
{
public:
	DECLARE_CLASS( CWeaponGhost, CWeaponNEOBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

/*#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif*/

	CWeaponGhost();	

#ifdef GAME_DLL
	virtual int ShouldTransmit( const CCheckTransmitInfo *pInfo );
	virtual int UpdateTransmitState();

	/*virtual void SetPickupTouch();
	virtual void DefaultTouch( CBaseEntity *pOther );*/

	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
#endif	   	

	virtual void WeaponIdle();

	virtual void PrimaryAttack() {}
	virtual void SecondaryAttack() {}

	virtual bool IsRemoveable() { return false; }
	virtual bool ShouldStayOnRoundRestart() { return true; }
	virtual bool CanAim() { return false; }

	virtual NEOWeaponID GetWeaponID( void ) const { return WEAPON_GHOST; }

private:			  
	CWeaponGhost( const CWeaponGhost& );
};


extern CUtlVector< CWeaponGhost* > g_Ghosts;
extern CWeaponGhost* GetGhost( int iIndex );
extern CWeaponGhost* GetGhost();
extern int GetNumberOfGhosts();


#endif // WEAPON_GHOST_H
