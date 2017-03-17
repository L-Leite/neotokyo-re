//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "in_buttons.h"
#include "takedamageinfo.h"
#include "weapon_neobase.h"
#include "ammodef.h"
#include "neo_gamerules.h"


#if defined( CLIENT_DLL )

	#include "c_neo_player.h"	
	#include "dlight.h"
	#include "iefx.h"
	#include "materialsystem\imaterialvar.h"

#else

	#include "neo_player.h"

#endif


// ----------------------------------------------------------------------------- //
// CWeaponNEOBase tables.
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponNEOBase, DT_WeaponNEOBase )

BEGIN_NETWORK_TABLE( CWeaponNEOBase, DT_WeaponNEOBase )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( bAimed ) ),
	RecvPropFloat( RECVINFO( m_fAccuracy ) ),
	RecvPropInt( RECVINFO( m_iTeam2ViewModelIndex ) ),
#else
	// world weapon models have no animations
  	SendPropExclude( "DT_AnimTimeMustBeFirst", "m_flAnimTime" ),
	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),

	SendPropInt( SENDINFO( bAimed ) ),
	SendPropFloat( SENDINFO( m_fAccuracy ) ),
	SendPropInt( SENDINFO( m_iTeam2ViewModelIndex ) ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponNEOBase )
	DEFINE_PRED_FIELD( m_flTimeWeaponIdle, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_NOERRORCHECK ),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_neo_base, CWeaponNEOBase );


#ifdef GAME_DLL

	BEGIN_DATADESC( CWeaponNEOBase )

		// New weapon Think and Touch Functions go here..

	END_DATADESC()

#endif

// ----------------------------------------------------------------------------- //
// CWeaponCSBase implementation. 
// ----------------------------------------------------------------------------- //
CWeaponNEOBase::CWeaponNEOBase()
{
	SetPredictionEligible( true );

	AddSolidFlags( FSOLID_TRIGGER ); // Nothing collides with these but it gets touches.

	bAimed = false;
	m_fAccuracy = 0.0f;

	m_Unknown1452 = 0.85f;
	m_Unknown1456 = false;
	m_Unknown1360 = false;
	m_Unknown1364 = 0.0f;
	m_Unknown1449 = false;
	m_Unknown1472 = true;
	m_bCanReconUse = true;
	m_bCanAssaultUse = true;
	m_bCanSupportUse = true;
	m_bCanOperatorUse = true;
	m_bCanVipUse = true;
}

void CWeaponNEOBase::Spawn()
{
	m_fMinRange1 = 0.0f;
	m_fMinRange2 = 0.0f;
	m_fMaxRange1 = 64.0f;
	m_fMaxRange2 = 64.0f;

	BaseClass::Spawn();
}

void CWeaponNEOBase::Precache()
{
	BaseClass::Precache();

	const CNEOWeaponInfo& wpnData = GetNEOWpnData();	   	

	//m_iTeam2ViewModelIndex = PrecacheModel( GetTeam2ViewModel() );

	m_fTPMuzzleFlashScale = wpnData.m_fTPMuzzleFlashScale;	
	m_bDrawCrosshair = wpnData.m_bDrawCrosshair;

	m_iAimType = wpnData.m_iAimType;
	m_iScopeStyle = wpnData.m_iScopeStyle;

	m_fVMFov = wpnData.m_fVMFov;
	m_fVMAimFov = wpnData.m_fVMAimFov;
	m_fAimFov = wpnData.m_fAimFov;

	m_fVMOffsetUp = wpnData.m_fVMOffsetUp;
	m_fVMOffsetForward = wpnData.m_fVMOffsetForward;
	m_fVMOffsetRight = wpnData.m_fVMOffsetRight;

	m_fVMAimOffsetUp = wpnData.m_fVMAimOffsetUp;
	m_fVMAimOffsetForward = wpnData.m_fVMAimOffsetForward;
	m_fVMAimOffsetRight = wpnData.m_fVMAimOffsetRight;

	m_fVMAngleYaw = wpnData.m_fVMAngleYaw;
	m_fVMAnglePitch = wpnData.m_fVMAnglePitch;
	m_fVMAngleRoll = wpnData.m_fVMAngleRoll;

	m_fVMAimAngleYaw = wpnData.m_fVMAimAngleYaw;
	m_fVMAimAnglePitch = wpnData.m_fVMAimAnglePitch;
	m_fVMAimAngleRoll = wpnData.m_fVMAimAngleRoll;
}

