//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef NEOVIEWPORT_H
#define NEOVIEWPORT_H


#include "neo_shareddefs.h"
#include "baseviewport.h"


using namespace vgui;

namespace vgui 
{
	class Panel;
}

class CNeoTeamMenu;
class CNEOClientScoreBoardDialog;
class CNeoClassMenu;
class CNeoLoadoutMenu;
class CNeoLoadoutMenu_Dev;

class NEOViewport : public CBaseViewport
{

private:
	DECLARE_CLASS_SIMPLE( NEOViewport, CBaseViewport );

public:

	IViewPortPanel* CreatePanelByName(const char *szPanelName);
	void CreateDefaultPanels( void );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
		
	int GetDeathMessageStartHeight( void );

};


#endif // NEOViewport_H
