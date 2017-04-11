//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef WEAPON_NEOBASE_H
#define WEAPON_NEOBASE_H
#ifdef _WIN32
#pragma once
#endif

#include "neo_playeranimstate.h"
#include "neo_weapon_parse.h"

#if defined( CLIENT_DLL )
	#define CWeaponNEOBase C_WeaponNEOBase
#endif

class CNEOPlayer;

// These are the names of the ammo types that the weapon script files reference.
#define AMMO_BULLETS			"AMMO_BULLETS"
#define AMMO_PRI				"AMMO_PRI"
#define AMMO_SEC				"AMMO_SEC"
#define AMMO_6_8MM				"AMMO_6.8MM"
#define AMMO_5_7MM				"AMMO_5_7MM"
#define AMMO_SMAC				"AMMO_SMAC"
#define AMMO_11_5MM				"AMMO_11.5MM"
#define AMMO_8_MM				"AMMO_8_MM"
#define AMMO_10G_SHELL			"AMMO_10G_SHELL"
#define AMMO_GRENADE			"AMMO_GRENADE"
#define AMMO_SMOKEGRENADE		"AMMO_SMOKEGRENADE"

typedef enum
{
	Primary_Mode = 0,
	Secondary_Mode,
} NEOWeaponMode;

const char *WeaponIDToAlias( int id );

class CWeaponNEOBase : public CBaseCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponNEOBase, CBaseCombatWeapon );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CWeaponNEOBase();

	#ifdef GAME_DLL
		DECLARE_DATADESC();
	#endif

	virtual void Spawn();
	virtual void Precache( void );

#ifdef CLIENT_DLL
	virtual bool ShouldPredict();

	//virtual int InternalDrawModel( int flags );
#endif

	// All predicted weapons need to implement and return true
	virtual bool	IsPredicted() const { return true; }
	virtual NEOWeaponID GetWeaponID( void ) const { return WEAPON_NONE; }
	
	virtual void	SetViewModel();

	virtual void	ItemPostFrame();

	// Get NEO weapon specific weapon data.
	CNEOWeaponInfo const	&GetNEOWpnData() const;

	// Get a pointer to the player that owns this weapon
	CNEOPlayer* GetPlayerOwner() const;

	virtual bool HasAmmoClip() { return true; }

	// override to play custom empty sounds
	virtual bool PlayEmptySound();

#ifdef GAME_DLL
	virtual void SendReloadEvents();
#endif

	virtual float	GetFireRate() { return 1.0f; }

#ifdef CLIENT_DLL
	virtual void	UpdateShouldDrawViewmodel();
	virtual void	SetShouldDrawViewmodel();
	virtual void	OverrideViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles );
#endif
	virtual void	AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles );
	virtual	float	CalcViewmodelBob();

	virtual const char*	GetWeaponTypeName() { return "primary"; }

	virtual bool		ShouldStayOnRoundRestart() { return false; }

	virtual wchar_t		GetWeaponChar() { return L'h'; } // Idk what's this about

	virtual float		GetUnknown1452() { return m_Unknown1452; }
#ifdef CLIENT_DLL
	virtual void	DoNEOMuzzleFlash();
#endif

	virtual const char*	GetTeam2ViewModel();

	virtual bool		IsDropable() { return true; }
	virtual bool		IsTossable() { return true; }
	virtual bool		CanAim() { return true; }

	virtual bool		IsGrenade() { return false; }
	virtual bool		IsPinPulled() { return false; }
	virtual void		ExplodeDroppedGrenade() {}

	virtual float	GetRunningFov() { return 75.f; }
	virtual float	GetAimingFov() { return 50.f; }

	bool			CanClassUseThis( int iClassType );
	int				GetOwnerTeamNumber();

	int				GetAimType() const { return m_iAimType; }

public:
	CNetworkVar( int, m_iTeam2ViewModelIndex );

	float m_fFov;

protected:
	float		m_fLastPrimaryAttackTime;

	bool m_Unknown1360;

	float m_Unknown1364;

	bool		m_bDrawCrosshair;

public:
	CNetworkVar( bool, bAimed );		

#ifdef CLIENT_DLL
public:
	// Prob not in the server since it's viewmodel related
	int			m_Unknown1892;

	protected:
	float		m_Unknown1896;
	float		m_Unknown1904;

	int			m_Unknown1912;
	float		m_Unknown1928;
	float		m_Unknown1932;
#endif

private:
	int			m_iAimType;
	int			m_iScopeStyle;		 	

	float		m_fVMFov;
	float		m_fVMAimFov;
	float		m_fAimFov;

	float		m_fVMOffsetUp;
	float		m_fVMOffsetForward;
	float		m_fVMOffsetRight;

	float		m_fVMAimOffsetUp;
	float		m_fVMAimOffsetForward;
	float		m_fVMAimOffsetRight;

	float		m_fVMAngleYaw;
	float		m_fVMAnglePitch;
	float		m_fVMAngleRoll;

	float		m_fVMAimAngleYaw;
	float		m_fVMAimAnglePitch;
	float		m_fVMAimAngleRoll;

	float		m_fTPMuzzleFlashScale;

	float m_Unknown1452;
	bool m_Unknown1456;

	bool m_bCanReconUse;
	bool m_bCanAssaultUse;
	bool m_bCanSupportUse;
	bool m_bCanOperatorUse;
	bool m_bCanVipUse;
	bool m_Unknown1449;

public:
	CNetworkVar( float, m_fAccuracy );

protected:
	float		m_fAccuracyshit;
	float		m_fOldAccuracy; // not sure about the purpose of this	 	

	bool m_Unknown1472;
	int m_Unknown1476;

private:
	CWeaponNEOBase( const CWeaponNEOBase & );
};


#endif // WEAPON_NEOBASE_H
