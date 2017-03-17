//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for HL1.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "neo_player.h"
#include "neo_gamerules.h"
#include "weapon_neobase.h"
#include "neo_viewmodel.h"
#include "iservervehicle.h"
#include "viewport_panel_names.h"
#include "gib.h"
#include "in_buttons.h"
#include "team.h"
#include "vphysics/player_controller.h"

ConVar sv_motd_unload_on_dismissal( "sv_motd_unload_on_dismissal", "0", 0, "If enabled, the MOTD contents will be unloaded when the player closes the MOTD." );
ConVar neo_ShowStateTransitions( "neo_ShowStateTransitions", "-2", FCVAR_CHEAT, "cs_ShowStateTransitions <ent index or -1 for all>. Show player state transitions." );

ConVar bot_class( "bot_class", "0", 0, "", true, 0, true, 2 );
ConVar bot_var( "bot_var", "0", 0, "", true, 0, true, 2 );

#define GHOST_MAX_TRANSMIT_DISTANCE 1000.0f



// -------------------------------------------------------------------------------- //
// Player animation event. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //

class CTEPlayerAnimEvent : public CBaseTempEntity
{
public:
	DECLARE_CLASS( CTEPlayerAnimEvent, CBaseTempEntity );
	DECLARE_SERVERCLASS();

	CTEPlayerAnimEvent( const char *name ) : CBaseTempEntity( name )
	{
	}

	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
	CNetworkVar( int, m_nData );
};

#define THROWGRENADE_COUNTER_BITS 3

IMPLEMENT_SERVERCLASS_ST_NOBASE( CTEPlayerAnimEvent, DT_TEPlayerAnimEvent )
SendPropEHandle( SENDINFO( m_hPlayer ) ),
SendPropInt( SENDINFO( m_iEvent ), Q_log2( PLAYERANIMEVENT_COUNT ) + 1, SPROP_UNSIGNED ),
SendPropInt( SENDINFO( m_nData ), 32 )
END_SEND_TABLE()

static CTEPlayerAnimEvent g_TEPlayerAnimEvent( "PlayerAnimEvent" );

void TE_PlayerAnimEvent( CBasePlayer *pPlayer, PlayerAnimEvent_t event, int nData )
{
	CPVSFilter filter( (const Vector&) pPlayer->EyePosition() );

	g_TEPlayerAnimEvent.m_hPlayer = pPlayer;
	g_TEPlayerAnimEvent.m_iEvent = event;
	g_TEPlayerAnimEvent.m_nData = nData;
	g_TEPlayerAnimEvent.Create( filter, 0 );
}

// -------------------------------------------------------------------------------- //
// Tables.
// -------------------------------------------------------------------------------- //
BEGIN_DATADESC( CNEOPlayer )
//DEFINE_THINKFUNC( NEOPushawayThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( player, CNEOPlayer );
PRECACHE_REGISTER( player );

BEGIN_SEND_TABLE_NOBASE( CNEOPlayer, DT_NEOLocalPlayerExclusive )
SendPropInt( SENDINFO( m_iShotsFired ), 8, SPROP_UNSIGNED ),
SendPropFloat( SENDINFO( m_fSprintNRG ) ),
SendPropFloat( SENDINFO( m_fThermopticNRG ) ),
SendPropInt( SENDINFO( m_iReinforceTimer ) ),
SendPropInt( SENDINFO( m_iSprint ), 4, SPROP_CHANGES_OFTEN ),
SendPropFloat( SENDINFO( m_fTurnSpeed ) ),
END_SEND_TABLE()

IMPLEMENT_SERVERCLASS_ST( CNEOPlayer, DT_NEOPlayer )
SendPropExclude( "DT_BaseAnimating", "m_flPoseParameter" ),
SendPropExclude( "DT_BaseAnimating", "m_flPlaybackRate" ),
SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
SendPropExclude( "DT_BaseAnimatingOverlay", "overlay_vars" ),

// playeranimstate and clientside animation takes care of these on the client
SendPropExclude( "DT_ServerAnimationData", "m_flCycle" ),
SendPropExclude( "DT_AnimTimeMustBeFirst", "m_flAnimTime" ),

// Data that only gets sent to the local player.
SendPropDataTable( "neolocaldata", 0, &REFERENCE_SEND_TABLE( DT_NEOLocalPlayerExclusive ), SendProxy_SendLocalDataTable ),

SendPropAngle( SENDINFO_VECTORELEM( m_angEyeAngles, 0 ), 11 ),
SendPropAngle( SENDINFO_VECTORELEM( m_angEyeAngles, 1 ), 11 ),
SendPropEHandle( SENDINFO( m_hRagdoll ) ),

SendPropInt( SENDINFO( m_iThrowGrenadeCounter ), THROWGRENADE_COUNTER_BITS, SPROP_UNSIGNED ),
SendPropInt( SENDINFO( m_iClassType ) ),
SendPropInt( SENDINFO( m_iLives ) ),
SendPropInt( SENDINFO( m_iRank ) ),
SendPropInt( SENDINFO( m_iStar ) ),
SendPropInt( SENDINFO( m_iLean ), 4, SPROP_CHANGES_OFTEN ),
SendPropInt( SENDINFO( m_iThermoptic ), 4, SPROP_CHANGES_OFTEN ),
SendPropInt( SENDINFO( m_iNMFlash ), 4, SPROP_CHANGES_OFTEN ),
SendPropInt( SENDINFO( m_iVision ), 4, SPROP_CHANGES_OFTEN ),
SendPropBool( SENDINFO( m_bIsVIP ) ),
END_SEND_TABLE()

class CPhysicsPlayerCallback : public IPhysicsPlayerControllerEvent
{
public:
	int ShouldMoveTo( IPhysicsObject *pObject, const Vector &position )
	{
		CNEOPlayer *pPlayer = (CNEOPlayer *) pObject->GetGameData();
		if ( pPlayer )
		{
			if ( pPlayer->TouchedPhysics() )
			{
				return 0;
			}
		}
		return 1;
	}
};

static CPhysicsPlayerCallback playerCallback;

class CNEORagdoll : public CBaseAnimatingOverlay
{
public:
	DECLARE_CLASS( CNEORagdoll, CBaseAnimatingOverlay );
	DECLARE_SERVERCLASS();

	// Transmit ragdolls to everyone.
	virtual int UpdateTransmitState()
	{
		return SetTransmitState( FL_EDICT_ALWAYS );
	}

public:
	// In case the client has the player entity, we transmit the player index.
	// In case the client doesn't have it, we transmit the player's model index, origin, and angles
	// so they can create a ragdoll in the right place.
	CNetworkHandle( CBaseEntity, m_hPlayer );	// networked entity handle 
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );
};

LINK_ENTITY_TO_CLASS( neo_ragdoll, CNEORagdoll );

IMPLEMENT_SERVERCLASS_ST_NOBASE( CNEORagdoll, DT_NEORagdoll )
SendPropVector( SENDINFO( m_vecRagdollOrigin ), -1, SPROP_COORD ),
SendPropEHandle( SENDINFO( m_hPlayer ) ),
SendPropModelIndex( SENDINFO( m_nModelIndex ) ),
SendPropInt( SENDINFO( m_nForceBone ), 8, 0 ),
SendPropVector( SENDINFO( m_vecForce ), -1, SPROP_NOSCALE ),
SendPropVector( SENDINFO( m_vecRagdollVelocity ) )
END_SEND_TABLE()


// -------------------------------------------------------------------------------- //

void cc_CreatePredictionError_f()
{
	CBaseEntity *pEnt = CBaseEntity::Instance( 1 );
	pEnt->SetAbsOrigin( pEnt->GetAbsOrigin() + Vector( 63, 0, 0 ) );
}

ConCommand cc_CreatePredictionError( "CreatePredictionError", cc_CreatePredictionError_f, "Create a prediction error", FCVAR_CHEAT );


ConVar neo_ff_feedback( "neo_ff_feedback", "0.25", 0, "Friendly fire feedback." );


