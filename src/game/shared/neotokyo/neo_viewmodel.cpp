#include "cbase.h"
#include "neo_viewmodel.h"
#ifdef CLIENT_DLL
#include "cneo_muzzleflashfp.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( neo_viewmodel, CNeoViewModel );

IMPLEMENT_NETWORKCLASS_ALIASED( NeoViewModel, DT_NeoViewModel )

BEGIN_NETWORK_TABLE( CNeoViewModel, DT_NeoViewModel )
END_NETWORK_TABLE()



CNeoViewModel::CNeoViewModel()
{	
#ifdef CLIENT_DLL
	m_pMuzzleFlashFP = nullptr;
#endif
}

#ifdef CLIENT_DLL		
CNEO_MuzzleflashFP* CreateMuzzleFlashFp( CNeoViewModel* pViewModel, int iViewmodelIndex )
{
	CNEO_MuzzleflashFP* pMuzzleFlashFp = (CNEO_MuzzleflashFP*) CreateEntityByName( "muzzleflashfp" );

	if ( !pMuzzleFlashFp )
	{
		Warning( "classname %s used to create wrong class type\n" );
		return nullptr;
	}

	pMuzzleFlashFp->SetOwnerEntity( pViewModel );
	pMuzzleFlashFp->CreateMuzzleFlashModel( pViewModel, iViewmodelIndex );
	pMuzzleFlashFp->SetAbsOrigin( pViewModel->GetAbsOrigin() );
	pMuzzleFlashFp->SetEffectEntity( pViewModel );

	return pMuzzleFlashFp;
}

void CNeoViewModel::AddEntity()
{
	BaseClass::AddEntity();

	if ( !m_pMuzzleFlashFP )
		m_pMuzzleFlashFP = CreateMuzzleFlashFp( this, true );
}

void CNeoViewModel::SetWeaponModel( const char *pszModelname, CBaseCombatWeapon *weapon )
{
	BaseClass::SetWeaponModel( pszModelname, weapon );

	if ( !m_pMuzzleFlashFP )
		return;

	m_pMuzzleFlashFP->something3();

	int muzzle = LookupAttachment( "muzzle" );

	if ( muzzle > 0 )
	{
		m_pMuzzleFlashFP->SetAttachment( muzzle );
		m_pMuzzleFlashFP->SetUnknown1784( true );
	}
	else
	{
		m_pMuzzleFlashFP->SetUnknown1784( false );
	}
}
#endif
