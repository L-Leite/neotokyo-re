#ifndef WEAPON_NEOBASEFIREARM_H
#define WEAPON_NEOBASEFIREARM_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_neobase.h"

#if defined( CLIENT_DLL )
	#define CWeaponNeoBaseFirearm C_WeaponNeoBaseFirearm
#endif

class CWeaponNeoBaseFirearm : public CWeaponNEOBase
{
public:
	DECLARE_CLASS( CWeaponNeoBaseFirearm, CWeaponNEOBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CWeaponNeoBaseFirearm();

	virtual bool Deploy();
	virtual bool ReloadOrSwitchWeapons();
	virtual void ItemPostFrame();
	virtual void WeaponIdle();
	virtual void FinishReload();
	virtual bool Reload();

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();

	virtual	bool ShouldPlayEmptySound();   
	virtual void NEOPlayEmptySound();

#ifdef CLIENT_DLL
	virtual void	DoNEOMuzzleFlash();
#endif

	//virtual void EjectBrass();

private:
	bool		m_bShouldReload;

public: 	   
	CNetworkVar( int, m_iFireMode );

private:
	int			m_Unknown1488;

	float		m_fLastItemPostFrameTime;

	bool		m_bIsSprinting;

	bool		m_bIsReadyToFire;

	float		m_Unknown1504;

	bool		m_bIsReloading;

private:
	CWeaponNeoBaseFirearm( const CWeaponNeoBaseFirearm & );
};


#endif // WEAPON_NEOBASEFIREARM_H
