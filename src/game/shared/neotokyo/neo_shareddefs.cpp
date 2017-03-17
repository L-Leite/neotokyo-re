//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "cbase.h"
#include "neo_shareddefs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

const char* s_szJinraiModels[]
{
	// Non existant class
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,

	// Recon
	"models/player/jinrai_msf.mdl", // index 21

	"models/player/msf_dead_head.mdl",
	"models/player/msf_dead_rleg.mdl",
	"models/player/msf_dead_lleg.mdl",
	"models/player/msf_dead_rarm.mdl",
	"models/player/msf_dead_larm.mdl",
	"models/player/msf_dead_all.mdl",

	"models/player/jinrai_msf2.mdl",

	"models/player/msf2_dead_head.mdl",
	"models/player/msf2_dead_rleg.mdl",
	"models/player/msf2_dead_lleg.mdl",
	"models/player/msf2_dead_rarm.mdl",
	"models/player/msf2_dead_larm.mdl",
	"models/player/msf2_dead_all.mdl",

	"models/player/jinrai_msf3.mdl",

	"models/player/msf3_dead_head.mdl",
	"models/player/msf3_dead_rleg.mdl",
	"models/player/msf3_dead_lleg.mdl",
	"models/player/msf3_dead_rarm.mdl",
	"models/player/msf3_dead_larm.mdl",
	"models/player/msf3_dead_all.mdl",

	// Assault
	"models/player/jinrai_mam.mdl",

	"models/player/mam_dead_head.mdl",
	"models/player/mam_dead_rleg.mdl",
	"models/player/mam_dead_lleg.mdl",
	"models/player/mam_dead_rarm.mdl",
	"models/player/mam_dead_larm.mdl",
	"models/player/mam_dead_all.mdl",

	"models/player/jinrai_mam2.mdl",

	"models/player/mam2_dead_head.mdl",
	"models/player/mam2_dead_rleg.mdl",
	"models/player/mam2_dead_lleg.mdl",
	"models/player/mam2_dead_rarm.mdl",
	"models/player/mam2_dead_larm.mdl",
	"models/player/mam2_dead_all.mdl",

	"models/player/jinrai_mam3.mdl",

	"models/player/mam3_dead_head.mdl",
	"models/player/mam3_dead_rleg.mdl",
	"models/player/mam3_dead_lleg.mdl",
	"models/player/mam3_dead_rarm.mdl",
	"models/player/mam3_dead_larm.mdl",
	"models/player/mam3_dead_all.mdl",

	// Heavy
	"models/player/jinrai_mhm.mdl",

	"models/player/mhm_dead_head.mdl",
	"models/player/mhm_dead_rleg.mdl",
	"models/player/mhm_dead_lleg.mdl",
	"models/player/mhm_dead_rarm.mdl",
	"models/player/mhm_dead_larm.mdl",
	"models/player/mhm_dead_all.mdl",

	"models/player/jinrai_mhm2.mdl",

	"models/player/mhm2_dead_head.mdl",
	"models/player/mhm2_dead_rleg.mdl",
	"models/player/mhm2_dead_lleg.mdl",
	"models/player/mhm2_dead_rarm.mdl",
	"models/player/mhm2_dead_larm.mdl",
	"models/player/mhm2_dead_all.mdl",

	"models/player/jinrai_mhm3.mdl",

	"models/player/mhm3_dead_head.mdl",
	"models/player/mhm3_dead_rleg.mdl",
	"models/player/mhm3_dead_lleg.mdl",
	"models/player/mhm3_dead_rarm.mdl",
	"models/player/mhm3_dead_larm.mdl",
	"models/player/mhm3_dead_all.mdl",

	// Non existant 4th class
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL
};

