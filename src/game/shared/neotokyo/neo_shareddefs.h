//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef NEO_SHAREDDEFS_H
#define NEO_SHAREDDEFS_H
#ifdef _WIN32
#pragma once
#endif						  


// NEO Team IDs
#define TEAM_JINRAI				2
#define	TEAM_NSF				3
#define TEAM_MAXCOUNT			4

#define MAX_TEAM_PLAYERS		16

enum NEOClass
{	
	CLASS_UNASSIGNED = 0,
	CLASS_RECON,
	CLASS_ASSAULT,
	CLASS_SUPPORT,
	CLASS_OPERATOR,
	CLASS_VIP,
	CLASS_MAXCOUNT
};

enum NEOClassVariant
{
	CLASSVARIANT_0 = 0,
	CLASSVARIANT_1,
	CLASSVARIANT_2,
	CLASSVARIANT_MAXCOUNT
};

enum NEORank
{
	RANK_RANKLESS = 0,
	RANK_PRIVATE,
	RANK_CORPORAL,
	RANK_SERGEANT,
	RANK_LIEUTENANT,
	RANK_MAJOR,
	RANK_MAXCOUNT
};

enum NEOStar
{
	STAR_NONE = 0,
	STAR_ALPHA,
	STAR_BRAVO,
	STAR_CHARLIE,
	STAR_DELTA,
	STAR_ECHO,
	STAR_MAXCOUNT
};

enum NEOAttackingTeam
{	
	TEAM_ATTACKER = 0,
	TEAM_DEFENDER
};

enum NEOGameType
{
	GAMETYPE_TDM = 0,
	GAMETYPE_CTG,
	GAMETYPE_VIP, // Or control point???
	GAMETYPE_MAXCOUNT
};

enum NEOLean
{
	LEAN_NONE = 0,
	LEAN_LEFT,
	LEAN_RIGHT
};

enum NEODeathNoticeIcon
{
	DEATH_NOTICE_KILL = 1,
	DEATH_NOTICE_GUN,
	DEATH_NOTICE_HEADSHOT,
	DEATH_NOTICE_EXPLODE,
	DEATH_NOTICE_MELEE,
	DEATH_NOTICE_SHORTBUS,
	DEATH_NOTICE_MAXCOUNT
};

#define NEO_MAX_SQUAD_SIZE 7

#define GHOST_RETRIEVAL_DEFAULT_RADIUS_DIST	128
#define GHOST_RETRIEVAL_MIN_RADIUS_DIST		48
#define GHOST_RETRIEVAL_MAX_RADIUS_DIST		256

#define NEO_VIP_PLAYER_MODEL "models/player/vip.mdl"
#define NEO_VIP_DEAD_PLAYER_MODEL "models/player/vip_dead.mdl"
#define NEO_TERROR_PLAYER_MODEL "models/player/terror2.mdl"

#define NEO_VIP_GIB1 "models/nt/vip_gib01.mdl"
#define NEO_VIP_GIB2 "models/nt/vip_gib02.mdl"
#define NEO_VIP_GIB3 "models/nt/vip_gib03.mdl"

#define NEO_GLOW_MATERIAL "sprites/glow01.vmt"
#define NEO_THERMAL_MATERIAL "dev/thermal.vmt"
#define NEO_VM_THERMAL_MATERIAL "dev/vm_thermal.vmt"

#define VISION_NONE		0
#define VISION_NIGHT	2
#define VISION_THERMAL 	3
#define VISION_MOTION	4

extern const char* s_szJinraiModels[];
extern const char* s_szNSFModels[];		 
extern const char* s_szGibletsModels[];

struct NEOLoadout
{
	const char* szWeaponName;
	const char* szLoadoutMaterialName;
	const char* szGiveWeaponName;
	const char* szAmmoName;
	int iAmmo;
};

extern NEOLoadout s_DefaultLoadout[];
extern NEOLoadout s_ReconLoadout[];
extern NEOLoadout s_AssaultLoadout[];
extern NEOLoadout s_SupportLoadout[];

#define PANEL_LOADOUT		"loadout"
#define PANEL_LOADOUT_DEV	"loadout_dev"		   

#define NEO_VEC_VIEW_SCALED( classType, player )				( m_ClassViewVectors[ classType ].m_vView * player->GetModelScale() )
#define NEO_VEC_HULL_MIN_SCALED( classType, player )			( m_ClassViewVectors[ classType ].m_vHullMin * player->GetModelScale() )
#define NEO_VEC_HULL_MAX_SCALED( classType, player )			( m_ClassViewVectors[ classType ].m_vHullMax * player->GetModelScale() )

#define NEO_VEC_DUCK_HULL_MIN_SCALED( classType, player )		( m_ClassViewVectors[ classType ].m_vDuckHullMin * player->GetModelScale() )
#define NEO_VEC_DUCK_HULL_MAX_SCALED( classType, player )		( m_ClassViewVectors[ classType ].m_vDuckHullMax * player->GetModelScale() )
#define NEO_VEC_DUCK_VIEW_SCALED( classType, player )			( m_ClassViewVectors[ classType ].m_vDuckView * player->GetModelScale() )

#define NEO_VEC_OBS_HULL_MIN_SCALED( classType, player )		( m_ClassViewVectors[ classType ].m_vObsHullMin * player->GetModelScale() )
#define NEO_VEC_OBS_HULL_MAX_SCALED( classType, player )		( m_ClassViewVectors[ classType ].m_vObsHullMax * player->GetModelScale() )

#define NEO_VEC_DEAD_VIEWHEIGHT_SCALED( classType, player )		( m_ClassViewVectors[ classType ].m_vDeadViewHeight * player->GetModelScale() )


#define NEO_MIN_SPEEDBOOST_AMOUNT 33.0f


// The various states the player can be in during the join game process.
enum NEOPlayerState
{
	// Happily running around in the game.
	// You can't move though if CSGameRules()->IsFreezePeriod() returns true.
	// This state can jump to a bunch of other states like STATE_PICKINGCLASS or STATE_DEATH_ANIM.
	STATE_ACTIVE=0,
	
	// This is the state you're in when you first enter the server.
	// It's switching between intro cameras every few seconds, and there's a level info 
	// screen up.
	STATE_INTRO,				// Show the level intro screen.
	
	// During these states, you can either be a new player waiting to join, or
	// you can be a live player in the game who wants to change teams.
	// Either way, you can't move while choosing team or class (or while any menu is up).
	STATE_PICKINGTEAM,			// Choosing team.

	STATE_PICKINGCLASS,			// Choosing class.

	STATE_PICKINGLOADOUT,		// Choosing loadout.
	
	STATE_PLAYDEATH,			// Playing death anim, waiting for that to finish.
	STATE_DEAD,					// RIP
	STATE_OBSERVERMODE,			// Noclipping around, watching players, etc.

	NUM_PLAYER_STATES
};
#define NEO_PLAYER_DEATH_TIME	5.0f	//Minimum Time before respawning

// Special Damage types
enum
{
	NEO_DMG_CUSTOM_NONE = 0,
	NEO_DMG_CUSTOM_SUICIDE,
};

// Player avoidance
#define PUSHAWAY_THINK_INTERVAL		(1.0f / 20.0f)

#endif // NEO_SHAREDDEFS_H
