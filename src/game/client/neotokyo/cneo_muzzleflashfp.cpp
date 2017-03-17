#include "cbase.h"
#include "cneo_muzzleflashfp.h"
#include "model_types.h"
#include "materialsystem/imaterialsystem.h"


LINK_ENTITY_TO_CLASS( muzzleflashfp, CNEO_MuzzleflashFP );


CNEO_MuzzleflashFP::CNEO_MuzzleflashFP()
{
	m_Unknown1784 = false;
	m_pMuzzleFlashModel = nullptr;
	m_iMfMaterialIndex = 0;
}

RenderGroup_t CNEO_MuzzleflashFP::GetRenderGroup()
{
	return RENDER_GROUP_VIEW_MODEL_TRANSLUCENT;
}

void CNEO_MuzzleflashFP::CreateMuzzleFlashModel( CNeoViewModel* pViewModel, int iAttachment )
{
	//m_pMuzzleFlashModel = engine->LoadModel( "models/effect/fpmf/fpmf01.mdl" );

	if ( m_pMuzzleFlashModel && pViewModel )
	{
		if ( iAttachment >= 0 )
		{
			m_iAttachment = iAttachment;
			index = -1;
			Clear();
			SetModelPointer( m_pMuzzleFlashModel );
			SetRenderMode( kRenderTransAdd );
			m_nRenderFX = 0;		
			m_pRagdoll = nullptr;
			m_nBody = 0;
		}
	}
}

void CNEO_MuzzleflashFP::DrawMuzzleFlash()
{
	if ( !GetUnknown1784() || !m_pMuzzleFlashModel )
		return;

	if ( modelinfo->GetModelType( GetModel() ) != mod_studio )
		return;

	C_NeoViewModel* pViewModel = dynamic_cast<C_NeoViewModel*>(GetOwnerEntity());

	if ( !pViewModel || !GetModelPtr() )
		return;

	IMaterial* ppMfMaterials[ 3 ]; // Maybe make this static after reversing everything
	ppMfMaterials[ 0 ] = g_pMaterialSystem->FindMaterial( "effects/nmf_01", TEXTURE_GROUP_OTHER );
	ppMfMaterials[ 1 ] = g_pMaterialSystem->FindMaterial( "effects/nmf_02", TEXTURE_GROUP_OTHER );
	ppMfMaterials[ 2 ] = g_pMaterialSystem->FindMaterial( "effects/nmf_03", TEXTURE_GROUP_OTHER );

	GetAttachment( m_iAttachment, m_vecSomething, m_angSomething );
	SetAbsOrigin( m_vecSomething );
	SetAbsAngles( m_angSomething );

	for ( int i = 0; i < _ARRAYSIZE( m_Idks ); i++ )
	{ 
		CIdk idk = m_Idks[ i ];

		if ( !idk.m_Unknown00 )
			continue;

		idk.m_Unknown28++;

		SetBoneController( 0, idk.m_Unknown24 );
		SetModelScale( idk.m_Unknown08 );
		InvalidateBoneCache();

		if ( idk.m_Unknown20 == 1 )
		{
			modelrender->DrawModel( STUDIO_RENDER, this, MODEL_INSTANCE_INVALID, entindex(), GetModel(), GetAbsOrigin(), GetAbsAngles(), GetSkin(), GetBody(), GetHitboxSet() );
		}
		else if ( idk.m_Unknown20 == 2 )
		{
			modelrender->ForcedMaterialOverride( ppMfMaterials[ m_iMfMaterialIndex ] );
			modelrender->DrawModel( STUDIO_RENDER, this, MODEL_INSTANCE_INVALID, entindex(), GetModel(), GetAbsOrigin(), GetAbsAngles(), GetSkin(), GetBody(), GetHitboxSet() );
			modelrender->ForcedMaterialOverride( nullptr );
		}

		m_iMfMaterialIndex++;

		if ( m_iMfMaterialIndex > 2 )
			m_iMfMaterialIndex = 0;
	}
}

void CNEO_MuzzleflashFP::something()
{
	for ( int i = 0; i < _ARRAYSIZE( m_Idks ); i++ )
	{
		CIdk idk = m_Idks[ i ];

		if ( !idk.m_Unknown00 )
			continue;

		if ( idk.m_Unknown28 >= 2 )
			idk.m_Unknown00 = false;

		if ( (idk.m_Unknown16 - idk.m_Unknown12) * 0.5f + idk.m_Unknown12 > gpGlobals->curtime )
			idk.m_Unknown08 = (idk.m_Unknown16 - gpGlobals->curtime) / (idk.m_Unknown16 - idk.m_Unknown12) * idk.m_Unknown04;
		else
			idk.m_Unknown00 = false;
	}
}

void CNEO_MuzzleflashFP::something2( float a2, float a3, float a4, int a5 )
{
	for ( int i = 0; i < _ARRAYSIZE( m_Idks ); i++ )
	{
		CIdk idk = m_Idks[ i ];

		if ( idk.m_Unknown12 > idk.m_Unknown08 )
		{ 
			idk.m_Unknown00 = true;
			idk.m_Unknown04 = a2;
			idk.m_Unknown12 = gpGlobals->curtime;
			idk.m_Unknown20 = a5;
			idk.m_Unknown28 = 0;
			idk.m_Unknown16 = gpGlobals->curtime + a4;
			idk.m_Unknown24 = a3;
			return;
		}
	}

	m_Idks[ 0 ].m_Unknown00 = true;
	m_Idks[ 0 ].m_Unknown04 = a2;
	m_Idks[ 0 ].m_Unknown12 = gpGlobals->curtime;
	m_Idks[ 0 ].m_Unknown20 = a5;
	m_Idks[ 0 ].m_Unknown28 = 0;
	m_Idks[ 0 ].m_Unknown16 = gpGlobals->curtime + a4;
	m_Idks[ 0 ].m_Unknown24 = a3;
}

void CNEO_MuzzleflashFP::something3()
{
	for ( int i = 0; i < _ARRAYSIZE( m_Idks ); i++ )
		m_Idks[ i ].m_Unknown00 = false;
}

void CNEO_MuzzleflashFP::SetAttachment( int attachment )
{
	m_iAttachment = attachment;
}

void CNEO_MuzzleflashFP::SetUnknown1784( bool bSet )
{
	m_Unknown1784 = bSet;
}

bool CNEO_MuzzleflashFP::GetUnknown1784()
{
	return m_Unknown1784;
}
