#ifndef NEO_VIEWMODEL_H
#define NEO_VIEWMODEL_H
#ifdef _WIN32
#pragma once
#endif

#include "predicted_viewmodel.h"

#if defined( CLIENT_DLL )
#define CNeoViewModel C_NeoViewModel
#endif

class CNEO_MuzzleflashFP;

class CNeoViewModel : public CPredictedViewModel
{
public:
	DECLARE_CLASS( CNeoViewModel, CPredictedViewModel );   
	DECLARE_NETWORKCLASS();

	CNeoViewModel();

#ifdef CLIENT_DLL
	virtual void AddEntity();

	virtual bool ShouldPredict() { return true; }

	virtual void SetWeaponModel( const char *pszModelname, CBaseCombatWeapon *weapon );

private:
	CNEO_MuzzleflashFP* m_pMuzzleFlashFP;
#endif
};

#endif // NEO_VIEWMODEL_H