#ifdef CLIENT_DLL
bool CWeaponNEOBase::ShouldPredict()
{
	if ( GetOwner() && GetOwner() == C_BasePlayer::GetLocalPlayer() )
		return true;

	return BaseClass::ShouldPredict();
}

class CThermopticHandler
{
public:
	CThermopticHandler()
	{
		m_pThermMaterial = GetThermopticMaterial();
		m_fTransparency = 0.0f;
	}

public:
	void UpdateThermopticMaterial( float fNewTransparency )	
	{
		m_fTransparency = fNewTransparency * 0.2f; 

		if ( !material )
			return;

		bool found = false;

		IMaterialVar* refractamount = material->FindVar( "$refractamount", &found );

		if ( found )
			refractamount->SetFloatValue( m_fTransparency * 0.035f );

		IMaterialVar* bluramount = material->FindVar( "$bluramount", &found );

		if ( found )
			bluramount->SetFloatValue( m_fTransparency * 20.0f + 10.5f );

		IMaterialVar* refracttint = material->FindVar( "$refracttint", &found );

		if ( found )
			refracttint->SetVecValue( 0.85f - m_fTransparency * 0.8f, 0.85f - m_fTransparency * 0.6f, 0.85f - m_fTransparency * 0.6f );

		IMaterialVar* fresnelreflection = material->FindVar( "$fresnelreflection", &found );

		if ( found )
			fresnelreflection->SetFloatValue( m_fTransparency + m_fTransparency + 0.25f );

		IMaterialVar* bumptransform = material->FindVar( "$bumptransform", &found );

		if ( found )
		{
			VMatrix matrix( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );

			if ( bumptransform->GetType() == MATERIAL_VAR_TYPE_MATRIX )
				bumptransform->SetMatrixValue( matrix );
			else
				bumptransform->SetVecValue( 0.0f, 0.0f, 0.0f );
		}
	}
	
private:
	IMaterial* CreateThermopticMaterial()
	{
		if ( !g_pMaterialSystem )
			return nullptr;

		KeyValues* keyValues = new KeyValues( "Refract" );

		IMaterial* pThermMaterial = g_pMaterialSystem->CreateMaterial( "thermoptic", keyValues );

		if ( !pThermMaterial )
			return nullptr;

		bool found = false;

		pThermMaterial->SetShader( "Refract" );
		pThermMaterial->SetMaterialVarFlag( MATERIAL_VAR_MODEL, true );
		pThermMaterial->SetMaterialVarFlag( MATERIAL_VAR_SUPPRESS_DECALS, true );

		IMaterialVar* refractamount = pThermMaterial->FindVar( "$refractamount", &found );

		if ( found )
			refractamount->SetFloatValue( 0.8f );

		IMaterialVar* normalmap = pThermMaterial->FindVar( "$normalmap", &found );

		if ( found )
		{
			ITexture* waterTexture = g_pMaterialSystem->FindTexture( "dev/water_normal", "ClientEffect textures" );

			if ( waterTexture )
				normalmap->SetTextureValue( waterTexture );
		}

		IMaterialVar* bumpframe = pThermMaterial->FindVar( "$bumpframe", &found );

		if ( found )
			bumpframe->SetIntValue( 0 );

		return pThermMaterial;
	}

	IMaterial* GetThermopticMaterial()
	{
		static IMaterial* pThermopticMaterial = nullptr;

		if ( !pThermopticMaterial )
		{
			pThermopticMaterial = CreateThermopticMaterial();
			Assert( pThermopticMaterial );
		}

		return pThermopticMaterial;
	}

private:
	IMaterial* m_pThermMaterial;
	float m_fTransparency;
};

CThermopticHandler g_ThermopticHandler;

