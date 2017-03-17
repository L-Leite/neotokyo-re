#include "cbase.h"
#include "neo_thermhandler.h"
#include "materialsystem\imaterialvar.h"


CThermopticHandler g_ThermopticHandler;

CThermopticHandler::CThermopticHandler()
{
	m_pThermMaterial = nullptr;
	m_fTransparency = 0.0f;
	m_bTriedToInitMat = false;		
}

IMaterial* CThermopticHandler::GetThermopticMaterial()
{
	if ( !m_bTriedToInitMat && !CreateThermopticMaterial() )
		return nullptr;

	return m_pThermMaterial;
}

void CThermopticHandler::UpdateThermopticMaterial( float fNewTransparency )	
{
	m_fTransparency = fNewTransparency * 0.2f; 

	bool found = false;

	IMaterialVar* refractamount = m_pThermMaterial->FindVar( "$refractamount", &found );

	if ( found )
		refractamount->SetFloatValue( m_fTransparency * 0.035f );

	IMaterialVar* bluramount = m_pThermMaterial->FindVar( "$bluramount", &found );

	if ( found )
		bluramount->SetFloatValue( m_fTransparency * 20.0f + 10.5f );

	IMaterialVar* refracttint = m_pThermMaterial->FindVar( "$refracttint", &found );

	if ( found )
		refracttint->SetVecValue( 0.85f - m_fTransparency * 0.8f, 0.85f - m_fTransparency * 0.6f, 0.85f -m_fTransparency * 0.6f );

	IMaterialVar* fresnelreflection = m_pThermMaterial->FindVar( "$fresnelreflection", &found );

	if ( found )
		fresnelreflection->SetFloatValue( m_fTransparency + m_fTransparency + 0.25f );

	IMaterialVar* bumptransform = m_pThermMaterial->FindVar( "$bumptransform", &found );

	if ( found )
	{
		VMatrix matrix( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );

		if ( bumptransform->GetType() == MATERIAL_VAR_TYPE_MATRIX )
			bumptransform->SetMatrixValue( matrix );
		else
			bumptransform->SetVecValue( 0.0f, 0.0f, 0.0f );
	}
}

bool CThermopticHandler::CreateThermopticMaterial()
{
	m_bTriedToInitMat = true;

	if ( !g_pMaterialSystem )
		return false;

	KeyValues* keyValues = new KeyValues( "Refract" );

	m_pThermMaterial = g_pMaterialSystem->CreateMaterial( "thermoptic", keyValues );

	if ( !m_pThermMaterial )
		return false;

	bool found = false;

	m_pThermMaterial->SetShader( "Refract" );
	m_pThermMaterial->SetMaterialVarFlag( MATERIAL_VAR_MODEL, true );
	m_pThermMaterial->SetMaterialVarFlag( MATERIAL_VAR_SUPPRESS_DECALS, true );

	IMaterialVar* refractamount = m_pThermMaterial->FindVar( "$refractamount", &found );

	if ( found )
		refractamount->SetFloatValue( 0.8f );

	IMaterialVar* normalmap = m_pThermMaterial->FindVar( "$normalmap", &found );

	if ( found )
	{
		ITexture* waterTexture = g_pMaterialSystem->FindTexture( "dev/water_normal", "ClientEffect textures" );

		if ( waterTexture )
			normalmap->SetTextureValue( waterTexture );
	}

	IMaterialVar* bumpframe = m_pThermMaterial->FindVar( "$bumpframe", &found );

	if ( found )
		bumpframe->SetIntValue( 0 );	 
		
	return true;
}  		