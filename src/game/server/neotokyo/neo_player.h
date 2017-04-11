//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for NEO Game
//
// $NoKeywords: $
//=============================================================================//

#ifndef NEO_PLAYER_H
#define NEO_PLAYER_H
#pragma once


#define NEOPLAYER_VEC_DUCK_HULL_MIN( classType ) m_ViewVectors[ classType ].m_vDuckHullMin
#define NEOPLAYER_VEC_DUCK_HULL_MAX( classType ) m_ViewVectors[ classType ].m_vDuckHullMax
#define NEOPLAYER_VEC_HULL_MIN( classType ) m_ViewVectors[ classType ].m_vHullMin
#define NEOPLAYER_VEC_HULL_MAX( classType ) m_ViewVectors[ classType ].m_vHullMax


#include "basemultiplayerplayer.h"
#include "server_class.h"
#include "neo_playeranimstate.h"
#include "neo_shareddefs.h"
#include "neo_playerclass_info.h"

#define NEO_BODYPART_MAX HITGROUP_GEAR+1

// Function table for each player state.
class CNEOPlayerStateInfo
{
public:
	NEOPlayerState m_iPlayerState;
	const char *m_pStateName;

	void (CNEOPlayer::*pfnEnterState)();	// Init and deinit the state.
	void (CNEOPlayer::*pfnLeaveState)();

	void (CNEOPlayer::*pfnPreThink)();	// Do a PreThink() in this state.
};					  


//=============================================================================
// >> NEO Game player
//=============================================================================
class CNEOPlayer : public CBaseMultiplayerPlayer, public INEOPlayerAnimStateHelpers
{
public:
	DECLARE_CLASS( CNEOPlayer, CBaseMultiplayerPlayer );
	DECLARE_SERVERCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CNEOPlayer();
	~CNEOPlayer();

	static CNEOPlayer *CreatePlayer( const char *className, edict_t *ed );
	static CNEOPlayer* Instance( int iEnt );

	// This passes the event to the client's and server's CPlayerAnimState.
	void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );

	virtual int ShouldTransmit( const CCheckTransmitInfo *pInfo );

	virtual void FlashlightTurnOn( void );
	virtual void FlashlightTurnOff( void );
	virtual int FlashlightIsOn( void );

	virtual void PreThink();
	virtual void PostThink();
	virtual void Spawn();
	virtual void InitialSpawn();
	virtual void Precache();
	virtual void TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator );
	virtual int OnTakeDamage( const CTakeDamageInfo &info );
	virtual void Event_Killed( const CTakeDamageInfo &info );
	virtual void Touch( CBaseEntity *pOther );
	virtual void UpdateOnRemove();
	virtual Vector EyePosition();
	virtual int VPhysicsTakeDamage( const CTakeDamageInfo &info ) { return 1; }

	virtual void DoMuzzleFlash();

	virtual bool WantsLagCompensationOnEntity( const CBasePlayer *pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits ) const;
	
	virtual void PlayerDeathThink() {}

	virtual void PackDeadPlayerItems();

	virtual void LeaveVehicle( const Vector &vecExitPoint, const QAngle &vecExitAngles );
	
	CWeaponNEOBase* GetActiveNEOWeapon() const;
	CWeaponNEOBase*	GetNEOWeapon( int i ) const;
	virtual void	CreateViewModel( int viewmodelindex = 0 );

	virtual void	CheatImpulseCommands( int iImpulse );
	virtual bool	ClientCommand( const CCommand &args );

	bool SelectSpawnSpot( const char *pEntClassName, CBaseEntity* &pSpot );
	virtual CBaseEntity* EntSelectSpawnPoint();

	virtual bool BumpWeapon( CBaseCombatWeapon *pWeapon );

	virtual bool CanHearAndReadChatFrom( CBasePlayer *pPlayer );

	virtual void InitVCollision( const Vector &vecAbsOrigin, const Vector &vecAbsVelocity );

	void FixViewPunch( float viewPunch );

	static const char* GetPlayerModelString( int iTeamNum, int iClassNum, int iClassVariant, int iHitLocation );
	static const char* GetGibletsString( int iClassNum, int iClassVariant, int iHitLocation );

	static int GetMaxLoadoutIndex( int iTeamNum, int iClassType, int iRank, bool bUseDefault = false );
	static const char* GetLoadoutWeaponString( int iTeamNum, int iClassType, int iRank, int iSelectedLoadout, bool bUseDefault = false );
	static const char* GetLoadoutAmmoType( int iTeamNum, int iClassType, int iRank, int iSelectedLoadout, bool bUseDefault = false );
	static int GetLoadoutAmmoAmount( int iTeamNum, int iClassType, int iRank, int iSelectedLoadout, bool bUseDefault = false );

	void GiveEquipment();
	void PickLoadout();	 	

	void FreeWeaponSlot();	  

	void ResetMaxSpeed();
	void OnReconSuperJump();

	bool HandleCommand_JoinTeam( int iTeam );
	void HandleCommand_JoinStar( int iStar );
	bool HandleCommand_SetClass( int iClass, bool bForce = false );
	bool HandleCommand_SetVariant( int iVariant );

	void GetIntoGame();
	bool UpdateTransition();

	void ResetSprint();	   
	void ResetSprintNRG();
	void ResetThermoptics();
	void ResetLean();
	void ResetAim();

	void TossCurrentWeapon();		

	void UpdateVision();	   
	void SetSomething( float idk );

	bool CanSpawn()	{ return m_Unknown3708 && m_lifeState == LIFE_RESPAWNABLE; }
	bool CanSpeedBoost() { return m_fSprintNRG > NEO_MIN_SPEEDBOOST_AMOUNT; }

	CNetworkVar( int, m_iThrowGrenadeCounter );	// used to trigger grenade throw animations.
	CNetworkQAngle( m_angEyeAngles );	// Copied from EyeAngles() so we can send it to the client.
	CNetworkVar( int, m_iShotsFired );	// number of shots fired recently

	int m_iSelectedLoadout;

	int m_Unknown3628;

	bool m_Unknown3504;

	float m_Unknown3508;

	int m_Unknown3532;

	bool m_Unknown3536;
	
	int m_Unknown3540;
	float m_Unknown3544;
	float m_Unknown3548;

	float m_Unknown3564;