const char* s_szNSFModels[]
{
	// Non existant class
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,

	// Recon
	"models/player/nsf_gsf.mdl",

	"models/player/gsf_dead_head.mdl",
	"models/player/gsf_dead_rleg.mdl",
	"models/player/gsf_dead_lleg.mdl",
	"models/player/gsf_dead_rarm.mdl",
	"models/player/gsf_dead_larm.mdl",
	"models/player/gsf_dead_all.mdl",

	"models/player/nsf_gsf2.mdl",

	"models/player/gsf2_dead_head.mdl",
	"models/player/gsf2_dead_rleg.mdl",
	"models/player/gsf2_dead_lleg.mdl",
	"models/player/gsf2_dead_rarm.mdl",
	"models/player/gsf2_dead_larm.mdl",
	"models/player/gsf2_dead_all.mdl",

	"models/player/nsf_gsf3.mdl",

	"models/player/gsf3_dead_head.mdl",
	"models/player/gsf3_dead_rleg.mdl",
	"models/player/gsf3_dead_lleg.mdl",
	"models/player/gsf3_dead_rarm.mdl",
	"models/player/gsf3_dead_larm.mdl",
	"models/player/gsf3_dead_all.mdl",

	// Assault
	"models/player/nsf_gam.mdl",

	"models/player/gam_dead_head.mdl",
	"models/player/gam_dead_rleg.mdl",
	"models/player/gam_dead_lleg.mdl",
	"models/player/gam_dead_rarm.mdl",
	"models/player/gam_dead_larm.mdl",
	"models/player/gam_dead_all.mdl",

	"models/player/nsf_gam2.mdl",

	"models/player/gam2_dead_head.mdl",
	"models/player/gam2_dead_rleg.mdl",
	"models/player/gam2_dead_lleg.mdl",
	"models/player/gam2_dead_rarm.mdl",
	"models/player/gam2_dead_larm.mdl",
	"models/player/gam2_dead_all.mdl",

	"models/player/nsf_gam3.mdl",

	"models/player/gam3_dead_head.mdl",
	"models/player/gam3_dead_rleg.mdl",
	"models/player/gam3_dead_lleg.mdl",
	"models/player/gam3_dead_rarm.mdl",
	"models/player/gam3_dead_larm.mdl",
	"models/player/gam3_dead_all.mdl",

	// Heavy
	"models/player/nsf_ghm.mdl",

	"models/player/ghm_dead_head.mdl",
	"models/player/ghm_dead_rleg.mdl",
	"models/player/ghm_dead_lleg.mdl",
	"models/player/ghm_dead_rarm.mdl",
	"models/player/ghm_dead_larm.mdl",
	"models/player/ghm_dead_all.mdl",

	"models/player/nsf_ghm2.mdl",

	"models/player/ghm2_dead_head.mdl",
	"models/player/ghm2_dead_rleg.mdl",
	"models/player/ghm2_dead_lleg.mdl",
	"models/player/ghm2_dead_rarm.mdl",
	"models/player/ghm2_dead_larm.mdl",
	"models/player/ghm2_dead_all.mdl",

	"models/player/nsf_ghm3.mdl",

	"models/player/ghm3_dead_head.mdl",
	"models/player/ghm3_dead_rleg.mdl",
	"models/player/ghm3_dead_lleg.mdl",
	"models/player/ghm3_dead_rarm.mdl",
	"models/player/ghm3_dead_larm.mdl",
	"models/player/ghm3_dead_all.mdl",

	// Non existant 4th class
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL,
	NEO_VIP_PLAYER_MODEL
};