CNEOPlayer::CNEOPlayer()
{	 
	m_PlayerAnimState = CreatePlayerAnimState( this, this, LEGANIM_9WAY, true );

	V_memset( m_iBodyPartsHit, 0, sizeof( m_iBodyPartsHit ) );

	UseClientSideAnimation();

	m_angEyeAngles.Init();

	SetViewOffset( Vector( 0.0f, 0.0f, 64.0f ) );

	m_iThrowGrenadeCounter = 0;		 
	m_pPlayerClassInfo = nullptr;	
	m_pCurStateInfo = nullptr;

	m_lifeState = LIFE_DEAD;

	m_iClassVariant = 0;
	m_Unknown3728 = 0;
	m_iCurrentPlayerClassInfo = 0;		

	HandleCommand_SetClass( CLASS_RECON );

	m_iRank = RANK_PRIVATE;
	m_Unknown3532 = 0;
	m_fSprintNRG = 100.0f;
	m_fThermopticNRG = 1.0f;
	m_iVision = 0;
	m_Unknown3508 = 0.0f;
	m_iReinforceTimer = 0;
	m_iLives = -1;
	m_Unknown4380 = 0.0f;
	m_iSelectedLoadout = 0;
	m_fTurnSpeed = 1.0f;
	m_bIsVIP = false;
	m_Unknown3684 = false;
	m_Unknown3504 = false;

	m_Unknown3540 = 0;

	m_ViewVectors[ CLASS_NONE ] = CViewVectors(
		Vector( 0, 0, 64 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 72 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 36 ),
		Vector( 0, 0, 28 ),

		Vector( -10, -10, -10 ),
		Vector( 10, 10, 10 ),

		Vector( 0, 0, 14 )
		);

	m_ViewVectors[ CLASS_RECON ] = CViewVectors(
		Vector( 0, 0, 57 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 64 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 46 ),
		Vector( 0, 0, 41 ),

		Vector( -10, -10, -10 ),
		Vector( 10, 10, 10 ),

		Vector( 0, 0, 14 )
		);

	m_ViewVectors[ CLASS_ASSAULT ] = CViewVectors(
		Vector( 0, 0, 57.5 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 65 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 48 ),
		Vector( 0, 0, 41 ),

		Vector( -10, -10, -10 ),
		Vector( 10, 10, 10 ),

		Vector( 0, 0, 14 )
		);

	m_ViewVectors[ CLASS_SUPPORT ] = CViewVectors(
		Vector( 0, 0, 60 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 70 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 59 ),
		Vector( 0, 0, 50 ),

		Vector( -10, -10, -10 ),
		Vector( 10, 10, 10 ),

		Vector( 0, 0, 14 )
		);

	m_ViewVectors[ CLASS_OPERATOR ] = CViewVectors(
		Vector( 0, 0, 57.5 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 65 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 48 ),
		Vector( 0, 0, 41 ),

		Vector( -10, -10, -10 ),
		Vector( 10, 10, 10 ),

		Vector( 0, 0, 14 )
		);

	m_ViewVectors[ CLASS_VIP ] = CViewVectors(
		Vector( 0, 0, 64 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 72 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 36 ),
		Vector( 0, 0, 28 ),

		Vector( -10, -10, -10 ),
		Vector( 10, 10, 10 ),

		Vector( 0, 0, 14 )
		);
}


CNEOPlayer::~CNEOPlayer()
{
	m_PlayerAnimState->Release();
}


CNEOPlayer *CNEOPlayer::CreatePlayer( const char *className, edict_t *ed )
{
	CNEOPlayer::s_PlayerEdict = ed;
	return (CNEOPlayer*) CreateEntityByName( className );
}

int CNEOPlayer::ShouldTransmit( const CCheckTransmitInfo *pInfo )
{
	edict_t* pEdict = pInfo->m_pClientEnt;

	if ( !pEdict )
		pEdict = engine->PEntityOfEntIndex( 0 );

	if ( pEdict )
	{
		CNEOPlayer* pPlayer = (CNEOPlayer*) pEdict->GetIServerEntity()->GetBaseEntity();

		if ( pPlayer )
		{
			if ( pPlayer->GetTeamNumber() == GetTeamNumber() && pPlayer->m_iStar == m_iStar || pPlayer->m_bIsVIP )
				return FL_EDICT_ALWAYS;
		}

		if ( !IsDead() )
		{
			if ( pPlayer && pPlayer->Weapon_OwnsThisType( "weapon_ghost" ) )
			{
				CWeaponNEOBase* pWeapon = GetActiveNEOWeapon();

				if ( pWeapon && pWeapon->GetWeaponID() == WEAPON_GHOST )
				{
					if ( pPlayer->GetTeamNumber() != GetTeamNumber() && GetTeamNumber() > TEAM_SPECTATOR )
					{
						float fDistance = GetAbsOrigin().LengthSqr() - pPlayer->GetAbsOrigin().LengthSqr();

						if ( fDistance < GHOST_MAX_TRANSMIT_DISTANCE )
							return 8;
					}
				}
			}
		}
	}

	return BaseClass::ShouldTransmit( pInfo );
}

void CNEOPlayer::LeaveVehicle( const Vector &vecExitPoint, const QAngle &vecExitAngles )
{
	BaseClass::LeaveVehicle( vecExitPoint, vecExitAngles );

	//teleport physics shadow too
	Vector newPos = GetAbsOrigin();
	QAngle newAng = GetAbsAngles();

	Teleport( &newPos, &newAng, &vec3_origin );
}

void CNEOPlayer::PreThink( void )
{
	// Riding a vehicle?
	if ( IsInAVehicle() )
	{
		// make sure we update the client, check for timed damage and update suit even if we are in a vehicle
		UpdateClientData();
		CheckTimeBasedDamage();

		// Allow the suit to recharge when in the vehicle.
		CheckSuitUpdate();

		WaterMove();

		m_Unknown3564 = gpGlobals->curtime;
		return;
	}

	BaseClass::PreThink();	 
	State_PreThink();

	ResetMaxSpeed();

	ResetSprint();
	ResetSprintNRG();
	ResetThermoptics();
	ResetLean();
	ResetAim();

	if ( m_afButtonReleased & IN_TOSS )
		TossCurrentWeapon();

	UpdateVision();

	SetSomething( gpGlobals->curtime - m_Unknown3564 );

	if ( m_Local.m_bDucked )	   
		m_fTurnSpeed = 1.0f;
	else
		m_fTurnSpeed = 0.25f;

	m_Unknown3564 = gpGlobals->curtime;
	m_vecForce = vec3_origin;
}


void CNEOPlayer::PostThink()
{
	BaseClass::PostThink();

	QAngle angles = GetLocalAngles();
	angles[ PITCH ] = 0;
	SetLocalAngles( angles );

	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();

	m_PlayerAnimState->Update( m_angEyeAngles[ YAW ], m_angEyeAngles[ PITCH ] );

	if ( !g_fGameOver && IsAlive() )
	{
		if ( GetFlags() & FL_DUCKING )
			SetCollisionBounds( NEOPLAYER_VEC_DUCK_HULL_MIN( m_iClassType ), NEOPLAYER_VEC_DUCK_HULL_MAX( m_iClassType ) );
		else
			SetCollisionBounds( NEOPLAYER_VEC_HULL_MIN( m_iClassType ), NEOPLAYER_VEC_HULL_MAX( m_iClassType ) );
	}
}


void CNEOPlayer::Precache()
{
	PrecacheModel( NEO_VIP_PLAYER_MODEL );
	PrecacheModel( NEO_VIP_DEAD_PLAYER_MODEL );
	PrecacheModel( NEO_TERROR_PLAYER_MODEL );

	for ( int i = 0; i < 4; i++ ) // Class
	{
		for ( int j = 0; j < 3; j++ ) // Class variants
		{
			for ( int k = 0; k < 7; k++ ) // Giblets
			{
				PrecacheModel( GetPlayerModelString( TEAM_JINRAI, i, j, k ) );
				PrecacheModel( GetPlayerModelString( TEAM_NSF, i, j, k ) );
				PrecacheModel( GetGibletsString( i, j, k ) );
			}
		}
	}

	PrecacheModel( NEO_VIP_GIB1 );
	PrecacheModel( NEO_VIP_GIB1 );
	PrecacheModel( NEO_VIP_GIB2 );

	BaseClass::Precache();

	// precachemodel for materials ok then
	PrecacheModel( NEO_GLOW_MATERIAL );
	PrecacheModel( NEO_THERMAL_MATERIAL );
	PrecacheModel( NEO_VM_THERMAL_MATERIAL );
}

void CNEOPlayer::Spawn()
{
	SetModel( NEO_VIP_PLAYER_MODEL );
	//RemoveEffects( EF_NODRAW | EF_NOINTERP );
	SetMoveType( MOVETYPE_WALK );
	RemoveSolidFlags( FSOLID_NOT_SOLID );

	if ( m_hRagdoll )
	{
		UTIL_Remove( m_hRagdoll );
	}

	m_hRagdoll = NULL;	 
	
	BaseClass::Spawn();

	AddFlag( FL_ONGROUND );

	SetMaxSpeed( 250.0f );
	SetFOV( this, 75 );
	SetViewOffset( m_ViewVectors[ m_iClassType ].m_vView );
	
	State_Transition( m_iPlayerState );
}

void CNEOPlayer::InitialSpawn( void )
{
	BaseClass::InitialSpawn(); 
	State_Enter( STATE_INTRO );
}

void CNEOPlayer::TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator )
{
	if ( m_takedamage != DAMAGE_YES )
		return;

	CTakeDamageInfo dmgInfo( info );

	CNEOPlayer* pAttacker = (CNEOPlayer*) ToBasePlayer( info.GetAttacker() );

	if ( NEOGameRules()->PlayerRelationship( this, pAttacker ) == GR_TEAMMATE && !m_Unknown3684 )
	{
		CTakeDamageInfo tkDmgInfo( this, pAttacker, vec3_origin, GetAbsOrigin(), dmgInfo.GetDamage() * neo_ff_feedback.GetFloat(), 0 );
		TakeDamage( tkDmgInfo );
	}

	SetLastHitGroup( ptr->hitgroup );

	float fDamage = dmgInfo.GetDamage();
	bool bHeadshot = false;
	int iHitLocation = 0;

	dmgInfo.AddDamageType( NEOGameRules()->Damage_GetNoPhysicsForce() );

	switch ( ptr->hitgroup )
	{
		case HITGROUP_GENERIC:
			break;

		case HITGROUP_HEAD:
			bHeadshot = true;
			iHitLocation = 1;
			DevMsg( "HitLoc: Head\n" );
			break;

		case HITGROUP_CHEST:
			break;

		case HITGROUP_STOMACH:
			fDamage *= 0.75f;
			break;

		case HITGROUP_LEFTARM:
			iHitLocation = 5;
			fDamage *= 0.75f;
			break;

		case HITGROUP_RIGHTARM:
			iHitLocation = 4;
			fDamage *= 0.75f;
			break;

		case HITGROUP_LEFTLEG:
			iHitLocation = 3;
			fDamage *= 0.6f;
			break;

		case HITGROUP_RIGHTLEG:
			iHitLocation = 2;
			fDamage *= 0.6f;
			break;

		default:
			fDamage *= 0.6f;
	}

	if ( m_bIsVIP && ptr->hitgroup != HITGROUP_HEAD )
		fDamage *= 0.05f;

	dmgInfo.SetDamage( fDamage );
	dmgInfo.SetHeadshot( bHeadshot );
	dmgInfo.SetHitLocation( iHitLocation );

	AddMultiDamage( dmgInfo, this );

	pAttacker->BodyPartHit( ptr->hitgroup );
}

