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
class CNHudWeapon : public CHudElement, public CNHudNumericDisplay
{
	DECLARE_CLASS_SIMPLE( CNHudWeapon, CNHudNumericDisplay );

public:
	CNHudWeapon( const char *pElementName );

	virtual void Init();
	virtual void VidInit();

	virtual void OnThink();	   
	virtual void Paint();

	virtual void DrawNumber( vgui::HFont hFont, int x, int y, int number );

	void DrawBullets( vgui::HFont hFont, int x, int y, int iClip, int iDefaultClip );
	void DrawWeaponIcon( vgui::HFont hFont );
	void DrawWeaponName( vgui::HFont hFont );

	void CopyWeaponName( wchar_t* szNewWepName );

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

	CPanelAnimationVarAliasType( float, m_flText_XPos, "text_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flText_YPos, "text_ypos", "20", "proportional_float" );

	CPanelAnimationVar( Color, m_TextColor, "TextColor", "FgColor" );

	CPanelAnimationVar( vgui::HFont, m_hTFont, "TextFont", "NHudOCRSmall" );
	CPanelAnimationVar( vgui::HFont, m_hNumberFont, "NumberFont", "NHudOCR" );
	CPanelAnimationVar( vgui::HFont, m_hNumberGlowFont, "NumberGlowFont", "NHudOCRBlur" );
	CPanelAnimationVar( vgui::HFont, m_hBulletFont, "HudBulletsFont", "NHudBullets" );

	int	m_iWhiteAdditiveTexture;

	wchar_t m_szWeaponName[ 16 ];

	EHANDLE m_hCurrentWeapon;

	int m_iClip1;	 
	int m_iDefaultClip;
	int m_iAmmoCount;

	wchar_t m_uBulletChar;
	wchar_t m_uWeaponChar;

	bool m_bUsesClips;
};	

DECLARE_HUDELEMENT_DEPTH( CNHudWeapon, 50 );

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CNHudWeapon::CNHudWeapon( const char *pElementName ) : CHudElement( pElementName ), CNHudNumericDisplay( nullptr, "NHudWeapon" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_PLAYERDEAD | HIDEHUD_HEALTH | HIDEHUD_WEAPONSELECTION );

	m_iWhiteAdditiveTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iWhiteAdditiveTexture, "vgui/white_additive", true, false );
}	  

void CNHudWeapon::Init()
{
	m_iClip1 = -1;
	m_iDefaultClip = -1;
	m_iAmmoCount = -1;
	m_uBulletChar = L'a';
	m_uWeaponChar = L'h';
}

void CNHudWeapon::VidInit()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNHudWeapon::OnThink()
{
	C_NEOPlayer* pLocalPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( pLocalPlayer )
	{
		C_WeaponNEOBase* pWeapon = pLocalPlayer->GetActiveNEOWeapon();

		if ( pWeapon )
		{
			SetPaintEnabled( true );
			SetPaintBackgroundEnabled( true );

			if ( pWeapon == m_hCurrentWeapon )
			{		
				m_iClip1 = pWeapon->Clip1();	
				m_iAmmoCount = pLocalPlayer->GetAmmoCount( pWeapon->GetPrimaryAmmoType() );
				
				if ( pWeapon->GetUnknown1452() )
					m_iAmmoCount /=	pWeapon->GetDefaultClip1();

				m_uWeaponChar = pWeapon->GetWeaponChar();
			}
			else
			{ 
				m_hCurrentWeapon = pWeapon;	  
				m_iClip1 = pWeapon->Clip1();
				m_iDefaultClip = pWeapon->GetDefaultClip1();
				m_uBulletChar = pWeapon->GetNEOWpnData().m_szBulletCharacter[0];
				m_iAmmoCount = pLocalPlayer->GetAmmoCount( pWeapon->GetPrimaryAmmoType() );

				if ( pWeapon->GetUnknown1452() )
					m_iAmmoCount /= pWeapon->GetDefaultClip1();

				m_bUsesClips = pWeapon->UsesClipsForAmmo1();
			}

			return;
		}
	}

	SetPaintEnabled( false );
	SetPaintBackgroundEnabled( false );
}

