#include "BaseVSShader.h"
#include "neotokyo_thermalvision_modelshader_ps20.inc"
#include "neotokyo_thermalvision_modelshader_vs20.inc"

BEGIN_VS_SHADER( neo_motionvision_modelshader, "Help for neotokyo_motionvision_modelshader" )
 
	BEGIN_SHADER_PARAMS
		SHADER_PARAM( TVMGRADTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
		SHADER_PARAM( THERMAL, SHADER_PARAM_TYPE_FLOAT, "1.0", "" );
		SHADER_PARAM( EYEVEC, SHADER_PARAM_TYPE_VEC3, "0.0, 0.0, 1.0", "" );
	END_SHADER_PARAMS   	
 
	bool NeedsFullFrameBufferTexture(IMaterialVar **params, bool bCheckSpecificToThisFrame /* = true */) const
	{
		return true;
	}
 
	SHADER_FALLBACK
	{
		if (g_pHardwareConfig->GetDXSupportLevel() < 80)
		{
			Assert(0);
			return "Wireframe";
		}
		return 0;
	}

	SHADER_INIT
	{  
		if ( params[ TVMGRADTEXTURE ]->IsDefined() )
		{
			LoadTexture( TVMGRADTEXTURE );
		}
	}
 
	SHADER_DRAW
	{
		SHADOW_STATE
		{		
			pShaderShadow->EnableDepthTest( true );	
			pShaderShadow->EnableDepthWrites( true );
			pShaderShadow->EnableAlphaWrites( true );
			pShaderShadow->EnableSRGBWrite( true );
			pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
			int fmt = VERTEX_POSITION | VERTEX_NORMAL;
			
			if ( IS_FLAG_SET( MATERIAL_VAR_HALFLAMBERT ) ) // VERY LIKELY TO BE WRONG
				fmt = VERTEX_POSITION | VERTEX_NORMAL | VERTEX_BONE_INDEX;
			
			pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 );		
			
 
			DECLARE_STATIC_VERTEX_SHADER( neotokyo_thermalvision_modelshader_vs20 );
			SET_STATIC_VERTEX_SHADER( neotokyo_thermalvision_modelshader_vs20 );
 
			DECLARE_STATIC_PIXEL_SHADER( neotokyo_thermalvision_modelshader_ps20 );
			SET_STATIC_PIXEL_SHADER( neotokyo_thermalvision_modelshader_ps20 );
		}
 
		DYNAMIC_STATE
		{
			pShaderAPI->SetVertexShaderIndex( pShaderAPI->GetCurrentNumBones() );
			pShaderAPI->SetPixelShaderIndex();
			
			float fPos;
			pShaderAPI->GetWorldSpaceCameraPosition( &fPos );
			pShaderAPI->SetPixelShaderConstant( 30, &fPos );
			BindTexture( SHADER_SAMPLER0, TVMGRADTEXTURE );
			
			float fTvmValue = params[ TVMGRADTEXTURE ]->GetFloatValue();
			pShaderAPI->SetPixelShaderConstant( 0, &fTvmValue );
		}
 
		Draw();
	}
 
END_SHADER