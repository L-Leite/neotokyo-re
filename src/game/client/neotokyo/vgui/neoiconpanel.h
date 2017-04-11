#ifndef ICONPANEL_H
#define ICONPANEL_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls\Panel.h>

namespace vgui
{

class IconPanel : public Panel
{
public:
	DECLARE_CLASS_SIMPLE( IconPanel, Panel );  
	IconPanel( Panel* parent, const char* panelName );

	virtual void ApplySettings( KeyValues* inResourceData );

	virtual void PaintBackground();
	virtual void Paint();
	virtual void PaintBorder();

private:
	int		m_iImageTextureId;
};	

}

#endif