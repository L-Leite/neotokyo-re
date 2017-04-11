#include "cbase.h"
#include "weapon_neobasefirearm.h"
#include "neo_fx_shared.h"
#include "in_buttons.h"


#ifdef CLIENT_DLL
	#include "c_neo_player.h"
	#include "dlight.h"
	#include "iefx.h"
#else
	#include "neo_player.h"
#endif



IMPLEMENT_NETWORKCLASS_ALIASED( WeaponNeoBaseFirearm, DT_WeaponNeoBaseFirearm )

BEGIN_NETWORK_TABLE( CWeaponNeoBaseFirearm, DT_WeaponNeoBaseFirearm )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_iFireMode ) ),
#else
	SendPropInt( SENDINFO( m_iFireMode ) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponNeoBaseFirearm )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_neobasefirearm, CWeaponNeoBaseFirearm );



CWeaponNeoBaseFirearm::CWeaponNeoBaseFirearm()
{
	m_iFireMode = 0;
	m_fAccuracyshit = 0.0f;
	m_bShouldReload = false;
	m_bIsSprinting = false;
	m_fOldAccuracy = 1.0f;
	m_bIsReadyToFire = false;
	m_bIsReloading = false;
	m_Unknown1504 = 0.0f;
}

bool CWeaponNeoBaseFirearm::Deploy()
{
	CNEOPlayer *pPlayer = GetPlayerOwner();

	if ( !pPlayer )
		return false;

	pPlayer->m_iShotsFired = 0;
	bAimed = false;
#ifdef CLIENT_DLL
	m_Unknown1892 = 0;
	m_Unknown1904 = 0;
#endif

	return BaseClass::Deploy();
}

bool CWeaponNeoBaseFirearm::ReloadOrSwitchWeapons()
{
	CNEOPlayer* pPlayer = GetPlayerOwner();

	if ( !pPlayer || !pPlayer->IsPlayer() )
		return false;

	m_bFireOnEmpty = false;

	if ( (HasAnyAmmo() || m_flNextPrimaryAttack < gpGlobals->curtime || m_flNextSecondaryAttack < gpGlobals->curtime)
		&& UsesClipsForAmmo1()
		&& m_iClip1 == 0
		&& (gpGlobals->curtime - 0.2f) > m_flNextPrimaryAttack )
	{
		if ( Reload() )
			return true;
	}

	return false;
}

void CWeaponNeoBaseFirearm::ItemPostFrame()
{	 
	CNEOPlayer* pPlayer = GetPlayerOwner();

	if ( !pPlayer )
		return BaseClass::ItemPostFrame();

	m_bIsSprinting = pPlayer->m_iSprint == 1 || pPlayer->IsOnLadder();

	float timeSinceLastPostFrame = gpGlobals->curtime - m_fLastItemPostFrameTime;

	if ( !( pPlayer->GetFlags() & FL_ONGROUND ) )
		m_fAccuracy = timeSinceLastPostFrame * 3;

	float fVelocity = pPlayer->GetAbsVelocity().Length2D();

	if ( fVelocity > 5.0f && !bAimed )
		m_fAccuracy = timeSinceLastPostFrame + timeSinceLastPostFrame;

	if ( pPlayer->GetFlags() & FL_DUCKING )
		m_fAccuracy	= timeSinceLastPostFrame;

	m_fAccuracy *= 1.2f;

	if ( m_fOldAccuracy < m_fAccuracy || m_fAccuracyshit > m_fAccuracy )
		m_fAccuracy = m_fAccuracyshit;

	m_fLastItemPostFrameTime = gpGlobals->curtime;

	if ( m_bIsSprinting )
	{
		if ( !m_bIsReadyToFire )
		{
			m_bIsReadyToFire = true;
			SendWeaponAnim( ACT_VM_LOWERED_TO_IDLE );
		}

		SetWeaponIdleTime( gpGlobals->curtime + 0.125 );
		return;
	}

	if ( !m_bIsReadyToFire )
	{
		if ( pPlayer->m_afButtonReleased & IN_ATTACK )	   
			pPlayer->m_iShotsFired = 0;

		else if ( !( pPlayer->m_nButtons & IN_ATTACK ) )
			m_flNextPrimaryAttack = gpGlobals->curtime - 0.1f;

		else if ( (gpGlobals->curtime > m_flNextPrimaryAttack) && m_iClip1 <= 0 )
			NEOPlayEmptySound();

		return BaseClass::ItemPostFrame();
	}	
	else
	{
		m_bIsReadyToFire = false;
		SendWeaponAnim( ACT_VM_IDLE_TO_LOWERED );
		SetWeaponIdleTime( gpGlobals->curtime + 0.25 );

		if ( gpGlobals->curtime >= m_flNextPrimaryAttack )
			m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->curtime + 0.25;
		else
			m_flNextSecondaryAttack = m_flNextPrimaryAttack = 0.25;
	}	   		   	
}