int CNEOPlayer::OnTakeDamage( const CTakeDamageInfo &info )
{
	CTakeDamageInfo dmgInfo = info;

	dmgInfo.ScaleDamageForce( 0.0625 );
	m_vForce += dmgInfo.GetDamageForce();

	if ( m_pPlayerClassInfo )
		dmgInfo.ScaleDamage( m_pPlayerClassInfo->fDamageScale );

	return BaseClass::OnTakeDamage( info );
}

void CNEOPlayer::Event_Killed( const CTakeDamageInfo &info )
{
	BaseClass::Event_Killed( info );

	if ( m_iLives != -1 && NEOGameRules()->m_iGameState != 1 )
		m_iLives = 0;

	CTakeDamageInfo dmgInfo = info;

	m_Unknown3708 = false;

	DevMsg( "Death Hitlocation %i\n", info.GetHitLocation() );

	float fDamage = dmgInfo.GetDamage();

	if ( fDamage + random->RandomFloat( 0.0f, 30.0f ) <= 50.0f )
		fDamage += random->RandomFloat( 0.0f, 1.0f ); // Not sure about this

	const char* desiredModel = nullptr;

	if ( dmgInfo.GetDamageType() & DMG_BLAST )
		desiredModel = GetPlayerModelString( GetTeamNumber(), m_iClassType, m_iClassVariant, 6 );
	else
		desiredModel = GetPlayerModelString( GetTeamNumber(), m_iClassType, m_iClassVariant, dmgInfo.GetHitLocation() );

	SetModel( desiredModel );

	if ( dmgInfo.GetHitLocation() > 0 || dmgInfo.GetDamageType() & DMG_BLAST )
	{
		if ( dmgInfo.GetHitLocation() == 6 )
		{
			UTIL_BloodSpray( dmgInfo.GetDamagePosition(), dmgInfo.GetDamageForce(), BLOOD_COLOR_RED, 10, FX_BLOODSPRAY_ALL ); // blood color prob is wrong

			for ( int i = 1; i <= 5; i++ )
				CGib::SpawnSpecificGibs( this, 1, 10.0f, 1000.0f, GetGibletsString( m_iClassType, m_iClassVariant, i ) );
		}
		else
		{
			UTIL_BloodSpray( dmgInfo.GetDamagePosition(), dmgInfo.GetDamageForce(), BLOOD_COLOR_RED, 10, FX_BLOODSPRAY_GORE | FX_BLOODSPRAY_DROPS ); // blood color prob is wrong
			CGib::SpawnSpecificGibs( this, 1, 10.0f, 1000.0f, GetGibletsString( m_iClassType, m_iClassVariant, dmgInfo.GetHitLocation() ) );
		}
	}

	if ( m_bIsVIP )
	{
		SetModel( NEO_VIP_DEAD_PLAYER_MODEL );

		CGib::SpawnSpecificGibs( this, 1, 10.0f, 1000.0f, "models/nt/vip_gib01.mdl" );
		CGib::SpawnSpecificGibs( this, 1, 10.0f, 1000.0f, "models/nt/vip_gib02.mdl" );
		CGib::SpawnSpecificGibs( this, 1, 10.0f, 1000.0f, "models/nt/vip_gib03.mdl" );

		CNEOPlayer* pAttacker = (CNEOPlayer*) info.GetAttacker();

		if ( pAttacker && pAttacker->IsPlayer() )
		{
			if ( pAttacker->GetTeamNumber() != NEOGameRules()->m_iAttackingTeam )
			{
				int edictIndex = engine->IndexOfEdict( pAttacker->edict() );
				pAttacker->IncrementFragCount( 2 );

				NEOGameRules()->m_iMVP = edictIndex;
				NEOGameRules()->m_Unknown624 = true;
			}		
			else
			{
				UTIL_ClientPrintAll( HUD_PRINTCENTER, "%s1 fucked up!", pAttacker->GetPlayerName() );
				m_iRank -= 1;

				int edictIndex = engine->IndexOfEdict( pAttacker->edict() );

				NEOGameRules()->m_iMVP = edictIndex;
				NEOGameRules()->m_Unknown624 = true;
			}
		}  
	}

	CreateRagdollEntity();
	State_Transition( STATE_PLAYDEATH );

	color32 nothing = { 0, 0, 0, 255 };
	UTIL_ScreenFade( this, nothing, 0, 0, FFADE_OUT | FFADE_STAYOUT );
	
	m_bWasKilled = true;
	m_fTimeKilled = gpGlobals->curtime;	
}

void CNEOPlayer::CreateRagdollEntity()
{
	// If we already have a ragdoll, don't make another one.
	CNEORagdoll *pRagdoll = dynamic_cast<CNEORagdoll*>(m_hRagdoll.Get());

	if ( pRagdoll )
	{
		UTIL_RemoveImmediate( pRagdoll );
		m_hRagdoll = nullptr;
	}

	if ( !pRagdoll )
	{
		// create a new one
		pRagdoll = dynamic_cast<CNEORagdoll*>(CreateEntityByName( "neo_ragdoll" ));
	}

	if ( pRagdoll )
	{
		pRagdoll->m_hPlayer = this;
		pRagdoll->m_vecRagdollOrigin = GetAbsOrigin();
		pRagdoll->m_vecRagdollVelocity = GetAbsVelocity();
		pRagdoll->m_nModelIndex = m_nModelIndex;
		pRagdoll->m_nForceBone = m_nForceBone;
		pRagdoll->m_vecForce = m_vecForce;
	}

	// ragdolls will be removed on round restart automatically
	m_hRagdoll = pRagdoll;
}

void CNEOPlayer::Touch( CBaseEntity *pOther )
{
	if ( pOther == GetGroundEntity() )
		return;

	if ( pOther->GetMoveType() != MOVETYPE_VPHYSICS || pOther->GetSolid() != SOLID_VPHYSICS )
		return;

	IPhysicsObject *pPhys = pOther->VPhysicsGetObject();
	if ( !pPhys || !pPhys->IsMoveable() )
		return;

	SetTouchedPhysics( true );
}

void CNEOPlayer::UpdateOnRemove()
{
	CNEORagdoll* pRagdoll = (CNEORagdoll*) m_hRagdoll.Get();

	if ( pRagdoll )
	{	
		UTIL_RemoveImmediate( pRagdoll );
		m_hRagdoll = nullptr;
	}

	BaseClass::UpdateOnRemove();
}

Vector CNEOPlayer::EyePosition()
{
	Vector vEyePos = BaseClass::EyePosition();

	Vector vRight;
	GetVectors( nullptr, &vRight, nullptr );

	switch ( m_Unknown3540 )
	{
		case 1:
			vEyePos.x -= (-7.5f * m_Unknown3544 + 7.5f) * vRight.x;
			vEyePos.y -= (-7.5f * m_Unknown3544 + 7.5f) * vRight.y;
			break;

		case 2:
			vEyePos.x -= vRight.x * 15.0f * 0.5f;
			vEyePos.y -= vRight.y * 15.0f * 0.5f;
			break;

		case 3:
			vEyePos.x -= (-7.5f * m_Unknown3544) * vRight.x;
			vEyePos.y -= (-7.5f * m_Unknown3544) * vRight.y;
			break;

		case 4:
			vEyePos.x -= (-15.0f * m_Unknown3544 + 15.0f) * vRight.x;
			vEyePos.y -= (-15.0f * m_Unknown3544 + 15.0f) * vRight.y;
			break;

		case 5:
			vEyePos.x -= 15.0f * vRight.x;
			vEyePos.y -= 15.0f * vRight.y;
			break;

		case 6:
			vEyePos.x -= 15.0f * m_Unknown3544 * vRight.x;
			vEyePos.y -= 15.0f * m_Unknown3544 * vRight.y;
			break;
	}
			 
	return vEyePos;
}

void CNEOPlayer::DoMuzzleFlash()
{
	BaseClass::DoMuzzleFlash();

	m_iNMFlash++;

	if ( m_iNMFlash > 5 )
		m_iNMFlash = 0;
}

void CNEOPlayer::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	if ( event == PLAYERANIMEVENT_THROW_GRENADE )
	{
		// Grenade throwing has to synchronize exactly with the player's grenade weapon going away,
		// and events get delayed a bit, so we let CCSPlayerAnimState pickup the change to this
		// variable.
		m_iThrowGrenadeCounter = (m_iThrowGrenadeCounter + 1) % (1 << THROWGRENADE_COUNTER_BITS);
	}
	else
	{
		m_PlayerAnimState->DoAnimationEvent( event, nData );
		TE_PlayerAnimEvent( this, event, nData );	// Send to any clients who can see this guy.
	}
}

