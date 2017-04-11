#ifndef NEO_FRAME_H
#define NEO_FRAME_H

#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls\Frame.h"

namespace vgui
{

class CNeoFrame : public Frame
{
	DECLARE_CLASS_SIMPLE( CNeoFrame, Frame );

public:
	CNeoFrame( Panel* parent, const char *panelName );
	virtual ~CNeoFrame();

	virtual bool IsVisible();
	virtual void SetParent( VPANEL newParent );
	virtual void ApplySchemeSettings( IScheme *pScheme );
	virtual void PaintBackground();
	virtual void Paint();
	virtual Panel *CreateControlByName( const char *controlName );

private:
	int m_iCornerTextureId;
	int m_iTitleBarTextureId;
	int m_iBodyTextureId;
};

}

#endif // NEO_FRAME_H