bool CWeaponNeoBaseFirearm::NEOBaseGunFire( float flCycleTime )
{
	if ( m_flNextPrimaryAttack > gpGlobals->curtime
		|| m_flNextSecondaryAttack > gpGlobals->curtime )
		return false;

	CNEOPlayer* pPlayer = GetPlayerOwner();

	if ( !pPlayer || pPlayer->m_iSprint || pPlayer->IsOnLadder() )
		return false;

	if ( bAimed )
		m_fAccuracy *= 0.26f + 0.35f;

	pPlayer->m_iShotsFired++;

	if ( m_iFireMode == 0 && pPlayer->m_iShotsFired > 1 )
		return false;

	// Out of ammo?
	if ( m_iClip1 <= 0 )
	{
		if ( ShouldPlayEmptySound() )
		{
			NEOPlayEmptySound();
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
		}

		return false;
	}

#ifdef GAME_DLL
	pPlayer->BodyPartHit( HITGROUP_GENERIC );
#endif

	SendWeaponAnim( m_iClip1 <= 0 ? ACT_VM_DRYFIRE : ACT_VM_PRIMARYATTACK );

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

#ifdef CLIENT_DLL
	DoNEOMuzzleFlash();
#endif

	m_fAccuracy = 0.8f;

	if ( m_fOldAccuracy < m_fAccuracy )
		m_fAccuracy = m_fOldAccuracy;

	if ( pPlayer )
		pPlayer->FixViewPunch( 0 );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + flCycleTime;

	SetWeaponIdleTime( gpGlobals->curtime + 2.0 );

	AddViewKick();

	return true;
}

void CWeaponNeoBaseFirearm::WeaponIdle()
{
	CNEOPlayer* pOwner = GetPlayerOwner();

	if ( !pOwner || !pOwner->IsPlayer() )
		return;

	if ( !HasWeaponIdleTimeElapsed() || m_bIsReadyToFire )
		return;

	if ( m_iClip1 > 0 )
	{
		SetWeaponIdleTime( gpGlobals->curtime + 5.0f );
		SendWeaponAnim( ACT_VM_IDLE );
	}
	else				
	{
		SetWeaponIdleTime( gpGlobals->curtime + 0.4f );
		SendWeaponAnim( ACT_VM_IDLE_EMPTY ); // Or ACT_VM_IDLE_DEPLOYED_EMPTY?

		if ( m_bShouldReload )
			Reload();
		else
			m_bShouldReload = true;
	}
}