CWeaponNEOBase* CNEOPlayer::GetActiveNEOWeapon() const
{
	return dynamic_cast<CWeaponNEOBase*>(GetActiveWeapon());
}

CWeaponNEOBase*	CNEOPlayer::GetNEOWeapon( int i ) const
{
	return dynamic_cast<CWeaponNEOBase*>(GetWeapon(i));
}

void CNEOPlayer::CreateViewModel( int index )
{
	Assert( index >= 0 && index < MAX_VIEWMODELS );

	if ( GetViewModel( index ) )
		return;

	CNeoViewModel* vm = (CNeoViewModel*) CreateEntityByName( "neo_viewmodel" );
	if ( vm )
	{
		vm->SetAbsOrigin( GetAbsOrigin() );
		vm->SetOwner( this );
		vm->SetIndex( index );
		DispatchSpawn( vm );
		vm->FollowEntity( this, false );
		m_hViewModel.Set( index, vm );
	}
}

bool CNEOPlayer::WantsLagCompensationOnEntity( const CBasePlayer *pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits ) const
{
	// If this entity hasn't been transmitted to us and acked, then don't bother lag compensating it.
	if ( pEntityTransmitBits && !pEntityTransmitBits->Get( pPlayer->entindex() ) )
		return false;

	const Vector &vMyOrigin = GetAbsOrigin();
	const Vector &vHisOrigin = pPlayer->GetAbsOrigin();

	// get max distance player could have moved within max lag compensation time, 
	// multiply by 1.5 to to avoid "dead zones"  (sqrt(2) would be the exact value)
	float maxDistance = 1.5 * pPlayer->MaxSpeed();

	// If the player is within this distance, lag compensate them in case they're running past us.
	if ( vHisOrigin.DistTo( vMyOrigin ) < maxDistance )
		return true;

	// If their origin is not within a 45 degree cone in front of us, no need to lag compensate.
	Vector vForward;
	AngleVectors( pCmd->viewangles, &vForward );

	Vector vDiff = vHisOrigin - vMyOrigin;
	VectorNormalize( vDiff );

	float flCosAngle = 0.707107f;	// 45 degree angle
	if ( vForward.Dot( vDiff ) < flCosAngle )
		return false;

	return true;
}

void CNEOPlayer::PackDeadPlayerItems()
{
	Vector vForward;
	GetVectors( &vForward, nullptr, nullptr );

	Vector vTarget = WorldSpaceCenter() + vForward * 144.0f;
	Vector vVelocity = vForward * 10.0f;

	for ( int i = 0; i < WeaponCount(); i++ )
	{
		CWeaponNEOBase* pWeapon = GetNEOWeapon( i );

		if ( !pWeapon )
			continue;

		if ( pWeapon->IsGrenade() && pWeapon->IsPinPulled() )
			pWeapon->ExplodeDroppedGrenade();
		else if ( pWeapon->IsDropable() )
			Weapon_Drop( pWeapon, &vTarget, &vVelocity );
	}

	RemoveAllItems( true );
}

void CNEOPlayer::FlashlightTurnOn( void )
{
	if ( !IsAlive() )
		return;

	AddEffects( EF_DIMLIGHT );
	EmitSound( "HL2Player.FlashLightOn" );
	m_iVision = 1;
}

void CNEOPlayer::FlashlightTurnOff( void )
{
	RemoveEffects( EF_DIMLIGHT );
	m_iVision = 0;

	if ( IsAlive() )
		EmitSound( "HL2Player.FlashLightOff" );
}

int CNEOPlayer::FlashlightIsOn( void )
{
	return IsEffectActive( EF_DIMLIGHT );
}

void CNEOPlayer::CheatImpulseCommands( int iImpulse )
{
	switch ( iImpulse )
	{
		case 101:
		{
			if ( sv_cheats->GetBool() )
			{
				extern int gEvilImpulse101;
				gEvilImpulse101 = true;

				GiveAmmo( 250, AMMO_BULLETS );
				GiveAmmo( 250, AMMO_PRI );
				GiveAmmo( 250, AMMO_SEC );
				GiveAmmo( 250, AMMO_6_8MM );
				GiveAmmo( 250, AMMO_5_7MM );
				GiveAmmo( 250, AMMO_SMAC );
				GiveAmmo( 250, AMMO_11_5MM );
				GiveAmmo( 250, AMMO_8_MM );
				GiveAmmo( 250, AMMO_10G_SHELL );

				gEvilImpulse101 = false;
			}
		}
		break;

		default:
		{
			BaseClass::CheatImpulseCommands( iImpulse );
		}
	}
}


bool CNEOPlayer::ClientCommand( const CCommand &args )
{
	const char *pcmd = args[0];

	if ( FStrEq( pcmd, "jointeam" ) )
	{
		if ( args.ArgC() < 2 )
		{
			Warning( "Player sent bad jointeam syntax\n" );
		}

		if ( ShouldRunRateLimitedCommand( args ) )
		{
			HandleCommand_JoinTeam( atoi( args[ 1 ] ) );
		}
		return true;
	}

	if ( FStrEq( pcmd, "playerstate_reverse" ) )
	{
		if ( ShouldRunRateLimitedCommand( args ) )
		{
			if ( m_iPlayerState == STATE_PICKINGCLASS )
				State_Transition( STATE_PICKINGTEAM );
			else if ( m_iPlayerState == STATE_PICKINGLOADOUT )
				State_Transition( STATE_PICKINGCLASS );		
			return true;
		}
	}

	if ( FStrEq( pcmd, "playerready" ) )
	{
		if ( ShouldRunRateLimitedCommand( args ) )
		{
			UpdateTransition();
			return true;
		}
	}

	if ( FStrEq( pcmd, "setclass" ) )
	{
		if ( args.ArgC() < 2 )
		{
			Warning( "Player sent bad setclass syntax\n" );
		}

		if ( ShouldRunRateLimitedCommand( args ) )
		{
			HandleCommand_SetClass( atoi( args[ 1 ] ) );
		}
		return true;
	}

	if ( FStrEq( pcmd, "SetVariant" ) )
	{
		if ( args.ArgC() < 2 )
		{
			Warning( "Player sent bad SetVariant syntax\n" );
		}

		if ( ShouldRunRateLimitedCommand( args ) )
		{
			HandleCommand_SetVariant( atoi( args[ 1 ] ) );
			UpdateTransition();
		}
		return true;
	}

	if ( FStrEq( pcmd, "loadout" ) )
	{
		if ( args.ArgC() < 2 )
		{
			Warning( "Player sent bad loadout syntax\n" );
		}

		if ( ShouldRunRateLimitedCommand( args ) )
		{
			m_iSelectedLoadout = atoi( args[ 1 ] );
		}
		return true;
	}

	if ( FStrEq( pcmd, "joinstar" ) )
	{
		if ( args.ArgC() < 2 )
		{
			Warning( "Player sent bad joinstar syntax\n" );
		}

		if ( ShouldRunRateLimitedCommand( args ) )
		{
			HandleCommand_JoinStar( atoi( args[ 1 ] ) );
		}
		return true;
	}

	return BaseClass::ClientCommand( args );
}

bool CNEOPlayer::SelectSpawnSpot( const char *pEntClassName, CBaseEntity* &pSpot )
{
	// Find the next spawn spot.
	pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );

	if ( pSpot == NULL ) // skip over the null point
		pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );

	CBaseEntity *pFirstSpot = pSpot;
	do
	{
		if ( pSpot )
		{
			// check if pSpot is valid
			if ( g_pGameRules->IsSpawnPointValid( pSpot, this ) )
			{
				if ( pSpot->GetAbsOrigin() == Vector( 0, 0, 0 ) )
				{
					pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );
					continue;
				}

				// if so, go to pSpot
				return true;
			}
		}
		// increment pSpot
		pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );
	}
	while ( pSpot != pFirstSpot ); // loop if we're not back to the start

	DevMsg( "CCSPlayer::SelectSpawnSpot: couldn't find valid spawn point.\n" );

	return false;
}


CBaseEntity* CNEOPlayer::EntSelectSpawnPoint()
{
	CBaseEntity* pSpawnPoint = NEOGameRules()->GetSpawnPoint( this );

	if ( pSpawnPoint )
		return pSpawnPoint;

	const char* pSpawnpointName = "info_player_start";

	if ( GetTeamNumber() == TEAM_JINRAI )
	{
		pSpawnpointName = "info_player_jinrai";
	}
	else if ( GetTeamNumber() == TEAM_NSF )
	{
		pSpawnpointName = "info_player_nsf";
	}

	if ( SelectSpawnSpot( pSpawnpointName, pSpawnPoint ) )
		goto ReturnSpot;

	if ( !gpGlobals->startspot || !V_strlen( STRING( gpGlobals->startspot ) ) )
	{
		pSpawnPoint = gEntList.FindEntityByClassname( nullptr, "info_player_start" );
		if ( pSpawnPoint )
			goto ReturnSpot;
	}
	else
	{
		pSpawnPoint = gEntList.FindEntityByTarget( NULL, STRING( gpGlobals->startspot ) );
		if ( pSpawnPoint )
			goto ReturnSpot;
	}

ReturnSpot:
	if ( !pSpawnPoint )
	{
		Warning( "PutClientInServer: no info_player_start on level\n" );   
		return CBaseEntity::Instance( INDEXENT( 0 ) );
	}

	return pSpawnPoint;
}

