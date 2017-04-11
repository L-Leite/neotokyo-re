#include "cbase.h"
#include "neo_hud_numericdisplay.h"
#include "iclientmode.h"

#include <vgui/ISurface.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CNHudNumericDisplay::CNHudNumericDisplay( Panel* parent, const char *pElementName ) : BaseClass( parent, pElementName )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_Unknown244 = true;
	m_Unknown245 = false;
	
	m_iHudGradTexture = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iHudGradTexture, "vgui/white_additive", true, false );
}

void CNHudNumericDisplay::PaintBackground()
{
	// int wide, tall;
	// GetSize( wide, tall );
	// This was probably used for some stuff that was removed from release
}

//-------------------------------------------------------------------------
// Purpose: draws the selection area
//-------------------------------------------------------------------------
void CNHudNumericDisplay::Paint()
{
	int wide, tall;
	GetSize( wide, tall );

	surface()->DrawSetColor( 0, 0, 0, 100 );
	surface()->DrawFilledRect( 0, 0, wide, tall );

	if ( m_Unknown244 )
	{
		surface()->DrawSetColor( 255, 255, 255, 255 );
		DrawNumber( m_hNumberFont, m_Unknown312, m_Unknown320, m_Unknown172 );
	}

	if ( m_Unknown245 )
	{
		surface()->DrawSetColor( GetFgColor() );
		DrawNumber( m_hNumberFont, m_Unknown328, m_Unknown336, m_Unknown280 );
	}

	surface()->DrawSetColor( 255, 255, 255, 255 );	  
	surface()->DrawSetTexture( m_iHudGradTexture );
	surface()->DrawTexturedRect( 0, 0, wide, tall );

	surface()->DrawSetColor( 0, 0, 0, 100 );
	surface()->DrawOutlinedRect( 0, 0, wide, tall );
}

void CNHudNumericDisplay::DrawNumber( HFont hFont, int x, int y, int number )
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
	surface()->DrawSetTextColor( 128, 128, 128, 255 );
	surface()->DrawSetTextFont( m_hSmallNumberFont );

	int bufLen = V_wcslen( buffer );

	for ( int i = 0; i < bufLen; i++ )
		surface()->DrawUnicodeChar( buffer[ i ] );

	surface()->DrawSetTextColor( 255, 255, 255, 255 );
	surface()->DrawSetTextFont( hFont );

	for ( int i = 0; i < bufLen; i++ )
		surface()->DrawUnicodeChar( buffer[ i ] );
}
