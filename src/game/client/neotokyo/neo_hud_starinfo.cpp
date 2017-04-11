#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "iclientmode.h"
#include "c_neo_player.h"
#include "c_playerresource.h"

#include <vgui/ILocalize.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui_controls/AnimationController.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Draws the zoom screen
//-----------------------------------------------------------------------------
class CHudStarInfo : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudStarInfo, vgui::Panel );

public:
	CHudStarInfo( const char *pElementName );
	
	void	Init( void );
	void	VidInit( void );
	void	Reset( void );

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	virtual void OnThink();
	virtual void PaintBackground();
	virtual void Paint();

private:				
	void DrawStar( C_NEOPlayer* pLocalPlayer );
	void DrawSquadmatesNames( C_NEOPlayer* pLocalPlayer );

	CPanelAnimationVarAliasType( float, m_flLineHeight, "LineHeight", "48", "proportional_float" );

	CPanelAnimationVar( float, m_flMaxDeathNotices, "MaxDeathNotices", "4" );

	CPanelAnimationVar( bool, m_bRightJustify, "RightJustify", "0" );

	CPanelAnimationVar( vgui::HFont, m_hTextFont, "TextFont", "NHudOCRSmall" );

	int m_Unknown168;
	int m_Unknown172;

	int m_iStarTexture;
	int m_iNonStarTexture;
	int m_StarTextures[ STAR_MAXCOUNT ];

	bool m_Unknown220;
	float m_Unknown224;
};

DECLARE_HUDELEMENT_DEPTH( CHudStarInfo, 62 );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudStarInfo::CHudStarInfo( const char *pElementName ) : CHudElement(pElementName), BaseClass(NULL, "HudGameMessage")
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	
	SetHiddenBits( HIDEHUD_MISCSTATUS | HIDEHUD_HEALTH | HIDEHUD_WEAPONSELECTION );

	m_iStarTexture = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_iStarTexture, "vgui/hud/star", true, false );

	m_iNonStarTexture = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_iNonStarTexture, "vgui/hud/non_star", true, false );

	m_StarTextures[ STAR_NONE ] = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_StarTextures[ STAR_NONE ], "vgui/hud/star_none", true, false );

	m_StarTextures[ STAR_ALPHA ] = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_StarTextures[ STAR_ALPHA ], "vgui/hud/star_alpha", true, false );

	m_StarTextures[ STAR_BRAVO ] = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_StarTextures[ STAR_BRAVO ], "vgui/hud/star_bravo", true, false );

	m_StarTextures[ STAR_CHARLIE ] = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_StarTextures[ STAR_CHARLIE ], "vgui/hud/star_charlie", true, false );

	m_StarTextures[ STAR_DELTA ] = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_StarTextures[ STAR_DELTA ], "vgui/hud/star_delta", true, false );

	m_StarTextures[ STAR_ECHO ] = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_StarTextures[ STAR_ECHO ], "vgui/hud/star_echo", true, false );
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CHudStarInfo::Init( void )
{
	Reset();
}

void CHudStarInfo::VidInit( void )
{
	Reset();

	int wide, tall;
	vgui::surface()->GetScreenSize( wide, tall );

	SetBounds( 0, 0, wide, tall );
}

void CHudStarInfo::Reset( void )
{
	m_Unknown220 = false;
	m_Unknown224 = 5.0f;
}

//-----------------------------------------------------------------------------
// Purpose: sets scheme colors
//-----------------------------------------------------------------------------
void CHudStarInfo::ApplySchemeSettings( vgui::IScheme *scheme )
{
	BaseClass::ApplySchemeSettings(scheme);

	int screenWide, screenTall;
	GetHudSize(screenWide, screenTall);
	SetBounds(0, 0, screenWide, screenTall);
}

void CHudStarInfo::OnThink()
{
}

void CHudStarInfo::PaintBackground()
{
}

static const char* s_szClassRanks[]
{
	"Unassigned",
	"Recon",
	"Assault",
	"Support",
	"Operator"
};

