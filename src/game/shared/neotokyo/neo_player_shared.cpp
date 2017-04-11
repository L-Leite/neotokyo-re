//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"

#ifdef CLIENT_DLL
	
	#include "c_neo_player.h"

#else

	#include "neo_player.h"

#endif

#include "gamevars_shared.h"
#include "takedamageinfo.h"
#include "effect_dispatch_data.h"
#include "engine/ivdebugoverlay.h"
#include "decals.h"
#include "neo_gamerules.h"

ConVar sv_showimpacts( "sv_showimpacts", "0", FCVAR_REPLICATED, "Shows client (red) and server (blue) bullet impact point" );

#define MAX_PENETRATE_WALLS 9
#define MAX_PENETRATION_DISTANCE 12.0f

void DispatchEffect( const char *pName, const CEffectData &data );

CWeaponNEOBase* CNEOPlayer::NEOAnim_GetActiveWeapon()
{
	return GetActiveNEOWeapon();
}

bool CNEOPlayer::NEOAnim_CanMove()
{
	if ( IsObserver() )
		return false;

	if ( NEOGameRules()->IsInFreezePeriod() )
		return false;

	return true;
}

// Class variant goes from 0 to 2 and gibs from 0 to 7, 0 being the usual player model
const char* CNEOPlayer::GetPlayerModelString( int iTeamNum, int iClassNum, int iClassVariant, int iHitLocation )
{
	if ( (iTeamNum != TEAM_JINRAI && iTeamNum != TEAM_NSF)
		|| iClassNum > CLASS_SUPPORT || iClassVariant >= CLASSVARIANT_MAXCOUNT
		|| iHitLocation > 7 )
	{
		Assert( 0 ); // Whoopsies
		return NEO_TERROR_PLAYER_MODEL;
	}

	if ( iTeamNum == TEAM_JINRAI )
		return s_szJinraiModels[ iHitLocation + 7 * (iClassNum + iClassVariant + 2 * iClassNum) ]; // This stuff probably is a structure, whatever 
	else
		return s_szNSFModels[ iHitLocation + 7 * (iClassNum + iClassVariant + 2 * iClassNum) ];
}

// Original function has two string arrays for both teams, but they seem to be the same
const char* CNEOPlayer::GetGibletsString( int iClassNum, int iClassVariant, int iHitLocation )
{
	if ( iClassNum > CLASS_SUPPORT || iClassVariant >= CLASSVARIANT_MAXCOUNT
		|| iHitLocation > 7 )
	{
		Assert( 0 ); // Whoopsies
		return "";
	}

	return s_szGibletsModels[ iHitLocation + 7 * (iClassNum + iClassVariant + 2 * iClassNum) ];
}

int CNEOPlayer::GetMaxLoadoutIndex( int iTeamNum, int iClassType, int iRank, bool bUseDefault )
{
	NEOLoadout* pLoadout = nullptr;

	if ( bUseDefault )
		pLoadout = s_DefaultLoadout;
	else if ( iClassType == CLASS_RECON )
		pLoadout = s_ReconLoadout;
	else if ( iClassType == CLASS_ASSAULT )
		pLoadout = s_AssaultLoadout;
	else if ( iClassType == CLASS_SUPPORT )
		pLoadout = s_SupportLoadout;
	else
		return 0;

	int i = 0;

	for ( ; i < _ARRAYSIZE(pLoadout); i++ )
	{
		if ( pLoadout[ i ].szWeaponName == '\0' )
			break;

		if ( i >= iRank )
			return i;
	}

	return i;
}

