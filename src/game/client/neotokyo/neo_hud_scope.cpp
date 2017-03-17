#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"
#include "c_neo_player.h"
#include "weapon_neobase.h"
#include "vguimatsurface/IMatSystemSurface.h"
#include "materialsystem/IMaterial.h"
#include "materialsystem/IMesh.h"
#include "materialsystem/imaterialvar.h"

#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui_controls/AnimationController.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Draws the zoom screen
//-----------------------------------------------------------------------------
class CHudScope : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE( CHudScope, vgui::Panel );

public:
			CHudScope( const char *pElementName );
	
	void	Init( void );
	void	VidInit( void );
	void	LevelInit( void );
	void	Reset( void );

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	virtual void OnThink();
	virtual void Paint( void );

private:								
	bool m_bIsScoped;
	int m_iScopeTexture;
};

DECLARE_HUDELEMENT_DEPTH( CHudScope, 60 );

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudScope::CHudScope( const char *pElementName ) : CHudElement(pElementName), BaseClass(NULL, "HudScope")
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	
	SetHiddenBits( HIDEHUD_PLAYERDEAD );

	m_bIsScoped = false;

	m_iScopeTexture = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_iScopeTexture, "vgui/hud/scopes/scope01", true, false );
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CHudScope::Init( void )
{
	Reset();
}

void CHudScope::VidInit( void )
{
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CHudScope::LevelInit( void )
{
	Init();
}

void CHudScope::Reset( void )
{
	// They must have some test code that wasn't on the release build
}

//-----------------------------------------------------------------------------
// Purpose: sets scheme colors
//-----------------------------------------------------------------------------
void CHudScope::ApplySchemeSettings( vgui::IScheme *scheme )
{
	BaseClass::ApplySchemeSettings(scheme);

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);

	int screenWide, screenTall;
	GetHudSize(screenWide, screenTall);
	SetBounds(0, 0, screenWide, screenTall);
}

void CHudScope::OnThink()
{
	C_NEOPlayer* pPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( pPlayer )
	{	 
		C_WeaponNEOBase *pWeapon = pPlayer->GetActiveNEOWeapon();

		if ( pWeapon && pWeapon->m_Unknown1892 == 2 && pWeapon->GetAimType() == 2 )
		{ 
			m_bIsScoped = true;
			return;
		}	 		
	}

	m_bIsScoped = false;
}

//-----------------------------------------------------------------------------
// Purpose: draws the zoom effect
//-----------------------------------------------------------------------------
void CHudScope::Paint( void )
{		  
	if ( !m_bIsScoped )
		return;

	Assert( m_iScopeTexture );		 
	
	int screenWide, screenTall;
	GetHudSize(screenWide, screenTall);

	int iMiddleX = (screenWide / 2 );
	int iMiddleY = (screenTall / 2 );	  

	vgui::surface()->DrawSetColor( 255, 255, 255, 255 );
	vgui::surface()->DrawSetTexture( m_iScopeTexture );		 
	vgui::surface()->DrawTexturedRect( 0, 0, screenWide, screenTall );

	vgui::surface()->DrawSetColor( 0, 0, 0, 255 );
	vgui::surface()->DrawFilledRect( iMiddleX, 0, iMiddleX + 1, screenTall );
	vgui::surface()->DrawFilledRect( 0, iMiddleY, iMiddleX, iMiddleY + 1 );
}