const char* s_szGibletsModels[]
{
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_rightleg_gib.mdl",
	"models/player/playergib/msf_leftleg_gib.mdl",
	"models/player/playergib/msf_rightarm_gib.mdl",
	"models/player/playergib/msf_leftarm_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_rightleg_gib.mdl",
	"models/player/playergib/msf_leftleg_gib.mdl",
	"models/player/playergib/msf_rightarm_gib.mdl",
	"models/player/playergib/msf_leftarm_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_rightleg_gib.mdl",
	"models/player/playergib/msf_leftleg_gib.mdl",
	"models/player/playergib/msf_rightarm_gib.mdl",
	"models/player/playergib/msf_leftarm_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_rightleg_gib.mdl",
	"models/player/playergib/msf_leftleg_gib.mdl",
	"models/player/playergib/msf_rightarm_gib.mdl",
	"models/player/playergib/msf_leftarm_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_rightleg_gib.mdl",
	"models/player/playergib/msf_leftleg_gib.mdl",
	"models/player/playergib/msf_rightarm_gib.mdl",
	"models/player/playergib/msf_leftarm_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_rightleg_gib.mdl",
	"models/player/playergib/msf_leftleg_gib.mdl",
	"models/player/playergib/msf_rightarm_gib.mdl",
	"models/player/playergib/msf_leftarm_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/mam_head_gib.mdl",
	"models/player/playergib/mam_head_gib.mdl",
	"models/player/playergib/mam_rightleg_gib.mdl",
	"models/player/playergib/mam_leftleg_gib.mdl",
	"models/player/playergib/mam_rightarm_gib.mdl",
	"models/player/playergib/mam_leftarm_gib.mdl",
	"models/player/playergib/mam_head_gib.mdl",
	"models/player/playergib/mam_head_gib.mdl",
	"models/player/playergib/mam_head_gib.mdl",
	"models/player/playergib/mam_rightleg_gib.mdl",
	"models/player/playergib/mam_leftleg_gib.mdl",
	"models/player/playergib/mam_rightarm_gib.mdl",
	"models/player/playergib/mam_leftarm_gib.mdl",
	"models/player/playergib/mam_head_gib.mdl",
	"models/player/playergib/mam_head_gib.mdl",
	"models/player/playergib/mam_head_gib.mdl",
	"models/player/playergib/mam_rightleg_gib.mdl",
	"models/player/playergib/mam_leftleg_gib.mdl",
	"models/player/playergib/mam_rightarm_gib.mdl",
	"models/player/playergib/mam_leftarm_gib.mdl",
	"models/player/playergib/mam_head_gib.mdl",
	"models/player/playergib/mhm_head_gib.mdl",
	"models/player/playergib/mhm_head_gib.mdl",
	"models/player/playergib/mhm_rightleg_gib.mdl",
	"models/player/playergib/mhm_leftleg_gib.mdl",
	"models/player/playergib/mhm_rightarm_gib.mdl",
	"models/player/playergib/mhm_leftarm_gib.mdl",
	"models/player/playergib/mhm_head_gib.mdl",
	"models/player/playergib/mhm_head_gib.mdl",
	"models/player/playergib/mhm_head_gib.mdl",
	"models/player/playergib/mhm_rightleg_gib.mdl",
	"models/player/playergib/mhm_leftleg_gib.mdl",
	"models/player/playergib/mhm_rightarm_gib.mdl",
	"models/player/playergib/mhm_leftarm_gib.mdl",
	"models/player/playergib/mhm_head_gib.mdl",
	"models/player/playergib/mhm_head_gib.mdl",
	"models/player/playergib/mhm_head_gib.mdl",
	"models/player/playergib/mhm_rightleg_gib.mdl",
	"models/player/playergib/mhm_leftleg_gib.mdl",
	"models/player/playergib/mhm_rightarm_gib.mdl",
	"models/player/playergib/mhm_leftarm_gib.mdl",
	"models/player/playergib/mhm_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_rightleg_gib.mdl",
	"models/player/playergib/msf_leftleg_gib.mdl",
	"models/player/playergib/msf_rightarm_gib.mdl",
	"models/player/playergib/msf_leftarm_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_rightleg_gib.mdl",
	"models/player/playergib/msf_leftleg_gib.mdl",
	"models/player/playergib/msf_rightarm_gib.mdl",
	"models/player/playergib/msf_leftarm_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl",
	"models/player/playergib/msf_rightleg_gib.mdl",
	"models/player/playergib/msf_leftleg_gib.mdl",
	"models/player/playergib/msf_rightarm_gib.mdl",
	"models/player/playergib/msf_leftarm_gib.mdl",
	"models/player/playergib/msf_head_gib.mdl"
};

NEOLoadout s_DefaultLoadout[]
{
	{ "MPN45",
	"vgui/loadout/loadout_mpn",
	"weapon_mpn",
	"AMMO_PRI",
	150 },

	{ "SRM",
	"vgui/loadout/loadout_srm",
	"weapon_srm",
	"AMMO_PRI",
	200 },

	{ "ZR68-C (compact)",
	"vgui/loadout/loadout_zr68c",
	"weapon_zr68c",
	"AMMO_PRI",
	150 },

	{ "Mosok",
	"vgui/loadout/loadout_mosok",
	"weapon_m41",
	"AMMO_PRI",
	60 },

	{ "Murata Supa-7",
	"vgui/loadout/loadout_supa7",
	"weapon_supa7",
	"AMMO_10G_SHELL",
	28 },

	{ "MX",
	"vgui/loadout/loadout_mx",
	"weapon_mx",
	"AMMO_PRI",
	120 },

	{ "Mosok Silenced",
	"vgui/loadout/loadout_mosokl",
	"weapon_m41s",
	"AMMO_PRI",
	60 },

	{ "MX Silenced",
	"vgui/loadout/loadout_mxs",
	"weapon_mxs",
	"AMMO_PRI",
	120 },

	{ "PZ252",
	"vgui/loadout/loadout_pz",
	"weapon_pz",
	"AMMO_5_7MM",
	200 },

	{ "",
	"",
	"",
	"AMMO_PRI",
	0 }
};