const char* CNEOPlayer::GetLoadoutWeaponString( int iTeamNum, int iClassType, int iRank, int iSelectedLoadout, bool bUseDefault )
{
	if ( iSelectedLoadout > GetMaxLoadoutIndex( iTeamNum, iClassType, iRank ) )
		return "";	  

	NEOLoadout* pLoadout = nullptr;

	if ( bUseDefault )
		pLoadout = s_DefaultLoadout;
	else if ( iClassType == CLASS_RECON )
		pLoadout = s_ReconLoadout;
	else if ( iClassType == CLASS_ASSAULT )
		pLoadout = s_AssaultLoadout;
	else if ( iClassType == CLASS_SUPPORT )
		pLoadout = s_SupportLoadout;
	else
		return "";

	return pLoadout[ iSelectedLoadout ].szGiveWeaponName;
}	   

const char* CNEOPlayer::GetLoadoutAmmoType( int iTeamNum, int iClassType, int iRank, int iSelectedLoadout, bool bUseDefault )
{
	if ( iSelectedLoadout >= GetMaxLoadoutIndex( iTeamNum, iClassType, iRank ) )
		return "";

	NEOLoadout* pLoadout = nullptr;

	if ( bUseDefault )
		pLoadout = s_DefaultLoadout;
	else if ( iClassType == CLASS_RECON )
		pLoadout = s_ReconLoadout;
	else if ( iClassType == CLASS_ASSAULT )
		pLoadout = s_AssaultLoadout;
	else if ( iClassType == CLASS_SUPPORT )
		pLoadout = s_SupportLoadout;
	else
		return "";

	return pLoadout[ iSelectedLoadout ].szAmmoName;
}

int CNEOPlayer::GetLoadoutAmmoAmount( int iTeamNum, int iClassType, int iRank, int iSelectedLoadout, bool bUseDefault )
{
	if ( iSelectedLoadout >= GetMaxLoadoutIndex( iTeamNum, iClassType, iRank ) )
		return 0;

	NEOLoadout* pLoadout = nullptr;

	if ( bUseDefault )
		pLoadout = s_DefaultLoadout;
	else if ( iClassType == CLASS_RECON )
		pLoadout = s_ReconLoadout;
	else if ( iClassType == CLASS_ASSAULT )
		pLoadout = s_AssaultLoadout;
	else if ( iClassType == CLASS_SUPPORT )
		pLoadout = s_SupportLoadout;
	else
		return 0;

	return pLoadout[ iSelectedLoadout ].iAmmo;
}

static void GetMaterialParameters( int iMaterial, float &flPenetrationModifier )
{
	switch ( iMaterial )
	{
		case CHAR_TEX_METAL:
			flPenetrationModifier = 0.5;  // If we hit metal, reduce the thickness of the brush we can't penetrate
			break;
		case CHAR_TEX_DIRT:
			flPenetrationModifier = 0.2;
			break;
		case CHAR_TEX_CONCRETE:
			flPenetrationModifier = 0.3;
			break;
		case CHAR_TEX_GRATE:
			flPenetrationModifier = 0.75;
			break;
		case CHAR_TEX_VENT:
			flPenetrationModifier = 0.75;
			break;
		case CHAR_TEX_TILE:
			flPenetrationModifier = 0.5;
			break;
		case CHAR_TEX_COMPUTER:
			flPenetrationModifier = 0.5;
			break;
		case CHAR_TEX_WOOD:
			flPenetrationModifier = 0.75;
			break;
		case CHAR_TEX_SAND:
			flPenetrationModifier = 0.2;
			break;
		case CHAR_TEX_GLASS:
			flPenetrationModifier = 0.9;
			break;
		case CHAR_TEX_PLASTIC:
			flPenetrationModifier = 0.8;
			break;
		case CHAR_TEX_FOLIAGE:
			flPenetrationModifier = 0.8;
			break;
		case CHAR_TEX_FLESH:
			flPenetrationModifier = 0.6;
			break;
		default:
			flPenetrationModifier = 1.0;
			break;
	}

	Assert( flPenetrationModifier > 0 );
}

