#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "iclientmode.h"
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>

#include "c_neo_player.h"
#include "neo_gamerules.h"
#include "weapon_ghost.h"
#include "neo_ghost_retrievalpoint.h"
#include "c_playerresource.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

class CHudGame : public CHudElement, public Panel
{
	DECLARE_CLASS_SIMPLE( CHudGame, Panel );

public:
	CHudGame( const char *pElementName );

	virtual void Init();
	virtual void VidInit();
	virtual void Reset();

private:
	virtual void PaintBackground();
	virtual void Paint();
	virtual void OnThink();
	virtual void ApplySchemeSettings( IScheme *pScheme );

	void DrawCompass( C_NEOPlayer* pLocalPlayer );
	void DrawGhosts( C_NEOPlayer* pLocalPlayer );
	void DrawGhostRetrievalPoints( C_NEOPlayer* pLocalPlayer );
	void DrawFreezeTimeInfo();
	void DrawGhostScan();
	void DrawEnemyBeacon( C_NEOPlayer* pLocalPlayer );
	void DrawControlPoints( C_NEOPlayer* pLocalPlayer );

	void DrawRoundMvp( C_NEOPlayer* pLocalPlayer, int iAlpha );
	void DrawJinraiWinLogo( C_NEOPlayer* pLocalPlayer );
	void DrawNsfWinLogo( C_NEOPlayer* pLocalPlayer );
	void DrawTieLogo( C_NEOPlayer* pLocalPlayer );

	void DrawGameElements( C_NEOPlayer* pLocalPlayer );

private:														  
	CPanelAnimationVar( HFont, m_hNumberFont, "NumberFont", "NHudOCRSmall" );
	CPanelAnimationVar( HFont, m_hMVPFont, "MVPFont", "MVP" );
	CPanelAnimationVar( HFont, m_hBootFont, "BootFont", "BOOT" );

	int			m_iCpNumberTextures[ 6 ];
	
	int			m_iCpATexture;
	int			m_iCpBTexture;
	int			m_iCpCTexture;
	int			m_iCpDTexture;
	int			m_iCpETexture;
	int			m_iCpFTexture;

	int			m_iCpTeamTextures[ TEAM_MAXCOUNT ];

	bool		m_bShouldDrawHint;
	int			m_iHintTextureId;
	
	int			m_iRoundStartTexture;
	int			m_iRoundEndTieTexture;
	int			m_iRoundEndJinraiTexture;
	int			m_iRoundEndNsfTexture;
	
	int			m_iCtgGhostBeaconCircleTexture;
	int			m_iCtgGhostBeaconArrowDownTexture;
	int			m_iCtgGhostBeaconEnemyTexture;
	int			m_iCtgGhostScanTextures[ 4 ];

	int			m_Unknown304;

	float		m_Unknown416;
	float		m_Unknown420;

	float		m_Unknown428;

	float		m_Unknown432;  
	int			m_iGameHud;
	float		m_fLastGameHudChangeTime;

	float		m_Unknown444;
	float		m_Unknown448;
	float		m_Unknown452;
	float		m_Unknown456;	   
	float		m_fLastThinkTime;
	float		m_Unknown464;
	float		m_Unknown468;

	bool		m_Unknown472;
	bool		m_bCanPlayGhostPickupSound;
	bool		m_bShouldPlayGhostEquip;
};

DECLARE_HUDELEMENT( CHudGame );


