#include "cbase.h"
#include "hud.h"
#include "hud_macros.h"
#include "view.h"
#include "iclientmode.h"

#include <KeyValues.h>
#include <vgui/ISystem.h>
#include <vgui/ISurface.h>

#include <vgui/ILocalize.h>

#include "hudelement.h"
#include "neo_hud_numericdisplay.h"

#include "c_neo_player.h"
#include "neo_gamerules.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SECONDS_TO_MINUTES( seconds ) seconds / 60 < 0 ? 0 : seconds / 60
#define SECONDS_TO_SECONDS( seconds ) seconds % 60 < 0 ? 0 : seconds % 60

//-----------------------------------------------------------------------------
// Purpose: Health panel
//-----------------------------------------------------------------------------
class CNHudRoundTimer : public CHudElement, public CNHudNumericDisplay
{
	DECLARE_CLASS_SIMPLE( CNHudRoundTimer, CNHudNumericDisplay );

public:
	CNHudRoundTimer( const char *pElementName );

	virtual void Init();
	virtual void VidInit();

	virtual void OnThink();	   
	virtual void Paint();

	virtual void DrawNumber( vgui::HFont hFont, int x, int y, int number );

	void DrawTime( vgui::HFont hFont, int x, int y, int iMinutes, int iSeconds );

private:																				
	CPanelAnimationVarAliasType( float, m_flTimer_XPos, "timer_xpos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flTimer_YPos, "timer_ypos", "0", "proportional_float" );

	CPanelAnimationVar( Color, m_TextColor, "TextColor", "FgColor" );

	CPanelAnimationVar( vgui::HFont, m_hNumberFont, "NumberFont", "NHudOCRSmall" );

	int	m_iWhiteAdditiveTexture;
};	

DECLARE_HUDELEMENT_DEPTH( CNHudRoundTimer, 50 );

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CNHudRoundTimer::CNHudRoundTimer( const char *pElementName ) : CHudElement( pElementName ), CNHudNumericDisplay( nullptr, "NHudRoundTimer" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_PLAYERDEAD | HIDEHUD_HEALTH | HIDEHUD_WEAPONSELECTION );

	m_iWhiteAdditiveTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iWhiteAdditiveTexture, "vgui/white_additive", true, false );
}	  

void CNHudRoundTimer::Init()
{
}

void CNHudRoundTimer::VidInit()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNHudRoundTimer::OnThink()
{
	C_NEOPlayer* pLocalPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( pLocalPlayer && pLocalPlayer->m_iClassType >= CLASS_UNASSIGNED )
	{
		SetPaintEnabled( true );
		SetPaintBackgroundEnabled( true );
	}
	else
	{
		SetPaintEnabled( false );
		SetPaintBackgroundEnabled( false );
	}
}

void CNHudRoundTimer::Paint()
{				
	int wide, tall;
	GetSize( wide, tall );

	surface()->DrawSetTextColor( 255, 255, 255, 100 );
	
	float fTimeLeft = NEOGameRules()->m_fRoundTimeLeft;

	if ( fTimeLeft < 0.0f )
		fTimeLeft = 0.0f;

	DrawTime( m_hNumberFont, m_flTimer_XPos, m_flTimer_YPos, SECONDS_TO_MINUTES( (int)fTimeLeft ), SECONDS_TO_SECONDS( (int)fTimeLeft ) );
}

void CNHudRoundTimer::DrawNumber( HFont hFont, int x, int y, int number )
{
	wchar_t buffer[ 10 ];
	V_snwprintf( buffer, ARRAYSIZE( buffer ), L"%d", number );

	int charWidth = surface()->GetCharacterWidth( hFont, 48 );

	if ( number < 100 && m_Unknown246 )
		x += charWidth;

	if ( number < 10 && m_Unknown246 )
		x += charWidth;

	int textWide, textTall;
	surface()->GetTextSize( hFont, buffer, textWide, textTall );

	surface()->DrawSetTextPos( x - textWide, y );
	surface()->DrawSetTextFont( hFont );

	int bufLen = V_wcslen( buffer );

	for ( int i = 0; i < bufLen; i++ )
		surface()->DrawUnicodeChar( buffer[ i ] );
}

void CNHudRoundTimer::DrawTime( HFont hFont, int x, int y, int iMinutes, int iSeconds )
{
	wchar_t buffer[ 16 ];
	V_snwprintf( buffer, ARRAYSIZE( buffer ), L"%d:%.2d", iMinutes, iSeconds );

	surface()->DrawSetTextPos( x, y );
	surface()->DrawSetTextFont( hFont );

	int bufLen = V_wcslen( buffer );

	for ( int i = 0; i < bufLen; i++ )
		surface()->DrawUnicodeChar( buffer[ i ] );
}
