//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The NEO Game rules 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "neo_gamerules.h"
#include "ammodef.h"
#include "KeyValues.h"
#include "weapon_neobase.h"	  


#ifdef CLIENT_DLL


#else
	
	#include "voice_gamemgr.h"
	#include "team.h"
	#include "neo_player.h"
	#include "neo_game_config.h"
	#include "teamplayroundbased_gamerules.h"
	#include "eventqueue.h"
	#include "mapentities.h"
	#include "gameinterface.h"
	#include "neo_ghost_retrievalpoint.h"
	#include "weapon_ghost.h"

#endif

#define NEO_FREEZE_TIME		15.0f

ConVar neo_disable_tie( "neo_disable_tie", "0", 0, "!!!!" );
ConVar neo_round_timelimit( "neo_round_timelimit", "1.0", 0, "Round time-limit (minutes)." );
ConVar neo_score_limit( "neo_score_limit", "10.0", 0, "Score limit before map change(team score)." );
ConVar neo_restart_this( "neo_restart_this", "0", 0, "Restarts the game" );
ConVar neo_teamkill_punish( "neo_teamkill_punish", "1", 0, "Punishes teamkillers" );


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CViewVectors g_NEOViewVectors(
	Vector( 0, 0, 60 ),		// eye position

	Vector( -16, -16, 0 ),	// hull min
	Vector( 16, 16, 70 ),	// hull max

	Vector( -16, -16, 0 ),	// duck hull min
	Vector( 16, 16, 59 ),	// duck hull max
	Vector( 0, 0, 50 ),		// duck view

	Vector( -10, -10, -10 ),	// observer hull min
	Vector( 10, 10, 10 ),	// observer hull max

	Vector( 0, 0, 14 )		// dead view height
	);


#ifndef CLIENT_DLL
LINK_ENTITY_TO_CLASS(info_player_terrorist, CPointEntity);
LINK_ENTITY_TO_CLASS(info_player_counterterrorist,CPointEntity);
#endif

REGISTER_GAMERULES_CLASS( CNEOGameRules );


BEGIN_NETWORK_TABLE_NOBASE( CNEOGameRules, DT_NEOGameRules )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_iGameType ) ),
	RecvPropFloat( RECVINFO( m_fRoundTimeLeft ) ),
	RecvPropInt( RECVINFO( m_iRoundNumber ) ),
	RecvPropInt( RECVINFO( m_iGameState ) ),
	RecvPropInt( RECVINFO( m_iGameHud ) ),
	RecvPropInt( RECVINFO( m_iMVP ) ),
	RecvPropInt( RECVINFO( m_iAttackingTeam ) ),
	RecvPropBool( RECVINFO( m_bFreezePeriod ) ),
#else	 
	SendPropInt( SENDINFO( m_iGameType ) ),
	SendPropFloat( SENDINFO( m_fRoundTimeLeft ) ),
	SendPropInt( SENDINFO( m_iRoundNumber ) ),
	SendPropInt( SENDINFO( m_iGameState ) ),
	SendPropInt( SENDINFO( m_iGameHud ) ),
	SendPropInt( SENDINFO( m_iMVP ) ),
	SendPropInt( SENDINFO( m_iAttackingTeam ) ),
	SendPropBool( SENDINFO( m_bFreezePeriod ) ),
#endif
END_NETWORK_TABLE()


LINK_ENTITY_TO_CLASS( neo_gamerules, CNEOGameRulesProxy );
IMPLEMENT_NETWORKCLASS_ALIASED( NEOGameRulesProxy, DT_NEOGameRulesProxy )


#ifdef CLIENT_DLL
	void RecvProxy_NEOGameRules( const RecvProp *pProp, void **pOut, void *pData, int objectID )
	{
		CNEOGameRules *pRules = NEOGameRules();
		Assert( pRules );
		*pOut = pRules;
	}

	BEGIN_RECV_TABLE( CNEOGameRulesProxy, DT_NEOGameRulesProxy )
		RecvPropDataTable( "neo_gamerules_data", 0, 0, &REFERENCE_RECV_TABLE( DT_NEOGameRules ), RecvProxy_NEOGameRules )
	END_RECV_TABLE()
#else
	void *SendProxy_NEOGameRules( const SendProp *pProp, const void *pStructBase, const void *pData, CSendProxyRecipients *pRecipients, int objectID )
	{
		CNEOGameRules *pRules = NEOGameRules();
		Assert( pRules );
		pRecipients->SetAllRecipients();
		return pRules;
	}

	BEGIN_SEND_TABLE( CNEOGameRulesProxy, DT_NEOGameRulesProxy )
		SendPropDataTable( "neo_gamerules_data", 0, &REFERENCE_SEND_TABLE( DT_NEOGameRules ), SendProxy_NEOGameRules )
	END_SEND_TABLE()
#endif


#ifdef CLIENT_DLL