CHudGame::CHudGame( const char *pElementName ) : CHudElement( pElementName ), BaseClass(NULL, "HudGame")
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	
	SetHiddenBits( HIDEHUD_ALL );
	
	m_iCpNumberTextures[ 0 ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpNumberTextures[ 0 ], "vgui/hud/cp/cp_1", true, false );
	
	m_iCpNumberTextures[ 1 ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpNumberTextures[ 1 ], "vgui/hud/cp/cp_2", true, false );
	
	m_iCpNumberTextures[ 2 ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpNumberTextures[ 2 ], "vgui/hud/cp/cp_3", true, false );
	
	m_iCpNumberTextures[ 3 ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpNumberTextures[ 3 ], "vgui/hud/cp/cp_4", true, false );
	
	m_iCpNumberTextures[ 4 ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpNumberTextures[ 4 ], "vgui/hud/cp/cp_5", true, false );
	
	m_iCpNumberTextures[ 5 ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpNumberTextures[ 5 ], "vgui/hud/cp/cp_6", true, false );
	
	m_iCpATexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpATexture, "vgui/hud/cp/cp_a", true, false );
	
	m_iCpBTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpBTexture, "vgui/hud/cp/cp_b", true, false );
	
	m_iCpCTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpCTexture, "vgui/hud/cp/cp_c", true, false );
	
	m_iCpDTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpDTexture, "vgui/hud/cp/cp_d", true, false );
	
	m_iCpETexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpETexture, "vgui/hud/cp/cp_e", true, false );
	
	m_iCpFTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpFTexture, "vgui/hud/cp/cp_f", true, false );
	
	m_iCpTeamTextures[ TEAM_SPECTATOR ] = m_iCpTeamTextures[ TEAM_UNASSIGNED ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpTeamTextures[ TEAM_UNASSIGNED ], "vgui/hud/cp/cp_none", true, false );
	
	m_iCpTeamTextures[ TEAM_JINRAI ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpTeamTextures[ TEAM_JINRAI ], "vgui/hud/cp/cp_jinrai", true, false );
	
	m_iCpTeamTextures[ TEAM_NSF ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCpTeamTextures[ TEAM_NSF ], "vgui/hud/cp/cp_nsf", true, false );

	m_iRoundStartTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iRoundStartTexture, "vgui/hud/roundstart", true, false );

	m_iRoundEndTieTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iRoundEndTieTexture, "vgui/hud/roundend_tie", true, false );

	m_iRoundEndJinraiTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iRoundEndJinraiTexture, "vgui/hud/roundend_jinrai", true, false );

	m_iRoundEndNsfTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iRoundEndNsfTexture, "vgui/hud/roundend_nsf", true, false );

	m_iCtgGhostBeaconCircleTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCtgGhostBeaconCircleTexture, "vgui/hud/ctg/g_beacon_circle", true, false );

	m_iCtgGhostBeaconArrowDownTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCtgGhostBeaconArrowDownTexture, "vgui/hud/ctg/g_beacon_arrow_down", true, false );

	m_iCtgGhostBeaconEnemyTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCtgGhostBeaconEnemyTexture, "vgui/hud/ctg/g_beacon_enemy", true, false );

	m_iCtgGhostScanTextures[ 0 ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCtgGhostScanTextures[ 0 ], "vgui/hud/ctg/g_gscan_00", true, false );

	m_iCtgGhostScanTextures[ 1 ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCtgGhostScanTextures[ 1 ], "vgui/hud/ctg/g_gscan_01", true, false );

	m_iCtgGhostScanTextures[ 2 ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCtgGhostScanTextures[ 2 ], "vgui/hud/ctg/g_gscan_02", true, false );

	m_iCtgGhostScanTextures[ 3 ] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCtgGhostScanTextures[ 3 ], "vgui/hud/ctg/g_gscan_03", true, false );

	m_Unknown428 = 0;  
	m_Unknown432 = 0; 
	m_fLastGameHudChangeTime = 0;

	m_Unknown304 = 0;

	m_iGameHud = -1;

	m_bShouldDrawHint = true;
	m_iHintTextureId = surface()->CreateNewTextureID();

	m_Unknown472 = false;
	m_bCanPlayGhostPickupSound = false;
	m_bShouldPlayGhostEquip = false;

	m_Unknown444 = 0.2f;
	m_Unknown448 = 0.4f;
	m_Unknown452 = 0.6f;
	m_Unknown456 = 0.8f;		 
	m_fLastThinkTime = gpGlobals->curtime;
	m_Unknown464 = 0;
	m_Unknown468 = 0.33f;
}

void CHudGame::Init()
{
	Reset();
}

void CHudGame::VidInit()
{
	Reset();

	int wide, tall;
	surface()->GetScreenSize( wide, tall );
	SetBounds( 0, 0, wide, tall );
}

void CHudGame::Reset()
{
	m_Unknown444 = 0.2f;
	m_Unknown448 = 0.4f;
	m_Unknown452 = 0.6f;
	m_Unknown456 = 0.8f;
	m_fLastThinkTime = gpGlobals->curtime;
	m_Unknown464 = 0;
	m_Unknown468 = 0.33f;
}

void CHudGame::PaintBackground()
{	
}

