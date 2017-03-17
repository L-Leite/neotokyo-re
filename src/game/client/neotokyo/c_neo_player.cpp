//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "c_neo_player.h"
#include "weapon_neobase.h"
#include "c_basetempentity.h"
#include "dlight.h"
#include "iefx.h"
#include "beamdraw.h"
#include "prediction.h"
#include "iviewrender.h"
#include "ivieweffects.h"
#include "view.h"
#include "materialsystem\imaterialvar.h"
#include "neo_thermhandler.h"
#include "view_scene.h"

#if defined( CNEOPlayer )
	#undef CNEOPlayer
#endif			 



// -------------------------------------------------------------------------------- //
// Player animation event. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //

class C_TEPlayerAnimEvent : public C_BaseTempEntity
{
public:
	DECLARE_CLASS( C_TEPlayerAnimEvent, C_BaseTempEntity );
	DECLARE_CLIENTCLASS();

	virtual void PostDataUpdate( DataUpdateType_t updateType )
	{
		// Create the effect.
		C_NEOPlayer *pPlayer = dynamic_cast< C_NEOPlayer* >( m_hPlayer.Get() );
		if ( pPlayer && !pPlayer->IsDormant() )
		{
			pPlayer->DoAnimationEvent( (PlayerAnimEvent_t)m_iEvent.Get(), m_nData );
		}	
	}

public:
	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
	CNetworkVar( int, m_nData );
};

IMPLEMENT_CLIENTCLASS_EVENT( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent, CTEPlayerAnimEvent );

BEGIN_RECV_TABLE_NOBASE( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent )
	RecvPropEHandle( RECVINFO( m_hPlayer ) ),
	RecvPropInt( RECVINFO( m_iEvent ) ),
	RecvPropInt( RECVINFO( m_nData ) )
END_RECV_TABLE()

BEGIN_RECV_TABLE_NOBASE( C_NEOPlayer, DT_NEOLocalPlayerExclusive )
	RecvPropInt( RECVINFO( m_iShotsFired ) ),
	RecvPropFloat( RECVINFO( m_fSprintNRG ) ),
	RecvPropFloat( RECVINFO( m_fThermopticNRG ) ),
	RecvPropInt( RECVINFO( m_iReinforceTimer ) ),
	RecvPropInt( RECVINFO( m_iSprint ) ),
	RecvPropFloat( RECVINFO( m_fTurnSpeed ) ),
END_RECV_TABLE()


IMPLEMENT_CLIENTCLASS_DT( C_NEOPlayer, DT_NEOPlayer, CNEOPlayer )
	RecvPropDataTable( "neolocaldata", 0, 0, &REFERENCE_RECV_TABLE(DT_NEOLocalPlayerExclusive) ),
	RecvPropFloat( RECVINFO( m_angEyeAngles[0] ) ),
	RecvPropFloat( RECVINFO( m_angEyeAngles[1] ) ),
	RecvPropInt( RECVINFO( m_iThrowGrenadeCounter ) ),
	RecvPropEHandle( RECVINFO( m_hRagdoll ) ),
	RecvPropInt( RECVINFO( m_iClassType ) ),
	RecvPropInt( RECVINFO( m_iLives ) ),
	RecvPropInt( RECVINFO( m_iRank ) ),
	RecvPropInt( RECVINFO( m_iStar ) ),
	RecvPropInt( RECVINFO( m_iLean ) ),
	RecvPropInt( RECVINFO( m_iThermoptic ) ),
	RecvPropInt( RECVINFO( m_iNMFlash ) ),
	RecvPropInt( RECVINFO( m_iVision ) ),
	RecvPropBool( RECVINFO( m_bIsVIP ) ),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_NEOPlayer )
	DEFINE_PRED_FIELD( m_flCycle, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK ),
	DEFINE_PRED_FIELD( m_iShotsFired, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),   
END_PREDICTION_DATA()

class C_NEORagdoll : public C_BaseAnimatingOverlay
{
public:
	DECLARE_CLASS( C_NEORagdoll, C_BaseAnimatingOverlay );
	DECLARE_CLIENTCLASS();

	C_NEORagdoll();
	~C_NEORagdoll();

	virtual void OnDataChanged( DataUpdateType_t type );

	int GetPlayerEntIndex() const;
	IRagdoll* GetIRagdoll() const;

	void ImpactTrace( trace_t *pTrace, int iDamageType, const char *pCustomImpactName );

private:

	C_NEORagdoll( const C_NEORagdoll & ) {}

	void Interp_Copy( C_BaseAnimatingOverlay *pSourceEntity );

	void CreateRagdoll();


private:

	EHANDLE	m_hPlayer;
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );
};