static const char* s_szRankAbreviations[]
{
	"Fng",
	"Pvt",
	"Cpl",
	"Sgt",
	"Lt",
	"Maj"
};

void CHudStarInfo::DrawStar( C_NEOPlayer* pLocalPlayer )
{
	if ( m_Unknown224 - gpGlobals->curtime < 0.0f )
	{
		m_Unknown220 = !m_Unknown220;
		m_Unknown224 = 5.0f;
	}

	Color starColor( 128, 128, 128, 96 );

	if ( pLocalPlayer->m_iStar > STAR_NONE )
	{
		starColor = g_PR->GetTeamColor( pLocalPlayer->GetTeamNumber() );
		starColor[ 3 ] = 155;
	}

	vgui::surface()->DrawSetColor( starColor );
	vgui::surface()->DrawSetTexture( m_StarTextures[ pLocalPlayer->m_iStar ] );
	vgui::surface()->DrawTexturedRect( 0, 0, 192, 48 );

	vgui::surface()->DrawSetTextFont( m_hTextFont );

	wchar_t buffer[ 512 ];
	g_pVGuiLocalize->ConvertANSIToUnicode( "TEST", buffer, sizeof( buffer ) );

	int wide, tall;
	vgui::surface()->GetTextSize( m_hTextFont, buffer, wide, tall );

	int iCurrentY = 48;

	wchar_t convertedBuffer[ 512 ];

	if ( pLocalPlayer->m_iStar > STAR_NONE )
	{
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			C_NEOPlayer* player = (C_NEOPlayer*) UTIL_PlayerByIndex( i );

			if ( !player || g_PR->IsLocalPlayer( i ) || g_PR->GetTeam( i ) != player->GetTeamNumber() || g_PR->GetStar( i ) <= STAR_NONE || g_PR->GetStar( i ) != player->GetTeamNumber() )
				continue;		  
			
			char starBuffer[ 512 ];

			if ( g_PR->IsAlive( i ) )
			{			 				
				V_snprintf( starBuffer, sizeof( starBuffer ), "%s\t%s  [%s]  Integrity %d", g_PR->GetPlayerName( i ), s_szRankAbreviations[ player->m_iRank ], s_szClassRanks[ player->m_iClassType ], player->GetHealth() );
				g_pVGuiLocalize->ConvertANSIToUnicode( starBuffer, convertedBuffer, sizeof( convertedBuffer ) );

				vgui::surface()->DrawSetTextPos( 10, iCurrentY );
				vgui::surface()->DrawSetTextColor( 255, 255, 255, 125 );
			}
			else
			{
				V_snprintf( starBuffer, sizeof( starBuffer ), "%s  [%s]  DEAD", g_PR->GetPlayerName( i ), s_szClassRanks[ player->m_iClassType ] );

				vgui::surface()->DrawSetTextPos( 10, iCurrentY );
				vgui::surface()->DrawSetTextColor( 255, 255, 255, 55 );
			}

			vgui::surface()->DrawPrintText( convertedBuffer, V_wcslen( convertedBuffer ) );

			iCurrentY += tall;
		}
	}
	else
	{	
		const char* pDefaultString = "PRESS F5-F9 TO JOIN A SQUAD, OR F10 FOR NO SQUAD";

		if ( m_Unknown220 )
			pDefaultString = "EACH SQUAD HAS A LIMIT OF 7";

		g_pVGuiLocalize->ConvertANSIToUnicode( pDefaultString, convertedBuffer, sizeof( convertedBuffer ) );

		vgui::surface()->DrawSetTextPos( 10, 48 );
		vgui::surface()->DrawSetTextColor( 255, 255, 255, 55 );
	}
}


