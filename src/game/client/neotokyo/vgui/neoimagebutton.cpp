#include "cbase.h"
#include "neoimagebutton.h"
#include <vgui/ISurface.h>

namespace vgui
{

CNImageButton::CNImageButton( Panel *parent, const char *panelName, const char *text ) : Button( parent, panelName, text )
{
	SetPaintBackgroundEnabled( false );

	m_iTextureID = 0;
	m_bUnknown = false;
}

CNImageButton::~CNImageButton()
{
}

void CNImageButton::ApplySettings( KeyValues *inResourceData )
{
	const char* image = inResourceData->GetString( "image", "" );

	if ( image )
	{
		m_iTextureID = surface()->CreateNewTextureID();
		surface()->DrawSetTextureFile( m_iTextureID, image, true, false );
	}

	BaseClass::ApplySettings( inResourceData );
}

void CNImageButton::OnMousePressed( MouseCode code )
{
	BaseClass::OnMousePressed( code );
}

void CNImageButton::PaintBackground()
{	 	
}

void CNImageButton::Paint()
{
	int wide, tall;
	GetSize( wide, tall );

	surface()->DrawSetColor( 0, 0, 0, 255 );
	surface()->DrawFilledRect( 0, 0, wide, tall );

	surface()->DrawSetColor( 255, 255, 255, 255 );
	surface()->DrawSetTexture( m_iTextureID );
	surface()->DrawTexturedRect( 0, 0, wide, tall );

	if ( IsArmed() && !IsDepressed() )
	{ 
		surface()->DrawOutlinedRect( 2, 2, wide - 2, tall - 2 );
	}				
	else if ( m_bUnknown )
	{
		surface()->DrawSetColor( 155, 155, 155, 255 );
		surface()->DrawOutlinedRect( 2, 2, wide - 2, tall - 2 );
	}

	BaseClass::Paint();
}

void CNImageButton::PaintBorder()
{
}

void CNImageButton::DrawFocusBorder( int tx0, int ty0, int tx1, int ty1 )
{
}

void CNImageButton::SetTexture( const char* textureName )
{
	m_iTextureID = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iTextureID, textureName, true, false );
}

}