IMPLEMENT_CLIENTCLASS_DT_NOBASE( C_NEORagdoll, DT_NEORagdoll, CNEORagdoll )
	RecvPropVector( RECVINFO(m_vecRagdollOrigin) ),
	RecvPropEHandle( RECVINFO( m_hPlayer ) ),
	RecvPropInt( RECVINFO( m_nModelIndex ) ),
	RecvPropInt( RECVINFO(m_nForceBone) ),
	RecvPropVector( RECVINFO(m_vecForce) ),
	RecvPropVector( RECVINFO( m_vecRagdollVelocity ) )
END_RECV_TABLE()


C_NEORagdoll::C_NEORagdoll()
{
}

C_NEORagdoll::~C_NEORagdoll()
{
	PhysCleanupFrictionSounds( this );
}

void C_NEORagdoll::Interp_Copy( C_BaseAnimatingOverlay *pSourceEntity )
{
	if ( !pSourceEntity )
		return;
	
	VarMapping_t *pSrc = pSourceEntity->GetVarMapping();
	VarMapping_t *pDest = GetVarMapping();
    	
	// Find all the VarMapEntry_t's that represent the same variable.
	for ( int i = 0; i < pDest->m_Entries.Count(); i++ )
	{
		VarMapEntry_t *pDestEntry = &pDest->m_Entries[i];
		for ( int j=0; j < pSrc->m_Entries.Count(); j++ )
		{
			VarMapEntry_t *pSrcEntry = &pSrc->m_Entries[j];
			if ( !Q_strcmp( pSrcEntry->watcher->GetDebugName(),
				pDestEntry->watcher->GetDebugName() ) )
			{
				pDestEntry->watcher->Copy( pSrcEntry->watcher );
				break;
			}
		}
	}
}

void C_NEORagdoll::ImpactTrace( trace_t *pTrace, int iDamageType, const char *pCustomImpactName )
{
	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();

	if( !pPhysicsObject )
		return;

	Vector dir = pTrace->endpos - pTrace->startpos;

	if ( iDamageType == DMG_BLAST )
	{
		dir *= 4000;  // adjust impact strenght

		// apply force at object mass center
		pPhysicsObject->ApplyForceCenter( dir );
	}
	else
	{
		Vector hitpos;  

		VectorMA( pTrace->startpos, pTrace->fraction, dir, hitpos );
		VectorNormalize( dir );

		dir *= 4000;  // adjust impact strenght

		// apply force where we hit it
		pPhysicsObject->ApplyForceOffset( dir, hitpos );	
	}

	m_pRagdoll->ResetRagdollSleepAfterTime();
}


void C_NEORagdoll::CreateRagdoll()
{
	// First, initialize all our data. If we have the player's entity on our client,
	// then we can make ourselves start out exactly where the player is.
	C_NEOPlayer *pPlayer = dynamic_cast< C_NEOPlayer* >( m_hPlayer.Get() );

	if ( pPlayer && !pPlayer->IsDormant() )
	{
		// move my current model instance to the ragdoll's so decals are preserved.
		pPlayer->SnatchModelInstance( this );

		VarMapping_t *varMap = GetVarMapping();

		// Copy all the interpolated vars from the player entity.
		// The entity uses the interpolated history to get bone velocity.
		bool bRemotePlayer = (pPlayer != C_BasePlayer::GetLocalPlayer());			
		if ( bRemotePlayer )
		{
			Interp_Copy( pPlayer );

			SetAbsAngles( pPlayer->GetRenderAngles() );
			GetRotationInterpolator().Reset();

			m_flAnimTime = pPlayer->m_flAnimTime;
			SetSequence( pPlayer->GetSequence() );
			m_flPlaybackRate = pPlayer->GetPlaybackRate();
		}
		else
		{
			// This is the local player, so set them in a default
			// pose and slam their velocity, angles and origin
			SetAbsOrigin( m_vecRagdollOrigin );

			SetAbsAngles( pPlayer->GetRenderAngles() );

			SetAbsVelocity( m_vecRagdollVelocity );

			int iSeq = LookupSequence( "walk_lower" );
			if ( iSeq == -1 )
			{
				Assert( false );	// missing walk_lower?
				iSeq = 0;
			}

			SetSequence( iSeq );	// walk_lower, basic pose
			SetCycle( 0.0 );

			Interp_Reset( varMap );
		}		
	}
	else
	{
		// overwrite network origin so later interpolation will
		// use this position
		SetNetworkOrigin( m_vecRagdollOrigin );

		SetAbsOrigin( m_vecRagdollOrigin );
		SetAbsVelocity( m_vecRagdollVelocity );

		Interp_Reset( GetVarMapping() );

	}

	SetModelIndex( m_nModelIndex );

	// Make us a ragdoll..
	m_nRenderFX = kRenderFxRagdoll;

	matrix3x4_t boneDelta0[MAXSTUDIOBONES];
	matrix3x4_t boneDelta1[MAXSTUDIOBONES];
	matrix3x4_t currentBones[MAXSTUDIOBONES];
	const float boneDt = 0.05f;

	if ( pPlayer && !pPlayer->IsDormant() )
	{
		pPlayer->GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
	}
	else
	{
		GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
	}

	InitAsClientRagdoll( boneDelta0, boneDelta1, currentBones, boneDt );
}