bool CNEOPlayer::BumpWeapon( CBaseCombatWeapon *pWeapon )
{	
	if ( HasPhysicsFlag( PFLAG_OBSERVER ) || !IsAlive() || m_iLives < 1 )
		return false;

	CWeaponNEOBase* pNEOWeapon = dynamic_cast<CWeaponNEOBase*>(pWeapon);

	if ( !pNEOWeapon || !pNEOWeapon->CanClassUseThis( m_iClassType ) )
		return false;

	// Can I have this weapon type?
	if ( !IsAllowedToPickupWeapons() )
		return false;

	int iSlot = pNEOWeapon->GetSlot();

	if ( iSlot == 1 || iSlot == 2 )
	{
		for ( int i = 0; i < WeaponCount(); i++ )
		{
			CWeaponNEOBase* pMyWep = GetNEOWeapon( i );

			if ( !pMyWep || pMyWep->GetSlot() != iSlot )
				continue;

			if ( pNEOWeapon->GetPrimaryAmmoType() == pMyWep->GetPrimaryAmmoType() )
				Weapon_EquipAmmoOnly( pNEOWeapon );

			if ( !pNEOWeapon->UsesClipsForAmmo1() || !pNEOWeapon->HasPrimaryAmmo() )
				UTIL_Remove( pNEOWeapon );

			return false;
		}
	}

	return BaseClass::BumpWeapon( pWeapon );
}

bool CNEOPlayer::CanHearAndReadChatFrom( CBasePlayer *pPlayer )
{
	if ( !pPlayer )
		return true;

	if ( IsAlive() && pPlayer->GetTeamNumber() == TEAM_SPECTATOR )
		return false;

	if ( !pPlayer->IsAlive() )
		return false;

	return true;
}

void CNEOPlayer::InitVCollision( const Vector &vecAbsOrigin, const Vector &vecAbsVelocity )
{
	BaseClass::InitVCollision( vecAbsOrigin, vecAbsVelocity );

	if ( m_pPhysicsController )
		m_pPhysicsController->SetEventHandler( &playerCallback );
}

void CNEOPlayer::FixViewPunch( float viewPunch )
{
	if ( viewPunch == 0.0f || (viewPunch * 2) > (m_Local.m_vecPunchAngle->LengthSqr() + m_Local.m_vecPunchAngleVel->LengthSqr()) )
	{
		m_Local.m_vecPunchAngle * 0.0f;
		m_Local.m_vecPunchAngleVel * 0.0f;
	}
}

void CNEOPlayer::BodyPartHit( int iHitGroup )
{
	if ( iHitGroup > NEO_BODYPART_MAX )
	{
		Assert( 0 );
		return;
	}

	m_iBodyPartsHit[ iHitGroup ]++;
}

void CNEOPlayer::State_Transition( NEOPlayerState newState )
{
	State_Leave();
	State_Enter( newState );
}


void CNEOPlayer::State_Enter( NEOPlayerState newState )
{
	m_iPlayerState = newState;
	m_pCurStateInfo = State_LookupInfo( newState );

	if ( neo_ShowStateTransitions.GetInt() == -1 || neo_ShowStateTransitions.GetInt() == entindex() )
	{
		if ( m_pCurStateInfo )
			Msg( "ShowStateTransitions: entering '%s'\n", m_pCurStateInfo->m_pStateName );
		else
			Msg( "ShowStateTransitions: entering #%d\n", newState );
	}

	// Initialize the new state.
	if ( m_pCurStateInfo && m_pCurStateInfo->pfnEnterState )
		(this->*m_pCurStateInfo->pfnEnterState)();
}


void CNEOPlayer::State_Leave()
{
	if ( m_pCurStateInfo && m_pCurStateInfo->pfnLeaveState )
	{
		(this->*m_pCurStateInfo->pfnLeaveState)();
	}
}


void CNEOPlayer::State_PreThink()
{
	if ( m_pCurStateInfo && m_pCurStateInfo->pfnPreThink )
	{
		(this->*m_pCurStateInfo->pfnPreThink)();
	}
}


CNEOPlayerStateInfo* CNEOPlayer::State_LookupInfo( NEOPlayerState state )
{
	// This table MUST match the 
	static CNEOPlayerStateInfo playerStateInfos[] =
	{
		{ STATE_ACTIVE, "STATE_ACTIVE", &CNEOPlayer::State_Enter_ACTIVE, NULL, &CNEOPlayer::State_PreThink_ACTIVE },
		{ STATE_INTRO, "STATE_INTRO", &CNEOPlayer::State_Enter_INTRO, NULL, &CNEOPlayer::State_PreThink_INTRO },
		{ STATE_PICKINGTEAM, "STATE_PICKINGTEAM", &CNEOPlayer::State_Enter_PICKINGTEAM, NULL, &CNEOPlayer::State_PreThink_OBSERVERMODE },
		{ STATE_PICKINGCLASS, "STATE_PICKINGCLASS", &CNEOPlayer::State_Enter_PICKINGCLASS, NULL, &CNEOPlayer::State_PreThink_OBSERVERMODE },
		{ STATE_PICKINGLOADOUT, "STATE_PICKINGLOADOUT", &CNEOPlayer::State_Enter_PICKINGLOADOUT, NULL, &CNEOPlayer::State_PreThink_OBSERVERMODE },
		{ STATE_PLAYDEATH, "STATE_PLAYDEATH", &CNEOPlayer::State_Enter_PLAYDEATH, NULL, &CNEOPlayer::State_PreThink_PLAYDEATH },
		{ STATE_DEAD, "STATE_DEAD", &CNEOPlayer::State_Enter_DEAD, NULL, &CNEOPlayer::State_PreThink_DEAD },
		{ STATE_OBSERVERMODE, "STATE_OBSERVERMODE", &CNEOPlayer::State_Enter_OBSERVERMODE, NULL, &CNEOPlayer::State_PreThink_OBSERVERMODE }
	};

	for ( int i = 0; i < ARRAYSIZE( playerStateInfos ); i++ )
	{
		if ( playerStateInfos[ i ].m_iPlayerState == state )
			return &playerStateInfos[ i ];
	}

	return NULL;
}

void CNEOPlayer::State_Enter_ACTIVE()
{
	if ( m_bIsVIP )
	{
		if ( GetFlags() & FL_FAKECLIENT )
			m_Unknown3728 = bot_var.GetInt();

		m_iClassType = CLASS_RECON;
		UpdatePlayerClassInfo();
	}
	else
	{
		if ( GetFlags() & FL_FAKECLIENT )
		{
			m_iClassType = bot_class.GetInt() + 1;
			m_Unknown3728 = bot_var.GetInt();
		}

		UpdatePlayerClassInfo();
	}

	SetModel( GetPlayerModelString( GetTeamNumber(), m_iClassType, m_iClassVariant, 0 ) );
	SetMoveType( MOVETYPE_WALK );
	RemoveSolidFlags( FSOLID_NOT_SOLID );

	SetPhysicsFlag( PFLAG_OBSERVER, false );

	m_Local.m_iHideHUD = 0;

	//RemoveEffects( EF_NODRAW | EF_NOSHADOW | EF_NOINTERP );

	if ( GetTeamNumber() == TEAM_JINRAI || GetTeamNumber() == TEAM_NSF )
		GiveEquipment();

	m_iThermoptic = 0;
	m_fThermopticNRG = m_pPlayerClassInfo->fMaxThermopticNRG;
	m_iSprint = 0;
	m_fSprintNRG = 100.0f;

	AddFlag( FL_ONGROUND );

	m_nButtons = 0;
	m_afButtonPressed = 0;
	m_afButtonReleased = 0;
}


void CNEOPlayer::State_PreThink_ACTIVE()
{
	// We only allow noclip here only because noclip is useful for debugging.
	// It would be nice if the noclip command set some flag so we could tell that they
	// did it intentionally.
	if ( IsEFlagSet( EFL_NOCLIP_ACTIVE ) )
	{
		//		Assert( GetMoveType() == MOVETYPE_NOCLIP );
	}
	else
	{
		//		Assert( GetMoveType() == MOVETYPE_WALK );
	}

	Assert( !IsSolidFlagSet( FSOLID_NOT_SOLID ) );
}

void CNEOPlayer::State_Enter_INTRO()
{
	SetMoveType( MOVETYPE_NONE );
	SetSolidFlags( FSOLID_NOT_SOLID );

	m_iLives = 1;

	if ( m_lifeState != LIFE_RESPAWNABLE )
		m_lifeState = LIFE_RESPAWNABLE;

	m_Unknown4384 = gpGlobals->curtime;	  
}

void CNEOPlayer::State_PreThink_INTRO()
{
	// Verify some state.
	Assert( IsSolidFlagSet( FSOLID_NOT_SOLID ) );
	Assert( GetAbsVelocity().Length() == 0 );

	if ( GetFlags() & FL_FAKECLIENT )
	{
		m_Unknown3700 = false;
		State_Enter( STATE_PICKINGTEAM );
		return;
	}

	if ( !m_Unknown3700 )
		m_Unknown3700 = true;

	if ( gpGlobals->curtime >= m_Unknown4384 + 1.0f )
	{
		m_Unknown3700 = false;
		m_afButtonPressed &= ~(IN_ATTACK | IN_ATTACK2);
		State_Enter( STATE_PICKINGTEAM );
	}
}