NEOLoadout s_ReconLoadout[]
{
	{ "MPN45",
	"vgui/loadout/loadout_mpn",
	"weapon_mpn",
	"AMMO_PRI",
	120 },

	{ "SRM",
	"vgui/loadout/loadout_srm",
	"weapon_srm",
	"AMMO_PRI",
	150 },

	{ "Jitte",
	"vgui/loadout/loadout_jitte",
	"weapon_jitte",
	"AMMO_PRI",
	120 },

	{ "SRM (silenced)",
	"vgui/loadout/loadout_srms",
	"weapon_srm_s",
	"AMMO_PRI",
	150 },

	{ "Jitte (with scope)",
	"vgui/loadout/loadout_jittes",
	"weapon_jittes",
	"AMMO_PRI",
	120 },

	{ "ZR68-L (accurized)",
	"vgui/loadout/loadout_zr68l",
	"weapon_zr68l",
	"AMMO_PRI",
	30 },

	{ "ZR68C",
	"vgui/loadout/loadout_zr68c",
	"weapon_zr68c",
	"AMMO_PRI",
	90 },

	{ "Murata Supa-7",
	"vgui/loadout/loadout_supa7",
	"weapon_supa7",
	"AMMO_10G_SHELL",
	21 },

	{ "Mosok Silenced",
	"vgui/loadout/loadout_mosokl",
	"weapon_m41s",
	"AMMO_PRI",
	60 },

	{ "",
	"",
	"",
	"AMMO_PRI",
	0 },
};

NEOLoadout s_AssaultLoadout[]
{
	{ "MPN45",
	"vgui/loadout/loadout_mpn",
	"weapon_mpn",
	"AMMO_PRI",
	150 },

	{ "SRM",
	"vgui/loadout/loadout_srm",
	"weapon_srm",
	"AMMO_PRI",
	200 },

	{ "Jitte",
	"vgui/loadout/loadout_jitte",
	"weapon_jitte",
	"AMMO_PRI",
	150 },

	{ "ZR68-C (compact)",
	"vgui/loadout/loadout_zr68c",
	"weapon_zr68c",
	"AMMO_PRI",
	120 },

	{ "ZR68-S (silenced)",
	"vgui/loadout/loadout_zr68s",
	"weapon_zr68s",
	"AMMO_PRI",
	120 },

	{ "Murata Supa-7",
	"vgui/loadout/loadout_supa7",
	"weapon_supa7",
	"AMMO_10G_SHELL",
	28 },

	{ "Mosok",
	"vgui/loadout/loadout_mosok",
	"weapon_m41",
	"AMMO_PRI",
	60 },

	{ "Mosok Silenced",
	"vgui/loadout/loadout_mosokl",
	"weapon_m41s",
	"AMMO_PRI",
	60 },

	{ "MX",
	"vgui/loadout/loadout_mx",
	"weapon_mx",
	"AMMO_PRI",
	120 },

	{ "MX Silenced",
	"vgui/loadout/loadout_mxs",
	"weapon_mxs",
	"AMMO_PRI",
	120 },

	{ "AA13",
	"vgui/loadout/loadout_aa13",
	"weapon_aa13",
	"AMMO_10G_SHELL",
	64 },

	{ "SRS",
	"vgui/loadout/loadout_srs",
	"weapon_srs",
	"AMMO_PRI",
	18 },

	{ "",
	"",
	"",
	"AMMO_PRI",
	0 },
};

NEOLoadout s_SupportLoadout[]
{
	{ "MPN45",
	"vgui/loadout/loadout_mpn",
	"weapon_mpn",
	"AMMO_PRI",
	150 },

	{ "SRM",
	"vgui/loadout/loadout_srm",
	"weapon_srm",
	"AMMO_PRI",
	200 },

	{ "ZR68-C (compact)",
	"vgui/loadout/loadout_zr68c",
	"weapon_zr68c",
	"AMMO_PRI",
	150 },

	{ "Mosok",
	"vgui/loadout/loadout_mosok",
	"weapon_m41",
	"AMMO_PRI",
	60 },

	{ "Murata Supa-7",
	"vgui/loadout/loadout_supa7",
	"weapon_supa7",
	"AMMO_10G_SHELL",
	28 },

	{ "MX",
	"vgui/loadout/loadout_mx",
	"weapon_mx",
	"AMMO_PRI",
	120 },

	{ "Mosok Silenced",
	"vgui/loadout/loadout_mosokl",
	"weapon_m41s",
	"AMMO_PRI",
	60 },

	{ "MX Silenced",
	"vgui/loadout/loadout_mxs",
	"weapon_mxs",
	"AMMO_PRI",
	120 },

	{ "PZ252",
	"vgui/loadout/loadout_pz",
	"weapon_pz",
	"AMMO_5_7MM",
	200 },

	{ "",
	"",
	"",
	"AMMO_PRI",
	0 },
};