void C_NEORagdoll::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		CreateRagdoll();
	}
}

IRagdoll* C_NEORagdoll::GetIRagdoll() const
{
	return m_pRagdoll;
}

C_BaseAnimating * C_NEOPlayer::BecomeRagdollOnClient()
{
	// Let the C_CSRagdoll entity do this.
	// m_builtRagdoll = true;
	return NULL;
}


IRagdoll* C_NEOPlayer::GetRepresentativeRagdoll() const
{
	if ( m_hRagdoll.Get() )
	{
		C_NEORagdoll *pRagdoll = (C_NEORagdoll*)m_hRagdoll.Get();

		return pRagdoll->GetIRagdoll();
	}
	else
	{
		return NULL;
	}
}



C_NEOPlayer::C_NEOPlayer() : 
	m_iv_angEyeAngles( "C_NEOPlayer::m_iv_angEyeAngles" )
{
	m_PlayerAnimState = CreatePlayerAnimState( this, this, LEGANIM_9WAY, true );

	m_angEyeAngles.Init();
	AddVar( &m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR );

	m_fRagdollCreationTime = 0.0f;
	m_iLives = -1;
	m_fTurnSpeed = 0.0f;
	m_fOldTurnSpeed = 0.0f;
	m_pFlashlightBeam = nullptr;
	m_iReinforceTimer = 0;
	m_bIsOnDeathScreen = false;		  
	m_iThermoptic = m_iOldThermoptic = 0;
	m_bIsVIP = false;
	m_iRank = m_iOldRank = 0;

	m_Unknown4108 = gpGlobals->curtime;
	m_Unknown4132 = false;
	m_Unknown4164 = false;

	m_bAnyoneInCross = false;	  
	m_iIDEntIndex = 0;

	m_pPlayerClassInfo = new CDefaultClassInfo;
	m_iCurrentPlayerClassInfo = CLASS_UNASSIGNED;
}


C_NEOPlayer::~C_NEOPlayer()
{
	m_PlayerAnimState->Release();
}


C_NEOPlayer* C_NEOPlayer::GetLocalNEOPlayer()
{
	return ToNEOPlayer( C_BasePlayer::GetLocalPlayer() );
}

bool C_NEOPlayer::IsLocalNEOPlayer() const
{
	return (GetLocalNEOPlayer() == this);
}


const QAngle& C_NEOPlayer::GetRenderAngles()
{
	if ( IsRagdoll() )
	{
		return vec3_angle;
	}
	else
	{
		return m_PlayerAnimState->GetRenderAngles();
	}
}