void CNEOPlayer::State_Enter_PICKINGTEAM()
{
	if ( GetFlags() & FL_FAKECLIENT )
	{
		static int iCurrentStar = STAR_ALPHA;

		HandleCommand_JoinTeam( TEAM_UNASSIGNED );
		HandleCommand_JoinStar( iCurrentStar );

		iCurrentStar++;

		if ( iCurrentStar >= STAR_MAXCOUNT )
			iCurrentStar = STAR_ALPHA;	   

		return;
	}

	ShowViewPortPanel( PANEL_SCOREBOARD, false );
	ShowViewPortPanel( PANEL_TEAM ); // show the team menu
}	 

void CNEOPlayer::State_Enter_PICKINGCLASS()
{
	if ( GetFlags() & FL_FAKECLIENT )
	{
		m_iClassType = bot_class.GetInt() + 1;
		m_Unknown3728 = bot_var.GetInt();

		UpdatePlayerClassInfo();
		UpdateTransition();		
		return;
	}

	if ( GetTeamNumber() == TEAM_SPECTATOR )
	{
		Spawn();
		return;
	}

	ShowViewPortPanel( PANEL_SCOREBOARD, false );
	ShowViewPortPanel( PANEL_SPECGUI, false );
	ShowViewPortPanel( PANEL_SPECMENU, false );
	ShowViewPortPanel( PANEL_LOADOUT, false );
	ShowViewPortPanel( PANEL_CLASS );
}

void CNEOPlayer::State_Enter_PICKINGLOADOUT()
{
	if ( GetFlags() & FL_FAKECLIENT )
	{			 
		m_iSelectedLoadout = 0;
		UpdateTransition();	   
		return;
	}

	ShowViewPortPanel( PANEL_SCOREBOARD, false );
	ShowViewPortPanel( PANEL_SPECGUI, false );
	ShowViewPortPanel( PANEL_SPECMENU, false );
	ShowViewPortPanel( PANEL_CLASS, false );
	ShowViewPortPanel( PANEL_LOADOUT );
}

void CNEOPlayer::State_Enter_PLAYDEATH()
{
	FlashlightTurnOff();

	if ( HasWeapons() )
		PackDeadPlayerItems();

	m_lifeState = LIFE_DYING;
	m_flDeathTime = gpGlobals->curtime;

	StartObserverMode( OBS_MODE_ROAMING );
}

void CNEOPlayer::State_PreThink_PLAYDEATH()
{
	if ( m_bWasKilled && gpGlobals->curtime >= m_fTimeKilled + 10.0f )
	{
		m_bWasKilled = false;
		color32 nothing = { 0, 0, 0, 255 };
		UTIL_ScreenFade( this, nothing, 0, 0, FFADE_IN | FFADE_PURGE );
	}

	if ( GetFlags() & FL_ONGROUND )
	{
		float flForward = GetAbsVelocity().Length() - 20;
		if ( flForward <= 0 )
		{
			SetAbsVelocity( vec3_origin );
		}
		else
		{
			Vector vAbsVel = GetAbsVelocity();
			VectorNormalize( vAbsVel );
			vAbsVel *= flForward;
			SetAbsVelocity( vAbsVel );
		}
	}

	if ( m_Unknown3704 <= gpGlobals->curtime )
		State_Transition( STATE_DEAD );
}

void CNEOPlayer::State_Enter_DEAD()
{
	m_lifeState = LIFE_DEAD;
	m_flPlaybackRate = 0.0f;
	//AddEffects( EF_NOINTERP );
	//AddEffects( EF_NODRAW | EF_NOINTERP );
}

void CNEOPlayer::State_PreThink_DEAD()
{
	if ( GetMoveType() > 0 && GetFlags() & FL_ONGROUND )
		SetMoveType( MOVETYPE_NONE );

	if ( m_Unknown3704 + 15.0f >= gpGlobals->curtime || HasPhysicsFlag( PFLAG_OBSERVER ) )
	{	   
		if ( m_lifeState == LIFE_DEAD )
		{
			if ( m_iLives > 0 )
			{
				if ( NEOGameRules()->IsRoundInProgress() )
				{		
					ClientPrint( this, HUD_PRINTCENTER, "-ROUND ALREADY IN PROGRESS-" );
					State_Transition( STATE_OBSERVERMODE );
				}
				else
				{
					m_lifeState = LIFE_RESPAWNABLE;
					State_Transition( STATE_PICKINGCLASS );
				}
			}
			else
			{
				State_Transition( STATE_OBSERVERMODE );
			}
		}
		else
		{		
			if ( GetFlags() & FL_FAKECLIENT )
			{
				m_lifeState = LIFE_RESPAWNABLE;
				State_Transition( STATE_PICKINGCLASS );
			}
		}
	}
}

void CNEOPlayer::State_Enter_OBSERVERMODE()
{
	SetMoveType( MOVETYPE_OBSERVER );
	ResetMaxSpeed();

	if ( GetTeamNumber() == TEAM_SPECTATOR || m_iLives < 0 )
	{		
		ResetObserverMode();
		StartObserverMode( OBS_MODE_CHASE );
		SetPhysicsFlag( PFLAG_OBSERVER, true );

		ShowViewPortPanel( PANEL_SCOREBOARD, false );
		ShowViewPortPanel( PANEL_SPECGUI );
	}
}

void CNEOPlayer::State_PreThink_OBSERVERMODE()
{
	if ( GetTeamNumber() == TEAM_SPECTATOR )
	{
		// Make sure nobody has changed any of our state.
		//	Assert( GetMoveType() == MOVETYPE_FLY );
		Assert( m_takedamage == DAMAGE_NO );
		Assert( IsSolidFlagSet( FSOLID_NOT_SOLID ) );
		//	Assert( IsEffectActive( EF_NODRAW ) );

		// Must be dead.
		Assert( m_lifeState == LIFE_DEAD );
		Assert( pl.deadflag );
	}		
}


void CNEOPlayer::GiveEquipment()
{
	if ( HasPhysicsFlag( PFLAG_OBSERVER ) || IsDead() || m_iLives < 1 )
		return;

	if ( m_iClassType != CLASS_VIP )
		GiveNamedItem( GetLoadoutWeaponString( GetTeamNumber(), m_iClassType, m_iRank, m_iSelectedLoadout ) );

	if ( m_iClassType == CLASS_RECON )
	{
		GiveNamedItem( "weapon_milso" );
		GiveNamedItem( "weapon_knife" );

		if ( m_iRank >= RANK_CORPORAL )
			GiveNamedItem( "weapon_remotedet" );
	}
	else if ( m_iClassType == CLASS_ASSAULT )
	{
		GiveNamedItem( "weapon_tachi" );
		GiveNamedItem( "weapon_knife" );
		GiveNamedItem( "weapon_grenade" );
	}
	else if ( m_iClassType == CLASS_SUPPORT )
	{
		GiveNamedItem( "weapon_kyla" );
		GiveNamedItem( "weapon_smokegrenade" );
	}
	else if ( m_iClassType == CLASS_VIP )
	{
		GiveNamedItem( "weapon_smac" );
	}

	GiveAmmo( 13, AMMO_BULLETS );
	GiveAmmo( 45, AMMO_SEC );
	GiveAmmo( GetLoadoutAmmoAmount( GetTeamNumber(), m_iClassType, m_iRank, m_iSelectedLoadout ),
		GetLoadoutAmmoType( GetTeamNumber(), m_iClassType, m_iRank, m_iSelectedLoadout ) );
}