void CNEOPlayer::FireBullet( 
						   Vector vecSrc,	// shooting postion
						   const QAngle &shootAngles,  //shooting angle
						   float vecSpread, // spread vector
						   int iDamage, // base damage
						   int iBulletType, // ammo type
						   float fPenetration,
						   CBaseEntity *pevAttacker, // shooter
						   bool bDoEffects,	// create impact effect ?
						   float x,	// spread x factor
						   float y	// spread y factor
						   )
{
	float fCurrentDamage = iDamage;   // damage of the bullet at it's current trajectory
	float fCurrentPenetration = fPenetration;

	Vector vecDirShooting, vecRight, vecUp;
	AngleVectors( shootAngles, &vecDirShooting, &vecRight, &vecUp );

	if ( !pevAttacker )
		pevAttacker = this;  // the default attacker is ourselves

	// add the spray 
	Vector vecDir = vecDirShooting +
		x * vecSpread * vecRight +
		y * vecSpread * vecUp;

	VectorNormalize( vecDir );

	float flMaxRange = 8000;	  	

	Vector vecEnd = vecSrc + vecDir * flMaxRange; // max bullet range is 10000 units

#ifdef CLIENT_DLL
	if ( bDoEffects ) // It doesn't use this idk
	{
		trace_t tr;

		UTIL_TraceLine( vecSrc, vecEnd, MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX, this, COLLISION_GROUP_NONE, &tr );

		if ( tr.fraction == 1.0f )
		{ 
			UTIL_TraceLine( vecEnd, vecEnd + vecSrc, MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX, this, COLLISION_GROUP_NONE, &tr );
			UTIL_Tracer( vecEnd, tr.endpos, pevAttacker->entindex(), -1, 6500.0f, true );
		}
	}
#endif

	int	iCurrentPenetration = 0;

	while ( iCurrentPenetration <= MAX_PENETRATE_WALLS )
	{						
		trace_t tr; // main enter bullet trace

		UTIL_TraceLine( vecSrc, vecEnd, MASK_SOLID|CONTENTS_DEBRIS|CONTENTS_HITBOX, this, COLLISION_GROUP_NONE, &tr );
		   
		/************* MATERIAL DETECTION ***********/
		surfacedata_t* pSurfaceData = physprops->GetSurfaceData( tr.surface.surfaceProps );
		int iEnterMaterial = pSurfaceData->game.material;

		float fDistancePenetrated = 0.0f;
		float flPenetrationModifier = 1.0f;

		GetMaterialParameters( iEnterMaterial, flPenetrationModifier );					

		if ( sv_showimpacts.GetBool() )
		{
#ifdef CLIENT_DLL
			// draw red client impact markers
			debugoverlay->AddBoxOverlay( tr.endpos, Vector(-2,-2,-2), Vector(2,2,2), QAngle( 0, 0, 0), 255,0,0,127, 4 );

			if ( tr.m_pEnt && tr.m_pEnt->IsPlayer() )
			{
				C_BasePlayer *player = ToBasePlayer( tr.m_pEnt );
				player->DrawClientHitboxes( 4, true );
			}
#else
			// draw blue server impact markers
			NDebugOverlay::Box( tr.endpos, Vector(-2,-2,-2), Vector(2,2,2), 0,0,255,127, 4 );

			if ( tr.m_pEnt && tr.m_pEnt->IsPlayer() )
			{
				CBasePlayer *player = ToBasePlayer( tr.m_pEnt );
				player->DrawServerHitboxes( 4, true );
			}
#endif
		}

		int iDamageType = DMG_BULLET | DMG_NEVERGIB;

		if ( bDoEffects )
		{
			// See if the bullet ended up underwater + started out of the water
			if ( enginetrace->GetPointContents( tr.endpos ) & (CONTENTS_WATER | CONTENTS_SLIME) )
			{
				trace_t waterTrace;
				UTIL_TraceLine( vecSrc, tr.endpos, (MASK_SHOT | CONTENTS_WATER | CONTENTS_SLIME), this, COLLISION_GROUP_NONE, &waterTrace );

				if ( waterTrace.allsolid != 1 )
				{
					CEffectData	data;
					data.m_vOrigin = waterTrace.endpos;
					data.m_vNormal = waterTrace.plane.normal;
					data.m_flScale = random->RandomFloat( 8, 12 );

					if ( waterTrace.contents & CONTENTS_SLIME )
					{
						data.m_fFlags |= FX_WATER_IN_SLIME;
					}

					DispatchEffect( "gunshotsplash", data );
				}
			}
			else
			{
				//Do Regular hit effects

				// Don't decal nodraw surfaces
				if ( !(tr.surface.flags & (SURF_SKY | SURF_NODRAW | SURF_HINT | SURF_SKIP)) )
				{
					CBaseEntity *pEntity = tr.m_pEnt;
					if ( !(!friendlyfire.GetBool() && pEntity && pEntity->IsPlayer() && pEntity->GetTeamNumber() == GetTeamNumber()) )
					{
						UTIL_ImpactTrace( &tr, iDamageType );
					}
				}
			}
		} // bDoEffects

		if ( tr.fraction == 1.0f )
			return; // we didn't hit anything, stop tracing shoot

		bool bHitWall = true;

		float flPenetrationDistance = fCurrentPenetration * flPenetrationModifier;

		if ( flPenetrationDistance > MAX_PENETRATION_DISTANCE )
			flPenetrationDistance = MAX_PENETRATION_DISTANCE;

		trace_t tr2;

		UTIL_TraceLine( tr.endpos + vecDir, tr.endpos, MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX, this, COLLISION_GROUP_NONE, &tr2 );

		if ( tr2.fraction != 1.0f )
		{	
			float fDistance = (tr2.endpos - tr.endpos).Length();

			surfacedata_t* pSurfaceData = physprops->GetSurfaceData( tr2.surface.surfaceProps );
			int iEnterMaterial = pSurfaceData->game.material;

			fDistancePenetrated = fDistance * (1.0f / flPenetrationModifier);

			if ( bDoEffects )
			{ 
				UTIL_ImpactTrace( &tr2, iDamageType );

				if ( iEnterMaterial == CHAR_TEX_FLESH )
				{ 	
					UTIL_BloodDrips( tr2.endpos, vecDir, 247, iDamage );

					UTIL_TraceLine( tr2.endpos, tr2.endpos + vecDir * iDamage * 4.0f, CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW, this, COLLISION_GROUP_NONE, &tr2 );
											 
					if ( tr2.fraction != 1.0f )
						UTIL_BloodDecalTrace( &tr2, 247 );
				}
			}
		}
		else
		{
			bHitWall = false;
		}

		if ( fCurrentPenetration < 0 )
			fCurrentPenetration = 0;

		// damage get weaker of distance
		fCurrentDamage *= fCurrentPenetration / fPenetration;

		if ( fCurrentDamage < iDamage * 0.25f )
			fCurrentDamage = iDamage;

		// add damage to entity that we hit

#ifdef GAME_DLL
		ClearMultiDamage();

		CTakeDamageInfo info( pevAttacker, pevAttacker, fCurrentDamage, iDamageType );
		CalculateBulletDamageForce( &info, iBulletType, vecDir, tr.endpos );
		tr.m_pEnt->DispatchTraceAttack( info, vecDir, &tr );

		TraceAttackToTriggers( info, tr.startpos, tr.endpos, vecDir );

		ApplyMultiDamage();
#endif

		if ( bDoEffects )
		{
			UTIL_ImpactTrace( &tr, iDamageType );
		}

		fCurrentPenetration -= fDistancePenetrated;

		if ( fCurrentPenetration <= 0 )
			break;

		if ( tr.m_pEnt->IsPlayer() || bHitWall )
			break;

		iCurrentPenetration++;

		vecSrc = tr2.endpos;
		vecEnd = tr2.endpos + vecDir * flMaxRange;		
	}
}
