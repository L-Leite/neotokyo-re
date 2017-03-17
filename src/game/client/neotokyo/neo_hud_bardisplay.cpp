#include "cbase.h"
#include "iclientmode.h"

#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/Panel.h>

#include "vgui/ILocalize.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CNHudBarDisplay : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CNHudBarDisplay, vgui::Panel );

public:
	CNHudBarDisplay( const char *pElementName );

protected:
	virtual void PaintBackground();
	virtual void Paint();

private:																			 
	CPanelAnimationVar( float, m_flBlur, "Blur", "0" );	

	CPanelAnimationVar( Color, m_TextColor, "TextColor", "FgColor" );
	CPanelAnimationVar( Color, m_Ammo2Color, "Ammo2Color", "FgColor" );

	CPanelAnimationVar( vgui::HFont, m_hNumberFont, "NumberFont", "HudNumbers" );
	CPanelAnimationVar( vgui::HFont, m_hNumberGlowFont, "NumberGlowFont", "HudNumbersGlow" );
	CPanelAnimationVar( vgui::HFont, m_hSmallNumberFont, "SmallNumberFont", "SmallHudNumbers" );
	CPanelAnimationVar( vgui::HFont, m_hTextFont, "TextFont", "NHudText" );

	CPanelAnimationVarAliasType( float, m_flText_XPos, "text_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flText_YPos, "text_ypos", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flDigit_XPos, "digit_xpos", "50", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flDigit_YPos, "digit_ypos", "2", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flDigit2_XPos, "digit2_xpos", "98", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flDigit2_YPos, "digit2_ypos", "16", "proportional_float" );

	int	m_iHudGradTexture;
};

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CNHudBarDisplay::CNHudBarDisplay( const char *pElementName ) : BaseClass( nullptr, "NHudBarDisplay" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	
	m_iHudGradTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iHudGradTexture, "vgui/hud/hudgrad", true, false );
}

void CNHudBarDisplay::PaintBackground()
{
}

//-------------------------------------------------------------------------
// Purpose: draws the selection area
//-------------------------------------------------------------------------
void CNHudBarDisplay::Paint()
{
	int wide, tall;
	GetSize( wide, tall );

	surface()->DrawSetColor( 0, 0, 0, 100 );
	surface()->DrawFilledRect( 0, 0, wide, tall );

	surface()->DrawSetColor( 255, 255, 255, 255 );	  
	surface()->DrawSetTexture( m_iHudGradTexture );
	surface()->DrawTexturedRect( 0, 0, wide, tall );

	surface()->DrawSetColor( 0, 0, 0, 100 );
	surface()->DrawOutlinedRect( 0, 0, wide, tall );
}
