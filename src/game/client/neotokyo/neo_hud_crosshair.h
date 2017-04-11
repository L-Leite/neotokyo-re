#ifndef NEO_HUD_CROSSHAIR_H
#define NEO_HUD_CROSSHAIR_H
#ifdef _WIN32
#pragma once
#endif

#include "hudelement.h"
#include <vgui_controls/Panel.h>

namespace vgui
{
	class IScheme;
};

class CHudCrosshair : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudCrosshair, vgui::Panel );
public:
	CHudCrosshair( const char *pElementName );
	virtual ~CHudCrosshair();

protected:
	virtual void	ApplySchemeSettings( vgui::IScheme *scheme );
	virtual void	Paint();

	int				m_Unknown172;
	int				m_Unknown180;

	int				m_iCrosshairTexture;
	int				m_iDotDotTexture;
	int				m_iCircle3Texture;
	int				m_iCrossTexture;
	int				m_iX1Texture;

	float			m_Unknown212;
};


// Enable/disable crosshair rendering.
extern ConVar crosshair;


#endif // NEO_HUD_CROSSHAIR_H