void CNHudWeapon::Paint()
{				
	int wide, tall;
	GetSize( wide, tall );

	DrawBox( 0, 0, wide + wide / 2, tall, Color( 200, 200, 200, 28 ), 1.0f );

	C_NEOPlayer* pLocalPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( !pLocalPlayer )
		return;

	C_WeaponNEOBase* pWeapon = pLocalPlayer->GetActiveNEOWeapon();

	if ( !pWeapon )
		return;

	surface()->DrawSetTextColor( 255, 255, 255, 150 );

	if ( pWeapon->HasAmmoClip() )
	{
		int iDefaultClip = 30;

		if ( m_iDefaultClip <= 30 )
			iDefaultClip = m_iDefaultClip;

		int iClip = m_iClip1;

		if ( iClip > 30 )
			iClip = 30;

		DrawBullets( m_hBulletFont, m_flDigit_XPos, m_flDigit_YPos, iClip, iDefaultClip );

		if ( m_bUsesClips )
		{
			surface()->DrawSetTextColor( 255, 255, 255, 100 );
			DrawNumber( m_hNumberFont, m_flDigit2_XPos, m_flDigit2_YPos, m_iAmmoCount );
			DrawWeaponIcon( m_hBulletFont );				
		} 		
	}	  

	wchar_t weaponName[ 16 ];
	g_pVGuiLocalize->ConvertANSIToUnicode( pWeapon->GetPrintName(), weaponName, sizeof( weaponName ) );

	CopyWeaponName( weaponName ); 
	DrawWeaponName( m_hTFont );
}

void CNHudWeapon::DrawNumber( HFont hFont, int x, int y, int number )
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

void CNHudWeapon::DrawBullets( HFont hFont, int x, int y, int iClip, int iDefaultClip )
{
	wchar_t buffer[ 10 ];
	V_snwprintf( buffer, ARRAYSIZE( buffer ), L"%d", iClip );

	int bulletWidth = surface()->GetCharacterWidth( hFont, m_uBulletChar );

	int textWide, textTall;
	surface()->GetTextSize( hFont, buffer, textWide, textTall );

	surface()->DrawSetTextPos( x - iDefaultClip * bulletWidth, y );
	surface()->DrawSetTextFont( hFont );

	if ( m_uBulletChar == L'd' )
		surface()->DrawSetTextPos( x - bulletWidth * ( iDefaultClip + 1 ), y );

	for ( int i = 0; i < iDefaultClip; i++ )
	{																				 
		if ( i < iDefaultClip - iClip )
			surface()->DrawSetTextColor( 255, 255, 255, 50 );
		else
			surface()->DrawSetTextColor( 255, 255, 255, 150 );	   

		surface()->DrawUnicodeChar( m_uBulletChar );
	}
}

void CNHudWeapon::DrawWeaponIcon( vgui::HFont hFont )
{
	surface()->DrawSetTextPos( m_flIcon_XPos, m_flIcon_YPos );
	surface()->DrawSetTextFont( hFont );
	surface()->DrawSetTextColor( 255, 255, 255, 100 );
	surface()->DrawUnicodeChar( m_uWeaponChar );
}

void CNHudWeapon::DrawWeaponName( vgui::HFont hFont )
{
	surface()->DrawSetTextFont( hFont );

	int textWide, textTall;
	surface()->GetTextSize( hFont, m_szWeaponName, textWide, textTall );

	surface()->DrawSetTextPos( m_flText_XPos - textWide, m_flText_YPos );

	int bufLen = V_wcslen( m_szWeaponName );

	for ( int i = 0; i < bufLen; i++ )
		surface()->DrawUnicodeChar( m_szWeaponName[ i ] );
}

void CNHudWeapon::CopyWeaponName( wchar_t* szNewWepName )
{
	V_wcsncpy( m_szWeaponName, szNewWepName, sizeof( m_szWeaponName ) );
	m_uWeaponChar = 0;
}
