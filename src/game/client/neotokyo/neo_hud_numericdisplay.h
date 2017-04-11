#ifndef NEO_HUD_NUMERIC_DISPLAY_H
#define NEO_HUD_NUMERIC_DISPLAY_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>

class CNHudNumericDisplay : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CNHudNumericDisplay, vgui::Panel );

public:
	CNHudNumericDisplay( Panel* parent, const char *pElementName );

protected:
	virtual void PaintBackground();
	virtual void Paint();

	virtual void DrawNumber( vgui::HFont hFont, int x, int y, int number );

protected:																			 
	CPanelAnimationVar( float, m_flBlur, "Blur", "0" );	

	CPanelAnimationVar( Color, m_TextColor, "TextColor", "FgColor" );
	CPanelAnimationVar( Color, m_Ammo2Color, "Ammo2Color", "FgColor" );

	CPanelAnimationVar( vgui::HFont, m_hNumberFont, "NumberFont", "NHudNumbers" );
	CPanelAnimationVar( vgui::HFont, m_hNumberGlowFont, "NumberGlowFont", "NHudNumbersBlur" );
	CPanelAnimationVar( vgui::HFont, m_hSmallNumberFont, "SmallNumberFont", "HudNumbersSmall" );
	CPanelAnimationVar( vgui::HFont, m_hTextFont, "TextFont", "NHudText" );

	CPanelAnimationVarAliasType( float, m_flText_XPos, "text_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flText_YPos, "text_ypos", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flDigit_XPos, "digit_xpos", "50", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flDigit_YPos, "digit_ypos", "2", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flDigit2_XPos, "digit2_xpos", "98", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flDigit2_YPos, "digit2_ypos", "16", "proportional_float" );

	int	m_iHudGradTexture;

protected:
	int m_Unknown172;
	int m_Unknown176;

	bool m_Unknown244;
	bool m_Unknown245;
	bool m_Unknown246;

	int m_Unknown280;

	int m_Unknown312;

	int m_Unknown320;

	int m_Unknown328;

	int m_Unknown336;
};


#endif // NEO_HUD_NUMERIC_DISPLAY_H