#else			 
	// Falling damage stuff.
	#define NEO_PLAYER_FATAL_FALL_SPEED		1024	// approx 60 feet
	#define NEO_PLAYER_MAX_SAFE_FALL_SPEED	580		// approx 20 feet
	#define NEO_DAMAGE_FOR_FALL_SPEED		((float)100 / ( NEO_PLAYER_FATAL_FALL_SPEED - NEO_PLAYER_MAX_SAFE_FALL_SPEED )) // damage per unit per second.

	#define NEO_NAME_CHANGE_DELAY 5.0f // Time between name changes

	// These entities are preserved each round restart. The rest are removed and recreated.
	static const char *s_PreserveEnts[] =
	{
		"ai_network",
		"ai_hint",
		"hl2mp_gamerules",
		"team_manager",
		"player_manager",
		"env_soundscape",
		"env_soundscape_proxy",
		"env_soundscape_triggerable",
		"env_sun",
		"env_wind",
		"env_fog_controller",
		"func_brush",
		"func_wall",
		"func_buyzone",
		"func_illusionary",
		"infodecal",
		"info_projecteddecal",
		"info_node",
		"info_target",
		"info_node_hint",
		"info_player_deathmatch",
		"info_player_combine",
		"info_player_rebel",
		"info_map_parameters",
		"keyframe_rope",
		"move_rope",
		"info_ladder",
		"player",
		"point_viewcontrol",
		"scene_manager",
		"shadow_control",
		"sky_camera",
		"soundent",
		"trigger_soundscape",
		"viewmodel",
		"predicted_viewmodel",
		"worldspawn",
		"point_devshot_camera",
		"neo_gamerules",
		"neo_team_manager",
		"neo_viewmodel",
		"info_player_attacker",
		"info_player_defender",
		"neo_bloom_controller",
		"neo_game_config",
		"weapon_ghost",
		"neo_ghost_retrieval_point",
		"neo_mobilearmor_base",
		"neo_ghostspawnpoint",
		"neo_escape_point",
		"neo_controlpoint",
		"", // END Marker
	};

	// --------------------------------------------------------------------------------------------------- //
	// Voice helper
	// --------------------------------------------------------------------------------------------------- //

	class CVoiceGameMgrHelper : public IVoiceGameMgrHelper
	{
	public:
		virtual bool		CanPlayerHearPlayer( CBasePlayer *pListener, CBasePlayer *pTalker, bool &bProximity )
		{
			// Dead players can only be heard by other dead team mates
			if ( pTalker->IsAlive() == false )
			{
				if ( pListener->IsAlive() == false )
					return ( pListener->InSameTeam( pTalker ) );

				return false;
			}

			return ( pListener->InSameTeam( pTalker ) );
		}
	};
	CVoiceGameMgrHelper g_VoiceGameMgrHelper;
	IVoiceGameMgrHelper *g_pVoiceGameMgrHelper = &g_VoiceGameMgrHelper;



	// --------------------------------------------------------------------------------------------------- //
	// Globals.
	// --------------------------------------------------------------------------------------------------- //

	// NOTE: the indices here must match TEAM_TERRORIST, TEAM_CT, TEAM_SPECTATOR, etc.
	char *sTeamNames[] =
	{
		"Unassigned",
		"Spectator",
		"Jinrai",
		"NSF"
	};


	// --------------------------------------------------------------------------------------------------- //
	// Global helper functions.
	// --------------------------------------------------------------------------------------------------- //

	// World.cpp calls this but we don't use it in SDK.
	void InitBodyQue()
	{
	}


	// --------------------------------------------------------------------------------------------------- //
	// CNEOGameRules implementation.
	// --------------------------------------------------------------------------------------------------- //

	CNEOGameRules::CNEOGameRules()
	{
		// Create the team managers
		for ( int i = 0; i < ARRAYSIZE( sTeamNames ); i++ )
		{
			CTeam *pTeam = static_cast<CTeam*>(CreateEntityByName( "neo_team_manager" ));
			pTeam->Init( sTeamNames[i], i );

			g_Teams.AddToTail( pTeam );
		}

		CreateEntityByName( "neo_gamerules" );

		m_Unknown684 = 0.0f;
		m_bDeterminedGameType = false;
		m_fLastRoundStartTime = gpGlobals->curtime;
		m_fRoundTimeLeft = 15.0f;
		m_iRoundNumber = -1;
		m_bUnknown688 = false;
		m_Unknown620 = 0;
		m_Unknown624 = false;
		m_bShouldChangeMap = false;
		m_bFreezePeriod = false;
		m_Unknown612 = gpGlobals->curtime;
		m_Unknown616 = 3600.0f;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	//-----------------------------------------------------------------------------
	CNEOGameRules::~CNEOGameRules()
	{
		// Note, don't delete each team since they are in the gEntList and will 
		// automatically be deleted from there, instead.
		g_Teams.Purge();
	}

	//-----------------------------------------------------------------------------
	// Purpose: Neotokyo Specific Client Commands
	// Input  :
	// Output :
	//-----------------------------------------------------------------------------
	bool CNEOGameRules::ClientCommand( CBaseEntity *pEdict, const CCommand &args )
	{
		CNEOPlayer* pNeoPlayer = ToNEOPlayer( pEdict );

		if ( BaseClass::ClientCommand( pEdict, args ) )
			return true;
		else
			return pNeoPlayer->ClientCommand( args );
	}

	//-----------------------------------------------------------------------------
	// Purpose: Player has just spawned. Equip them.
	//-----------------------------------------------------------------------------

	void CNEOGameRules::RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore )
	{
		RadiusDamage( info, vecSrcIn, flRadius, iClassIgnore, false );
	}

	// Add the ability to ignore the world trace
	void CNEOGameRules::RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore, bool bIgnoreWorld )
	{
		CBaseEntity *pEntity = NULL;
		trace_t		tr;
		float		flAdjustedDamage, falloff;
		Vector		vecSpot;
		Vector		vecToTarget;
		Vector		vecEndPos;

		Vector vecSrc = vecSrcIn;

		if ( flRadius )
			falloff = info.GetDamage() / flRadius;
		else
			falloff = 1.0;

		int bInWater = (UTIL_PointContents ( vecSrc ) & MASK_WATER) ? true : false;
		
		vecSrc.z += 1;// in case grenade is lying on the ground

		// iterate on all entities in the vicinity.
		for ( CEntitySphereQuery sphere( vecSrc, flRadius ); ( pEntity = sphere.GetCurrentEntity() ) != NULL; sphere.NextEntity() )
		{
			if ( pEntity->m_takedamage != DAMAGE_NO )
			{
				// UNDONE: this should check a damage mask, not an ignore
				if ( iClassIgnore != CLASS_NONE && pEntity->Classify() == iClassIgnore )
				{// houndeyes don't hurt other houndeyes with their attack
					continue;
				}

				// blast's don't tavel into or out of water
				if (bInWater && pEntity->GetWaterLevel() == 0)
					continue;
				if (!bInWater && pEntity->GetWaterLevel() == 3)
					continue;

				// radius damage can only be blocked by the world
				vecSpot = pEntity->BodyTarget( vecSrc );



				bool bHit = false;

				if( bIgnoreWorld )
				{
					vecEndPos = vecSpot;
					bHit = true;
				}
				else
				{
					UTIL_TraceLine( vecSrc, vecSpot, MASK_SOLID_BRUSHONLY, info.GetInflictor(), COLLISION_GROUP_NONE, &tr );

					if (tr.startsolid)
					{
						// if we're stuck inside them, fixup the position and distance
						tr.endpos = vecSrc;
						tr.fraction = 0.0;
					}

					vecEndPos = tr.endpos;

					if( tr.fraction == 1.0 || tr.m_pEnt == pEntity )
					{
						bHit = true;
					}
				}

				if ( bHit )
				{
					// the explosion can 'see' this entity, so hurt them!
					//vecToTarget = ( vecSrc - vecEndPos );
					vecToTarget = ( vecEndPos - vecSrc );

					// decrease damage for an ent that's farther from the bomb.
					flAdjustedDamage = vecToTarget.Length() * falloff;
					flAdjustedDamage = info.GetDamage() - flAdjustedDamage;
				
					if ( flAdjustedDamage > 0 )
					{
						CTakeDamageInfo adjustedInfo = info;
						adjustedInfo.SetDamage( flAdjustedDamage );

						Vector dir = vecToTarget;
						VectorNormalize( dir );

						// If we don't have a damage force, manufacture one
						if ( adjustedInfo.GetDamagePosition() == vec3_origin || adjustedInfo.GetDamageForce() == vec3_origin )
						{
							CalculateExplosiveDamageForce( &adjustedInfo, dir, vecSrc, 1.5	/* explosion scale! */ );
						}
						else
						{
							// Assume the force passed in is the maximum force. Decay it based on falloff.
							float flForce = adjustedInfo.GetDamageForce().Length() * falloff;
							adjustedInfo.SetDamageForce( dir * flForce );
							adjustedInfo.SetDamagePosition( vecSrc );
						}

						pEntity->TakeDamage( adjustedInfo );
			
						// Now hit all triggers along the way that respond to damage... 
						pEntity->TraceAttackToTriggers( adjustedInfo, vecSrc, vecEndPos, dir );
					}
				}
			}
		}
	}

	void CNEOGameRules::DetermineGametype()
	{
		DevMsg( "CNEOGameRules::DetermineGametype\n" );

		if ( m_iGameState != 1 )
			m_iGameState = 1;

		if ( m_iAttackingTeam != TEAM_JINRAI )
			m_iAttackingTeam = TEAM_JINRAI;

		if ( m_iRoundNumber > 0 )
			m_iRoundNumber = 0;

		if ( m_fRoundTimeLeft != 15.0f )
			m_fRoundTimeLeft = 15.0f;

		if ( NeoGameConfig() )
		{
			int gameType = NeoGameConfig()->GetGameType();

			if ( gameType < 0 || gameType >= 3 )
				gameType = 0;

			m_iGameType = gameType;
		}
		else if ( m_iGameType > GAMETYPE_TDM )
		{
			m_iGameType = GAMETYPE_TDM;
		}

		m_iLastAttackerSpawn = 0;
		m_iLastDefenderSpawn = 0;

		if ( m_iGameType )
		{
			m_iLastGhostSpawnPointIndex = 0;
			UpdateAttackerTeam();
		}
	}

	void CNEOGameRules::RestartGame()
	{
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CNEOPlayer* pPlayer = (CNEOPlayer*) UTIL_PlayerByIndex( i );

			if ( !pPlayer || !pPlayer->IsPlayer() )
				continue;

			pPlayer->m_iRank = RANK_PRIVATE;
			pPlayer->CommitSuicide();
		}

		GetGlobalTeam( TEAM_JINRAI )->SetScore( 0 );
		GetGlobalTeam( TEAM_NSF )->SetScore( 0 );

		m_iGameState = 1;
		m_fRoundTimeLeft = 0.0f;
	}  	

	void CNEOGameRules::RoundStart()
	{
		CleanupMap();

		IGameEvent* pEvent = gameeventmanager->CreateEvent( "game_round_start" );

		if ( pEvent )
			gameeventmanager->FireEvent( pEvent );

		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CNEOPlayer* pPlayer = (CNEOPlayer*) UTIL_PlayerByIndex( i );

			if ( !pPlayer || !pPlayer->IsPlayer() )
				continue;

			pPlayer->m_iLives = 1;

			if ( pPlayer->State_Get() == STATE_OBSERVERMODE && pPlayer->GetTeamNumber() != TEAM_SPECTATOR )
				pPlayer->State_Transition( STATE_PICKINGCLASS );
			else if ( pPlayer->State_Get() == STATE_ACTIVE )
				pPlayer->PickLoadout();
		}
	}

	void CNEOGameRules::PlayGhostPing()
	{	 
		float v6 = m_Unknown616 / 1800.0f * 10.0f;

		if ( v6 < 1.0f )
			v6 = 1.0f;

		if ( m_Unknown616 > 1800.0f || m_Unknown612 + v6 >= gpGlobals->curtime )
		{
			m_Unknown616 = 3600.0f;
			return;
		}

		UTIL_EmitAmbientSound( engine->IndexOfEdict( GetGhost()->edict() ), GetGhost()->GetAbsOrigin(), "NeoPlayer.GhostPing", (1.0f - (m_Unknown612 + v6) / 10.0f) * 0.75, SNDLVL_NORM, 0, PITCH_NORM );
	}

	bool CNEOGameRules::IsRoundInProgress()
	{
		return (m_iGameState == 2 && m_Unknown648 - m_fRoundTimeLeft > 30.0f) || m_iGameState == 3;
	}

	void CNEOGameRules::CheckRoundWinner()
	{
		if ( m_iGameType == GAMETYPE_TDM )
		{
			int iAliveJinrai = 0;
			int iAliveNsf = 0;

			for ( int i = 1; i <= gpGlobals->maxClients; i++ )
			{
				CNEOPlayer* pPlayer = (CNEOPlayer*) UTIL_PlayerByIndex( i );

				if ( !pPlayer || !pPlayer->IsPlayer() || pPlayer->m_iLives <= 0 )
					continue;

				if ( pPlayer->GetTeamNumber() == TEAM_JINRAI )
					iAliveJinrai++;
				else if ( pPlayer->GetTeamNumber() == TEAM_NSF )
					iAliveNsf++;
			}

			if ( iAliveJinrai > 0 && iAliveNsf > 0 )
			{
				if ( m_fRoundTimeLeft == 0.0f )
				{	 					
					if ( iAliveJinrai > iAliveNsf )
					{
						m_iGameHud = 4;
						GetGlobalTeam( TEAM_JINRAI )->AddScore( 1 );
					}			 
					else if ( iAliveJinrai < iAliveNsf )
					{
						m_iGameHud = 5;
						GetGlobalTeam( TEAM_NSF )->AddScore( 1 );
					}	  
					else if ( !neo_disable_tie.GetBool() )
					{
						m_iGameHud = 3;
					}		  					
				}
			}
			else
			{
				if ( iAliveJinrai > 0 && iAliveNsf == 0 )
				{
					m_iGameHud = 4;
					GetGlobalTeam( TEAM_JINRAI )->AddScore( 1 );
				}
				else if ( iAliveNsf > 0 && iAliveJinrai == 0 )
				{
					m_iGameHud = 5;
					GetGlobalTeam( TEAM_NSF )->AddScore( 1 );
				}
			}

			m_iGameState = 3;
			m_fRoundTimeLeft = 15.0f;
		}
		else if ( m_iGameType == GAMETYPE_CTG )
		{
			if ( GetNumberOfGhosts() > 0 )
			{
				CWeaponGhost* pGhost = GetGhost();
				Vector vecGhostOrigin = pGhost->GetAbsOrigin();

				CNEOPlayer* pPlayer = pGhost->GetPlayerOwner();
				int iTeamNum = pPlayer->GetTeamNumber();

				if ( iTeamNum > TEAM_SPECTATOR )
				{
					for ( int i = 0; i < GetNumberOfGhostRetrievalPoints(); i++ )
					{
						CNeoGhostRetrievalPoint* pRetrievalPoint = GetGhostRetrievalPoint( i );

						if ( pRetrievalPoint->GetOwningTeamNumber() == iTeamNum )
						{
							Vector vecDelta = pRetrievalPoint->GetAbsOrigin() - vecGhostOrigin;

							if ( vecDelta.Length() < pRetrievalPoint->GetRadius() )
							{
								m_iGameState = 3;

								if ( iTeamNum == TEAM_JINRAI )
									m_iGameHud = 4;
								else if ( iTeamNum == TEAM_NSF )
									m_iGameHud = 5;

								m_fRoundTimeLeft = 15.0f;
								GetGlobalTeam( iTeamNum )->AddScore( 1 );
								m_iMVP = engine->IndexOfEdict( pPlayer->edict() );

								for ( int j = 0; j <= gpGlobals->maxClients; j++ )
								{
									CNEOPlayer* pPlaya = (CNEOPlayer*) UTIL_PlayerByIndex( j );

									if ( !pPlaya || !pPlaya->IsPlayer() )
										continue;

									if ( pPlaya->GetTeamNumber() == iTeamNum )
									{
										if ( pPlaya->m_iLives > 0 )
											pPlaya->m_iRank++;
										else
											pPlaya->IncrementFragCount( 1 );
									}
								}
							}
						}
					}
				}
			}

			if ( m_iGameState == 3 )
				return;

			int iAliveJinrai = 0;
			int iAliveNsf = 0;

			for ( int i = 1; i <= gpGlobals->maxClients; i++ )
			{
				CNEOPlayer* pPlayer = (CNEOPlayer*) UTIL_PlayerByIndex( i );

				if ( !pPlayer || !pPlayer->IsPlayer() || pPlayer->m_iLives <= 0 )
					continue;

				if ( pPlayer->GetTeamNumber() == TEAM_JINRAI )
					iAliveJinrai++;
				else if ( pPlayer->GetTeamNumber() == TEAM_NSF )
					iAliveNsf++;
			}

			if ( iAliveJinrai > 0 && iAliveNsf > 0 )
			{
				if ( m_fRoundTimeLeft == 0.0f )
				{
					if ( iAliveJinrai > iAliveNsf )
					{
						m_iGameHud = 4;
						GetGlobalTeam( TEAM_JINRAI )->AddScore( 1 );

						for ( int i = 1; i <= gpGlobals->maxClients; i++ )
						{
							CNEOPlayer* pPlayer = (CNEOPlayer*) UTIL_PlayerByIndex( i );

							if ( pPlayer && pPlayer->IsPlayer() && pPlayer->m_iLives > 0 && pPlayer->GetTeamNumber() == TEAM_JINRAI )
								pPlayer->IncrementFragCount( 1 );
						}
					}
					else if ( iAliveJinrai < iAliveNsf )
					{
						m_iGameHud = 5;
						GetGlobalTeam( TEAM_NSF )->AddScore( 1 );

						for ( int i = 1; i <= gpGlobals->maxClients; i++ )
						{
							CNEOPlayer* pPlayer = (CNEOPlayer*) UTIL_PlayerByIndex( i );

							if ( pPlayer && pPlayer->IsPlayer() && pPlayer->m_iLives > 0 && pPlayer->GetTeamNumber() == TEAM_NSF )
								pPlayer->IncrementFragCount( 1 );
						}
					}
					else if ( !neo_disable_tie.GetBool() )
					{
						m_iGameHud = 3;
					}
				} 					
			}	
			else if ( iAliveNsf > 0 )
			{
				m_iGameHud = 5;
				GetGlobalTeam( TEAM_NSF )->AddScore( 1 );

				for ( int i = 1; i <= gpGlobals->maxClients; i++ )
				{
					CNEOPlayer* pPlayer = (CNEOPlayer*) UTIL_PlayerByIndex( i );

					if ( pPlayer && pPlayer->IsPlayer() && pPlayer->m_iLives > 0 && pPlayer->GetTeamNumber() == TEAM_NSF )
						pPlayer->IncrementFragCount( 1 );
				}
			}
			else
			{
				m_iGameHud = 4;
				GetGlobalTeam( TEAM_JINRAI )->AddScore( 1 );

				for ( int i = 1; i <= gpGlobals->maxClients; i++ )
				{
					CNEOPlayer* pPlayer = (CNEOPlayer*) UTIL_PlayerByIndex( i );

					if ( pPlayer && pPlayer->IsPlayer() && pPlayer->m_iLives > 0 && pPlayer->GetTeamNumber() == TEAM_JINRAI )
						pPlayer->IncrementFragCount( 1 );
				}
			}

			m_iGameState = 3;
			m_fRoundTimeLeft = 15.0f;
		}
		else if ( m_iGameType == GAMETYPE_VIP )
		{
			m_iGameState = 3;
			m_fRoundTimeLeft = 15.0f;
		}
	}

	void CNEOGameRules::CheckScoreLimit()
	{
		if ( GetGlobalTeam( TEAM_JINRAI )->GetScore() > neo_score_limit.GetInt()
			|| GetGlobalTeam( TEAM_NSF )->GetScore() > neo_score_limit.GetInt() )
		{
			UTIL_ClientPrintAll( HUD_PRINTTALK, "Changing level..." );
			m_bShouldChangeMap = true;
		}
	}

	void CNEOGameRules::StartNewRound()
	{
		m_fRoundTimeLeft -= (gpGlobals->curtime - m_fLastRoundStartTime);

		if ( m_fRoundTimeLeft < 0.0f )
			m_fRoundTimeLeft = 0.0f;

		if ( m_bFreezePeriod && neo_round_timelimit.GetFloat() * 60 - NEO_FREEZE_TIME > m_fRoundTimeLeft )
		{
			m_bFreezePeriod = false;
			UTIL_ClientPrintAll( HUD_PRINTCENTER, "- GO! GO! GO! -" );
		}

		for ( int i = 0; i < m_ControlPoints.Count(); i++ )
			m_ControlPoints[ i ]->UpdatePointOwner();

		if ( m_iGameType == GAMETYPE_TDM )
		{
			if ( m_iGameState == 1 )
			{
				if ( m_fRoundTimeLeft == 0.0f )
				{
					if ( GetGlobalTeam( TEAM_JINRAI )->HasPlayers() && GetGlobalTeam( TEAM_NSF )->HasPlayers() )
					{
						RoundStart();

						m_Unknown648 = m_fRoundTimeLeft = neo_round_timelimit.GetFloat() * 60;
						m_bUnknown688 = true;
						m_iGameState = 2;
						m_iGameHud = 0;
						m_iRoundNumber = 0;

						char szRoundNum[ 64 ];
						V_snprintf( szRoundNum, sizeof( szRoundNum ), "%d", m_iRoundNumber );
						UTIL_ClientPrintAll( HUD_PRINTCENTER, "- ROUND %s1 STARTED -", szRoundNum );
					}  
					else
					{
						m_fRoundTimeLeft = 15.0f;
						UTIL_ClientPrintAll( HUD_PRINTCENTER, "- NEW ROUND START DELAYED - ONE OR BOTH TEAMS HAS NO PLAYERS -" );
					}
				}					
			}  
			else if ( m_iGameState == 3 )
			{
				if ( m_fRoundTimeLeft == 0 )
				{
					IGameEvent* pEvent = gameeventmanager->CreateEvent( "game_round_end" );

					if ( pEvent )
						gameeventmanager->FireEvent( pEvent );

					if ( m_bShouldChangeMap )
					{
						m_bShouldChangeMap = false;
						ChangeToNextMapCycleMap();	  	
						return;
					}

					if ( GetGlobalTeam( TEAM_JINRAI )->HasPlayers() && GetGlobalTeam( TEAM_NSF )->HasPlayers() )
					{
						SpawnGhost();
						RoundStart();

						m_Unknown648 = m_fRoundTimeLeft = neo_round_timelimit.GetFloat() * 60.0f;
						m_bUnknown688 = true;
						m_iGameState = 2;
						m_iGameHud = 0;
						m_iRoundNumber = 0;

						char szRoundNum[ 64 ];
						V_snprintf( szRoundNum, sizeof( szRoundNum ), "%d", m_iRoundNumber );
						UTIL_ClientPrintAll( HUD_PRINTCENTER, "- ROUND %s1 STARTED -", szRoundNum );
					}
					else
					{
						m_iGameState = 1; // warmup?
						m_fRoundTimeLeft = 15.0f;
						UTIL_ClientPrintAll( HUD_PRINTCENTER, "- NEW ROUND START DELAYED - ONE OR BOTH TEAMS HAS NO PLAYERS -" );
					}	
				}					
			}
		}	
		else if ( m_iGameType == GAMETYPE_CTG )
		{
			if ( m_iGameState == 1 )
			{
				if ( m_fRoundTimeLeft == 0.0f )
				{
					if ( GetGlobalTeam( TEAM_JINRAI )->HasPlayers() && GetGlobalTeam( TEAM_NSF )->HasPlayers() )
					{
						RoundStart();
						SpawnGhost();

						m_Unknown648 = m_fRoundTimeLeft = neo_round_timelimit.GetFloat() * 60;
						m_bUnknown688 = true;
						m_iGameState = 2;
						m_iGameHud = 0;
						m_iRoundNumber++;

						char szRoundNum[ 64 ];
						V_snprintf( szRoundNum, sizeof( szRoundNum ), "%d", m_iRoundNumber );
						UTIL_ClientPrintAll( HUD_PRINTCENTER, "- CTG ROUND %s1 STARTED -", szRoundNum );
					}
					else
					{
						m_iGameState = 1;
						m_fRoundTimeLeft = 15.0f;
						UTIL_ClientPrintAll( HUD_PRINTCENTER, "- NEW ROUND START DELAYED - ONE OR BOTH TEAMS HAS NO PLAYERS -" );
					}
				}
			}
			else if ( m_iGameState == 2 )
			{	
				PlayGhostPing();
				CheckRoundWinner();
				CheckScoreLimit();
			}
			else if ( m_iGameState == 3 )
			{
				if ( m_fRoundTimeLeft == 0 )
				{
					IGameEvent* pEvent = gameeventmanager->CreateEvent( "game_round_end" );

					if ( pEvent )
						gameeventmanager->FireEvent( pEvent );

					if ( m_bShouldChangeMap )
					{
						m_bShouldChangeMap = false;
						ChangeToNextMapCycleMap();
						return;
					}

					if ( GetGlobalTeam( TEAM_JINRAI )->HasPlayers() && GetGlobalTeam( TEAM_NSF )->HasPlayers() )
					{
						SpawnGhost();
						RoundStart();

						m_Unknown648 = m_fRoundTimeLeft = neo_round_timelimit.GetFloat() * 60.0f;
						m_bUnknown688 = true;
						m_iGameState = 2;
						m_iGameHud = 0;
						m_iRoundNumber = 0;

						char szRoundNum[ 64 ];
						V_snprintf( szRoundNum, sizeof( szRoundNum ), "%d", m_iRoundNumber );
						UTIL_ClientPrintAll( HUD_PRINTCENTER, "- CTG ROUND %s1 STARTED -", szRoundNum );
					}
					else
					{
						m_iGameState = 1; // warmup?
						m_fRoundTimeLeft = 15.0f;
						UTIL_ClientPrintAll( HUD_PRINTCENTER, "- NEW ROUND START DELAYED - ONE OR BOTH TEAMS HAS NO PLAYERS -" );
					}
				}
			}
		}
		else if ( m_iGameType == GAMETYPE_VIP )
		{
			if ( m_iGameState == 1 )
			{
				if ( m_fRoundTimeLeft == 0.0f )
				{
					if ( GetGlobalTeam( TEAM_JINRAI )->HasPlayers() && GetGlobalTeam( TEAM_NSF )->HasPlayers() )
					{
						RoundStart();

						m_Unknown648 = m_fRoundTimeLeft = neo_round_timelimit.GetFloat() * 60;
						m_bUnknown688 = true;
						m_iGameState = 2;
						m_iGameHud = 0;
						m_iRoundNumber = 0;

						char szRoundNum[ 64 ];
						V_snprintf( szRoundNum, sizeof( szRoundNum ), "%d", m_iRoundNumber );
						UTIL_ClientPrintAll( HUD_PRINTCENTER, "- VIP ROUND %s1 STARTED -", szRoundNum );
					}
					else
					{
						m_iGameState = 1;
						m_fRoundTimeLeft = 15.0f;
						UTIL_ClientPrintAll( HUD_PRINTCENTER, "- NEW ROUND START DELAYED - ONE OR BOTH TEAMS HAS NO PLAYERS -" );
					}
				}
			}
		}

		m_fLastRoundStartTime = gpGlobals->curtime;
	}

	void CNEOGameRules::Think()
	{
		BaseClass::Think();

		if ( !m_bDeterminedGameType )
		{
			DetermineGametype();
			m_bDeterminedGameType = true;
		}

		if ( neo_restart_this.GetBool() )
		{		   
			neo_restart_this.SetValue( false );	
			RestartGame();
		}

		StartNewRound();

		if ( m_bUnknown688 && (m_Unknown648 - m_fRoundTimeLeft) > 30.0f)
		{		   
			m_bUnknown688 = false;

			for ( int i = 1; i <= gpGlobals->maxClients; i++ )
			{
				CNEOPlayer* pPlayer = (CNEOPlayer*) UTIL_PlayerByIndex( i );

				if ( !pPlayer || !pPlayer->IsPlayer() )
					continue;

				if ( pPlayer->GetTeamNumber() == TEAM_JINRAI || pPlayer->GetTeamNumber() == TEAM_NSF )
				{
					if ( pPlayer->State_Get() == STATE_PICKINGTEAM
						|| pPlayer->State_Get() == STATE_PICKINGCLASS 
						|| pPlayer->State_Get() == STATE_PICKINGLOADOUT )
						if ( pPlayer->m_iLives > 0 )
							pPlayer->GetIntoGame();
				}
			}
		}

		if ( gpGlobals->curtime >= m_Unknown684 )
		{
			for ( int i = 1; i <= gpGlobals->maxClients; i++ )
			{	
				CNEOPlayer* pPlayer = (CNEOPlayer*) UTIL_PlayerByIndex( i );

				if ( !pPlayer || !pPlayer->IsPlayer() )
					continue;

				if ( pPlayer->CanSpawn() )
					pPlayer->GetIntoGame();
			}
		}
		else
		{
			if ( m_Unknown684 > gpGlobals->curtime + 2.0f )
				m_Unknown684 = gpGlobals->curtime + 1.0f;
		}
	}

	void CNEOGameRules::LevelShutdown()
	{
		DevMsg( "Server CNEOGameRules::LevelShutdown\n" );

		m_ControlPoints.RemoveAll();
	}

	float CNEOGameRules::FlPlayerFallDamage( CBasePlayer *pPlayer )
	{
		pPlayer->m_Local.m_flFallVelocity -= NEO_PLAYER_MAX_SAFE_FALL_SPEED;
		return pPlayer->m_Local.m_flFallVelocity * NEO_DAMAGE_FOR_FALL_SPEED * 0.25;
	}

	void CNEOGameRules::PlayerSpawn( CBasePlayer *pPlayer )
	{
		pPlayer->EquipSuit();
	}	 	

	void CNEOGameRules::ClientSettingsChanged( CBasePlayer *pPlayer )
	{
		static float flLastNameChangeTime = 0.0f;

		if ( flLastNameChangeTime >= gpGlobals->curtime + NEO_NAME_CHANGE_DELAY )
			return;

		flLastNameChangeTime = gpGlobals->curtime;

		const char *pszName = engine->GetClientConVarValue( pPlayer->entindex(), "name" );

		const char *pszOldName = pPlayer->GetPlayerName();

		// msg everyone if someone changes their name,  and it isn't the first time (changing no name to current name)
		// Note, not using FStrEq so that this is case sensitive
		if ( pszOldName[ 0 ] != 0 && Q_strcmp( pszOldName, pszName ) )
		{
			IGameEvent * event = gameeventmanager->CreateEvent( "player_changename" );
			if ( event )
			{
				event->SetInt( "userid", pPlayer->GetUserID() );
				event->SetString( "oldname", pszOldName );
				event->SetString( "newname", pszName );
				gameeventmanager->FireEvent( event );
			}

			pPlayer->SetPlayerName( pszName );
		}
	}

	int CNEOGameRules::IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
	{
		if ( !pKilled )
			return 0;

		if ( !pAttacker )
			return 1;

		char banIdBuffer[ 1024 ];

		if ( pAttacker->GetTeamNumber() == TEAM_SPECTATOR )
		{
			V_snprintf( banIdBuffer, sizeof( banIdBuffer ), "banid 666 %d kick\n", pAttacker->GetUserID() );
			engine->ServerCommand( banIdBuffer );
		}

		if ( pAttacker != pKilled && PlayerRelationship( pAttacker, pKilled ) == GR_TEAMMATE && neo_teamkill_punish.GetBool() )
		{
			V_snprintf( banIdBuffer, sizeof( banIdBuffer ), "banid 1 %d kick\n", pAttacker->GetUserID() );
			engine->ServerCommand( banIdBuffer );
			return -1;
		}

		return 1;
	}

	void CNEOGameRules::DeathNotice( CBasePlayer *pVictim, const CTakeDamageInfo &info )
	{
		// Work out what killed the player, and send a message to all clients about it
		const char *killer_weapon_name = "world";		// by default, the player is killed by the world
		int killer_ID = 0;

		// Find the killer & the scorer
		CBaseEntity *pInflictor = info.GetInflictor();
		CBaseEntity *pKiller = info.GetAttacker();
		CBasePlayer *pScorer = GetDeathScorer( pKiller, pInflictor );

		// Custom kill type?
		if ( info.GetDamageCustom() )
		{
			killer_weapon_name = GetDamageCustomString( info );
			if ( pScorer )
			{
				killer_ID = pScorer->GetUserID();
			}
		}
		else
		{
			// Is the killer a client?
			if ( pScorer )
			{
				killer_ID = pScorer->GetUserID();

				if ( pInflictor )
				{
					if ( pInflictor == pScorer )
					{
						// If the inflictor is the killer,  then it must be their current weapon doing the damage
						if ( pScorer->GetActiveWeapon() )
						{
							killer_weapon_name = pScorer->GetActiveWeapon()->GetClassname();
						}
					}
					else
					{
						killer_weapon_name = pInflictor->GetClassname();  // it's just that easy
					}
				}
			}
			else
			{
				killer_weapon_name = pInflictor->GetClassname();
			}

			// strip the NPC_* or weapon_* from the inflictor's classname
			if ( strncmp( killer_weapon_name, "weapon_", 7 ) == 0 )
			{
				killer_weapon_name += 7;
			}
			else if ( strncmp( killer_weapon_name, "npc_", 4 ) == 0 )
			{
				killer_weapon_name += 4;
			}
			else if ( strncmp( killer_weapon_name, "func_", 5 ) == 0 )
			{
				killer_weapon_name += 5;
			}						  
		}

		IGameEvent *event = gameeventmanager->CreateEvent( "player_death" );
		if ( event )
		{
			event->SetInt( "userid", pVictim->GetUserID() );
			event->SetInt( "attacker", killer_ID );
			event->SetString( "weapon", killer_weapon_name );	

			int icon = 1;

			if ( info.IsHeadshot() )
				icon = 2;
			else if ( info.GetDamageType() & DMG_BLAST )
				icon = 4;

			event->SetInt( "icon", icon );
			event->SetInt( "priority", 7 );
			gameeventmanager->FireEvent( event );
		}
	}

	const char* CNEOGameRules::GetChatPrefix( bool bTeamOnly, CBasePlayer *pPlayer )
	{
		char *pszPrefix = nullptr;

		if ( !pPlayer )  // dedicated server output
		{
			pszPrefix = "[G33K]"; // what a great meme
		}
		else if ( pPlayer->GetTeamNumber() == TEAM_SPECTATOR )
		{
			pszPrefix = "[Spectator]";
		}
		else
		{
			if ( pPlayer->IsDead() )
			{
				if ( pPlayer->GetTeamNumber() == TEAM_JINRAI )
					pszPrefix = "[DEAD][Jinrai]";
				else if ( pPlayer->GetTeamNumber() == TEAM_NSF )
					pszPrefix = "[DEAD][NSF]";
				else
					pszPrefix = "[G33K]";
			}
			else
			{
				if ( pPlayer->GetTeamNumber() == TEAM_JINRAI )
					pszPrefix = "[Jinrai]";
				else if ( pPlayer->GetTeamNumber() == TEAM_NSF )
					pszPrefix = "[NSF]";
				else
					pszPrefix = "[G33K]";
			}
		}

		return pszPrefix;
	}

	void CNEOGameRules::CleanupMap()
	{
		CBaseEntity* pCur = gEntList.FirstEnt();

		while ( pCur )
		{
			CWeaponNEOBase* pWeapon = dynamic_cast< CWeaponNEOBase* >(pCur);

			if ( pWeapon )
			{
				if ( pWeapon->GetPlayerOwner() || pWeapon->ShouldStayOnRoundRestart() )
					continue;	 			
			}

			if ( FindInList( s_PreserveEnts, pCur->GetClassname() ) )
				continue;

			UTIL_Remove( pCur );

			pCur = gEntList.NextEnt( pCur );
		}

		// Really remove the entities so we can have access to their slots below.
		gEntList.CleanupDeleteList();

		// Cancel all queued events, in case a func_bomb_target fired some delayed outputs that
		// could kill respawning CTs
		g_EventQueue.Clear();

		// Now reload the map entities.
		class CNEOMapEntityFilter : public IMapEntityFilter
		{
		public:
			virtual bool ShouldCreateEntity( const char *pClassname )
			{
				// During map load, create all the entities.
				return true;
			}

			virtual CBaseEntity* CreateNextEntity( const char *pClassname )
			{
				CBaseEntity *pRet = CreateEntityByName( pClassname );

				CMapEntityRef ref;
				ref.m_iEdict = -1;
				ref.m_iSerialNumber = -1;

				if ( pRet )
				{
					ref.m_iEdict = pRet->entindex();
					if ( pRet->edict() )
						ref.m_iSerialNumber = pRet->edict()->m_NetworkSerialNumber;
				}

				g_MapEntityRefs.AddToTail( ref );
				return pRet;
			}

		public:
			int m_iIterator; // Unused but w/e
		};
		CNEOMapEntityFilter filter;
		filter.m_iIterator = g_MapEntityRefs.Head();

		// DO NOT CALL SPAWN ON info_node ENTITIES!

		MapEntity_ParseAllEntities( engine->GetMapEntitiesString(), &filter, true );
	}

	void CNEOGameRules::UpdateAttackerTeam()
	{
		if ( !GetNumberOfGhostRetrievalPoints() )
			return;

		int attackingTeam = m_iAttackingTeam == TEAM_JINRAI + 2;

		for ( int i = 0; i < GetNumberOfGhostRetrievalPoints(); i++ )
		{
			CNeoGhostRetrievalPoint* pRetPoint = GetGhostRetrievalPoint( i );

			int defaultTeam = pRetPoint->GetDefaultTeam();

			if ( defaultTeam == TEAM_ATTACKER ) // Attacker
				pRetPoint->SetOwningTeamNumber( m_iAttackingTeam );
			else // Defender
				pRetPoint->SetOwningTeamNumber( attackingTeam );
		}
	}

	void CNEOGameRules::SpawnGhost()
	{
		m_bFreezePeriod = true;

		if ( m_iAttackingTeam == TEAM_JINRAI )	
			m_iAttackingTeam = TEAM_NSF;
		else if ( m_iAttackingTeam == TEAM_NSF )
			m_iAttackingTeam = TEAM_JINRAI;

		m_iMVP = -1;				 
		m_iLastAttackerSpawn = 0;
		m_iLastDefenderSpawn = 0;

		if ( m_iGameType != GAMETYPE_CTG )
			return;

		UpdateAttackerTeam();

		CNeoGhostSpawnPoint* pSpawnPoint = GetRandomGhostSpawnPoint();

		if ( !pSpawnPoint )
		{
			Msg( "Game type is CTG yet there are no valid ghost spawn points!\n" );
			return;
		}  		

		Vector vGhostSpawn = pSpawnPoint->GetAbsOrigin();
		vGhostSpawn.z += 32.0f;

		CWeaponGhost* pGhost = GetGhost( 0 );

		if ( GetNumberOfGhosts() )
		{	   			
			pGhost->SetAbsOrigin( vGhostSpawn );
			pGhost->Materialize();
			return;
		}
		else
		{	   
			pGhost = (CWeaponGhost*) CreateEntityByName( "weapon_ghost" );

			if ( !pGhost )
			{
				Msg( "weapon_ghost spawn failed in StartNewRound()!\n" );
				return;
			}						 

			Vector vGhostSpawn = pSpawnPoint->GetAbsOrigin();
			vGhostSpawn.z += 32.0f;
			pGhost->SetAbsOrigin( vGhostSpawn );
			pGhost->AddEFlags( EFL_NO_DAMAGE_FORCES );
			DispatchSpawn( pGhost );
			pGhost->Materialize();
		}	  		
	}

	void CNEOGameRules::ChangeToNextMapCycleMap()
	{
		char mapName[ 32 ];
		DetermineMapCycleFilename( mapName, sizeof( mapName ), developer.GetBool() );
		g_fGameOver = false;
		Msg( "CHANGE LEVEL: %s\n", mapName );
		engine->ChangeLevel( mapName, nullptr );
	}

	void CNEOGameRules::AddPlayerSpawnPoint( CNEOSpawnPoint* pSpawnPoint )
	{
		if ( FStrEq( pSpawnPoint->GetClassname(), "info_player_attacker" ) )
			m_AttackerSpawnPoints.AddToTail( pSpawnPoint );
		else if ( FStrEq( pSpawnPoint->GetClassname(), "info_player_defender" ) )
			m_DefenderSpawnPoints.AddToTail( pSpawnPoint );		
	}		

	void CNEOGameRules::AddGhostSpawnPoint( CNeoGhostSpawnPoint* pSpawnPoint )
	{
		m_GhostSpawnPoints.AddToTail( pSpawnPoint );
	}

	CNeoGhostSpawnPoint* CNEOGameRules::GetRandomGhostSpawnPoint()
	{
		if ( m_GhostSpawnPoints.IsEmpty() )
			return nullptr;

		int index = m_iLastGhostSpawnPointIndex + RandomInt( 1, 3 );

		if ( index >= m_GhostSpawnPoints.Count() )
			index -= m_GhostSpawnPoints.Count();

		CNeoGhostSpawnPoint* res = nullptr;

		for ( int i = 0; i <= 3; i++ )
		{
			if ( m_GhostSpawnPoints.Count() < index )
			{
				index = 0;
				continue;
			}

			res = m_GhostSpawnPoints[ i ];

			if ( res )
				break;
		}

		m_iLastGhostSpawnPointIndex = index;

		return res;
	}

	CNEOSpawnPoint* CNEOGameRules::GetSpawnPoint( CNEOPlayer* pPlayer )
	{
		if ( pPlayer->GetTeamNumber() == m_iAttackingTeam )
		{					 
			int iNextSpawn = m_iLastAttackerSpawn + RandomInt( 1, 3 );

			if ( iNextSpawn >= m_AttackerSpawnPoints.Count() )
				iNextSpawn -= m_AttackerSpawnPoints.Count();

			for ( int i = iNextSpawn; i < m_AttackerSpawnPoints.Count(); i++ )
			{
				CNEOSpawnPoint* pSpawnPoint = m_AttackerSpawnPoints[ i ];

				if ( pSpawnPoint && pSpawnPoint->CanSpawn( pPlayer ) )
				{
					iNextSpawn = i;
					break;
				}

				if ( i + 1 == m_AttackerSpawnPoints.Count() )
					i = 0;

				if ( i == iNextSpawn )
					return nullptr;
			}

			m_iLastAttackerSpawn = iNextSpawn;

			if ( iNextSpawn >= m_AttackerSpawnPoints.Count() )
				return nullptr;

			m_AttackerSpawnPoints[ iNextSpawn ]->GetUnknownEvent()->FireOutput( m_AttackerSpawnPoints[ iNextSpawn ], pPlayer );

			return m_AttackerSpawnPoints[ iNextSpawn ];
		}
		else
		{
			int iNextSpawn = m_iLastDefenderSpawn + RandomInt( 1, 3 );

			if ( iNextSpawn >= m_DefenderSpawnPoints.Count() )
				iNextSpawn -= m_DefenderSpawnPoints.Count();

			for ( int i = iNextSpawn; i < m_DefenderSpawnPoints.Count(); i++ )
			{
				CNEOSpawnPoint* pSpawnPoint = m_DefenderSpawnPoints[ i ];

				if ( pSpawnPoint && pSpawnPoint->CanSpawn( pPlayer ) )
				{
					iNextSpawn = i;
					break;
				}

				if ( i + 1 == m_DefenderSpawnPoints.Count() )
					i = 0;

				if ( i == iNextSpawn )
					return nullptr;
			}

			m_iLastDefenderSpawn = iNextSpawn;

			if ( iNextSpawn >= m_DefenderSpawnPoints.Count() )
				return nullptr;

			m_DefenderSpawnPoints[ iNextSpawn ]->GetUnknownEvent()->FireOutput( m_DefenderSpawnPoints[ iNextSpawn ], pPlayer );

			return m_DefenderSpawnPoints[ iNextSpawn ];
		}
	}

	int CNEOGameRules::SelectDefaultTeam()
	{
		CTeam* pJinrai = GetGlobalTeam( TEAM_JINRAI );
		CTeam* pNsf = GetGlobalTeam( TEAM_NSF );

		int iJinraiPlayers = pJinrai->GetNumPlayers();
		int iNsfPlayers = pNsf->GetNumPlayers();
		int iJinraiScore = pJinrai->GetScore();
		int iNsfScore = pNsf->GetScore();

		int team = TEAM_UNASSIGNED;

		if ( iNsfPlayers > iJinraiPlayers )
			team = TEAM_JINRAI;
		else if ( iJinraiPlayers > iNsfPlayers )
			team = TEAM_NSF;
		else if ( iNsfScore > iJinraiScore )
			team = TEAM_JINRAI;
		else if ( iJinraiScore > iNsfScore )
			team = TEAM_NSF;
		else
			team = RandomInt( 0, 1 ) + 2;

		if ( TeamFull( team ) )
		{
			if ( team == TEAM_JINRAI )
				team = TEAM_NSF;
			else
				team = TEAM_JINRAI;

			if ( TeamFull( team ) )
				team = TEAM_UNASSIGNED;
		}

		return team;
	}

	bool CNEOGameRules::TeamFull( int iTeamNum )
	{
		CTeam* pJinrai = GetGlobalTeam( TEAM_JINRAI );
		CTeam* pNsf = GetGlobalTeam( TEAM_NSF );

		int iJinraiPlayers = pJinrai->GetNumPlayers();
		int iNsfPlayers = pNsf->GetNumPlayers();

		switch ( iTeamNum )
		{
			case TEAM_JINRAI:
				return iJinraiPlayers >= MAX_TEAM_PLAYERS;

			case TEAM_NSF:
				return iNsfPlayers >= MAX_TEAM_PLAYERS;
		}

		return false;
	}