int C_NEOPlayer::DrawModel( int flags )
{
	C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	int result = -1;

	if ( localPlayer )
	{
		if ( GetTeamNumber() == localPlayer->GetTeamNumber() )
			m_Unknown4132 = true;

		if ( m_iVision == VISION_THERMAL )
		{
			if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
				return BaseClass::DrawModel( flags );

			IMaterial* matThermal = g_pMaterialSystem->FindMaterial( "dev/thermal", TEXTURE_GROUP_OTHER );

			if ( IsErrorMaterial( matThermal ) )
			{
				DevMsg( "Fuck me...\n" );
				BaseClass::DrawModel( flags );
			}

			bool found = false;
			IMaterialVar* matVar = matThermal->FindVar( "$eyevec", &found );

			if ( found )
			{
				Vector forward;
				GetVectors( &forward, nullptr, nullptr );
				matVar->SetVecValue( forward.x, forward.y, forward.z, 3.f );
			}

			modelrender->ForcedMaterialOverride( matThermal );
			result = BaseClass::InternalDrawModel( flags );
			modelrender->ForcedMaterialOverride( nullptr );
		}
		if ( m_iThermoptic == 1 )
		{
			if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
				return BaseClass::DrawModel( flags );

			IMaterial* matMotion = g_pMaterialSystem->FindMaterial( "dev/motion", TEXTURE_GROUP_OTHER );

			if ( IsErrorMaterial( matMotion ) )
			{
				DevMsg( "Fuck me...\n" );
				BaseClass::DrawModel( flags );
			}

			float velocity = localPlayer->GetLocalVelocity().Length() / 75.f;

			if ( velocity > 4.f )
				velocity = 4.f;

			bool found = false;
			IMaterialVar* matVar = matMotion->FindVar( "$eyevec", &found );

			if ( found )
			{
				Vector forward;
				GetVectors( &forward, nullptr, nullptr );
				matVar->SetVecValue( forward.x, forward.y, forward.z, velocity );
			}

			modelrender->ForcedMaterialOverride( matMotion );
			result = BaseClass::InternalDrawModel( flags );
			modelrender->ForcedMaterialOverride( nullptr );
		}
		if ( m_iVision == VISION_THERMAL ) // why is this here again
		{
			if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
				return BaseClass::DrawModel( flags );

			IMaterial* matThermal = g_pMaterialSystem->FindMaterial( "dev/vm_thermal", "Other textures" );

			if ( IsErrorMaterial( matThermal ) )
			{
				DevMsg( "Fuck me...\n" );
				BaseClass::DrawModel( flags );
			}

			bool found = false;
			IMaterialVar* matVar = matThermal->FindVar( "$eyevec", &found );

			if ( found )
			{
				Vector forward;
				GetVectors( &forward, nullptr, nullptr );
				matVar->SetVecValue( forward.x, forward.y, forward.z, 3.f );
			}

			modelrender->ForcedMaterialOverride( matThermal );
			result = BaseClass::InternalDrawModel( flags );
			modelrender->ForcedMaterialOverride( nullptr );
		}
		else
		{
			if ( m_Unknown4164 )
			{
				m_Unknown4164 = false;

				dlight_t* light = effects->CL_AllocDlight( LIGHT_INDEX_TE_DYNAMIC + index );

				light->origin = GetAbsOrigin();
				light->radius = 96.f;
				light->decay = 192.f;
				light->color.r = 64;
				light->color.g = 64;
				light->color.b = 255;
				light->color.exponent = 10;
				light->die = gpGlobals->curtime + 0.1f;

				return 0;
			}

			if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
				return 0;

			UpdateRefractTexture();

			IMaterial* thermopticMaterial = g_ThermopticHandler.GetThermopticMaterial();

			g_ThermopticHandler.UpdateThermopticMaterial( m_fThermopticAlpha );

			modelrender->ForcedMaterialOverride( thermopticMaterial );
			result = BaseClass::InternalDrawModel( flags );
			modelrender->ForcedMaterialOverride( nullptr );
		}

		if ( m_Unknown4164 )
			m_Unknown4164 = false;

		if ( result >= 0 )
			return result;
	}

	return BaseClass::DrawModel( flags );
}

ShadowType_t C_NEOPlayer::ShadowCastType()
{
	if ( !IsVisible() )
		return SHADOWS_NONE;

	return m_iThermoptic != 1 ? SHADOWS_RENDER_TO_TEXTURE_DYNAMIC : SHADOWS_NONE;
}

void C_NEOPlayer::Simulate()
{
	if ( this != C_BasePlayer::GetLocalPlayer() )
	{
		if ( IsEffectActive( EF_DIMLIGHT ) )
		{
			QAngle eyeAngles = EyeAngles();
			Vector vForward;
			AngleVectors( eyeAngles, &vForward );

			int iAttachment = LookupAttachment( "muzzle_flash" );

			if ( iAttachment < 0 )
				return;

			Vector vecOrigin;
			QAngle dummy;
			GetAttachment( iAttachment, vecOrigin, dummy );

			trace_t tr;
			UTIL_TraceLine( vecOrigin, vecOrigin + (vForward * 200), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );

			if ( !m_pFlashlightBeam )
			{
				BeamInfo_t beamInfo;
				beamInfo.m_nType = TE_BEAMPOINTS;
				beamInfo.m_vecStart = tr.startpos;
				beamInfo.m_vecEnd = tr.endpos;
				beamInfo.m_pszModelName = "sprites/glow01.vmt";
				beamInfo.m_pszHaloName = "sprites/glow01.vmt";
				beamInfo.m_flHaloScale = 3.0;
				beamInfo.m_flWidth = 8.0f;
				beamInfo.m_flEndWidth = 35.0f;
				beamInfo.m_flFadeLength = 300.0f;
				beamInfo.m_flAmplitude = 0;
				beamInfo.m_flBrightness = 60.0;
				beamInfo.m_flSpeed = 0.0f;
				beamInfo.m_nStartFrame = 0.0;
				beamInfo.m_flFrameRate = 0.0;
				beamInfo.m_flRed = 255.0;
				beamInfo.m_flGreen = 255.0;
				beamInfo.m_flBlue = 255.0;
				beamInfo.m_nSegments = 8;
				beamInfo.m_bRenderable = true;
				beamInfo.m_flLife = 0.5;
				beamInfo.m_nFlags = FBEAM_FOREVER | FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

				m_pFlashlightBeam = beams->CreateBeamPoints( beamInfo );
			}

			if ( m_pFlashlightBeam )
			{
				BeamInfo_t beamInfo;
				beamInfo.m_vecStart = tr.startpos;
				beamInfo.m_vecEnd = tr.endpos;
				beamInfo.m_flRed = 255.0;
				beamInfo.m_flGreen = 255.0;
				beamInfo.m_flBlue = 255.0;

				beams->UpdateBeamInfo( m_pFlashlightBeam, beamInfo );

				dlight_t *el = effects->CL_AllocDlight( 0 );
				el->origin = tr.endpos;
				el->radius = 50;
				el->color.r = 200;
				el->color.g = 200;
				el->color.b = 200;
				el->die = gpGlobals->curtime + 0.1;
			}
		}
		else if ( m_pFlashlightBeam )
		{
			ReleaseFlashlight();
		}
	}

	BaseClass::Simulate();
}

