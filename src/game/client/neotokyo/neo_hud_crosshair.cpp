#include "cbase.h"
#include "hud.h"
#include "neo_hud_crosshair.h"
#include "hudelement.h"
#include "iclientmode.h"
#include "vgui/ISurface.h"
#include <vgui_controls/Panel.h>
#include "c_neo_player.h"
#include "weapon_neobase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar crosshair( "crosshair", "1", FCVAR_ARCHIVE );
//ConVar cl_observercrosshair( "cl_observercrosshair", "1", FCVAR_ARCHIVE ); // Not in the original 

using namespace vgui;

// TODO: Maybe implement something like this for neo
#ifdef TF_CLIENT_DLL 
// If running TF, we use CHudTFCrosshair instead (which is derived from CHudCrosshair)
#else
DECLARE_HUDELEMENT( CHudCrosshair );
#endif

CHudCrosshair::CHudCrosshair( const char *pElementName ) :
		CHudElement( pElementName ), BaseClass( NULL, "HudCrosshair" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_PLAYERDEAD | HIDEHUD_CROSSHAIR );

	m_iCrosshairTexture = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_iCrosshairTexture, "vgui/hud/crosshair", true, false );

	m_iDotDotTexture = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_iDotDotTexture, "vgui/hud/dotdot", true, false );

	m_iCircle3Texture = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_iCircle3Texture, "vgui/hud/circle3", true, false );

	m_iCrossTexture = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_iCrossTexture, "vgui/hud/cross", true, false );

	m_iX1Texture = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_iX1Texture, "vgui/hud/x1", true, false );

	m_Unknown212 = 1.0f;
}

CHudCrosshair::~CHudCrosshair()
{
}

void CHudCrosshair::ApplySchemeSettings( IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme ); 
	SetPaintBackgroundEnabled( false );
}			 			  

void CHudCrosshair::Paint()
{
	C_NEOPlayer* pPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( !pPlayer )
		return;

	C_WeaponNEOBase* pWeapon = pPlayer->GetActiveNEOWeapon();

	if ( !pWeapon )
		return;

	// This should be moved to ShouldDraw
	if ( !pWeapon->CanAim()
		|| (pWeapon->m_Unknown1892 == 2 && pWeapon->GetAimType() == 2)
		|| pPlayer->m_iSprint == 1
		|| !crosshair.GetBool()
		|| !g_pClientMode->ShouldDrawCrosshair()
		|| engine->IsDrawingLoadingImage()
		|| engine->IsPaused()
		|| (!pPlayer->IsAlive() && pPlayer->GetObserverMode() != OBS_MODE_IN_EYE)
		|| (pPlayer->GetFlags() & FL_FROZEN)
		|| pPlayer->entindex() != render->GetViewEntity()
		|| !pWeapon->bAimed )
		return;

	int iCenterX = ScreenWidth() / 2;
	int iCenterY = ScreenHeight() / 2;

	int iCrossSize = 0;

	if ( pPlayer->m_bAnyoneInCross ) // ??? Almost sure this is wrong since x1 is the crosshair used when aiming at a teammate
	{
		surface()->DrawSetColor( 255, 0, 0, 200 );
		surface()->DrawSetTexture( m_iX1Texture );
		iCrossSize = 32;
	}
	else
	{
		surface()->DrawSetColor( 255, 255, 255, 200 );
		surface()->DrawSetTexture( m_iCrosshairTexture );
		iCrossSize = 64;
	}

	surface()->DrawTexturedRect( iCenterX - iCrossSize, iCenterY - iCrossSize, iCenterX + iCrossSize, iCenterY + iCrossSize );
}
