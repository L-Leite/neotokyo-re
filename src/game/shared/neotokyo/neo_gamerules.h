//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The TF Game rules object
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//

#ifndef NEO_GAMERULES_H
#define NEO_GAMERULES_H

#ifdef _WIN32
#pragma once
#endif


#include "teamplay_gamerules.h"
#include "convar.h"
#include "gamevars_shared.h"
#include "neo_controlpoint.h"

#ifdef CLIENT_DLL
	#include "c_neo_player.h"
#else
	#include "neo_player.h"
	#include "neo_ghost_spawnpoint.h"
	#include "neo_spawnpoint.h"
#endif


#ifdef CLIENT_DLL
	#define CNEOGameRules C_NEOGameRules
	#define CNEOGameRulesProxy C_NEOGameRulesProxy
#endif


class CNEOGameRulesProxy : public CGameRulesProxy
{
public:
	DECLARE_CLASS( CNEOGameRulesProxy, CGameRulesProxy );
	DECLARE_NETWORKCLASS();
};


class CNEOGameRules : public CTeamplayRules
{
public:
	DECLARE_CLASS( CNEOGameRules, CTeamplayRules );

	virtual bool	ShouldCollide( int collisionGroup0, int collisionGroup1 );

	virtual int		PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );		
	virtual const CViewVectors*	GetViewVectors() const;		  
	//virtual const unsigned char	*GetEncryptionKey(); // We don't need it

#ifdef CLIENT_DLL

	DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.

#else

	DECLARE_SERVERCLASS_NOBASE(); // This makes datatables able to access our private vars.
	
	CNEOGameRules();
	virtual			~CNEOGameRules();

	virtual void	LevelShutdown();

	virtual bool	ClientCommand( CBaseEntity *pEdict, const CCommand &args );
	virtual void	RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore );
	virtual void	Think();

	virtual bool	IsTeamplay() { return true; }
	virtual bool	IsMultiplayer() { return true; }
	virtual const char *GetGameDescription() { return "Neotokyo"; }

	virtual float	FlPlayerFallDamage( CBasePlayer *pPlayer );
	virtual bool	FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker, const CTakeDamageInfo &info ) { return true; }	 
	virtual void	PlayerSpawn( CBasePlayer *pPlayer );

	virtual void	ClientSettingsChanged( CBasePlayer *pPlayer );

	virtual int		IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled );
	virtual void	DeathNotice( CBasePlayer *pVictim, const CTakeDamageInfo &info );

	virtual const char	*GetChatPrefix( bool bTeamOnly, CBasePlayer *pPlayer );

	int				SelectDefaultTeam();
	bool			TeamFull( int iTeamNum );

	virtual void	CleanupMap();

	void			AddPlayerSpawnPoint( CNEOSpawnPoint* pSpawnPoint );

	void			AddGhostSpawnPoint( CNeoGhostSpawnPoint* pSpawnPoint );
	CNeoGhostSpawnPoint* GetRandomGhostSpawnPoint();

	bool			IsRoundInProgress();

	CNEOSpawnPoint*	GetSpawnPoint( CNEOPlayer* pPlayer );

private:										 
	void			RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore, bool bIgnoreWorld );

	void			DetermineGametype();
	void			RestartGame();			  
	void			RoundStart();
	void			StartNewRound();
	void			PlayGhostPing();
		
	void			UpdateAttackerTeam();
	void			SpawnGhost();		 	

	void			CheckRoundWinner();

	void			CheckScoreLimit();
	virtual void	ChangeToNextMapCycleMap();

#endif																			  
	virtual bool	IsConnectedUserInfoChangeAllowed( CBasePlayer *pPlayer );		

public:
	void			AddControlPoint( CNeoControlPoint* pPoint );
	CNeoControlPoint* GetControlPoint( int index );
	int				GetNumControlPoints();

	bool			IsInFreezePeriod();

#ifdef GAME_DLL
	inline void		SetUnknown624( bool newValue ) { m_Unknown624 = newValue; }
#endif

private:
#ifdef GAME_DLL
	bool			m_bDeterminedGameType;
	bool			m_bShouldChangeMap;

	float			m_fLastRoundStartTime;

	CUtlVector< CNEOSpawnPoint* > m_AttackerSpawnPoints;
	CUtlVector< CNEOSpawnPoint* > m_DefenderSpawnPoints;

	int m_iLastAttackerSpawn;
	int m_iLastDefenderSpawn;

	CUtlVector< CNeoGhostSpawnPoint* > m_GhostSpawnPoints;

	int m_iLastGhostSpawnPointIndex;

	float m_Unknown612;
	float m_Unknown616;
	int m_Unknown620;
	bool m_Unknown624;
#endif

	CUtlVector< CNeoControlPoint* > m_ControlPoints;

	float m_Unknown648; // prob not in client

public:
	CNetworkVar( NEOGameType, m_iGameType );

	CNetworkVar( float, m_fRoundTimeLeft );
	CNetworkVar( int, m_iRoundNumber );

	CNetworkVar( int, m_iGameState );
	CNetworkVar( int, m_iGameHud );

	CNetworkVar( int, m_iMVP );

	CNetworkVar( int, m_iAttackingTeam );

	CNetworkVar( bool, m_bFreezePeriod );

private:
	float m_Unknown684;
	bool m_bUnknown688;
};

inline bool CNEOGameRules::IsInFreezePeriod()
{
	return m_bFreezePeriod;
}

//-----------------------------------------------------------------------------
// Gets us at the team fortress game rules
//-----------------------------------------------------------------------------

inline CNEOGameRules* NEOGameRules()
{
	return static_cast<CNEOGameRules*>(g_pGameRules);
}


#endif // NEO_GAMERULES_H