void C_NEOPlayer::ReleaseFlashlight()
{
	if ( m_pFlashlightBeam )
	{
		m_pFlashlightBeam->flags = 0;
		m_pFlashlightBeam->die = gpGlobals->curtime - 1;

		m_pFlashlightBeam = NULL;
	}
}

C_BaseAnimating* C_NEOPlayer::BecomeRagdollOnClient( bool bCopyEntity /*= true*/ )
{
	return nullptr;
}

void C_NEOPlayer::UpdateClientSideAnimation()
{
	// Update the animation data. It does the local check here so this works when using
	// a third-person camera (and we don't have valid player angles).
	if ( this == C_NEOPlayer::GetLocalNEOPlayer() )
		m_PlayerAnimState->Update( EyeAngles()[YAW], m_angEyeAngles[PITCH] );
	else
		m_PlayerAnimState->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );

	BaseClass::UpdateClientSideAnimation();
}

void C_NEOPlayer::DoMuzzleFlash()
{
	C_BaseAnimating::DoMuzzleFlash();
	m_Unknown4164 = true;
}

void C_NEOPlayer::ProcessMuzzleFlashEvent()
{
	Vector vector;
	QAngle angles;

	bool bFoundAttachment = GetAttachment( LookupAttachment( "muzzle_flash" ), vector, angles );

	if ( bFoundAttachment )
	{
		dlight_t *el = effects->CL_AllocDlight( LIGHT_INDEX_MUZZLEFLASH + index );
		el->origin = vector;
		el->radius = RandomFloat( 32.f, 64.f );
		el->decay = el->radius / 0.05f;
		el->die = gpGlobals->curtime + 0.05f;
		el->color.r = 255;
		el->color.g = 192;
		el->color.b = 64;
		el->color.exponent = 5;
	}

	CWeaponNEOBase* activeWeapon = GetActiveNEOWeapon();

	if ( activeWeapon )
		FX_MuzzleEffect( activeWeapon->GetAbsOrigin(), activeWeapon->GetAbsAngles(), 1.f, 0 ); 
}

void C_NEOPlayer::NEO_MuzzleFlash()
{
	if ( m_iOldNMFlash != m_iNMFlash )
	{
		m_iOldNMFlash = m_iNMFlash;
		DoMuzzleFlash();
	}
}

void C_NEOPlayer::CalcView( Vector &eyeOrigin, QAngle &eyeAngles, float &zNear, float &zFar, float &fov )
{
	C_BaseEntity* ragdoll = m_hRagdoll.Get();

	if ( m_lifeState != LIFE_ALIVE && ragdoll && m_bIsOnDeathScreen )
	{
		int attachment = ragdoll->LookupAttachment( "eyes" );

		if ( !attachment )
			return;

		Vector origin;
		QAngle angles;

		if ( !ragdoll->GetAttachment( attachment, origin, angles ) )
			return;

		Vector forward;
		AngleVectors( angles, &forward );

		Vector start;

		start = forward;
		start.z += 64.f;

		trace_t trace;
		UTIL_TraceHull( start, forward, -Vector( 12, 12, 12 ), Vector( 12, 12, 12 ), CONTENTS_MOVEABLE | CONTENTS_GRATE | CONTENTS_AUX | CONTENTS_WINDOW | CONTENTS_SOLID, this, 0, &trace );

		if ( trace.fraction < 1.f )
			forward = trace.endpos;

		if ( gpGlobals->curtime >= (m_fRagdollCreationTime + 10.f) )
			m_bIsOnDeathScreen = false;
	}
	else
	{
		BaseClass::CalcView( eyeOrigin, eyeAngles, zNear, zFar, fov );
	}
}

void C_NEOPlayer::PreThink()
{
	BaseClass::PreThink();

	ItemPreFrame();

	UpdatePlayerClassInfo();

	//UpdateLean();
	UpdateThermoptic();
	UpdateGeiger();
	UpdateVision();
	UpdateIDTarget();
	UpdateTurnSpeed( gpGlobals->curtime - m_fLastThinkTime );

	m_fLastThinkTime = gpGlobals->curtime;
}

float C_NEOPlayer::GetFOV()
{
	CWeaponNEOBase* pActiveWeapon = GetActiveNEOWeapon();

	if ( pActiveWeapon )
		return pActiveWeapon->m_fFov;
	
	return 75.f;
}

float C_NEOPlayer::GetMinFOV() const
{
	return 1.0f;
}					 