// In shared code.
public:
	// INEOPlayerAnimState overrides.
	virtual CWeaponNEOBase* NEOAnim_GetActiveWeapon();
	virtual bool NEOAnim_CanMove();

	bool CanMove();
	

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

public:
	CNetworkVar( int, m_iSprint );
	CNetworkVar( float, m_fSprintNRG );

	CNetworkVar( int, m_iThermoptic );
	CNetworkVar( float, m_fThermopticNRG );

	CNetworkVar( int, m_iNMFlash );
	CNetworkVar( int, m_iVision );

	CNetworkVar( int, m_iLives );

	CNetworkVar( int, m_iReinforceTimer );	   	

	CNetworkVar( int, m_iClassType );
	CNetworkVar( int, m_iRank );
	CNetworkVar( int, m_iLean );

	CNetworkVar( float, m_fTurnSpeed );

	// Tracks our ragdoll entity.
	CNetworkHandle( CBaseEntity, m_hRagdoll );	// networked entity handle 

private:
	Vector	m_vForce;

	int		m_iBodyPartsHit[ NEO_BODYPART_MAX ];

public:
	void BodyPartHit( int iHitGroup ); // Not our body parts

	void SetUnknown4380( float value );

	CNetworkVar( int, m_iStar );

	// ------------------------------------------------------------------------------------------------ //
	// Player state management.
	// ------------------------------------------------------------------------------------------------ //
public:		
	void State_Transition( NEOPlayerState newState );	// Cleanup the previous state and enter a new state.
	NEOPlayerState State_Get() const;					// Get the current state.

private:
	void State_Enter( NEOPlayerState newState );		// Initialize the new state.
	void State_Leave();								// Cleanup the previous state.
	void State_PreThink();							// Update the current state.

	// Find the state info for the specified state.
	static CNEOPlayerStateInfo* State_LookupInfo( NEOPlayerState state );

	// This tells us which state the player is currently in (joining, observer, dying, etc).
	// Each state has a well-defined set of parameters that go with it (ie: observer is movetype_noclip, nonsolid,
	// invisible, etc).	 																												
	CNEOPlayerStateInfo *m_pCurStateInfo;			// This can be NULL if no state info is defined for m_iPlayerState.		 
	NEOPlayerState m_iPlayerState;

	// Specific state handler functions.			 
	void State_Enter_ACTIVE();
	void State_PreThink_ACTIVE();

	void State_Enter_INTRO();
	void State_PreThink_INTRO();

	void State_Enter_PICKINGTEAM();
	void State_Enter_PICKINGCLASS();
	void State_Enter_PICKINGLOADOUT();

	void State_Enter_PLAYDEATH();
	void State_PreThink_PLAYDEATH();

	void State_Enter_DEAD();
	void State_PreThink_DEAD();

	void State_Enter_OBSERVERMODE();
	void State_PreThink_OBSERVERMODE();


	void UpdatePlayerClassInfo();

public:
	CNetworkVar( bool, m_bIsVIP );

private:
	bool m_Unknown3684;

	int m_Unknown3688;

	bool m_Unknown3700;

	float m_Unknown3704;

	bool m_Unknown3708;		 	

	CPlayerClassInfo* m_pPlayerClassInfo;

	int m_iClassVariant;

	int m_iCurrentPlayerClassInfo;

	int m_Unknown3728;

	CViewVectors m_ViewVectors[ CLASS_MAXCOUNT ];

	float m_Unknown4380;
	float m_Unknown4384;

	bool m_bWasKilled;
	float m_fTimeKilled;

private:		   
	void CreateRagdollEntity();

	INEOPlayerAnimState *m_PlayerAnimState;		
};


inline NEOPlayerState CNEOPlayer::State_Get() const
{
	return m_iPlayerState;
}

inline void CNEOPlayer::SetUnknown4380( float value )
{
	m_Unknown4380 = value;

	if ( m_Unknown4380 > 40.0f )
		m_Unknown4380 = 40.0f;
}


inline CNEOPlayer *ToNEOPlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;

#ifdef _DEBUG
	Assert( dynamic_cast<CNEOPlayer*>( pEntity ) != 0 );
#endif
	return static_cast< CNEOPlayer* >( pEntity );
}


#endif	// NEO_PLAYER_H