bool CNEOPlayer::HandleCommand_JoinTeam( int team )
{
	if ( (GetTeamNumber() == TEAM_JINRAI || GetTeamNumber() == TEAM_NSF) && !IsDead() )
	{
		ClientPrint( this, HUD_PRINTCENTER, "-YOU CAN'T SWITCH TEAMS WHILE YOU ARE ALIVE-" );
		return false;
	}

	int availableTeam = TEAM_UNASSIGNED;

	switch ( team )
	{
		case TEAM_UNASSIGNED:
			availableTeam = NEOGameRules()->SelectDefaultTeam();

			if ( availableTeam == TEAM_UNASSIGNED )
			{
				ClientPrint( this, HUD_PRINTCENTER, "#All_Teams_Full" );
				return false;
			}

			break;

		case TEAM_SPECTATOR:
			if ( GetTeamNumber() == TEAM_SPECTATOR )
			{
				ClientPrint( this, HUD_PRINTCENTER, "#Already_Is_Spectator" );
				return true;
			}

			if ( GetTeamNumber() && IsAlive() )
			{
				CommitSuicide();
				IncrementDeathCount( 1 );
			}

			HandleCommand_JoinStar( STAR_NONE );
			RemoveAllItems( true );
			ChangeTeam( TEAM_SPECTATOR );

			break;

		case TEAM_JINRAI:
			if ( GetTeamNumber() == TEAM_JINRAI )
				return false;	  
			break;

		case TEAM_NSF:
			if ( GetTeamNumber() == TEAM_NSF )
				return false;
			break;

		default:
			return false;
	}

	if ( NEOGameRules()->TeamFull( team ) )
	{
		ClientPrint( this, HUD_PRINTCENTER, "TEAM FULL - AUTO ASSIGNING..." );

		team = NEOGameRules()->SelectDefaultTeam();

		if ( team == TEAM_UNASSIGNED )
			return false;
	}

	if ( GetTeamNumber() == TEAM_SPECTATOR && team != TEAM_SPECTATOR )
	{
		HandleCommand_JoinStar( STAR_NONE );

		StopObserverMode();

		AddSolidFlags( FSOLID_NOT_SOLID );
		//AddEffects( EF_NODRAW | EF_NOINTERP );
		SetMoveType( MOVETYPE_NOCLIP );

		m_takedamage = DAMAGE_NO;
		m_lifeState = LIFE_DEAD;

		SetAbsVelocity( vec3_origin );
		SetPunchAngle( vec3_angle );

		Spawn();
	}

	if ( m_iPlayerState != STATE_ACTIVE )
	{
		ChangeTeam( team );
		HandleCommand_JoinStar( STAR_NONE );

		if ( NEOGameRules()->IsRoundInProgress() )
		{
			ClientPrint( this, HUD_PRINTCENTER, "-ROUND ALREADY IN PROGRESS-" );

			m_iLives = 0;
			m_lifeState = LIFE_DEAD;

			SetObserverMode( OBS_MODE_CHASE );
			State_Transition( STATE_OBSERVERMODE );
			Spawn();
		}
		else
		{
			State_Transition( STATE_PICKINGCLASS );
		}
		
		return true;
	}
	else
	{
		IncrementDeathCount( 1 );
		ChangeTeam( TEAM_SPECTATOR );
		HandleCommand_JoinStar( STAR_NONE );
		State_Transition( STATE_PLAYDEATH );
	}

	return false;
}

void CNEOPlayer::HandleCommand_JoinStar( int iStar )
{
	if ( iStar < STAR_NONE || iStar >= STAR_MAXCOUNT )
		iStar = STAR_NONE;

	if ( iStar == m_iStar )
		return;

	if ( iStar == STAR_NONE )
	{
		m_iStar = iStar;
		return;
	}

	if ( gpGlobals->maxClients >= 1 )
	{
		int iSquadSize = 0;

		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CNEOPlayer* pPlayer = (CNEOPlayer*) UTIL_PlayerByIndex( i );

			if ( !pPlayer || !pPlayer->IsPlayer() || pPlayer->GetTeamNumber() != GetTeamNumber() || pPlayer->m_iStar != iStar )
				continue;

			iSquadSize++;
		}

		if ( iSquadSize > NEO_MAX_SQUAD_SIZE )
		{
			ClientPrint( this, HUD_PRINTCENTER, "SQUAD FULL" );
			return;
		}
	}

	m_iStar = iStar;
}

bool CNEOPlayer::HandleCommand_SetClass( int iClass, bool bForce )
{
	if ( GetFlags() & FL_FAKECLIENT )
	{
		if ( !m_bIsVIP )
			iClass = bot_class.GetInt() + 1;

		m_Unknown3728 = bot_var.GetInt();
	}

	if ( iClass <= CLASS_NONE || iClass >= CLASS_MAXCOUNT )
		iClass = CLASS_RECON;

	if ( m_iPlayerState == STATE_PICKINGCLASS || bForce )
	{
		if ( m_iClassType == iClass )
			return true;

		if ( bForce || ((iClass == CLASS_RECON) || (iClass == CLASS_ASSAULT) || (iClass == CLASS_SUPPORT)) )
		{
			m_iClassType = iClass;		
			UpdatePlayerClassInfo();
			return m_pPlayerClassInfo != nullptr;
		}
	}

	return true;
}

bool CNEOPlayer::HandleCommand_SetVariant( int iVariant )
{
	if ( iVariant >= CLASSVARIANT_MAXCOUNT || State_Get() == STATE_ACTIVE )
		return false;

	m_Unknown3728 = m_iClassVariant = iVariant;

	return true;
}

void CNEOPlayer::PickLoadout()
{
	RemoveAllItems( false );

	m_iVision = 0;
	m_lifeState = LIFE_RESPAWNABLE;

	ResetObserverMode();
	SetObserverMode( OBS_MODE_CHASE );

	State_Transition( STATE_PICKINGLOADOUT );
}

void CNEOPlayer::FreeWeaponSlot()
{
	Vector vForward;
	GetVectors( &vForward, nullptr, nullptr );

	Vector vTarget = WorldSpaceCenter() + vForward * 144.0f;
	Vector vVelocity = vForward * 10.0f;

	for ( int i = 0; i < MAX_WEAPONS; i++ )
	{
		CWeaponNEOBase* pWeapon = dynamic_cast<CWeaponNEOBase*>(m_hMyWeapons[ i ].Get());

		if ( pWeapon && !pWeapon->AllowsAutoSwitchFrom() )
			Weapon_Drop( pWeapon, &vTarget, &vVelocity );
	}
}

void CNEOPlayer::GetIntoGame()
{
	StopObserverMode();

	ShowViewPortPanel( PANEL_SCOREBOARD, false );
	ShowViewPortPanel( PANEL_SPECGUI, false );
	ShowViewPortPanel( PANEL_SPECMENU, false );
	ShowViewPortPanel( PANEL_LOADOUT, false );
	ShowViewPortPanel( PANEL_LOADOUT_DEV, false );
	ShowViewPortPanel( PANEL_CLASS, false );

	m_lifeState = LIFE_ALIVE;
	Spawn();

	State_Enter( STATE_ACTIVE );
}

void CNEOPlayer::UpdatePlayerClassInfo()
{
	if ( m_iCurrentPlayerClassInfo == m_iClassType )
		return;

	delete m_pPlayerClassInfo;

	switch ( m_iClassType )
	{
		case CLASS_UNASSIGNED:
			m_pPlayerClassInfo = new CDefaultClassInfo();
			break;
		case CLASS_RECON:
			m_pPlayerClassInfo = new CReconClassInfo();
			break;
		case CLASS_ASSAULT:
			m_pPlayerClassInfo = new CAssaultClassInfo();
			break;
		case CLASS_SUPPORT:
			m_pPlayerClassInfo = new CSupportClassInfo();
			break;
		case CLASS_OPERATOR:
			m_pPlayerClassInfo = new COperatorClassInfo();
			break;
		case CLASS_VIP:
			m_pPlayerClassInfo = new CVipClassInfo();
			break;
	}

	m_iCurrentPlayerClassInfo = m_iClassType;
}

void CNEOPlayer::ResetSprint()
{
	if ( !(m_afButtonPressed & IN_WALK) )
		return;

	if ( m_Unknown3536 )
	{
		m_Unknown3536 = false;
		return;
	}

	m_Unknown3536 = true;

	if ( m_iSprint == 1 )
		m_iSprint = 0;
}

void CNEOPlayer::ResetSprintNRG()
{	   
	if ( !m_pPlayerClassInfo || !m_pPlayerClassInfo->Unknown32 )
	{
		m_iSprint = 0;
		return;
	}

	if ( m_afButtonReleased & IN_SPRINT )
		m_iSprint = 0;
	else if ( m_afButtonPressed & IN_SPRINT )
		m_iSprint = 1;

	float fVelocity = GetLocalVelocity().Length2D();

	if ( fVelocity < 2.0f && fVelocity > -2.0f )
		m_iSprint = 0;

	if ( m_iSprint == 0 || fVelocity <= 1.0f )
	{
		if ( m_iClassType == CLASS_RECON )
			m_fSprintNRG += (gpGlobals->curtime - m_Unknown3564) * 5.0f * 0.5f;
		else
			m_fSprintNRG += (gpGlobals->curtime - m_Unknown3564) * 5.0f;		

		if ( m_fSprintNRG > 100.0f )
			m_fSprintNRG = 100.0f;
	}
	else
	{
		if ( m_iClassType == CLASS_RECON )
			m_fSprintNRG -= (gpGlobals->curtime - m_Unknown3564) * 5.0f * 0.5f;	
		else
			m_fSprintNRG -= (gpGlobals->curtime - m_Unknown3564) * 5.0f;
		
		if ( m_fSprintNRG > 100.0f )
			m_fSprintNRG = 100.0f;

		if ( m_fSprintNRG <= 0.0f )
		{
			m_fSprintNRG = 0.0f;
			m_iSprint = 0;
		}
	}

	if ( Weapon_OwnsThisType( "weapon_ghost" ) )
		m_iSprint = 0;
}

void CNEOPlayer::ResetThermoptics()
{				  
	if ( !m_pPlayerClassInfo || !m_pPlayerClassInfo->Unknown20 )
		return;

	if ( !(m_afButtonReleased & IN_THERMOPTIC) && m_afButtonPressed & IN_THERMOPTIC )
		m_iThermoptic = !m_iThermoptic;

	if ( m_iThermoptic == 1 )
	{
		m_fThermopticNRG = gpGlobals->curtime - m_Unknown3564;

		if ( m_fThermopticNRG <= 0.0f )
		{
			m_iThermoptic = 0;
			m_fThermopticNRG = 0.0f;
			EmitSound( "NeoPlayer.ThermOpticOff" );
		}
	}
	else
	{
		m_fThermopticNRG = (gpGlobals->curtime - m_Unknown3564) * m_pPlayerClassInfo->Unknown28;

		if ( m_fThermopticNRG > m_pPlayerClassInfo->fMaxThermopticNRG )
			m_fThermopticNRG = m_pPlayerClassInfo->fMaxThermopticNRG;
	}
}