void C_NEOPlayer::CalcPlayerView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov )
{
	if ( !prediction->InPrediction() )
	{
		// FIXME: Move into prediction
		view->DriftPitch();
	}

	VectorCopy( EyePosition(), eyeOrigin );
	VectorCopy( EyeAngles(), eyeAngles );

	if ( !prediction->InPrediction() )
	{
		SmoothViewOnStairs( eyeOrigin );
	}

	// Snack off the origin before bob + water offset are applied
	Vector vecBaseEyePosition = eyeOrigin;

	CalcViewRoll( eyeAngles );

	// Apply punch angle
	VectorAdd( eyeAngles, m_Local.m_vecPunchAngle, eyeAngles );

	if ( !prediction->InPrediction() )
	{
		// Shake it up baby!
		vieweffects->CalcShake();
		vieweffects->ApplyShake( eyeOrigin, eyeAngles, 1.0 );
	}

	// Apply a smoothing offset to smooth out prediction errors.
	Vector vSmoothOffset;
	GetPredictionErrorSmoothingVector( vSmoothOffset );
	eyeOrigin += vSmoothOffset;
	m_flObserverChaseDistance = 0.0;

	// This is what they've added
	switch ( m_Unknown3956 )
	{
		case 0:
			eyeAngles.z = 0.f;
			break;

		case 1:
			eyeAngles.z = m_Unknown3960 * 20.f - 20.f;
			break;

		case 2:
			eyeAngles.z = -20.f;
			break;

		case 3:
			eyeAngles.z = -20.f * m_Unknown3960;
			break;

		case 4:
			eyeAngles.z = m_Unknown3960;
			break;

		case 5:
			eyeAngles.z = 20.f;
			break;

		case 6:
			eyeAngles.z = 20.f * m_Unknown3960;
			break;
	}

	// calc current FOV
	fov = GetFOV();
}

void C_NEOPlayer::PostDataUpdate( DataUpdateType_t updateType )
{
	// C_BaseEntity assumes we're networking the entity's angles, so pretend that it
	// networked the same value we already have.
	SetNetworkAngles( GetLocalAngles() );
	
	BaseClass::PostDataUpdate( updateType );
}

void C_NEOPlayer::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}

	UpdateVisibility();
}


void C_NEOPlayer::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	if ( event == PLAYERANIMEVENT_THROW_GRENADE )
	{
		// Let the server handle this event. It will update m_iThrowGrenadeCounter and the client will
		// pick up the event in CCSPlayerAnimState.
	}
	else	
	{
		m_PlayerAnimState->DoAnimationEvent( event, nData );
	}
}

bool C_NEOPlayer::ShouldDraw( void )
{
	// If we're dead, our ragdoll will be drawn for us instead.
	if ( !IsAlive() )
		return false;

	if( GetTeamNumber() == TEAM_SPECTATOR )
		return false;

	if( IsLocalPlayer() && IsRagdoll() )
		return true;

	return BaseClass::ShouldDraw();
}


void C_NEOPlayer::GetEyeVectors( Vector *forward, Vector *right, Vector *up )
{
	// This was supposed to have mobile armor checks in here
	AngleVectors( EyeAngles(), forward, right, up );
}

CWeaponNEOBase* C_NEOPlayer::GetActiveNEOWeapon() const
{
	return dynamic_cast< CWeaponNEOBase* >( GetActiveWeapon() );
}

// !!!
// TODO:
// Why isn't this being handled by the server? Byte patching it will (most likely) bypass lean checks
// !!!
int C_NEOPlayer::CanLean( int iLean )
{
	if ( iLean == LEAN_LEFT )
	{
		Vector vRight;

		AngleVectors( GetAbsAngles(), nullptr, &vRight, nullptr );

		trace_t tr;
		Vector vEnd = GetAbsOrigin() - vRight * 31.0f;			

		UTIL_TraceHull( GetAbsOrigin(), vEnd, Vector( -16.0f, -16.0f, 30.0f ), Vector( 16.0f, 16.0f, 72.0f ), MASK_ALL, this, COLLISION_GROUP_PLAYER, &tr );

		if ( tr.fraction >= 1.0f && !tr.allsolid && !tr.startsolid )
			return LEAN_LEFT;
	}
	else if ( iLean == LEAN_RIGHT )
	{
		Vector vRight;

		AngleVectors( GetAbsAngles(), nullptr, &vRight, nullptr );

		trace_t tr;
		Vector vEnd = GetAbsOrigin() + vRight * 31.0f;

		UTIL_TraceHull( GetAbsOrigin(), vEnd, Vector( -16.0f, -16.0f, 30.0f ), Vector( 16.0f, 16.0f, 72.0f ), MASK_ALL, this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr );

		if ( tr.fraction >= 1.0f && !tr.allsolid && !tr.startsolid )
			return LEAN_RIGHT;
	}

	return LEAN_NONE;
}