int CWeaponNEOBase::InternalDrawModel( int flags )
{
	C_NEOPlayer* owner = dynamic_cast< C_NEOPlayer* >(GetOwner());
	C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( !owner || !localPlayer )
		return BaseClass::InternalDrawModel( flags );

	if ( owner->m_iThermoptic == 1 )
	{
		if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
			return 0;

		IMaterial* thermopticMaterial = GetThermopticMaterial();

		g_ThermopticHandler.UpdateThermopticMaterial( owner->m_fThermopticAlpha );

		modelrender->ForcedMaterialOverride( thermopticMaterial );
		int result = BaseClass::InternalDrawModel( flags );
		modelrender->ForcedMaterialOverride( nullptr );

		return result;
	}
	else if ( owner->m_iVision == VISION_THERMAL ) // Thermal vision
	{
		if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
			return BaseClass::InternalDrawModel( flags );

		IMaterial* matThermal = g_pMaterialSystem->FindMaterial( "dev/vm_thermal", TEXTURE_GROUP_OTHER );

		if ( IsErrorMaterial( matThermal ) )
		{
			DevMsg( SPEW_MESSAGE, "Fuck me...\n" ); // Their message, not mine kek
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
		int result = BaseClass::InternalDrawModel( flags );
		modelrender->ForcedMaterialOverride( nullptr );

		return result;
	}
	else if ( owner->m_iVision == VISION_MOTION ) // Motion vision
	{
		if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
			return BaseClass::InternalDrawModel( flags );

		IMaterial* matMotion = g_pMaterialSystem->FindMaterial( "dev/motion", TEXTURE_GROUP_OTHER );

		if ( IsErrorMaterial( matMotion ) )
		{
			DevMsg( SPEW_MESSAGE, "Fuck me...\n" );
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
		int result = BaseClass::InternalDrawModel( flags );
		modelrender->ForcedMaterialOverride( nullptr );

		return result;
	}

	return BaseClass::InternalDrawModel( flags );
}
#endif

const CNEOWeaponInfo &CWeaponNEOBase::GetNEOWpnData() const
{
	const FileWeaponInfo_t *pWeaponInfo = &GetWpnData();
	const CNEOWeaponInfo *pNEOInfo;

	#ifdef _DEBUG
		pNEOInfo = dynamic_cast< const CNEOWeaponInfo* >( pWeaponInfo );
		Assert( pNEOInfo );
	#else
		pNEOInfo = static_cast< const CNEOWeaponInfo* >( pWeaponInfo );
	#endif

	return *pNEOInfo;
}

bool CWeaponNEOBase::PlayEmptySound()
{
	CPASAttenuationFilter filter( this );
	filter.UsePredictionRules();

	EmitSound( filter, entindex(), "Default.ClipEmpty_Rifle" );
	
	return 0;
}

CNEOPlayer* CWeaponNEOBase::GetPlayerOwner() const
{
	return dynamic_cast< CNEOPlayer* >( GetOwner() );
}

#ifdef GAME_DLL

void CWeaponNEOBase::SendReloadEvents()
{
	CNEOPlayer *pPlayer = dynamic_cast< CNEOPlayer* >( GetOwner() );
	if ( !pPlayer )
		return;

	// Send a message to any clients that have this entity to play the reload.
	CPASFilter filter( pPlayer->GetAbsOrigin() );
	filter.RemoveRecipient( pPlayer );

	UserMessageBegin( filter, "ReloadEffect" );
		WRITE_SHORT( pPlayer->entindex() );
	MessageEnd();

	// Make the player play his reload animation.
	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_RELOAD );
}

#endif

const char* CWeaponNEOBase::GetTeam2ViewModel()
{
	return GetNEOWpnData().m_szTeam2ViewModel;
}

void CWeaponNEOBase::SetViewModel()
{
	CBasePlayer* pOwner = ToBasePlayer( GetOwner() );

	if ( !pOwner )
		return;

	CBaseViewModel* vm = pOwner->GetViewModel( m_nViewModelIndex, false );

	if ( !vm )
		return;

	Assert( vm->ViewModelIndex() == m_nViewModelIndex );	

	const char* szViewModel = GetViewModel( m_nViewModelIndex );

	if ( pOwner->GetTeamNumber() == TEAM_NSF )
		szViewModel = GetTeam2ViewModel();

	vm->SetWeaponModel( szViewModel, this );
}

void CWeaponNEOBase::ItemPostFrame()
{
	CNEOPlayer* owner = ToNEOPlayer( GetOwner() );

	if ( owner->GetObserverMode() != OBS_MODE_NONE || NEOGameRules()->IsInFreezePeriod() )
		return;

	if ( !owner || owner->m_iSprint == 1 || owner->GetMoveType() == MOVETYPE_LADDER )
		return;

	if ( m_Unknown1456
		&& owner->m_nButtons & 0x40000000 // what button is this? ( 1 << 30 )
		&& gpGlobals->curtime > m_flNextPrimaryAttack && gpGlobals->curtime > m_flNextSecondaryAttack )
	{
		m_Unknown1360 = true;
		m_Unknown1364 = gpGlobals->curtime + 0.25f;

		if ( bAimed )
			bAimed = false;

		SendWeaponAnim( ACT_SLAM_STICKWALL_ND_IDLE ); // Yeah this anim is prob wrong

		float seqDuration = SequenceDuration() + gpGlobals->curtime;

		if ( m_flNextSecondaryAttack != seqDuration )
			m_flNextSecondaryAttack = seqDuration;

		if ( m_flNextPrimaryAttack != m_flNextSecondaryAttack )
			m_flNextPrimaryAttack = m_flNextSecondaryAttack;

		SetWeaponIdleTime( seqDuration );
	}
}

#ifdef CLIENT_DLL	

#define	NEO_BOB_CYCLE_MIN	1.0f
#define	NEO_BOB_CYCLE_MAX	0.45f
#define	NEO_BOB				0.002f
#define	NEO_BOB_UP			0.5f

float	g_lateralBob = 0;
float	g_verticalBob = 0;

ConVar v_vmtweak( "v_vmtweak", "0", FCVAR_CLIENTDLL );

ConVar v_vm_aimfov( "v_vm_aimfov", "54.0", FCVAR_CLIENTDLL );
ConVar v_vmaimoffsetup( "v_vmaimoffsetup", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmaimoffsetforward( "v_vmaimoffsetforward", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmaimoffsetright( "v_vmaimoffsetright", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmaimangleyaw( "v_vmaimangleyaw", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmaimanglepitch( "v_vmaimanglepitch", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmaimangleroll( "v_vmaimangleroll", "0.0", FCVAR_CLIENTDLL );

ConVar v_vm_fov( "v_vm_fov", "54.0", FCVAR_CLIENTDLL );
ConVar v_vmoffsetup( "v_vmoffsetup", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmoffsetforward( "v_vmoffsetforward", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmoffsetright( "v_vmoffsetright", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmangleyaw( "v_vmangleyaw", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmanglepitch( "v_vmanglepitch", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmangleroll( "v_vmangleroll", "0.0", FCVAR_CLIENTDLL );

float CWeaponNEOBase::CalcViewmodelBob()
{
	static	float bobtime;
	static	float lastbobtime;
	float	cycle;

	CBasePlayer *player = ToBasePlayer( GetOwner() );
	//Assert( player );

	//NOTENOTE: For now, let this cycle continue when in the air, because it snaps badly without it

	if ( (!gpGlobals->frametime) || (player == NULL) )
	{
		//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
		return 0.0f;// just use old value
	}

	//Find the speed of the player
	float speed = player->GetLocalVelocity().Length2D();

	//FIXME: This maximum speed value must come from the server.
	//		 MaxSpeed() is not sufficient for dealing with sprinting - jdw

	speed = clamp( speed, -320, 320 );

	float bob_offset = RemapVal( speed, 0, 320, 0.0f, 1.0f );

	bobtime += (gpGlobals->curtime - lastbobtime) * bob_offset;
	lastbobtime = gpGlobals->curtime;

	//Calculate the vertical bob
	cycle = bobtime - (int) (bobtime / NEO_BOB_CYCLE_MAX)*NEO_BOB_CYCLE_MAX;
	cycle /= NEO_BOB_CYCLE_MAX;

	if ( cycle < NEO_BOB_UP )
	{
		cycle = M_PI * cycle / NEO_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI*(cycle - NEO_BOB_UP) / (1.0 - NEO_BOB_UP);
	}

	g_verticalBob = speed*0.005f;
	g_verticalBob = g_verticalBob*0.3 + g_verticalBob*0.7*sin( cycle );

	g_verticalBob = clamp( g_verticalBob, -7.0f, 4.0f );

	//Calculate the lateral bob
	cycle = bobtime - (int) (bobtime / NEO_BOB_CYCLE_MAX * 2)*NEO_BOB_CYCLE_MAX * 2;
	cycle /= NEO_BOB_CYCLE_MAX * 2;

	if ( cycle < NEO_BOB_UP )
	{
		cycle = M_PI * cycle / NEO_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI*(cycle - NEO_BOB_UP) / (1.0 - NEO_BOB_UP);
	}

	g_lateralBob = speed*0.005f;
	g_lateralBob = g_lateralBob*0.3 + g_lateralBob*0.7*sin( cycle );
	g_lateralBob = clamp( g_lateralBob, -7.0f, 4.0f );

	//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
	return 0.0f;
}

void CWeaponNEOBase::AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles )
{
	float bobMult = 1.0f;

	switch ( m_iUnknown )
	{
		case 1:
			bobMult = m_fUnknown2 * 0.5f + 0.5f;
			break;

		case 2:
			bobMult = 0.5f;
			break;

		case 3:
			bobMult = 1.0f - 0.5f * m_fUnknown2;
	}

	Vector forward, right;

	AngleVectors( angles, &forward, &right, nullptr );

	CalcViewmodelBob();

	VectorMA( origin, g_verticalBob * 0.1 * bobMult, forward, origin );

	origin.z += g_verticalBob * 0.2f * bobMult;

	angles.z += g_verticalBob * 0.5f * bobMult;
	angles.x -= g_verticalBob * 0.4f * bobMult;

	angles.y -= g_lateralBob * 0.3f * bobMult;

	VectorMA( origin, bobMult * (g_lateralBob * 0.8f), right, origin );
}

void CWeaponNEOBase::OverrideViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles )
{
	Vector forward, right;

	AngleVectors( angles, &forward, &right, nullptr );

	float fOffsetUp = 0.0f;
	float fOffsetForward = 1.0f;
	float fOffsetRight = 1.0f;
	float fAngleYaw = 0.0f;
	float fAnglePitch = 0.0f;
	float fAngleRoll = 0.0f;

	if ( !v_vmtweak.GetBool() )
	{
		switch ( m_iUnknown )
		{
			case 1:
				m_fFov = m_fVMFov = m_fVMFov - m_fVMAimFov * m_fUnknown2 + m_fVMAimFov;
				fOffsetUp = m_fVMOffsetUp - m_fVMAimOffsetUp * m_fUnknown2 + m_fVMAimOffsetUp;
				fOffsetForward = m_fVMOffsetForward - m_fVMAimOffsetForward * m_fUnknown2 + m_fVMAimOffsetForward;
				fOffsetRight = m_fVMOffsetRight - m_fVMAimOffsetRight * m_fUnknown2 + m_fVMAimOffsetRight;
				fAngleYaw = m_fVMAngleYaw - m_fVMAimAngleYaw * m_fUnknown2 + m_fVMAimAngleYaw;
				fAnglePitch = m_fVMAnglePitch - m_fVMAimAnglePitch * m_fUnknown2 + m_fVMAimAnglePitch;
				fAngleRoll = m_fVMAngleRoll - m_fVMAimAngleRoll * m_fUnknown2 + m_fVMAimAngleRoll;
				break;

			case 2:
				m_fFov = m_fVMFov = m_fVMAimFov;
				fOffsetUp = m_fVMAimOffsetUp;
				fOffsetForward = m_fVMAimOffsetForward;
				fOffsetRight = m_fVMAimOffsetRight;
				fAngleYaw = m_fVMAimAngleYaw;
				fAnglePitch = m_fVMAimAnglePitch;
				fAngleRoll = m_fVMAimAngleRoll;
				break;

			case 3:
				m_fFov = m_fVMFov = m_fVMAimFov - m_fVMFov * m_fUnknown2 + m_fVMFov;
				fOffsetUp = m_fVMAimOffsetUp - m_fVMOffsetUp * m_fUnknown2 + m_fVMOffsetUp;
				fOffsetForward = m_fVMAimOffsetForward - m_fVMOffsetForward * m_fUnknown2 + m_fVMOffsetForward;
				fOffsetRight = m_fVMAimOffsetRight - m_fVMOffsetRight * m_fUnknown2 + m_fVMOffsetRight;
				fAngleYaw = m_fVMAimAngleYaw - m_fVMAngleYaw * m_fUnknown2 + m_fVMAngleYaw;
				fAnglePitch = m_fVMAimAnglePitch - m_fVMAnglePitch * m_fUnknown2 + m_fVMAnglePitch;
				fAngleRoll = m_fVMAimAngleRoll - m_fVMAngleRoll * m_fUnknown2 + m_fVMAngleRoll;
				break;

			default:
				m_fFov = m_fVMFov = m_fVMFov;
				fOffsetUp = m_fVMOffsetUp;
				fOffsetForward = m_fVMOffsetForward;
				fOffsetRight = m_fVMOffsetRight;
				fAngleYaw = m_fVMAngleYaw;
				fAnglePitch = m_fVMAnglePitch;
				fAngleRoll = m_fVMAngleRoll;
		}
	}	
	else
	{
		if ( bAimed )
		{
			m_fFov = m_fVMFov = v_vm_aimfov.GetFloat();
			fOffsetUp = v_vmaimoffsetup.GetFloat();
			fOffsetForward = v_vmaimoffsetforward.GetFloat();
			fOffsetRight = v_vmaimoffsetright.GetFloat();
			fAngleYaw = v_vmaimangleyaw.GetFloat();
			fAnglePitch = v_vmaimanglepitch.GetFloat();
			fAngleRoll = v_vmaimangleroll.GetFloat();
		}

		else
		{
			m_fFov = m_fVMFov = v_vm_fov.GetFloat();
			fOffsetUp = v_vmoffsetup.GetFloat();
			fOffsetForward = v_vmoffsetforward.GetFloat();
			fOffsetRight = v_vmoffsetright.GetFloat();
			fAngleYaw = v_vmangleyaw.GetFloat();
			fAnglePitch = v_vmanglepitch.GetFloat();
			fAngleRoll = v_vmangleroll.GetFloat();
		}
	}

	VectorMA( origin, fOffsetForward, forward, origin );
	VectorMA( origin, fOffsetRight, right, origin );

	origin.z += fOffsetUp;

	angles.x += fAnglePitch;
	angles.y += fAngleYaw;
	angles.z += fAngleRoll;
}

#else

float CWeaponNEOBase::CalcViewmodelBob( void )
{
	return 0.0f;
}

void CWeaponNEOBase::AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles )
{
}

void CWeaponNEOBase::OverrideViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles )
{  
}

#endif

#ifdef CLIENT_DLL
void CWeaponNEOBase::DoNEOMuzzleFlash()
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

	FX_MuzzleEffect( origin, angles, 1.f, 1 ); 
}
#endif*/

bool CWeaponNEOBase::CanClassUseThis( int iClassType )
{
	switch ( iClassType )
	{
		case CLASS_RECON:
			return m_bCanReconUse;
		case CLASS_ASSAULT:
			return m_bCanAssaultUse;
		case CLASS_SUPPORT:
			return m_bCanSupportUse;
		case CLASS_OPERATOR:
			return m_bCanOperatorUse;
		case CLASS_VIP:
			return m_bCanVipUse;
		case CLASS_UNASSIGNED:
		default:
			return false;
	}
}
