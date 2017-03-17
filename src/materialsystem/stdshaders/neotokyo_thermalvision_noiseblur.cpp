#include "BaseVSShader.h"
#include "neotokyo_thermalvision_noiseblur_ps20.inc"
#include "neotokyo_thermalvision_noiseblur_vs20.inc"

BEGIN_VS_SHADER( neotokyo_thermalvision_noiseblur, "Help for neotokyo_thermalvision_noiseblur" )
 
	BEGIN_SHADER_PARAMS
		SHADER_PARAM( BLURTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
		SHADER_PARAM( NOISETEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
		SHADER_PARAM( PIXELSIZEX, SHADER_PARAM_TYPE_FLOAT, "", "" );
		SHADER_PARAM( PIXELSIZEY, SHADER_PARAM_TYPE_FLOAT, "", "" );
		SHADER_PARAM( ITERATION, SHADER_PARAM_TYPE_FLOAT, "", "" );
		SHADER_PARAM( NOISETRANSFORM, SHADER_PARAM_TYPE_MATRIX, "center .5 .5 scale 1 1 rotate 0 translate 0 0", "" );
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
		if ( params[ BLURTEXTURE ]->IsDefined() )
		{
			LoadTexture( BLURTEXTURE );
		}
		if ( params[ NOISETEXTURE ]->IsDefined() )
		{
			LoadTexture( NOISETEXTURE );
		}
	}
 
	SHADER_DRAW
	{
		SHADOW_STATE
		{		
			pShaderShadow->EnableDepthTest( false );	
			pShaderShadow->EnableDepthWrites( false );
			pShaderShadow->EnableAlphaWrites( true );
			pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
			int fmt = VERTEX_POSITION;			
			pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 );		
			
 
			DECLARE_STATIC_VERTEX_SHADER( neotokyo_thermalvision_noiseblur_vs20 );
			SET_STATIC_VERTEX_SHADER( neotokyo_thermalvision_noiseblur_vs20 );
 
			DECLARE_STATIC_PIXEL_SHADER( neotokyo_thermalvision_noiseblur_ps20 );
			SET_STATIC_PIXEL_SHADER( neotokyo_thermalvision_noiseblur_ps20 );
		}
 
		DYNAMIC_STATE
		{
			BindTexture( SHADER_SAMPLER0, BLURTEXTURE );
			BindTexture( SHADER_SAMPLER1, NOISETEXTURE );
			
			float fBlurTextureVal[ 3 ]; 
			fBlurTextureVal[ 0 ] = params[ BASETEXTURE ]->GetFloatValue();
			fBlurTextureVal[ 1 ] = params[ BLURTEXTURE ]->GetFloatValue();
			fBlurTextureVal[ 2 ] = params[ NOISETEXTURE ]->GetFloatValue();
			pShaderAPI->SetPixelShaderConstant( 0, fBlurTextureVal );
			
			SetVertexShaderTextureTransform( VERTEX_SHADER_SHADER_SPECIFIC_CONST_0, NOISETRANSFORM );
		}
 
		Draw();
	}
 
END_SHADER