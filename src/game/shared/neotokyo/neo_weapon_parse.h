//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef NEO_WEAPON_PARSE_H
#define NEO_WEAPON_PARSE_H
#ifdef _WIN32
#pragma once
#endif


#include "weapon_parse.h"
#include "networkvar.h"


//--------------------------------------------------------------------------------------------------------
//
// Weapon IDs for all NEO Game weapons
//
typedef enum
{
	WEAPON_NONE = 0,

	WEAPON_MP5,
	WEAPON_SUPA7,
	WEAPON_GRENADE,
	WEAPON_HL2PISTOL,
	WEAPON_TACHI,
	WEAPON_ZR68S,
	WEAPON_ZR68C,
	WEAPON_ZR68L,
	WEAPON_MX,
	WEAPON_MILSO,
	WEAPON_SRM,
	WEAPON_SRM_S,
	WEAPON_MILSO_S,
	WEAPON_MPN,
	WEAPON_PZ,
	WEAPON_JITTE,
	WEAPON_JITTESCOPED,
	WEAPON_GHOST,
	WEAPON_SMAC,
	WEAPON_M41,
	WEAPON_M41L,
	WEAPON_SMOKEGRENADE,
	WEAPON_REMOTEDET,
	WEAPON_KYLA,
	WEAPON_AA13,
	WEAPON_KNIFE,
	WEAPON_MX_SILENCED,
	WEAPON_SRS,
	WEAPON_M41S,

	WEAPON_MAX, // number of weapons weapon index
} NEOWeaponID;

const char *WeaponIDToAlias( int id );
int AliasToWeaponID( const char *alias );


//--------------------------------------------------------------------------------------------------------
class CNEOWeaponInfo : public FileWeaponInfo_t
{
public:
	DECLARE_CLASS_GAMEROOT( CNEOWeaponInfo, FileWeaponInfo_t );
	
	CNEOWeaponInfo();
	
	virtual void Parse( ::KeyValues *pKeyValuesData, const char *szWeaponName );

	// Parameters for FX_FireBullets:
	int		m_iDamage;
	int		m_iBullets;
	float	m_flCycleTime;

	char	m_szAnimPrefix[ 16 ];
	char	m_szTeam2ViewModel[ 80 ];
	wchar_t	m_szBulletCharacter[ 1 ];

	bool	m_bDrawCrosshair;
	int		m_iAimType;
	int		m_iScopeStyle;

	float	m_fVMFov;
	float	m_fVMAimFov;
	float	m_fAimFov;

	float	m_fVMOffsetUp;
	float	m_fVMOffsetForward;
	float	m_fVMOffsetRight;

	float	m_fVMAimOffsetUp;
	float	m_fVMAimOffsetForward;
	float	m_fVMAimOffsetRight;

	float	m_fVMAngleYaw;
	float	m_fVMAnglePitch;
	float	m_fVMAngleRoll;

	float	m_fVMAimAngleYaw;
	float	m_fVMAimAnglePitch;
	float	m_fVMAimAngleRoll;

	float	m_fPenetration;

	float	m_fTPMuzzleFlashScale;
};


#endif // NEO_WEAPON_PARSE_H
