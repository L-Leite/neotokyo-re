//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client DLL VGUI2 Viewport
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"

#pragma warning( disable : 4800  )  // disable forcing int to bool performance warning

// VGUI panel includes
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui/Cursor.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>
#include <vgui/ILocalize.h>
#include <vgui/VGUI.h>

// client dll/engine defines
#include "hud.h"
#include <voice_status.h>

// viewport definitions
#include <baseviewport.h>
#include "neoviewport.h"

#include "vguicenterprint.h"
#include "text_message.h"

#include "neoteammenu.h"
#include "neoclientscoreboard.h"
#include "neoclassmenu.h"
#include "neoloadoutmenu_dev.h"
#include "neoloadoutmenu.h"			 


void NEOViewport::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	gHUD.InitColors( pScheme );

	SetPaintBackgroundEnabled( false );
}


IViewPortPanel* NEOViewport::CreatePanelByName(const char *szPanelName)
{
	IViewPortPanel* newpanel = nullptr;

	if ( V_strcmp( PANEL_TEAM, szPanelName ) == 0 )
	{
		newpanel = new CNeoTeamMenu( this );
	}
	else if ( V_strcmp( PANEL_SCOREBOARD, szPanelName ) == 0 )
	{
		newpanel = new CNEOClientScoreBoardDialog( this );
	}
	else if ( V_strcmp( PANEL_CLASS, szPanelName ) == 0 )
	{
		newpanel = new CNeoClassMenu( this );
	}
	else if ( V_strcmp( PANEL_LOADOUT, szPanelName ) == 0 )
	{
		newpanel = new CNeoLoadoutMenu( this );
	}
	else if ( V_strcmp( PANEL_LOADOUT_DEV, szPanelName ) == 0 )
	{
		newpanel = new CNeoLoadoutMenu_Dev( this );
	}
	else
	{
		// create a generic base panel, don't add twice
		newpanel = BaseClass::CreatePanelByName( szPanelName );
	}

	return newpanel; 
}

void NEOViewport::CreateDefaultPanels( void )
{
	AddNewPanel( CreatePanelByName( PANEL_TEAM ), "PANEL_TEAM" );
	AddNewPanel( CreatePanelByName( PANEL_CLASS ), "PANEL_CLASS" );
	AddNewPanel( CreatePanelByName( PANEL_LOADOUT ), "PANEL_LOADOUT" );
	AddNewPanel( CreatePanelByName( PANEL_LOADOUT_DEV ), "PANEL_LOADOUT_DEV" );

	BaseClass::CreateDefaultPanels();
}

int NEOViewport::GetDeathMessageStartHeight( void )
{
	int x = YRES(2);

	IViewPortPanel *spectator = gViewPortInterface->FindPanelByName( PANEL_SPECGUI );

	//TODO: Link to actual height of spectator bar
	if ( spectator && spectator->IsVisible() )
	{
		x += YRES(52);
	}

	return x;
}

