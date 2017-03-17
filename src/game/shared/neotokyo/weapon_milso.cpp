#include "cbase.h"
#include "weapon_neobasefirearm.h"
#include "neo_fx_shared.h"

#if defined( CLIENT_DLL )

	#define CWeaponMilso C_WeaponMilso
	#include "c_neo_player.h"

#else

	#include "neo_player.h"

#endif


class CWeaponMilso : public CWeaponNeoBaseFirearm
{
public:
	DECLARE_CLASS( CWeaponMilso, CWeaponNeoBaseFirearm );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponMilso();

	virtual NEOWeaponID GetWeaponID( void ) const		{ return WEAPON_MILSO; }

	virtual bool Deploy();
	virtual bool Reload();

	virtual void PrimaryAttack();

	virtual void AddViewKick();	 

private:

	CWeaponMilso( const CWeaponMilso & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponMilso, DT_WeaponMilso )

BEGIN_NETWORK_TABLE( CWeaponMilso, DT_WeaponMilso )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponMilso )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_milso, CWeaponMilso );
PRECACHE_WEAPON_REGISTER( weapon_milso );



CWeaponMilso::CWeaponMilso()
{
	m_fAccuracy = 0.6f;
}

bool CWeaponMilso::Deploy()
{
	m_fAccuracy = 0.9f;

	CNEOPlayer* pOwner = GetPlayerOwner();

	if ( pOwner->m_iShotsFired > 0 )
		pOwner->m_iShotsFired = 0;

	return BaseClass::Deploy();	
}

bool CWeaponMilso::Reload()
{
	CNEOPlayer* pOwner = GetPlayerOwner();

	if ( !pOwner || !pOwner->IsPlayer() || pOwner->IsOnLadder() )
		return false;

	if ( pOwner->GetAmmoCount( GetPrimaryAmmoType() ) <= 0 )
		return false;

	int iResult = DefaultReload( GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD );

	if ( !iResult )
		return false;

	pOwner->SetAnimation( PLAYER_RELOAD );

	m_fAccuracy = 0.9f;
	pOwner->m_iShotsFired = 0;

	return true;
}

void CWeaponMilso::PrimaryAttack()
{
	if ( m_flNextPrimaryAttack > gpGlobals->curtime
		|| m_flNextSecondaryAttack > gpGlobals->curtime )
		return;

	const CNEOWeaponInfo& pWeaponInfo = GetNEOWpnData();
	CNEOPlayer* pPlayer = GetPlayerOwner();

	if ( !pPlayer || pPlayer->m_iSprint || pPlayer->IsOnLadder() )
		return;

	float flCycleTime = pWeaponInfo.m_flCycleTime;	
	float fAccuracy = m_fAccuracy;

	if ( bAimed )
		fAccuracy = fAccuracy * 0.26f + 0.35f;		

	pPlayer->m_iShotsFired++;

	if ( m_iFireMode == 0 && pPlayer->m_iShotsFired > 1 )
		return;

	// Out of ammo?
	if ( m_iClip1 <= 0 )
	{
		NEOPlayEmptySound();
	}

#ifdef GAME_DLL
	pPlayer->BodyPartHit( HITGROUP_GENERIC );
#endif

	int actPrimAttack = m_iClip1 <= 0 ? ACT_VM_DRYFIRE : ACT_VM_PRIMARYATTACK;

	SendWeaponAnim( actPrimAttack );

#ifndef CLIENT_DLL
	pPlayer->CheatImpulseCommands( 5 );
#endif
													 
	m_iClip1--;

	// player "shoot" animation
	pPlayer->SetAnimation( PLAYER_ATTACK1 );

	FX_FireBullets(
		pPlayer->entindex(),
		pPlayer->Weapon_ShootPosition(),
		pPlayer->EyeAngles() + pPlayer->GetPunchAngle(),
		GetWeaponID(),
		Primary_Mode,
		CBaseEntity::GetPredictionRandomSeed() & 255,
		m_fAccuracy );

	m_fAccuracy = 0.8f;

	if ( m_fOldAccuracy < m_fAccuracy )
		m_fAccuracy = m_fOldAccuracy;

	if ( pPlayer )
		pPlayer->FixViewPunch( 0.0f );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + flCycleTime;

	SetWeaponIdleTime( gpGlobals->curtime + 2.0 );

	AddViewKick();

	pPlayer->DoMuzzleFlash();

#ifdef CLIENT_DLL
	DoNEOMuzzleFlash();
#endif								  	
}

void CWeaponMilso::AddViewKick( void )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( pPlayer == NULL )
		return;

	QAngle viewPunch;

	viewPunch.x = SharedRandomFloat( "pistolpax", 0.25f, 0.5f );
	viewPunch.y = SharedRandomFloat( "pistolpay", -.6f, .6f );
	viewPunch.z = 0.0f;

	//Add it to the view punch
	pPlayer->ViewPunch( viewPunch );
}