void CWeaponNeoBaseFirearm::FinishReload()
{
	CBaseCombatCharacter* pOwner = GetOwner();

	if ( !pOwner || this != pOwner->GetActiveWeapon() )
		return;

	// If I use primary clips, reload primary
	if ( UsesClipsForAmmo1() )
	{
		int primary = MIN( GetMaxClip1(), pOwner->GetAmmoCount( m_iPrimaryAmmoType ) );
		m_iClip1 = primary;
		pOwner->RemoveAmmo( primary, m_iPrimaryAmmoType );
	}

	// If I use secondary clips, reload secondary
	if ( UsesClipsForAmmo2() )
	{
		int secondary = MIN( GetMaxClip2(), pOwner->GetAmmoCount( m_iSecondaryAmmoType ) );
		m_iClip2 = secondary;
		pOwner->RemoveAmmo( secondary, m_iSecondaryAmmoType );
	}

	if ( m_bReloadsSingly )
	{
		m_bInReload = false;
	}

	m_bIsReloading = false;
}

bool CWeaponNeoBaseFirearm::Reload()
{	
	CNEOPlayer* pOwner = GetPlayerOwner();

	if ( !pOwner || pOwner->m_iSprint == 1 || pOwner->IsOnLadder() )
		return false;

	if ( pOwner->GetAmmoCount( GetPrimaryAmmoType() ) <= 0 )
		return false;

	int iResult = DefaultReload( GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD );

	if ( !iResult )
		return false;

	pOwner->SetAnimation( PLAYER_RELOAD );

#ifdef GAME_DLL
	SendReloadEvents();
#endif

	bAimed = false;
	pOwner->m_iShotsFired = 0;	  	

	m_bShouldReload = false;
	m_bIsReloading = true;

	return true;
}

void CWeaponNeoBaseFirearm::PrimaryAttack()
{
	// Derived classes should implement this and call CSBaseGunFire.
	Assert( false );
}

void CWeaponNeoBaseFirearm::SecondaryAttack()
{
	CNEOPlayer* pOwner = GetPlayerOwner();

	if ( !pOwner  )
		return;

	if ( m_Unknown1488 <= 1 || m_bIsSprinting )		  
		return;

	if ( m_flNextSecondaryAttack > gpGlobals->curtime )
		return;

	m_iFireMode = 0;

	if ( m_iFireMode >= m_Unknown1488 )
		m_iFireMode = 0;

	SendWeaponAnim( ACT_VM_SECONDARYATTACK );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + 0.65f;
}

void CWeaponNeoBaseFirearm::NEOPlayEmptySound()
{		
	CNEOPlayer* pOwner = GetPlayerOwner();

	if ( !pOwner || !pOwner->IsPlayer() )
		return;

	WeaponSound( EMPTY );
	SendWeaponAnim( ACT_VM_DRYFIRE );
	float fSeqDuration = SequenceDuration( GetSequence() );

	m_flNextPrimaryAttack = gpGlobals->curtime + fSeqDuration;
}

bool CWeaponNeoBaseFirearm::ShouldPlayEmptySound()
{
	return !m_bInReload;
}

#ifdef CLIENT_DLL
void CWeaponNeoBaseFirearm::DoNEOMuzzleFlash()
{
	CNEOPlayer* owner = ToNEOPlayer( GetOwner() );

	if ( !owner )
		return;

	CBaseViewModel* viewModel = owner->GetViewModel( m_nViewModelIndex );

	if ( !viewModel )
		return;

	Vector origin;
	QAngle angles;

	bool result = viewModel->GetAttachment( 1, origin, angles );

	if ( !result )
		return;

	dlight_t* light = effects->CL_AllocDlight( LIGHT_INDEX_TE_DYNAMIC + index );

	if ( !light )
		return;

	light->origin = origin;
	light->radius = RandomFloat( 64.f, 96.f );
	light->decay = RandomFloat( 64.f, 96.f ) / 0.05f;
	light->color.r = 255;
	light->color.g = 192;
	light->color.b = 64;
	light->color.exponent = 5;
	light->die = gpGlobals->curtime + 0.05f;

	FX_MuzzleEffect( origin, angles, 1.f, 1 ); // 1 is the world index
}
#endif