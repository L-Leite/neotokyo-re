//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef WEAPON_GRENADE_H
#define WEAPON_GRENADE_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_baseneogrenade.h"


#ifdef CLIENT_DLL
	
	#define CNEOGrenade C_NEOGrenade

#endif

//-----------------------------------------------------------------------------
// Fragmentation grenades
//-----------------------------------------------------------------------------
class CNEOGrenade : public CBaseNEOGrenade
{
public:
	DECLARE_CLASS( CNEOGrenade, CBaseNEOGrenade );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CNEOGrenade() {}

	virtual NEOWeaponID GetWeaponID( void ) const		{ return WEAPON_GRENADE; }

#ifdef CLIENT_DLL

#else
	DECLARE_DATADESC();

	virtual void EmitGrenade( Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CBasePlayer *pPlayer );
	
#endif

	CNEOGrenade( const CNEOGrenade & ) {}
};


#endif // WEAPON_GRENADE_H
