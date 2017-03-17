#ifndef NEO_LOADOUTMENU_H
#define NEO_LOADOUTMENU_H

#ifdef _WIN32
#pragma once
#endif

#include <string>
#include <vector>
#include "neoframe.h"
#include <game/client/iviewport.h>
#include "neo_shareddefs.h"

class CNeoLoadoutMenu : public vgui::CNeoFrame, public IViewPortPanel
{
	DECLARE_CLASS_SIMPLE( CNeoLoadoutMenu, vgui::CNeoFrame );

public:
	CNeoLoadoutMenu( IViewPort *pViewPort );
	virtual ~CNeoLoadoutMenu();

	virtual const char *GetName( void ) { return PANEL_LOADOUT; }
	virtual void SetData( KeyValues *data ) {};
	virtual void Reset() {};
	virtual void Update();
	virtual bool NeedsUpdate( void ) { return m_bNeedsUpdate; }
	virtual bool HasInputElements( void ) { return true; }
	virtual void ShowPanel( bool bShow );

	vgui::VPANEL GetVPanel( void ) { return BaseClass::GetVPanel(); }
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	virtual void SetParent( vgui::VPANEL parent ) { BaseClass::SetParent( parent ); }

	virtual void ApplySchemeSettings( vgui::IScheme* pScheme );
	virtual void OnCommand( const char* command );
	virtual void OnKeyCodePressed( vgui::KeyCode code );

	virtual void SetWeaponImages();

private:
	IViewPort* m_pViewPort;
	int m_iJumpKey;
	int m_iScoreboardKey;
	float m_fCreationTime;
	bool m_bUnknown;
	bool m_bNeedsUpdate;
};

class CLoadoutWeaponClass
{
public:
	CLoadoutWeaponClass( const char* weaponName, const char* vguiImage, const char* weaponEntityName, const char* ammoType );

public:
	const char* m_szWeaponName;
	const char* m_szVguiImage;
	const char* m_szWeaponEntityName;
	const char* m_szAmmoType;
};

extern CLoadoutWeaponClass s_DevLoadoutWeapons[];
extern CLoadoutWeaponClass s_ReconLoadoutWeapons[];
extern CLoadoutWeaponClass s_AssaultLoadoutWeapons[];
extern CLoadoutWeaponClass s_SupportLoadoutWeapons[];

extern const char* s_LoadoutMenuButtons[];

extern int GetNumberOfLoadoutWeapons( int teamNum, int classType, int rank, bool isDev );
extern const char* GetLoadoutVguiWeaponName( int teamNum, int classType, int rank, int num, bool isDev );

#endif // NEO_LOADOUTMENU_H