void CHudStarInfo::DrawSquadmatesNames( C_NEOPlayer* pLocalPlayer )
{
	int wide, tall;
	GetSize( wide, tall );

	bool bSomething = false;
	int iSomething = 0;

	int x = 0;
	int y = 0;

	C_NEOPlayer* pPlayer = nullptr;

	Vector vOffset( 0.0f, 0.0f, 55.0f );

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		if ( !g_PR->IsLocalPlayer( i ) )
		{ 
			pPlayer = (C_NEOPlayer*) UTIL_PlayerByIndex( i );

			if ( !pPlayer )
				continue;

			if ( g_PR->GetTeam( i ) != pPlayer->GetTeamNumber() )
			{
				pPlayer->m_Unknown4132 = 0;
				break;
			}

			if ( !g_PR->IsAlive( i ) )
				continue;

			iSomething = m_Unknown172;

			if ( g_PR->GetStar( i ) != pPlayer->m_iStar || pPlayer->m_iStar <= STAR_NONE )
				break;

			bSomething = true;
			iSomething = m_Unknown168;

			pPlayer->m_Unknown4132 = 0;

			if ( GetVectorInScreenSpace( pPlayer->GetAbsOrigin(), x, y, &vOffset ) )
				break;
		}
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
	
	float v22 = 0.0f;
	float v23 = 0.0f;

	if ( x > wide / 2 )
		v22 = 1 - (x - wide * 0.5f) / (wide * 0.5f) * 90;
	else
		v22 = x / (wide * 0.5f) * 90;

	if ( y > tall / 2 )
		v23 = 1 - (y - tall * 0.5f) / (tall * 0.5f) * 90;
	else
		v23 = y / (tall * 0.5f) * 90;

	int v24 = v23 + v22;

	int v25 = 16;

	if ( bSomething )
		v25 = 24;

	Color color = g_PR->GetTeamColor( pLocalPlayer->entindex() );

	if ( v24 > 150 )
	{
		if ( bSomething )
		{
			color[ 3 ] = (v24 - 150) * 255.0f / 30.0f;

			vgui::surface()->DrawSetTextFont( m_hTextFont );

			wchar_t buffer[ 250 ];
			g_pVGuiLocalize->ConvertANSIToUnicode( pPlayer->GetPlayerName(), buffer, sizeof( buffer ) ); // Why would you use vguilocalize

			int textWide, textTall;
			vgui::surface()->GetTextSize( m_hTextFont, buffer, textWide, textTall );

			vgui::surface()->DrawSetTextPos( x - textWide / 2, v25 + y );
			vgui::surface()->DrawSetTextColor( color );
			vgui::surface()->DrawPrintText( buffer, V_wcslen( buffer ) );

			Vector vOrigin = pPlayer->GetAbsOrigin();
			Vector vEyePosition = pLocalPlayer->EyePosition();
			vOrigin *= METERS_PER_INCH;
			vEyePosition *= METERS_PER_INCH;
					
			float fDistance = vOrigin.Length() - vEyePosition.Length();

			wchar_t distBuffer[ 30 ];					
			V_snwprintf( distBuffer, ARRAYSIZE( distBuffer ), L"DISTANCE: %dm", fDistance ); // Game has fDistance / 12.0f / 3.0f / 0.915f instead of fDistance *= METERS_PER_INCH... shouldn't make much a difference though.

			int distWide, distTall;
			vgui::surface()->GetTextSize( m_hTextFont, distBuffer, distWide, distTall );
			vgui::surface()->DrawSetTextPos( x - distWide / 2, y + v25 + distTall );
			vgui::surface()->DrawPrintText( distBuffer, V_wcslen( distBuffer ) );

			vgui::surface()->DrawSetTextPos( x - distWide / 2, y + v24 + distTall ); 	

			color[ 3 ] = 225;
		}
		else
		{
			color[ 3 ] = 190;
		}	
		
		vgui::surface()->DrawSetTextColor( color );
		vgui::surface()->DrawSetTexture( iSomething );
		vgui::surface()->DrawTexturedRect( x - v25, y - v25, x + v25, y + v25 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: draws the zoom effect
//-----------------------------------------------------------------------------
void CHudStarInfo::Paint()
{		  
	C_NEOPlayer* pLocalPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( !pLocalPlayer )
		return;

	DrawStar( pLocalPlayer );
	DrawSquadmatesNames( pLocalPlayer );
}