void CHudGame::DrawCompass( C_NEOPlayer* pLocalPlayer )
{
	int wide, tall;
	GetSize( wide, tall );

	int quarterWide = wide / 4;

	float fYaw = pLocalPlayer->GetAbsAngles().y + 180;
	float fOriginalYaw = fYaw;

	surface()->DrawSetTextFont( m_hNumberFont );
	surface()->DrawSetTextColor( 255, 255, 255, 150 );

	surface()->DrawSetColor( 200, 200, 200, 128 );

	int textWide, textTall;
	surface()->GetTextSize( m_hNumberFont, L"TT", textWide, textTall );

	int boxWideCenter = wide / 2 + textWide / 2 - quarterWide / 2;
	int boxTallCenter = tall - textTall - 8;
	DrawBox( boxWideCenter - textWide, boxTallCenter - 2, textWide + quarterWide, textTall + 2, Color( 200, 200, 200, 28 ), 1 );

	if ( fYaw > 314 && fYaw < 360 || fYaw <= 45 )
	{												
		if ( fYaw <= 314 )
			fYaw += 45;
		else
			fYaw -= 315;  

		float fAlpha = fabs( fYaw / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );	  			
		surface()->GetTextSize( m_hNumberFont, L"N", textWide, textTall );				
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * (fYaw / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"N", 1 ); // Didn't put strlen since I don't sure if the compiler can generate a constant

		fYaw = fOriginalYaw;
	}		

	if ( fYaw > 337 && fYaw < 360 || fYaw < 69 )
	{
		if ( fYaw <= 337 )
			fYaw += 23;
		else
			fYaw -= 338;	

		float fAlpha = fabs( fYaw / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"|", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * (fYaw / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"|", 1 );

		fYaw = fOriginalYaw;  		
	}	

	if ( fYaw <= 90 )
	{
		float fAlpha = fabs( fYaw / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"NW", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * (fYaw / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"NW", 2 );

		fYaw = fOriginalYaw;
	}

	if ( fYaw > 20 && fYaw < 110 )
	{
		float fAlpha = fabs( (fYaw - 23) / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"|", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * ((fYaw - 23) / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"|", 1 );

		fYaw = fOriginalYaw;
	}

	if ( fYaw > 42 && fYaw < 130 )
	{
		float fAlpha = fabs( (fYaw - 45) / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"W", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * ((fYaw - 45) / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"W", 1 );

		fYaw = fOriginalYaw;
	}

	if ( fYaw >= 65 && fYaw < 155 )
	{
		float fAlpha = fabs( (fYaw - 68) / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"|", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * ((fYaw - 68) / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"|", 1 );

		fYaw = fOriginalYaw;
	}

	if ( fYaw > 87 && fYaw < 177 )
	{
		float fAlpha = fabs( (fYaw - 90) / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"SW", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * ((fYaw - 90) / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"SW", 2 );

		fYaw = fOriginalYaw;
	}

	if ( fYaw > 110 && fYaw < 200 )
	{
		float fAlpha = fabs( (fYaw - 113) / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"|", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * ((fYaw - 113) / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"|", 1 );

		fYaw = fOriginalYaw;
	}

	if ( fYaw > 132 && fYaw < 222 )
	{
		float fAlpha = fabs( (fYaw - 135) / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"S", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * ((fYaw - 135) / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"S", 1 ); 

		fYaw = fOriginalYaw;
	}

	if ( fYaw > 155 && fYaw < 245 )
	{
		float fAlpha = fabs( (fYaw - 158) / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"|", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * ((fYaw - 158) / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"|", 1 );

		fYaw = fOriginalYaw;
	}

	if ( fYaw > 177 && fYaw < 267 )
	{
		float fAlpha = fabs( (fYaw - 180) / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"SE", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * ((fYaw - 180) / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"SE", 2 );

		fYaw = fOriginalYaw;
	}

	if ( fYaw > 200 && fYaw < 290 )
	{
		float fAlpha = fabs( (fYaw - 203) / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"|", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * ((fYaw - 203) / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"|", 1 );

		fYaw = fOriginalYaw;
	}

	if ( fYaw > 222 && fYaw < 312 )
	{
		float fAlpha = fabs( (fYaw - 225) / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"E", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * ((fYaw - 225) / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"E", 1 );

		fYaw = fOriginalYaw;
	}

	if ( fYaw > 245 && fYaw < 335 )
	{
		float fAlpha = fabs( (fYaw - 248) / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"|", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * ((fYaw - 248) / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"|", 1 );

		fYaw = fOriginalYaw;
	}

	if ( fYaw > 269 && fYaw < 361 || fYaw == 0 )
	{
		float fAlpha = fabs( (fYaw - 270) / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"NE", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * ((fYaw - 270) / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"NE", 2 );

		fYaw = fOriginalYaw;
	}

	if ( fYaw > 292 && fYaw < 360 || fYaw <= 22 )
	{
		if ( fYaw <= 292 )
			fYaw += 68;
		else
			fYaw -= 292;

		float fAlpha = fabs( fYaw / 90 - 0.5f );

		surface()->DrawSetTextColor( 255, 255, 255, 160 - fAlpha * 300 );
		surface()->GetTextSize( m_hNumberFont, L"|", textWide, textTall );
		surface()->DrawSetTextPos( boxWideCenter + (quarterWide * (fYaw / 90)) - textWide / 2, boxTallCenter );
		surface()->DrawPrintText( L"|", 1 );

		fYaw = fOriginalYaw;
	}
}

void CHudGame::DrawGhosts( C_NEOPlayer* pLocalPlayer )
{
	int wide, tall;
	GetSize( wide, tall );

	if ( m_Unknown464 < 0.33f )
		m_Unknown464 = 0.33f; 

	int x = 0;
	int y = 0;		

	for ( int i = 0; i < GetNumberOfGhosts(); i++ )
	{
		CWeaponGhost* pGhost = GetGhost( i );
		Assert( pGhost );

		if ( !GetVectorInScreenSpace( pGhost->GetAbsOrigin(), x, y ) )
			continue;

		bool bEnemyHasGhost = false;
		bool bFriendlyHasGhost = false;

		if ( pGhost->GetOwnerTeamNumber() > TEAM_UNASSIGNED && pLocalPlayer->GetTeamNumber() > TEAM_UNASSIGNED )
		{
			if ( pGhost->GetOwnerTeamNumber() == pLocalPlayer->GetTeamNumber() )
				bFriendlyHasGhost = true;
			else
				bEnemyHasGhost = true;

			if ( m_bCanPlayGhostPickupSound )
			{
				m_bCanPlayGhostPickupSound = false;
				pLocalPlayer->EmitSound( "HUD.GhostPickUp" );
			}
		}
		else
		{
			m_bCanPlayGhostPickupSound = true;
		}

		for ( int i = 0; i < 4; i++ )
		{
			int iAlpha = m_Unknown464 * (200 - m_Unknown444 * 200);
			int iTexSize = m_Unknown444 * 64;

			if ( bEnemyHasGhost )
			{
				surface()->DrawSetColor( 255, 0, 0, iAlpha );
			}
			else if ( bFriendlyHasGhost )
			{
				Color color = g_PR->GetTeamColor( pLocalPlayer->GetTeamNumber() );
				color[ 3 ] = iAlpha;
				surface()->DrawSetColor( color );
			}
			else
			{
				surface()->DrawSetColor( 200, 200, 200, iAlpha );
			}

			surface()->DrawSetTexture( m_iCtgGhostBeaconCircleTexture );
			surface()->DrawTexturedRect( x - iTexSize, y - iTexSize, x + iTexSize, y + iTexSize );

			m_Unknown444 += 4;
		}

		if ( bEnemyHasGhost )
			return;

		if ( x >= 0 )
		{
			if ( x > wide )
			{
				x = wide;
			}
		}
		else
		{
			x = 0;
		}

		if ( y >= 0 )
		{
			if ( y > tall )
			{
				y = tall;
			}
		}
		else
		{
			y = 0;
		}

		float v23 = 0;

		if ( x > wide / 2 )
			v23 = 1 - (x - wide * 0.5f) / (wide * 0.5f) * 90;
		else
			v23 = x / (wide * 0.5f) * 90;

		float v24 = 0;

		if ( y > tall / 2 )
			v24 = 1 - (y - tall * 0.5f) / (tall * 0.5f) * 90;
		else
			v24 = y / (tall * 0.5f) * 90;

		float fAlpha = v23 + v24;

		if ( fAlpha > 150 )
		{
			Vector vOrigin = pGhost->GetAbsOrigin();
			Vector vEyePosition = pLocalPlayer->EyePosition();
			vOrigin *= METERS_PER_INCH;
			vEyePosition *= METERS_PER_INCH;

			float fDistance = vOrigin.Length() - vEyePosition.Length();

			wchar_t buffer[ 32 ];
			V_snwprintf( buffer, ARRAYSIZE( buffer ), L"GHOST DISTANCE: %dm", fDistance );

			surface()->DrawSetTextFont( m_hNumberFont );

			int textWide, textTall;
			surface()->GetTextSize( m_hNumberFont, buffer, textWide, textTall );

			surface()->DrawSetTextPos( x - textWide / 2, y + 64 );
			surface()->DrawSetTextColor( 200, 200, 200, (fAlpha - 150) * 180 / 30 * m_Unknown464 ); // 180 / 30 must be some forumla (?)
			surface()->DrawPrintText( buffer, V_wcslen( buffer ) );
		}
	}				
}

void CHudGame::DrawGhostRetrievalPoints( C_NEOPlayer* pLocalPlayer )
{
	int wide, tall;
	GetSize( wide, tall );

	if ( m_Unknown464 < 0.33f )
		m_Unknown464 = 0.33f;

	int x = 0;
	int y = 0;

	for ( int i = 0; i < GetNumberOfGhostRetrievalPoints(); i++ )
	{
		C_NeoGhostRetrievalPoint* pRetrieval = GetGhostRetrievalPoint( i );
		Assert( pRetrieval );

		if ( !GetVectorInScreenSpace( pRetrieval->GetAbsOrigin(), x, y ) )
			continue;

		bool bFriendlyPoint = pRetrieval->GetOwningTeamNumber() == pLocalPlayer->GetTeamNumber();

		for ( int i = 0; i < 4; i++ )
		{
			int iAlpha = m_Unknown464 * (200 - m_Unknown444 * 200);
			int iTexSize = m_Unknown444 * 64;

			Color color; 	   
			if ( bFriendlyPoint )
				color = g_PR->GetTeamColor( pLocalPlayer->GetTeamNumber() );
			else
				color = Color( 255, 0, 0, 0 );	

			color[ 3 ] = iAlpha;

			surface()->DrawSetColor( color );

			surface()->DrawSetTexture( m_iCtgGhostBeaconArrowDownTexture );
			surface()->DrawTexturedRect( x - iTexSize, y - iTexSize, x + iTexSize, y + iTexSize );

			m_Unknown444 += 4;
		}

		// I don't understand what this one is doing out here, gotta test it later
		int iAlpha = m_Unknown464 * 200;
		int iTexSize = 64;

		Color color;
		if ( bFriendlyPoint )
			color = g_PR->GetTeamColor( pLocalPlayer->GetTeamNumber() );
		else
			color = Color( 255, 0, 0, 0 );

		color[ 3 ] = iAlpha;

		surface()->DrawSetColor( color );

		surface()->DrawSetTexture( m_iCtgGhostBeaconArrowDownTexture );
		surface()->DrawTexturedRect( x - iTexSize, y - iTexSize, x + iTexSize, y + iTexSize );

		if ( x >= 0 )
		{
			if ( x > wide )
			{
				x = wide;
			}
		}
		else
		{
			x = 0;
		}

		if ( y >= 0 )
		{
			if ( y > tall )
			{
				y = tall;
			}
		}
		else
		{
			y = 0;
		}

		float v23 = 0;

		if ( x > wide / 2 )
			v23 = 1 - (x - wide * 0.5f) / (wide * 0.5f) * 90;
		else
			v23 = x / (wide * 0.5f) * 90;

		float v24 = 0;

		if ( y > tall / 2 )
			v24 = 1 - (y - tall * 0.5f) / (tall * 0.5f) * 90;
		else
			v24 = y / (tall * 0.5f) * 90;

		float v25 = v23 + v24;

		if ( v25 > 150 )
		{
			Vector vOrigin = pRetrieval->GetPosition();
			Vector vEyePosition = pLocalPlayer->EyePosition();
			vOrigin *= METERS_PER_INCH;
			vEyePosition *= METERS_PER_INCH;

			float fDistance = vOrigin.Length() - vEyePosition.Length();

			wchar_t buffer[ 32 ];

			if ( bFriendlyPoint )
				V_snwprintf( buffer, ARRAYSIZE( buffer ), L"RETRIEVAL ZONE DISTANCE: %dm", fDistance );
			else
				V_snwprintf( buffer, ARRAYSIZE( buffer ), L"ENEMY RETRIEVAL ZONE DISTANCE: %dm", fDistance );

			surface()->DrawSetTextFont( m_hNumberFont );

			int textWide, textTall;
			surface()->GetTextSize( m_hNumberFont, buffer, textWide, textTall );

			surface()->DrawSetTextPos( x - textWide / 2, y + 64 );
			surface()->DrawSetTextColor( 200, 200, 200, (v25 - 150) * 180 / 30 * m_Unknown464 ); // 180 / 30 must be some forumla (?)
			surface()->DrawPrintText( buffer, V_wcslen( buffer ) );
		}
	}
}

static const wchar_t* s_szHintText[]
{	
	L".Init sequence started",
	L".Muxing input signals",
	L".Charging flux capacitor",
	L".Downloading update",
	L".ASSERT",
	L".Zero divide warning",
	L".Updating firmware",
	L".Booting combat readiness systems",
	L".Configuring firewalls",
	L".Testing neural weapon link sequencing",
	L".Compensating for combat network latency",
	L".Boosting comm signal",
	L".Repairing netsplit",
	L".Staging interrupts",
	L".Rebooting command module",
	L".Checking ROE subsystem",
	L".Initializing MARTYR(TM) system intercepts",
	L".HHOS",
	L".Linking to local systems",
	L".Analyzing signal cross talk",
	L".Loading runtime combat libraries",
	L".Warning, this has been code depreciated",
	L".Code 0x0",
	L".Reconfiguring threads for optimal speed",
	L".Reference count not zero",
	L".Garbage collection",
	L".Defraging memory",
	L".Sleeping",
	L".Entering critical section"
};

void CHudGame::DrawFreezeTimeInfo()
{
	static float fNextHintTime = 0;
	static int iCurrentHint = 0;

	if ( m_bShouldDrawHint )
	{
		m_bShouldDrawHint = false;

		fNextHintTime = gpGlobals->curtime + 1.0f;
		iCurrentHint = 0;

		char szHintTexture[ 24 ];
		V_snprintf( szHintTexture, sizeof( szHintTexture ), "vgui/hud/hints/hint%d", RandomInt( 0, 22 ) );

		surface()->DrawSetTextureFile( m_iHintTextureId, szHintTexture, true, false );
	}

	if ( gpGlobals->curtime > fNextHintTime )
	{
		fNextHintTime = gpGlobals->curtime + RandomFloat( 0.25f, 0.75f );
		iCurrentHint = RandomInt( 1, 28 );
	}

	int wide, tall;
	GetSize( wide, tall );

	float size = wide * 0.5f / 2.25f; // idk what to call these
	int x = wide - wide / 20 - size;
	int y = tall / 2 - 2 * size / 2 - 40;

	surface()->DrawSetColor( 0, 0, 0, 255 );
	surface()->DrawSetTexture( m_iHintTextureId );
	surface()->DrawTexturedRect( x - size, y, x + size, y + 2 * size );

	surface()->DrawSetTextFont( m_hBootFont );
	surface()->DrawSetTextPos( 20, tall - tall / 8 );
	surface()->DrawSetColor( 255, 255, 255, 100 );
	surface()->DrawPrintText( s_szHintText[ iCurrentHint ], V_wcslen( s_szHintText[ iCurrentHint ] ) );
}

void CHudGame::DrawGhostScan()
{
	int wide, tall;
	GetSize( wide, tall );

	surface()->DrawSetTextColor( 155, 55, 55, 255 );
	surface()->DrawSetTexture( m_iCtgGhostScanTextures[ m_Unknown304 ] );

	int textWide, textTall;
	surface()->GetTextSize( m_hNumberFont, L"T", textWide, textTall );

	surface()->DrawTexturedRect( wide / 2 - 64, tall - 2 * textTall - 69, wide / 2 + 64, tall - 2 * textTall - 5 );
}

#define SOMETHING_MAX_DISTANCE 1800

void CHudGame::DrawEnemyBeacon( C_NEOPlayer* pLocalPlayer )
{
	int wide, tall;
	GetSize( wide, tall );

	IGameResources* pGameResources = GameResources();

	int x = 0;
	int y = 0;

	float fDistance = 0;
	float v34 = 0;

	Vector vOffset( 0, 0, 50 );

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{		
		if ( pGameResources->IsLocalPlayer( i ) || pGameResources->GetTeam( i ) == pLocalPlayer->GetTeamNumber() || !pGameResources->IsAlive( i ) )
			continue;

		C_NEOPlayer* pPlayer = ToNEOPlayer( UTIL_PlayerByIndex( i ) );

		if ( !pPlayer->IsPlayer() || pPlayer->IsDormant() )
			continue;

		fDistance = (pPlayer->GetAbsOrigin() - pLocalPlayer->GetAbsOrigin()).Length();

		if ( fDistance > SOMETHING_MAX_DISTANCE )
			continue;

		if ( fDistance / SOMETHING_MAX_DISTANCE >= 0.75f )
			v34 = 1 - (fDistance - 0.75f) * 4;
		else
			v34 = 1;

		if ( GetVectorInScreenSpace( pPlayer->GetAbsOrigin(), x, y, &vOffset ) )
			break;
	}

	if ( x >= 0 )
	{
		if ( x > wide )
		{
			x = wide;
		}
	}
	else
	{
		x = 0;
	}

	if ( y >= 0 )
	{
		if ( y > tall )
		{
			y = tall;
		}
	}
	else
	{
		y = 0;
	}

	float v20 = 0;

	if ( x > wide / 2 )
		v20 = 1 - (x - wide * 0.5f) / (wide * 0.5f) * 90;
	else
		v20 = x / (wide * 0.5f) * 90;

	float v21 = 0;

	if ( y > tall / 2 )
		v21 = 1 - (y - tall * 0.5f) / (tall * 0.5f) * 90;
	else
		v21 = y / (tall * 0.5f) * 90;

	int iAlpha = (v20 + v21) * v34;

	surface()->DrawSetColor( 255, 255, 255, iAlpha );
	surface()->DrawSetTexture( m_iCtgGhostBeaconEnemyTexture );
	surface()->DrawTexturedRect( x - 32, y - 32, x + 32, y + 32 );

	wchar_t buffer[ 8 ];
	V_snwprintf( buffer, ARRAYSIZE( buffer ), L"%d m", fDistance *= METERS_PER_INCH ); // This way of convertion might be wrong

	surface()->DrawSetTextFont( m_hNumberFont );
	surface()->DrawSetTextPos( x + 32, y - 32 );
	surface()->DrawSetTextColor( 255, 255, 255, iAlpha );
	surface()->DrawPrintText( buffer, V_wcslen( buffer ) );
}

void CHudGame::DrawControlPoints( C_NEOPlayer* pLocalPlayer )
{
	int wide, tall;
	GetSize( wide, tall );

	IGameResources* pGameResources = GameResources();

	int x = 0;
	int y = 0;
	Vector vOffset( 0, 0, 50 );

	for ( int i = 0; i < NEOGameRules()->GetNumControlPoints(); i++ )
	{
		CNeoControlPoint* pControlPoint = NEOGameRules()->GetControlPoint( i );

		if ( !GetVectorInScreenSpace( pControlPoint->GetPosition(), x, y, &vOffset ) )
			continue;

		if ( x >= 0 )
		{
			if ( x > wide )
			{
				x = wide;
			}
		}
		else
		{
			x = 0;
		}

		if ( y >= 0 )
		{
			if ( y > tall )
			{
				y = tall;
			}
		}
		else
		{
			y = 0;
		}

		float v20 = 0;

		if ( x > wide / 2 )
			v20 = 1 - (x - wide * 0.5f) / (wide * 0.5f) * 90;
		else
			v20 = x / (wide * 0.5f) * 90;

		float v21 = 0;

		if ( y > tall / 2 )
			v21 = 1 - (y - tall * 0.5f) / (tall * 0.5f) * 90;
		else
			v21 = y / (tall * 0.5f) * 90;

		int iAlpha = v20 + v21;	// Not alpha idk

		int v24 = 32;

		if ( iAlpha > 150 )
		{
			v24 = iAlpha - 118;

			Color color = pGameResources->GetTeamColor( pControlPoint->GetOwner() );
			color[ 3 ] = iAlpha;

			surface()->DrawSetColor( color );
			surface()->DrawSetTexture( m_iCpTeamTextures[ pControlPoint->GetOwner() ] );
			surface()->DrawTexturedRect( x - v24, y - 3 * v24, x + v24, y - v24 );

			wchar_t buffer[ 64 ];
			g_pVGuiLocalize->ConvertANSIToUnicode( pControlPoint->GetName(), buffer, sizeof( buffer ) );

			int textWide, textTall;
			surface()->GetTextSize( m_hNumberFont, buffer, textWide, textTall );

			surface()->DrawSetTextPos( x - textWide / 2, y + v24 );
			surface()->DrawSetTextColor( 255, 255, 255, color.a() );
			surface()->DrawPrintText( buffer, V_wcslen( buffer ) );

			Vector vOrigin = pControlPoint->GetPosition();
			Vector vEyePosition = pLocalPlayer->EyePosition();
			vOrigin *= METERS_PER_INCH;
			vEyePosition *= METERS_PER_INCH;

			float fDistance = vOrigin.Length() - vEyePosition.Length();

			V_snwprintf( buffer, ARRAYSIZE( buffer ), L"DISTANCE: %d YDS", fDistance );

			surface()->GetTextSize( m_hNumberFont, buffer, textWide, textTall );

			surface()->DrawSetTextPos( x - textWide / 2, y + v24 + textTall );
			surface()->DrawSetTextColor( 255, 255, 255, color.a() );
			surface()->DrawPrintText( buffer, V_wcslen( buffer ) );
		}

		surface()->DrawSetColor( 255, 255, 255, iAlpha );
		surface()->DrawSetTexture( m_iCpNumberTextures[ pControlPoint->GetIcon() ] );
		surface()->DrawTexturedRect( x - v24, y - v24, x + v24, y + v24 );
	}
}

void CHudGame::DrawRoundMvp( C_NEOPlayer* pLocalPlayer, int iAlpha )
{
	if ( NEOGameRules()->m_iMVP <= 0 )
		return;

	int wide, tall;
	GetSize( wide, tall );

	IGameResources* pGameResources = GameResources();

	const char* szPlayerName = pGameResources->GetPlayerName( NEOGameRules()->m_iMVP );

	wchar_t wBufferName[ MAX_PLAYER_NAME_LENGTH + 1 ];
	g_pVGuiLocalize->ConvertANSIToUnicode( szPlayerName, wBufferName, sizeof( wBufferName ) );

	wchar_t buffer[ 64 ];
	V_snwprintf( buffer, ARRAYSIZE( buffer ), L"%s retrieved the ghost!", szPlayerName );

	surface()->DrawSetTextFont( m_hMVPFont );

	int textWide, textTall;
	surface()->GetTextSize( m_hMVPFont, buffer, textWide, textTall );

	surface()->DrawSetTextPos( wide / 2 - textWide / 2, tall - 4 * textTall );
	surface()->DrawSetTextColor( 200, 200, 200, iAlpha );
	surface()->DrawPrintText( buffer, V_wcslen( buffer ) );
}

void CHudGame::DrawJinraiWinLogo( C_NEOPlayer* pLocalPlayer )
{ 
	if ( m_Unknown472 )
	{
		m_Unknown472 = false;
		pLocalPlayer->EmitSound( "HUD.JinraiWin" );
	}

	int wide, tall;
	GetSize( wide, tall );

	float fDelta = gpGlobals->curtime - m_fLastGameHudChangeTime;
	int iAlpha = 155;

	if ( fDelta < 2 )
	{
		iAlpha = 155 - (2 - fDelta) * 155;

		if ( iAlpha < 0 )
			iAlpha = 0;
	}	
	else if ( fDelta > 6 )
	{ 
		fDelta = 8 - fDelta;
		iAlpha = 155 - (2 - fDelta) * 155;

		if ( iAlpha < 0 )
			iAlpha = 0;
	}

	surface()->DrawSetColor( 255, 255, 255, iAlpha );
	surface()->DrawSetTexture( m_iRoundEndJinraiTexture );
	surface()->DrawTexturedRect( wide / 2 - 256, tall / 3.25f - 128, wide / 2 + 256, tall / 3.25f + 128 );

	if ( NEOGameRules()->m_iGameType == GAMETYPE_CTG )
		DrawRoundMvp( pLocalPlayer, iAlpha );
}

void CHudGame::DrawNsfWinLogo( C_NEOPlayer* pLocalPlayer )
{
	if ( m_Unknown472 )
	{
		m_Unknown472 = false;
		pLocalPlayer->EmitSound( "HUD.NSFWin" );
	}

	int wide, tall;
	GetSize( wide, tall );

	float fDelta = gpGlobals->curtime - m_fLastGameHudChangeTime;
	int iAlpha = 155;

	if ( fDelta < 2 )
	{
		iAlpha = 155 - (2 - fDelta) * 155;

		if ( iAlpha < 0 )
			iAlpha = 0;
	}
	else if ( fDelta > 6 )
	{
		fDelta = 8 - fDelta;
		iAlpha = 155 - (2 - fDelta) * 155;

		if ( iAlpha < 0 )
			iAlpha = 0;
	}

	surface()->DrawSetColor( 255, 255, 255, iAlpha );
	surface()->DrawSetTexture( m_iRoundEndNsfTexture );
	surface()->DrawTexturedRect( wide / 2 - 256, tall / 3.25f - 128, wide / 2 + 256, tall / 3.25f + 128 );

	if ( NEOGameRules()->m_iGameType == GAMETYPE_CTG )
		DrawRoundMvp( pLocalPlayer, iAlpha );
}

void CHudGame::DrawTieLogo( C_NEOPlayer* pLocalPlayer )
{
	if ( m_Unknown472 )
	{
		m_Unknown472 = false;
		pLocalPlayer->EmitSound( "HUD.Draw" );
	}

	int wide, tall;
	GetSize( wide, tall );

	float fDelta = gpGlobals->curtime - m_fLastGameHudChangeTime;
	int iAlpha = 155;

	if ( fDelta < 2 )
	{
		iAlpha = 155 - (2 - fDelta) * 155;

		if ( iAlpha < 0 )
			iAlpha = 0;
	}
	else if ( fDelta > 6 )
	{
		fDelta = 8 - fDelta;
		iAlpha = 155 - (2 - fDelta) * 155;

		if ( iAlpha < 0 )
			iAlpha = 0;
	}

	surface()->DrawSetColor( 255, 255, 255, iAlpha );
	surface()->DrawSetTexture( m_iRoundEndTieTexture );
	surface()->DrawTexturedRect( wide / 2 - 256, tall / 3.25f - 128, wide / 2 + 256, tall / 3.25f + 128 );
}

void CHudGame::DrawGameElements( C_NEOPlayer* pLocalPlayer )
{
	if ( m_iGameHud != NEOGameRules()->m_iGameHud )
		m_fLastGameHudChangeTime = gpGlobals->curtime;

	switch ( NEOGameRules()->m_iGameHud )
	{
		case 0:
			switch ( NEOGameRules()->m_iGameType )
			{
				case GAMETYPE_CTG:
					if ( !pLocalPlayer->HasGhost() )
					{
						DrawGhosts( pLocalPlayer );		
						DrawGhostRetrievalPoints( pLocalPlayer );

						m_Unknown432 = 0;
						m_Unknown304 = 0;
					}
					else if ( pLocalPlayer->GetActiveNEOWeapon() && pLocalPlayer->GetActiveNEOWeapon()->GetWeaponID() == WEAPON_GHOST )
					{
						if ( m_bShouldPlayGhostEquip )
						{
							m_bShouldPlayGhostEquip = false;
							pLocalPlayer->EmitSound( "HUD.GhostEquip" );
						}

						m_Unknown432 += m_Unknown428;

						if ( m_Unknown432 >= 0.1f )
						{
							m_Unknown432 = 0;

							// dunno what should be here, got to actually think about it later
							m_Unknown304++;
							//for ( ; m_Unknown304 < 30; m_Unknown304++ )	// This is wrong for sure
							//{
								if ( m_Unknown304 == 30 )
									DrawEnemyBeacon( pLocalPlayer );

								DrawGhostScan();
								DrawGhostRetrievalPoints( pLocalPlayer );
							//}
						}
					}
					else
					{			
						DrawGhostRetrievalPoints( pLocalPlayer );

						m_bShouldPlayGhostEquip = true;
						m_Unknown432 = 0;
						m_Unknown304 = 0;
					}
					break;

				case GAMETYPE_VIP:
					DrawEnemyBeacon( pLocalPlayer );
					break;
			}
			break;

		case 1:
		case 2:
			break;

		case 3:
			break;

		case 4:	   			
			DrawJinraiWinLogo( pLocalPlayer );
			break;

		case 5:
			DrawNsfWinLogo( pLocalPlayer );
			break;

		case 6:
			DrawTieLogo( pLocalPlayer );
			break;
	}

	if ( NEOGameRules()->IsInFreezePeriod() && pLocalPlayer->GetActiveNEOWeapon() )
	{
		m_Unknown472 = true;
		DrawFreezeTimeInfo();
	}
	else
	{
		m_bShouldDrawHint = true;
	}

	m_iGameHud = NEOGameRules()->m_iGameHud;
}

void CHudGame::Paint()
{
	C_NEOPlayer* pLocalPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( !pLocalPlayer || !NEOGameRules() )
		return;

	DrawCompass( pLocalPlayer );
	DrawGameElements( pLocalPlayer );
}

void CHudGame::ApplySchemeSettings( IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	int wide, tall;
	surface()->GetScreenSize( wide, tall );

	SetBounds( 0, 0, wide, tall );
}

void CHudGame::OnThink()
{
	float fDelta = gpGlobals->curtime - m_fLastThinkTime;
	m_Unknown428 = fDelta;

	if ( fDelta > 1 )
		fDelta = 0.1f;

	m_Unknown444 += fDelta * 0.5f;

	if ( m_Unknown444 > 1 )
		m_Unknown444 -= 1;

	m_Unknown448 += fDelta * 0.5f;

	if ( m_Unknown448 > 1 )
		m_Unknown448 -= 1;

	m_Unknown452 += fDelta * 0.5f;

	if ( m_Unknown452 > 1 )
		m_Unknown452 -= 1;

	m_Unknown456 += fDelta * 0.5f;

	if ( m_Unknown456 > 1 )
		m_Unknown456 -= 1;

	m_Unknown464 += fDelta * m_Unknown468;

	if ( m_Unknown464 > 1 || m_Unknown464 < 0 )
	{
		m_Unknown464 = 0;
		m_Unknown468 *= -1;
	}

	m_fLastThinkTime = gpGlobals->curtime;
}