#endif


bool CNEOGameRules::ShouldCollide( int collisionGroup0, int collisionGroup1 )
{
	if ( collisionGroup0 > collisionGroup1 )
	{
		// swap so that lowest is always first
		V_swap(collisionGroup0,collisionGroup1);
	}

	if ( collisionGroup1 == COLLISION_GROUP_PROJECTILE
		&& (collisionGroup0 == COLLISION_GROUP_PLAYER || collisionGroup0 == COLLISION_GROUP_PLAYER_MOVEMENT) )
		return false;
	
	if ( collisionGroup0 == COLLISION_GROUP_PLAYER
		&& collisionGroup1 == COLLISION_GROUP_PLAYER_MOVEMENT )
		return false;

	if ( collisionGroup1 == COLLISION_GROUP_WEAPON )
		return false;
	
	return BaseClass::ShouldCollide( collisionGroup0, collisionGroup1 ); 
}

const CViewVectors* CNEOGameRules::GetViewVectors() const
{
	return &g_NEOViewVectors;
}

/*const unsigned char *CNEOGameRules::GetEncryptionKey()
{		
	return (unsigned char*)"tBA%-ygc";
}*/

void CNEOGameRules::AddControlPoint( CNeoControlPoint* pPoint )
{
	m_ControlPoints.AddToTail( pPoint );
}											 

