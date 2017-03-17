//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include <KeyValues.h>
#include "neo_weapon_parse.h"
																					   
//--------------------------------------------------------------------------------------------------------
static const char * s_WeaponAliasInfo[] =
{
	"none",			// WEAPON_NONE
	"mp5",			// WEAPON_MP5
	"supa7",		// WEAPON_SUPA7
	"grenade",		// WEAPON_GRENADE
	"hl2pistol",	// WEAPON_HL2PISTOL
	"tachi",		// WEAPON_TACHI
	"zr68s",		// WEAPON_ZR68S
	"zr68c",		// WEAPON_ZR68C
	"zr68l",		// WEAPON_ZR68L
	"mx",			// WEAPON_MX
	"milso",		// WEAPON_MILSO
	"srm",			// WEAPON_SRM
	"srm_s",		// WEAPON_SRM_S
	"milso_s",		// WEAPON_MILSO_S
	"mpn",			// WEAPON_MPN
	"pz",			// WEAPON_PZ
	"jitte",		// WEAPON_JITTE
	"jittescoped",	// WEAPON_JITTESCOPED
	"ghost",		// WEAPON_GHOST
	"smac",			// WEAPON_SMAC
	"m41",			// WEAPON_M41
	"m41l",			// WEAPON_M41L
	"smokegrenade",	// WEAPON_SMOKEGRENADE
	"remotedet",	// WEAPON_REMOTEDET
	"kyla",			// WEAPON_KYLA
	"aa13",			// WEAPON_AA13
	"knife",		// WEAPON_KNIFE
	"mx_silenced",	// WEAPON_MX_SILENCED
	"srs",			// WEAPON_SRS
	"m41s",			// WEAPON_M41S
	NULL,			// WEAPON_NONE
};

//--------------------------------------------------------------------------------------------------------
//
// Given an alias, return the associated weapon ID
//
int AliasToWeaponID( const char *alias )
{
	if ( alias )
	{
		for ( int i = 0; s_WeaponAliasInfo[ i ] != NULL; ++i )
			if ( !Q_stricmp( s_WeaponAliasInfo[ i ], alias ) )
				return i;
	}

	return WEAPON_NONE;
}

//--------------------------------------------------------------------------------------------------------
//
// Given a weapon ID, return its alias
//
const char *WeaponIDToAlias( int id )
{
	if ( (id >= WEAPON_MAX) || (id < 0) )
		return NULL;

	return s_WeaponAliasInfo[ id ];
}


FileWeaponInfo_t* CreateWeaponInfo()
{
	return new CNEOWeaponInfo;
}


CNEOWeaponInfo::CNEOWeaponInfo()
{
}


void CNEOWeaponInfo::Parse( KeyValues *pKeyValuesData, const char *szWeaponName )
{
	BaseClass::Parse( pKeyValuesData, szWeaponName );

	const char* pAnimPrefix = pKeyValuesData->GetString( "anim_prefix", "" );
	V_strncpy( m_szAnimPrefix, pAnimPrefix, sizeof( m_szAnimPrefix ) );

	m_iDamage		= pKeyValuesData->GetInt( "Damage", 42 ); // Douglas Adams 1952 - 2001
	m_iBullets		= pKeyValuesData->GetInt( "Bullets", 1 );
	m_flCycleTime	= pKeyValuesData->GetFloat( "CycleTime", 0.15 );

	const char* pTeam2vm = pKeyValuesData->GetString( "team2viewmodel", "" );
	V_strncpy( m_szTeam2ViewModel, pTeam2vm, sizeof( m_szTeam2ViewModel ) );

	const wchar_t* pBulletChar = pKeyValuesData->GetWString( "BulletCharacter", L"" );
	V_wcsncpy( m_szBulletCharacter, pBulletChar, sizeof( m_szBulletCharacter ) );

	m_bDrawCrosshair = (pKeyValuesData->GetInt( "DrawCrosshair", true ) != 0) ? true : false;		 
	m_iAimType = pKeyValuesData->GetInt( "AimType", 1 );
	m_iScopeStyle = pKeyValuesData->GetInt( "ScopeStyle", 1 );

	m_fVMFov = pKeyValuesData->GetFloat( "VMFov", 54.0f );
	m_fVMAimFov = pKeyValuesData->GetFloat( "VMAimFov", 54.0f );
	m_fAimFov = pKeyValuesData->GetFloat( "AimFov", 54.0f );

	m_fVMOffsetUp = pKeyValuesData->GetFloat( "VMOffsetUp", 0.0f );
	m_fVMOffsetForward = pKeyValuesData->GetFloat( "VMOffsetForward", 0.0f );
	m_fVMOffsetRight = pKeyValuesData->GetFloat( "VMOffsetRight", 0.0f );

	m_fVMAimOffsetUp = pKeyValuesData->GetFloat( "VMAimOffsetUp", 0.0f );
	m_fVMAimOffsetForward = pKeyValuesData->GetFloat( "VMAimOffsetForward", 0.0f );
	m_fVMAimOffsetRight = pKeyValuesData->GetFloat( "VMAimOffsetRight", 0.0f );

	m_fVMAngleYaw = pKeyValuesData->GetFloat( "VMAngleYaw", 0.0f );
	m_fVMAnglePitch = pKeyValuesData->GetFloat( "VMAnglePitch", 0.0f );
	m_fVMAngleRoll = pKeyValuesData->GetFloat( "VMAngleRoll", 0.0f );

	m_fVMAimAngleYaw = pKeyValuesData->GetFloat( "VMAimAngleYaw", 0.0f );
	m_fVMAimAnglePitch = pKeyValuesData->GetFloat( "VMAimAnglePitch", 0.0f );
	m_fVMAimAngleRoll = pKeyValuesData->GetFloat( "VMAimAngleRoll", 0.0f );

	m_fPenetration = pKeyValuesData->GetFloat( "Penetration", 1.0f );

	m_fTPMuzzleFlashScale = pKeyValuesData->GetFloat( "TPMuzzleFlashScale", 1.0f );		  	
}


