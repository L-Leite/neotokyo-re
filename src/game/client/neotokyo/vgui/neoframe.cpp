#include "cbase.h"
#include "neoframe.h"
#include <vgui/ISurface.h>

#include "neobutton.h"
#include "neoimagebutton.h"
#include "neoiconpanel.h"

namespace vgui
{

CNeoFrame::CNeoFrame( Panel* parent, const char *panelName ) : Frame( parent, panelName )
{
	ISurface* pSurface = surface();

	SetScheme( scheme()->LoadSchemeFromFile( "resource/ClientScheme.res", "ClientScheme" ) );

	m_iCornerTextureId = pSurface->CreateNewTextureID();
	pSurface->DrawSetTextureFile( m_iCornerTextureId, "vgui/ngui_corner", true, false );

	m_iTitleBarTextureId = pSurface->CreateNewTextureID();
	pSurface->DrawSetTextureFile( m_iTitleBarTextureId, "vgui/ngui_titlebar", true, false );

	m_iBodyTextureId = pSurface->CreateNewTextureID();
	pSurface->DrawSetTextureFile( m_iBodyTextureId, "vgui/ngui_body", true, false );

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
	GetSize( wide, tall );

	surface()->DrawSetColor( 255, 255, 255, 255 );

	surface()->DrawSetTexture( m_iCornerTextureId );
	surface()->DrawTexturedRect( 0, 0, 16, 16 );

	surface()->DrawSetTexture( m_iTitleBarTextureId );
	surface()->DrawTexturedRect( 16, 0, wide, 16 );

	surface()->DrawSetTexture( m_iBodyTextureId );
	surface()->DrawTexturedRect( 0, 16, wide, tall );

	surface()->DrawSetColor( 0, 0, 0, 255 );

	surface()->DrawFilledRect( 0, tall - 1, wide, tall );
	surface()->DrawFilledRect( wide - 1, 0, wide, tall );

	BaseClass::Paint();
}

Panel *CNeoFrame::CreateControlByName( const char *controlName )
{
	if ( !V_strcmp( controlName, "NButton" ) || !V_strcmp( controlName, "Button" ) )
		return new CNButton( nullptr, "NButton", "" );
	else if ( !V_strcmp( controlName, "NImageButton" ) )
		return new CNImageButton( nullptr, "NImageButton", "" );
	else if ( !V_strcmp( controlName, "IconPanel" ) )
		return new IconPanel( this, "IconPanel" );
	
	return BaseClass::CreateControlByName( controlName );
}

}