#include "cbase.h"
#include "neoframe.h"
#include <vgui/ISurface.h>

#include "neobutton.h"
#include "neoimagebutton.h"
#include "IconPanel.h"

namespace vgui
{

CNeoFrame::CNeoFrame( const char *panelName ) : Frame( nullptr, panelName )
{
	ISurface* pSurface = surface();

	SetScheme( scheme()->LoadSchemeFromFile( "resource/ClientScheme.res", "ClientScheme" ) );

	m_iTextureID1 = pSurface->CreateNewTextureID();
	pSurface->DrawSetTextureFile( m_iTextureID1, "vgui/ngui_corner", true, false );

	m_iTextureID2 = pSurface->CreateNewTextureID();
	pSurface->DrawSetTextureFile( m_iTextureID1, "vgui/ngui_titlebar", true, false );

	m_iTextureID3 = pSurface->CreateNewTextureID();
	pSurface->DrawSetTextureFile( m_iTextureID1, "vgui/ngui_body", true, false );

	SetMoveable( false );
	SetSizeable( false );
	SetPaintBackgroundEnabled( false );
	InvalidateLayout();
}

CNeoFrame::~CNeoFrame()
{
}

bool CNeoFrame::IsVisible()
{
	return BaseClass::IsVisible();
}

void CNeoFrame::SetParent( VPANEL newParent )
{
	BaseClass::SetParent( newParent );
}

void CNeoFrame::ApplySchemeSettings( IScheme *pScheme )
{
}

void CNeoFrame::PaintBackground()
{
	BaseClass::PaintBackground();
}

void CNeoFrame::Paint()
{
	int wide, tall;
	auto pSurface = surface();
	
	GetSize( wide, tall );

	pSurface->DrawSetColor( 255, 255, 255, 255 );

	pSurface->DrawSetTexture( m_iTextureID1 );
	pSurface->DrawTexturedRect( 0, 0, 16, 16 );

	pSurface->DrawSetTexture( m_iTextureID2 );
	pSurface->DrawTexturedRect( 16, 0, wide, 16 );

	pSurface->DrawSetTexture( m_iTextureID3 );
	pSurface->DrawTexturedRect( 0, 16, wide, tall );

	pSurface->DrawSetColor( 0, 0, 0, 255 );

	pSurface->DrawFilledRect( 0, tall - 1, wide, tall );
	pSurface->DrawFilledRect( wide - 1, 0, wide, tall );

	BaseClass::Paint();
}

Panel *CNeoFrame::CreateControlByName( const char *controlName )
{
	if ( !V_strcmp( controlName, "NButton" ) )
		return new CNButton( nullptr, "NButton", "" );
	else if ( !V_strcmp( controlName, "NImageButton" ) )
		return new CNImageButton( nullptr, "NButton", "" );
	else if ( !V_strcmp( controlName, "IconPanel" ) )
		return new CIconPanel( nullptr, "IconPanel" );
	else 
		return BaseClass::CreateControlByName( controlName );

	return nullptr;
}

}