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

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Health panel
//-----------------------------------------------------------------------------
class CNHudHealthSection : public CHudElement, public CNHudNumericDisplay
{
	DECLARE_CLASS_SIMPLE( CNHudHealthSection, CNHudNumericDisplay );

public:
	CNHudHealthSection( const char *pElementName );

	virtual void Init();

	virtual void OnThink();	   
	virtual void Paint();

	virtual void DrawNumber( vgui::HFont hFont, int x, int y, int number );
	void DrawText( vgui::HFont hFont, const wchar_t* szText, int x, int y );
	void DrawProgressBar( int x, int y, int w, int h, float fPercentage );

private:
	CPanelAnimationVarAliasType( float, m_flHealthText_XPos, "healthtext_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flHealthText_YPos, "healthtext_ypos", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flHealthBar_XPos, "healthbar_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flHealthBar_YPos, "healthbar_ypos", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flHealthBar_W, "healthbar_w", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flHealthBar_H, "healthbar_h", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flHealthNum_XPos, "healthnum_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flHealthNum_YPos, "healthnum_ypos", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flCamoText_XPos, "camotext_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flCamoText_YPos, "camotext_ypos", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flCamoBar_XPos, "camobar_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flCamoBar_YPos, "camobar_ypos", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flCamoBar_W, "camobar_w", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flCamoBar_H, "camobar_h", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flCamoNum_XPos, "camonum_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flCamoNum_YPos, "camonum_ypos", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flSprintText_XPos, "sprinttext_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flSprintText_YPos, "sprinttext_ypos", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flSprintBar_XPos, "sprintbar_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flSprintBar_YPos, "sprintbar_ypos", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flSprintBar_W, "sprintbar_w", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flSprintBar_H, "sprintbar_h", "20", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flSprintNum_XPos, "sprintnum_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flSprintNum_YPos, "sprintnum_ypos", "20", "proportional_float" );

	CPanelAnimationVar( vgui::HFont, m_hTFont, "TextFont", "NHudOCRSmall" );
	CPanelAnimationVar( vgui::HFont, m_hNumberFont, "NumberFont", "NHudOCR" )

	int	m_iWhiteAdditiveTexture;

	int m_iClassType;
	int m_iHealth;
	float m_fThermopticNRG;	 
	float m_fSprintNRG;
};	

DECLARE_HUDELEMENT_DEPTH( CNHudHealthSection, 50 );

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CNHudHealthSection::CNHudHealthSection( const char *pElementName ) : CHudElement( pElementName ), CNHudNumericDisplay( nullptr, "NHudHealth" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT | HIDEHUD_HEALTH | HIDEHUD_WEAPONSELECTION );

	m_iWhiteAdditiveTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iWhiteAdditiveTexture, "vgui/white_additive", true, false );
}	  

void CNHudHealthSection::Init()
{
	m_iClassType = CLASS_UNASSIGNED;
	m_iHealth = 100;
	m_fThermopticNRG = 0.0f;
	m_fSprintNRG = 13.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNHudHealthSection::OnThink()
{
	C_NEOPlayer* pLocalPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( !pLocalPlayer || pLocalPlayer->m_iClassType <= CLASS_UNASSIGNED || pLocalPlayer->m_bIsVIP )
	{
		SetPaintEnabled( false );
		SetPaintBackgroundEnabled( false );
		return;
	}

	SetPaintEnabled( true );
	SetPaintBackgroundEnabled( true );

	m_iClassType = pLocalPlayer->m_iClassType;
	m_iHealth = pLocalPlayer->GetHealth();

	if ( m_iHealth < 0 )
		m_iHealth = 0;

	m_fThermopticNRG = pLocalPlayer->m_fThermopticNRG;

	if ( m_fThermopticNRG < 0.0f )
		m_fThermopticNRG = 0.0f;

	m_fSprintNRG = pLocalPlayer->m_fSprintNRG;

	if ( m_fSprintNRG < 0.0f )
		m_fSprintNRG = 0.0f;
}

void CNHudHealthSection::Paint()
{
	int wide, tall;
	GetSize( wide, tall );

	DrawBox( wide / -2, 0, wide + wide / 2, tall, Color( 200, 200, 200, 28 ), 1.0f );

	//SetFgColor( GetFgColor() ); // What's this and why is this in the game's code

	surface()->DrawSetTextColor( 255, 255, 255, 100 );

	wchar_t buffer[ 64 ];
	g_pVGuiLocalize->ConvertANSIToUnicode( "INTEGRITY", buffer, sizeof( buffer ) );

	DrawText( m_hTFont, buffer, m_flHealthText_XPos, m_flHealthText_YPos );
	DrawProgressBar( m_flHealthBar_XPos, m_flHealthBar_XPos, m_flHealthBar_W, m_flHealthBar_H, m_iHealth / 100.0f );
	DrawNumber( m_hTFont, m_flHealthNum_XPos, m_flHealthNum_YPos, m_iHealth );

	C_NEOPlayer* pLocalPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( !pLocalPlayer || pLocalPlayer->m_iClassType == CLASS_SUPPORT )
		return;

	g_pVGuiLocalize->ConvertANSIToUnicode( "THERM-OPTIC", buffer, sizeof( buffer ) );

	DrawText( m_hTFont, buffer, m_flCamoText_XPos, m_flCamoText_YPos );
	DrawProgressBar( m_flCamoBar_XPos, m_flCamoBar_YPos, m_flCamoBar_W, m_flCamoBar_H, m_fThermopticNRG / pLocalPlayer->m_pPlayerClassInfo->fMaxThermopticNRG );
	DrawNumber( m_hTFont, m_flCamoNum_XPos, m_flCamoNum_YPos, m_fThermopticNRG );

	g_pVGuiLocalize->ConvertANSIToUnicode( "AUX POWER", buffer, sizeof( buffer ) );

	DrawText( m_hTFont, buffer, m_flSprintText_XPos, m_flSprintText_YPos );
	DrawProgressBar( m_flSprintBar_XPos, m_flSprintBar_YPos, m_flSprintBar_W, m_flSprintBar_H, m_fSprintNRG / 100.0f );
	DrawNumber( m_hTFont, m_flSprintNum_XPos, m_flSprintNum_YPos, m_fSprintNRG );
}

void CNHudHealthSection::DrawNumber( HFont hFont, int x, int y, int number )
{
	wchar_t buffer[ 10 ];
	V_snwprintf( buffer, sizeof( buffer ), L"%d", number );

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

void CNHudHealthSection::DrawText( HFont hFont, const wchar_t* szText, int x, int y )
{	 
	surface()->DrawSetTextFont( hFont );
	surface()->DrawSetTextPos( x, y );

	int textLength = V_wcslen( szText );

	for ( int i = 0; i < textLength; i++ )
		surface()->DrawUnicodeChar( szText[ i ] );
}

void CNHudHealthSection::DrawProgressBar( int x, int y, int w, int h, float fPercentage /* from 0.0 to 1.0*/ )
{
	surface()->DrawSetTextColor( 255, 255, 255, 50 );
	surface()->DrawOutlinedRect( x, y, x + w, y + h );

	surface()->DrawSetTextColor( 255, 255, 255, 150 );
	surface()->DrawFilledRect( x, y, x + w * fPercentage, y + h );
}