CNeoControlPoint* CNEOGameRules::GetControlPoint( int index )
{			   
	if ( index >= m_ControlPoints.Size() )
		return nullptr;

	return m_ControlPoints[ index ];
}

int CNEOGameRules::GetNumControlPoints()
{
	return m_ControlPoints.Size();
}

//-----------------------------------------------------------------------------
// Purpose: Init CS ammo definitions
//-----------------------------------------------------------------------------

// shared ammo definition
// JAY: Trying to make a more physical bullet response
#define BULLET_MASS_GRAINS_TO_LB(grains)	(0.002285*(grains)/16.0f)
#define BULLET_MASS_GRAINS_TO_KG(grains)	lbs2kg(BULLET_MASS_GRAINS_TO_LB(grains))

// exaggerate all of the forces, but use real numbers to keep them consistent
#define BULLET_IMPULSE_EXAGGERATION			1	

// convert a velocity in ft/sec and a mass in grains to an impulse in kg in/s
#define BULLET_IMPULSE(grains, ftpersec)	((ftpersec)*12*BULLET_MASS_GRAINS_TO_KG(grains)*BULLET_IMPULSE_EXAGGERATION)


CAmmoDef* GetAmmoDef()
{
	static CAmmoDef def;
	static bool bInitted = false;

	if ( !bInitted )
	{
		bInitted = true;
		
		def.AddAmmoType( AMMO_GRENADE, DMG_BLAST, TRACER_LINE, 0, 0,		1/*max carry*/, 1, 0 );
		def.AddAmmoType( AMMO_SMOKEGRENADE, DMG_BLAST, TRACER_LINE, 0, 0,	2/*max carry*/, 1, 0 );
		def.AddAmmoType( AMMO_BULLETS, DMG_BULLET, TRACER_LINE, 0, 0,		400/*max carry*/, 1000, 0 );
		def.AddAmmoType( AMMO_PRI, DMG_BULLET, TRACER_LINE, 0, 0,			400/*max carry*/, 1000, 0 );
		def.AddAmmoType( AMMO_SEC, DMG_BULLET, TRACER_LINE, 0, 0,			80/*max carry*/, 1000, 0 );
		def.AddAmmoType( AMMO_6_8MM, DMG_BULLET, TRACER_LINE, 0, 0,			400/*max carry*/, 1000, 0 );
		def.AddAmmoType( AMMO_5_7MM, DMG_BULLET, TRACER_LINE, 0, 0,			500/*max carry*/, 1000, 0 );
		def.AddAmmoType( AMMO_SMAC, DMG_BULLET, TRACER_LINE, 0, 0,			400/*max carry*/, 1000, 0 );
		def.AddAmmoType( AMMO_11_5MM, DMG_BULLET, TRACER_LINE, 0, 0,		270/*max carry*/, 1000, 0 );
		def.AddAmmoType( AMMO_8_MM, DMG_BULLET, TRACER_LINE, 0, 0,			270/*max carry*/, 1000, 0 );
		def.AddAmmoType( AMMO_10G_SHELL, DMG_BULLET, TRACER_LINE, 0, 0,		180/*max carry*/, 1000, 0 );
	}

	return &def;
}

//-----------------------------------------------------------------------------
// Purpose: Find the relationship between players (teamplay vs. deathmatch)
//-----------------------------------------------------------------------------
int CNEOGameRules::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
#ifndef CLIENT_DLL
	// half life multiplay has a simple concept of Player Relationships.
	// you are either on another player's team, or you are not.
	if ( !pPlayer || !pTarget || !pTarget->IsPlayer() || IsTeamplay() == false )
		return GR_NOTTEAMMATE;

	if ( (*GetTeamID(pPlayer) != '\0') && (*GetTeamID(pTarget) != '\0') && !stricmp( GetTeamID(pPlayer), GetTeamID(pTarget) ) )
		return GR_TEAMMATE;

#endif

	return GR_NOTTEAMMATE;
}

bool CNEOGameRules::IsConnectedUserInfoChangeAllowed( CBasePlayer *pPlayer )
{
	return true;
}
