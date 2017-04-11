#include "cbase.h"
#include "neoiconpanel.h"
#include <vgui/ISurface.h>

namespace vgui
{

IconPanel::IconPanel( Panel* parent, const char* panelName ) : Panel( parent, panelName )
{
	SetPaintBackgroundEnabled( false );

	m_iImageTextureId = 0;
}

void IconPanel::ApplySettings( KeyValues* inResourceData )
{
	const char* image = inResourceData->GetString( "image" );

	if ( image )
	{
		m_iImageTextureId = surface()->CreateNewTextureID();
		surface()->DrawSetTextureFile( m_iImageTextureId, image, true, false );
	}

	BaseClass::ApplySettings( inResourceData );
}

void IconPanel::PaintBackground()
{
}

void IconPanel::Paint()
{
	int wide, tall;
	GetSize( wide, tall );

	surface()->DrawSetColor( 255, 255, 255, 255 );
	surface()->DrawSetTexture( m_iImageTextureId );
	surface()->DrawTexturedRect( 0, 0, wide, tall );	
}

void IconPanel::PaintBorder()
{
}

}