void CNEOPlayer::ResetLean()
{
	if ( !GetGroundEntity() || m_iSprint == 1 )
	{
		m_iLean = LEAN_NONE;
	}
	else
	{
		if ( m_afButtonReleased & IN_LEANL )
			m_iLean = LEAN_NONE;
		else if ( m_afButtonPressed & IN_LEANL )
			m_iLean = LEAN_LEFT;
		else if ( m_afButtonReleased & IN_LEANR )
			m_iLean = LEAN_NONE;
		else if ( m_afButtonPressed & IN_LEANR )
			m_iLean = LEAN_RIGHT;
	}

	if ( m_Unknown3540 == 0 )
	{
		if ( m_iLean != LEAN_NONE )
		{
			if ( m_iLean == LEAN_LEFT )
				m_Unknown3540 = 1;
			else if ( m_iLean == LEAN_RIGHT )
				m_Unknown3540 = 4;

			m_Unknown3548 = gpGlobals->curtime + 0.15f;
			m_Unknown3544 = (m_Unknown3548 - gpGlobals->curtime) / 0.15f;
		}
	}
	else if ( m_Unknown3540 == 1 )
	{
		if ( m_iLean != LEAN_LEFT )
		{
			m_Unknown3540 = 3;
			m_Unknown3548 = 0.15f - (m_Unknown3548 - gpGlobals->curtime) + gpGlobals->curtime;
			m_Unknown3544 = (m_Unknown3548 - gpGlobals->curtime) / 0.15f;
		}
		else if ( m_Unknown3548 <= gpGlobals->curtime )
		{
			m_Unknown3540 = 2;
			m_Unknown3544 = 1.0f;
		}
		else			
		{ 
			m_Unknown3544 = (m_Unknown3548 - gpGlobals->curtime) / 0.15f;
		}
	}
	else if ( m_Unknown3540 == 2 )
	{
		if ( m_iLean != LEAN_LEFT )
		{
			m_Unknown3540 = 3;
			m_Unknown3548 = 0.15f + gpGlobals->curtime;
			m_Unknown3544 = 1.0f;
		}
	}
	else if ( m_Unknown3540 == 3 )
	{
		if ( m_iLean == LEAN_LEFT )
		{
			m_Unknown3540 = 1;
			m_Unknown3548 = 0.15f - (m_Unknown3548 - gpGlobals->curtime) + gpGlobals->curtime;
			m_Unknown3544 = (m_Unknown3548 - gpGlobals->curtime) / 0.15f;
		}
		else if ( m_Unknown3548 <= gpGlobals->curtime )
		{
			m_Unknown3540 = 0;
			m_Unknown3544 = 0.0f;
		}
		else
		{	  
			m_Unknown3544 = (m_Unknown3548 - gpGlobals->curtime) / 0.15f;
		}
	}
	else if ( m_Unknown3540 == 4 )
	{
		if ( m_iLean != LEAN_RIGHT )
		{
			m_Unknown3540 = 6;
			m_Unknown3548 = 0.15f - (m_Unknown3548 - gpGlobals->curtime) + gpGlobals->curtime;
			m_Unknown3544 = (m_Unknown3548 - gpGlobals->curtime) / 0.15f;
		}
		else if ( m_Unknown3548 <= gpGlobals->curtime )
		{
			m_Unknown3540 = 5;
			m_Unknown3544 = 1.0f;
		}
		else
		{ 
			m_Unknown3544 = (m_Unknown3548 - gpGlobals->curtime) / 0.15f;
		}
	}
	else if ( m_Unknown3540 == 5 )
	{
		if ( m_iLean != LEAN_RIGHT )
		{
			m_Unknown3540 = 6;
			m_Unknown3548 = 0.15f + gpGlobals->curtime;
			m_Unknown3544 = 1.0f;
		}
	}
	else if ( m_Unknown3540 == 6 )
	{
		if ( m_iLean == LEAN_RIGHT )
		{
			m_Unknown3540 = 1;
			m_Unknown3548 = 0.15f - (m_Unknown3548 - gpGlobals->curtime) + gpGlobals->curtime;
			m_Unknown3544 = (m_Unknown3548 - gpGlobals->curtime) / 0.15f;
		}
		else if ( m_Unknown3548 <= gpGlobals->curtime )
		{
			m_Unknown3540 = 0;
			m_Unknown3544 = 0.0f;
		}
		else
		{ 
			m_Unknown3544 = (m_Unknown3548 - gpGlobals->curtime) / 0.15f;
		}
	}

	float newCtrlValue = 0.0f;

	if ( m_Unknown3540 == 1 )
		newCtrlValue = m_Unknown3544 * (0.0f - -35.0f) - 35.0f;
	else if ( m_Unknown3540 == 2 )
		newCtrlValue = -35.0f;
	else if ( m_Unknown3540 == 3 )
		newCtrlValue = m_Unknown3544 * (-35.0f - 0.0f) + 0.0f;
	else if ( m_Unknown3540 == 4 )
		newCtrlValue = m_Unknown3544 * (0.0f - 35.0f) + 35.0f;
	else if ( m_Unknown3540 == 5 )
		newCtrlValue = 35.0f;
	else if ( m_Unknown3540 == 6 )
		newCtrlValue = m_Unknown3544 * (35.0f - 0.0f) + 0.0f;

	Assert( !IS_NAN( newCtrlValue ) );
	Assert( newCtrlValue <= 35.0f );
	Assert( newCtrlValue >= -35.0f );

	SetBoneController( 0, newCtrlValue );
}

void CNEOPlayer::ResetAim()
{
	CWeaponNEOBase* pWeapon = GetActiveNEOWeapon();

	if ( !pWeapon || !pWeapon->CanAim() )
		return;

	if ( m_afButtonReleased & IN_AIM )
		pWeapon->bAimed = !pWeapon->bAimed;

	if ( m_iSprint == 1 )
		pWeapon->bAimed = false;
}

void CNEOPlayer::TossCurrentWeapon()
{
	CWeaponNEOBase* pWeapon = GetActiveNEOWeapon();

	if ( !pWeapon || !pWeapon->IsTossable() )
		return;

	Vector vForward;
	GetVectors( &vForward, nullptr, nullptr );

	Vector vTarget = WorldSpaceCenter() + vForward * 144.0f;
	Vector vVelocity = vForward * 10.0f;

	Weapon_Drop( pWeapon, &vTarget, &vVelocity );
}

void CNEOPlayer::UpdateVision()
{
	if ( IsAlive() && m_pPlayerClassInfo && m_afButtonPressed & IN_VISION )
	{
		if ( m_pPlayerClassInfo->idk( 1 ) )
		{
			if ( FlashlightIsOn() )	 
				FlashlightTurnOff();
			else
				FlashlightTurnOn();
		}
		else
		{	   
			m_iVision = 0;

			while ( !m_pPlayerClassInfo->idk( m_iVision ) )
			{
				m_iVision++;

				if ( m_iVision >= 5 )
				{
					m_iVision = 0;
					break;
				}
			}
		}
	}
}

void CNEOPlayer::SetSomething( float idk )
{
	m_Unknown4380 -= idk * 35.0f;

	if ( m_Unknown4380 < 0.0f )
		m_Unknown4380 = 0.0f;
}

bool CNEOPlayer::UpdateTransition()
{
	if ( m_iLives <= 0 ) 
		return false;

	if ( m_iPlayerState == STATE_PICKINGCLASS )
	{
		State_Transition( STATE_PICKINGLOADOUT );
		return true;
	}

	if ( m_iPlayerState != STATE_PICKINGLOADOUT )
		return false;

	State_Transition( STATE_OBSERVERMODE );
	m_Unknown3708 = true;
	m_lifeState = LIFE_RESPAWNABLE;

	return true;
}

void CNEOPlayer::ResetMaxSpeed()
{
	float fMaxSpeed = 200.0f;

	if ( HasPhysicsFlag( PFLAG_OBSERVER ) )
		fMaxSpeed = 900.0f;

	CWeaponNEOBase* pWeapon = GetActiveNEOWeapon();

	if ( pWeapon )
	{ 
		fMaxSpeed *= pWeapon->GetUnknown1452();

		if ( pWeapon->bAimed )
			fMaxSpeed *= 0.6f;
	}

	if ( m_pPlayerClassInfo )
	{
		fMaxSpeed *= m_pPlayerClassInfo->fMaxSpeed;

		if ( m_pPlayerClassInfo->Unknown32 )
		{
			if ( m_iSprint == 1 )
				fMaxSpeed *= m_pPlayerClassInfo->Unknown36;
		}
	}

	float v10 =  1.0f - m_Unknown4380 / 20.0f;

	if ( v10 < 0.09 )
		v10 = 0.09;

	fMaxSpeed *= v10;

	if ( m_Unknown3536 )
		fMaxSpeed *= 0.75f;

	if ( m_Local.m_bDucked )
		fMaxSpeed *= 0.75f;

	if ( fMaxSpeed < 55.0f )
		fMaxSpeed = 75.0f;

	SetMaxSpeed( fMaxSpeed );
}

bool CNEOPlayer::CanMove()
{
	return (GetObserverMode() == OBS_MODE_CHASE || !NEOGameRules()->IsInFreezePeriod());
}

