#include "cbase.h"
#include "hud.h"
#include "hud_macros.h"
#include "view.h"
#include "iclientmode.h"

#include <KeyValues.h>
#include <vgui/ISystem.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Panel.h>

#include <vgui/ILocalize.h>

#include "hudelement.h"

#include "c_neo_player.h"
#include "weapon_neobase.h"
#include "c_playerresource.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static ConVar hud_centerid( "hud_centerid", "1" );
static ConVar hud_showtargetid( "hud_showtargetid", "1" );

//-----------------------------------------------------------------------------
// Purpose: Health panel
//-----------------------------------------------------------------------------
class CNeoHudTargetID : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CNeoHudTargetID, vgui::Panel );

public:
	CNeoHudTargetID( const char *pElementName );

	virtual void Init();
	virtual void VidInit();

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );

	virtual void Paint();

private:
	Color			GetColorForTargetTeam( int iTeamNumber );

	CPanelAnimationVar( vgui::HFont, m_hNumberFont, "NumberFont", "NHudOCRSmall" );

	vgui::HFont		m_hFont;
	int				m_iLastEntIndex;
	float			m_flLastChangeTime;
};	

DECLARE_HUDELEMENT_DEPTH( CNeoHudTargetID, 50 );

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CNeoHudTargetID::CNeoHudTargetID( const char *pElementName ) : CHudElement( pElementName ), Panel( nullptr, "TargetID" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_hFont = g_hFontTrebuchet24;
	m_flLastChangeTime = 0;
	m_iLastEntIndex = 0;

	SetHiddenBits( HIDEHUD_MISCSTATUS );
}	  

void CNeoHudTargetID::Init()
{
}

void CNeoHudTargetID::VidInit()
{
	CHudElement::VidInit();

	m_flLastChangeTime = 0;
	m_iLastEntIndex = 0;
}

Color CNeoHudTargetID::GetColorForTargetTeam( int iTeamNumber )
{
	return GameResources()->GetTeamColor( iTeamNumber );
}

void CNeoHudTargetID::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );		   

	m_hFont = pScheme->GetFont( "TargetID", IsProportional() );

	SetPaintBackgroundEnabled( false );
}

void CNeoHudTargetID::Paint()
{				
#define MAX_ID_STRING 256
	wchar_t sIDString[ MAX_ID_STRING ];
	sIDString[ 0 ] = 0;

	if ( !hud_showtargetid.GetBool() )
		return;

	C_NEOPlayer* pLocalPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( !pLocalPlayer )
		return;

	Color c;

	// Get our target's ent index
	int iEntIndex = pLocalPlayer->GetIDTarget();
	// Didn't find one?
	if ( !iEntIndex )
	{
		// Check to see if we should clear our ID
		if ( m_flLastChangeTime && (gpGlobals->curtime > (m_flLastChangeTime + 0.5)) )
		{
			m_flLastChangeTime = 0;
			m_iLastEntIndex = 0;
		}
		else
		{
			// Keep re-using the old one
			iEntIndex = m_iLastEntIndex;
		}
	}
	else
	{
		m_flLastChangeTime = gpGlobals->curtime;
	}

	if ( iEntIndex )
	{
		C_NEOPlayer* pPlayer = (C_NEOPlayer*) UTIL_PlayerByIndex( iEntIndex );

		wchar_t wszPlayerName[ MAX_PLAYER_NAME_LENGTH ];
		wchar_t wszHealthText[ 10 ];
		bool bShowPlayerName = false;

		if ( IsPlayerIndex( iEntIndex ) )
		{
			c = GetColorForTargetTeam( pPlayer->GetTeamNumber() );
			c[ 3 ] = 200;

			bShowPlayerName = true;
			g_pVGuiLocalize->ConvertANSIToUnicode( pPlayer->GetPlayerName(), wszPlayerName, sizeof( wszPlayerName ) );

			if ( pPlayer->InSameTeam( pLocalPlayer ) )
			{ 
				if ( pLocalPlayer->m_iStar >= STAR_NONE && pPlayer->m_iStar == pLocalPlayer->m_iStar )
				{
					bShowPlayerName = false;
				}
				else
				{
					_snwprintf( wszHealthText, ARRAYSIZE( wszHealthText ) - 1, L"%.0f%%", ((float) pPlayer->GetHealth() / (float) pPlayer->GetMaxHealth()) );
					wszHealthText[ ARRAYSIZE( wszHealthText ) - 1 ] = '\0';
					g_pVGuiLocalize->ConstructString( sIDString, sizeof( sIDString ), L"%s1 : %s2", 2, wszPlayerName, wszHealthText );
				}
			}
			else
			{ 
				g_pVGuiLocalize->ConstructString( sIDString, sizeof( sIDString ), L"%s1", 1, wszPlayerName );
			}
		}

		if ( sIDString[ 0 ] )
		{
			int wide, tall;
			int ypos = YRES( 260 );
			int xpos = XRES( 10 );

			vgui::surface()->GetTextSize( m_hFont, sIDString, wide, tall );

			if ( hud_centerid.GetInt() == 0 )
			{
				ypos = YRES( 420 );
			}
			else
			{
				xpos = (ScreenWidth() - wide) / 2;
			}

			vgui::surface()->DrawSetTextFont( m_hFont );
			vgui::surface()->DrawSetTextPos( xpos, ypos );
			vgui::surface()->DrawSetTextColor( c );
			vgui::surface()->DrawPrintText( sIDString, wcslen( sIDString ) );
		}
	}
}