void C_NEOPlayer::UpdateLean()
{
	if ( IsLocalNEOPlayer() )		   
		m_iLean = CanLean( m_iLean );

	if ( m_Unknown3956 == 0 )
	{
		if ( m_iLean != LEAN_NONE )
		{
			if ( m_iLean == LEAN_LEFT )
				m_Unknown3956 = 1;
			else if ( m_iLean == LEAN_RIGHT )
				m_Unknown3956 = 4;

			m_Unknown3964 = gpGlobals->curtime + 0.15f;
			m_Unknown3960 = (m_Unknown3964 - gpGlobals->curtime) / 0.15f;
		}
	}
	else if ( m_Unknown3956 == 1 )
	{
		if ( m_iLean != LEAN_LEFT )
		{
			m_Unknown3956 = 3;
			m_Unknown3964 = 0.15f - (m_Unknown3964 - gpGlobals->curtime) + gpGlobals->curtime;
			m_Unknown3960 = (m_Unknown3964 - gpGlobals->curtime) / 0.15f;
		}
		else if ( m_Unknown3964 <= gpGlobals->curtime )
		{
			m_Unknown3956 = 2;
			m_Unknown3960 = 1.0f;
		}
		else
		{
			m_Unknown3960 = (m_Unknown3964 - gpGlobals->curtime) / 0.15f;
		}
	}
	else if ( m_Unknown3956 == 2 )
	{
		if ( m_iLean != LEAN_LEFT )
		{
			m_Unknown3956 = 3;
			m_Unknown3964 = 0.15f + gpGlobals->curtime;
			m_Unknown3960 = 1.0f;
		}
	}
	else if ( m_Unknown3956 == 3 )
	{
		if ( m_iLean == LEAN_LEFT )
		{
			m_Unknown3956 = 1;
			m_Unknown3964 = 0.15f - (m_Unknown3964 - gpGlobals->curtime) + gpGlobals->curtime;
			m_Unknown3960 = (m_Unknown3964 - gpGlobals->curtime) / 0.15f;
		}
		else if ( m_Unknown3964 <= gpGlobals->curtime )
		{
			m_Unknown3956 = 0;
			m_Unknown3960 = 0.0f;
		}
		else
		{
			m_Unknown3960 = (m_Unknown3964 - gpGlobals->curtime) / 0.15f;
		}
	}
	else if ( m_Unknown3956 == 4 )
	{
		if ( m_iLean != LEAN_RIGHT )
		{
			m_Unknown3956 = 6;
			m_Unknown3964 = 0.15f - (m_Unknown3964 - gpGlobals->curtime) + gpGlobals->curtime;
			m_Unknown3960 = (m_Unknown3964 - gpGlobals->curtime) / 0.15f;
		}
		else if ( m_Unknown3964 <= gpGlobals->curtime )
		{
			m_Unknown3956 = 5;
			m_Unknown3960 = 1.0f;
		}
		else
		{
			m_Unknown3960 = (m_Unknown3964 - gpGlobals->curtime) / 0.15f;
		}
	}
	else if ( m_Unknown3956 == 5 )
	{
		if ( m_iLean != LEAN_RIGHT )
		{
			m_Unknown3956 = 6;
			m_Unknown3964 = 0.15f + gpGlobals->curtime;
			m_Unknown3960 = 1.0f;
		}
	}
	else if ( m_Unknown3956 == 6 )
	{
		if ( m_iLean == LEAN_RIGHT )
		{
			m_Unknown3956 = 1;
			m_Unknown3964 = 0.15f - (m_Unknown3964 - gpGlobals->curtime) + gpGlobals->curtime;
			m_Unknown3960 = (m_Unknown3964 - gpGlobals->curtime) / 0.15f;
		}
		else if ( m_Unknown3964 <= gpGlobals->curtime )
		{
			m_Unknown3956 = 0;
			m_Unknown3960 = 0.0f;
		}
		else
		{
			m_Unknown3960 = (m_Unknown3964 - gpGlobals->curtime) / 0.15f;
		}
	}

	float fCtrlValue = 0.0f;

	if ( m_Unknown3956 == 1 )
		fCtrlValue = m_Unknown3960 * -35.0f - 35.0f;
	else if ( m_Unknown3956 == 2 )
		fCtrlValue = -35.0f;
	else if ( m_Unknown3956 == 3 )
		fCtrlValue = m_Unknown3960 * -35.0f;
	else if ( m_Unknown3956 == 4 )
		fCtrlValue = m_Unknown3960 * 35.0f;
	else if ( m_Unknown3956 == 5 )
		fCtrlValue = -35.0f;
	else if ( m_Unknown3956 == 6 )
		fCtrlValue = m_Unknown3960 * 35.0f + 35.0f;

	SetBoneController( 0, fCtrlValue );
}

