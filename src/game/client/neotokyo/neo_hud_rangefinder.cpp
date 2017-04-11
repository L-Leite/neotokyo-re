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
#include "weapon_neobase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Health panel
//-----------------------------------------------------------------------------
class CNHudRangeFinder : public CHudElement, public CNHudNumericDisplay
{
	DECLARE_CLASS_SIMPLE( CNHudRangeFinder, CNHudNumericDisplay );

public:
	CNHudRangeFinder( const char *pElementName );

	virtual void Init();
	virtual void VidInit();

	virtual void OnThink();	   
	virtual void Paint();

	virtual void DrawNumber( vgui::HFont hFont, int x, int y, int number );

private:
	CPanelAnimationVarAliasType( float, m_flDigit_XPos, "digit_xpos", "50", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flDigit_YPos, "digit_ypos", "2", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flDigit2_XPos, "digit2_xpos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flDigit2_YPos, "digit2_ypos", "0", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flBar_XPos, "bar_xpos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flBar_YPos, "bar_ypos", "0", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flBar_Width, "bar_width", "2", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flBar_Height, "bar_height", "2", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flIcon_XPos, "icon_xpos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flIcon_YPos, "icon_ypos", "0", "proportional_float" );

	CPanelAnimationVar( Color, m_TextColor, "TextColor", "FgColor" );

	CPanelAnimationVar( vgui::HFont, m_hNumberFont, "NumberFont", "NHudOCRSmall" );

	int	m_iWhiteAdditiveTexture;

	wchar_t m_szRangeString[ 16 ];

	float m_fDistance;
	bool m_bShouldDraw;
};	

DECLARE_HUDELEMENT_DEPTH( CNHudRangeFinder, 50 );

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CNHudRangeFinder::CNHudRangeFinder( const char *pElementName ) : CHudElement( pElementName ), CNHudNumericDisplay( nullptr, "NHudRangeFinder" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_PLAYERDEAD | HIDEHUD_HEALTH | HIDEHUD_WEAPONSELECTION );

	m_bShouldDraw = false;

	m_iWhiteAdditiveTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iWhiteAdditiveTexture, "vgui/white_additive", true, false );
}	  

void CNHudRangeFinder::Init()
{
	V_wcsncpy( m_szRangeString, L"RANGE", sizeof( m_szRangeString ) );
}

void CNHudRangeFinder::VidInit()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNHudRangeFinder::OnThink()
{
	C_NEOPlayer* pLocalPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( pLocalPlayer )
	{
		C_WeaponNEOBase* pWeapon = pLocalPlayer->GetActiveNEOWeapon();

		if ( pWeapon )
		{
			SetPaintEnabled( true );
			SetPaintBackgroundEnabled( true );

			if ( pWeapon->m_Unknown1892 == 2 )
			{
				m_bShouldDraw = true;

				Vector vecForward;
				Vector vecEyePos = pLocalPlayer->EyePosition();
				pLocalPlayer->GetVectors( &vecForward, nullptr, nullptr );	

				trace_t ptr;
				UTIL_TraceLine( vecEyePos, vecEyePos + vecForward * 36000.0f, CONTENTS_HITBOX | CONTENTS_DEBRIS | CONTENTS_MONSTER | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_SOLID, pLocalPlayer, COLLISION_GROUP_NONE, &ptr );

				Vector vStart = ptr.startpos;
				Vector vEnd = ptr.endpos;
				vStart *= METERS_PER_INCH;
				vEnd *= METERS_PER_INCH;

				float fDistance = vEnd.Length() - vStart.Length();

				if ( ptr.surface.flags & SURF_SKY )
					m_fDistance = 1000.0f;
				else
					m_fDistance = fDistance;
			}
			else
			{
				m_bShouldDraw = false;
			}
		}
	}
	else
	{
		SetPaintEnabled( false );
		SetPaintBackgroundEnabled( false );
	}
}

void CNHudRangeFinder::Paint()
{
	if ( !m_bShouldDraw )
		return;

	int wide, tall;
	GetSize( wide, tall );

	surface()->DrawSetTextFont( m_hNumberFont );
	surface()->DrawSetTextColor( 255, 255, 255, 100 );
	surface()->DrawSetTextPos( m_flText_XPos, m_flText_YPos );

	surface()->DrawPrintText( m_szRangeString, V_wcslen( m_szRangeString ) );

	surface()->DrawSetTextColor( 255, 255, 255, 100 );
	DrawNumber( m_hNumberFont, m_flDigit_XPos, m_flDigit_YPos, m_fDistance );
}

void CNHudRangeFinder::DrawNumber( HFont hFont, int x, int y, int number )
{
	wchar_t buffer[ 24 ];

	if ( number < 999 )
		V_snwprintf( buffer, ARRAYSIZE( buffer ), L"%d", number );
	else
		V_snwprintf( buffer, ARRAYSIZE( buffer ), L"000m" );

	int charWidth = surface()->GetCharacterWidth( hFont, 48 );

	if ( number < 100 && m_Unknown246 )
		x += charWidth;

	if ( number < 10 && m_Unknown246 )
		x += charWidth;

	int textWide, textTall;
	surface()->GetTextSize( hFont, buffer, textWide, textTall );

	surface()->DrawSetTextPos( x - textWide, y );		  
	surface()->DrawSetTextFont( hFont );

	surface()->DrawPrintText( buffer, V_wcslen( buffer ) );
}
