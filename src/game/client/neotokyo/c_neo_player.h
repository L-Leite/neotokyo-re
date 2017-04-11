//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef C_NEO_PLAYER_H
#define C_NEO_PLAYER_H
#ifdef _WIN32
#pragma once
#endif


#include "neo_playeranimstate.h"
#include "c_baseplayer.h"
#include "neo_shareddefs.h"
#include "baseparticleentity.h"
#include "iviewrender_beams.h"
#include "neo_playerclass_info.h"


class C_NEOPlayer : public C_BasePlayer, public INEOPlayerAnimStateHelpers
{
public:
	DECLARE_CLASS( C_NEOPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();

	C_NEOPlayer();
	~C_NEOPlayer();

	static C_NEOPlayer* GetLocalNEOPlayer();
	bool IsLocalNEOPlayer() const;

	virtual void Simulate();
	void ReleaseFlashlight();

	virtual C_BaseAnimating* BecomeRagdollOnClient( bool bCopyEntity = true );
	virtual void UpdateClientSideAnimation();

	virtual void DoMuzzleFlash();
	virtual void ProcessMuzzleFlashEvent();
	virtual void NEO_MuzzleFlash();

	virtual void CalcView( Vector &eyeOrigin, QAngle &eyeAngles, float &zNear, float &zFar, float &fov );
	virtual void CalcPlayerView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov );

	virtual int	DrawModel( int flags );
	virtual ShadowType_t ShadowCastType();

	virtual void PreThink();

	virtual float GetFOV();
	virtual float GetMinFOV() const;

	virtual const QAngle& GetRenderAngles();
	virtual void PostDataUpdate( DataUpdateType_t updateType );
	virtual void OnDataChanged( DataUpdateType_t updateType );

	virtual void GetEyeVectors( Vector *forward, Vector *right, Vector *up );

	bool HasGhost();

	bool CanSpeedBoost() { return m_fSprintNRG > NEO_MIN_SPEEDBOOST_AMOUNT; }

private:
	int CanLean( int iLean );

	void idk2();
	void UpdateLean();
	void UpdateThermoptic();
	void UpdateGeiger();
	void UpdateVision();
	void UpdateIDTarget();
	void UpdateTurnSpeed( float fTimeDelta );


// Called by shared code.
public:
	
	// INEOPlayerAnimState overrides.
	virtual CWeaponNEOBase* NEOAnim_GetActiveWeapon();
	virtual bool NEOAnim_CanMove();

	void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );
	bool ShouldDraw();

	INEOPlayerAnimState *m_PlayerAnimState;		

	static const char* GetPlayerModelString( int iTeamNum, int iClassNum, int iClassVariant, int iHitLocation );
	static const char* GetGibletsString( int iClassNum, int iClassVariant, int iHitLocation );

	static int GetMaxLoadoutIndex( int iTeamNum, int iClassType, int iRank, bool bUseDefault = false );
	static const char* GetLoadoutWeaponString( int iTeamNum, int iClassType, int iRank, int iSelectedLoadout, bool bUseDefault = false );
	static const char* GetLoadoutAmmoType( int iTeamNum, int iClassType, int iRank, int iSelectedLoadout, bool bUseDefault = false );
	static int GetLoadoutAmmoAmount( int iTeamNum, int iClassType, int iRank, int iSelectedLoadout, bool bUseDefault = false );

	void SetUnknown4380( float value );

	CWeaponNEOBase *GetActiveNEOWeapon() const;

	C_BaseAnimating *BecomeRagdollOnClient();
	virtual IRagdoll* GetRepresentativeRagdoll() const;

	void FireBullet( 
		Vector vecSrc, 
		const QAngle &shootAngles, 
		float vecSpread, 
		int iDamage, 
		int iBulletType,
		float fPenetration,
		CBaseEntity *pevAttacker,
		bool bDoEffects,
		float x,
		float y );

	void FixViewPunch( float viewPunch );

	int	 GetIDTarget() const;

	void UpdatePlayerClassInfo();

	int m_Unknown3628;

public:
	CNetworkVar( bool, m_bIsVIP );

	float m_fThermopticAlpha;

	int m_Unknown3956;	   
	float m_Unknown3960;
	float m_Unknown3964;

	CNetworkVar( int, m_iLean );
	
	CNetworkVar( int, m_iSprint );
	CNetworkVar( float, m_fSprintNRG );

	CNetworkVar( int, m_iThermoptic );
	int m_iOldThermoptic;

	CPlayerClassInfo* m_pPlayerClassInfo;
	int m_iCurrentPlayerClassInfo;

	CNetworkVar( float, m_fThermopticNRG );

	CNetworkVar( int, m_iClassType );
	int m_iOldClassType;

	CNetworkVar( int, m_iRank );
	int m_iOldRank;

	QAngle	m_angEyeAngles;
	CInterpolatedVar< QAngle >	m_iv_angEyeAngles;

	CNetworkVar( int, m_iThrowGrenadeCounter );	// used to trigger grenade throw animations.

	CNetworkVar( int, m_iShotsFired );	// number of shots fired recently

	EHANDLE	m_hRagdoll;

	CNetworkVar( int, m_iNMFlash );
	int m_iOldNMFlash;

	float m_Unknown4108;

	CNetworkVar( int, m_iVision );
	int m_iOldVision;

	int m_iIDEntIndex;
	bool m_bAnyoneInCross;

	CNetworkVar( int, m_iReinforceTimer );

	CNetworkVar( int, m_iLives );

	CNetworkVar( int, m_iStar );		 

	CNetworkVar( float, m_fTurnSpeed ); 
	float m_fOldTurnSpeed; // ???

	bool m_Unknown4132;

private:
	Beam_t* m_pFlashlightBeam;

	bool m_bIsOnDeathScreen;
	float m_fRagdollCreationTime;

	float m_fLastThinkTime;
	bool m_Unknown4164;

	float m_Unknown4380; // 4380 is from server

private:   
	C_NEOPlayer( const C_NEOPlayer & );
};

inline void CNEOPlayer::SetUnknown4380( float value )
{
	m_Unknown4380 = value;

	if ( m_Unknown4380 > 40.0f )
		m_Unknown4380 = 40.0f;
}

inline int C_NEOPlayer::GetIDTarget() const
{
	return m_iIDEntIndex;
}


inline C_NEOPlayer* ToNEOPlayer( CBaseEntity *pPlayer )
{
	Assert( dynamic_cast< C_NEOPlayer* >( pPlayer ) != NULL );
	return static_cast< C_NEOPlayer* >( pPlayer );
}


#endif // C_NEO_PLAYER_H