void C_NEOPlayer::UpdateThermoptic()
{
	if ( m_iThermoptic != m_iOldThermoptic )
	{
		if ( m_iThermoptic == 1 )
		{
			EmitSound( "NeoPlayer.ThermOpticOn" );

			dlight_t* light = effects->CL_AllocDlight( LIGHT_INDEX_TE_DYNAMIC + index );

			light->origin = GetAbsOrigin();
			light->radius = 96.f;
			light->color.r = 64;
			light->color.b = 64;
			light->color.b = 255;
			light->color.exponent = 10;
			light->die = gpGlobals->curtime + 0.5f;
			light->decay = 192.f;
		}		   
		else
		{
			EmitSound( "NeoPlayer.ThermOpticOff" );
		}
	}

	Vector vel;
	EstimateAbsVelocity( vel );

	float speed = vel.Length2D();

	if ( speed <= 20.0f )
		m_fThermopticAlpha -= (gpGlobals->curtime - m_fLastThinkTime) * 1.1f;
	else
		m_fThermopticAlpha = speed * 0.004 * (gpGlobals->curtime - m_fLastThinkTime) * m_fThermopticAlpha;

	if ( m_fThermopticAlpha < 0.0f )
		m_fThermopticAlpha = 0.0f;

	if ( m_fThermopticAlpha > 1.0f )
		m_fThermopticAlpha = 1.0f;

	if ( !m_iThermoptic || gpGlobals->curtime >= (m_Unknown4108 + 1.f) )
	{
		m_nRenderFX = kRenderFxNone;
		m_iOldThermoptic = m_iThermoptic;
	}

	else
	{
		m_nRenderFX = kRenderFxDistort;
		m_iOldThermoptic = 1;
		m_fThermopticAlpha = 1.0f - m_fThermopticAlpha;
	}
}

void C_NEOPlayer::UpdateGeiger()
{
	if ( m_iRank < m_iOldRank )
		EmitSound( "Geiger.BeepLow" );
	else if ( m_iRank > m_iOldRank )
		EmitSound( "Geiger.BeepHigh" );

	m_iOldRank = m_iRank;
}

void C_NEOPlayer::UpdateVision()
{
	if ( !IsLocalNEOPlayer() )
		return;

	extern int g_iLocalPlayerVision;
	g_iLocalPlayerVision = m_iVision;

	if ( m_iVision != m_iOldVision )
	{
		if ( m_iVision )
			EmitSound( "NeoPlayer.VisionOn" );
		else
			EmitSound( "NeoPlayer.VisionOff" );
	}

	m_iOldVision = m_iVision;
}

//-----------------------------------------------------------------------------
// Purpose: Update this client's target entity
//-----------------------------------------------------------------------------
void C_NEOPlayer::UpdateIDTarget()
{
	if ( !IsLocalNEOPlayer() )
		return;

	// Clear old target and find a new one
	m_iIDEntIndex = 0;
	m_bAnyoneInCross = false;

	// don't show IDs in chase spec mode
	if ( GetObserverMode() == OBS_MODE_CHASE ||
		GetObserverMode() == OBS_MODE_DEATHCAM )
		return;

	trace_t tr;
	Vector vecStart, vecEnd;
	VectorMA( MainViewOrigin(), 1500, MainViewForward(), vecEnd );
	VectorMA( MainViewOrigin(), 10, MainViewForward(), vecStart );
	UTIL_TraceLine( vecStart, vecEnd, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );

	if ( !tr.startsolid && tr.DidHitNonWorldEntity() )
	{
		C_BaseEntity *pEntity = tr.m_pEnt;

		if ( pEntity && (pEntity != this) )
		{
			m_iIDEntIndex = pEntity->entindex();
			m_bAnyoneInCross = true;
		}
	}
}

void C_NEOPlayer::UpdateTurnSpeed( float fTimeDelta )
{
	if ( m_fTurnSpeed == m_fOldTurnSpeed )
		return;

	if ( m_fTurnSpeed > m_fOldTurnSpeed )
	{
		m_fOldTurnSpeed += fTimeDelta;

		if ( m_fTurnSpeed > m_fOldTurnSpeed )
			m_fOldTurnSpeed = m_fTurnSpeed;
	}

	else
	{
		m_fOldTurnSpeed -= fTimeDelta;

		if ( m_fTurnSpeed < m_fOldTurnSpeed )
			m_fOldTurnSpeed = m_fTurnSpeed;
	}
}

void C_NEOPlayer::FixViewPunch( float viewPunch )
{
	if ( viewPunch == 0.0f || (viewPunch * 2) > (m_Local.m_vecPunchAngle->LengthSqr() + m_Local.m_vecPunchAngleVel->LengthSqr()) )
	{
		m_Local.m_vecPunchAngle * 0.0f;
		m_Local.m_vecPunchAngleVel * 0.0f;
	}
}

void C_NEOPlayer::UpdatePlayerClassInfo()
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
