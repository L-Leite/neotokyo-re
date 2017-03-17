#include "BaseVSShader.h"
#include "neotokyo_motionvision_ps20.inc"
#include "neotokyo_nightvision_nv_vs20.inc"

BEGIN_VS_SHADER( neo_motionvision, "Help for neotokyo_motionvision" )
 
	BEGIN_SHADER_PARAMS
		SHADER_PARAM( FBTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
		SHADER_PARAM( BLURTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
		SHADER_PARAM( NVTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
		SHADER_PARAM( NOISETEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
		SHADER_PARAM( PIXELSIZEX, SHADER_PARAM_TYPE_FLOAT, "", "" );
		SHADER_PARAM( PIXELSIZEY, SHADER_PARAM_TYPE_FLOAT, "", "" );
		SHADER_PARAM( BLOOMLVL, SHADER_PARAM_TYPE_FLOAT, "0.55", "" );
		SHADER_PARAM( FINALGAMMA, SHADER_PARAM_TYPE_FLOAT, "0.75", "" );
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
		if ( params[ FBTEXTURE ]->IsDefined() )
		{
			LoadTexture( FBTEXTURE );
		}
		if ( params[ BLURTEXTURE ]->IsDefined() )
		{
			LoadTexture( BLURTEXTURE );
		}
		if ( params[ NVTEXTURE ]->IsDefined() )
		{
			LoadTexture( NVTEXTURE );
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
			pShaderShadow->EnableDepthWrites( false );
			pShaderShadow->EnableAlphaWrites( false );
			pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
			pShaderShadow->EnableTexture( SHADER_SAMPLER1, true );
			pShaderShadow->EnableTexture( SHADER_SAMPLER2, true );
			pShaderShadow->EnableTexture( SHADER_SAMPLER3, true );
			int fmt = VERTEX_POSITION;
			pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 );
 
			DECLARE_STATIC_VERTEX_SHADER( neotokyo_nightvision_nv_vs20 );
			SET_STATIC_VERTEX_SHADER( neotokyo_nightvision_nv_vs20 );
 
			DECLARE_STATIC_PIXEL_SHADER( neotokyo_motionvision_ps20 );
			SET_STATIC_PIXEL_SHADER( neotokyo_motionvision_ps20 );
		}
 
		DYNAMIC_STATE
		{
			BindTexture( SHADER_SAMPLER0, FBTEXTURE );
			BindTexture( SHADER_SAMPLER1, BLURTEXTURE );
			BindTexture( SHADER_SAMPLER2, NVTEXTURE );
			BindTexture( SHADER_SAMPLER3, NOISETEXTURE );

			float fFbTextureVal[ 3 ];
			fFbTextureVal[ 0 ] = params[ BASETEXTURE ]->GetFloatValue();
			fFbTextureVal[ 1 ] = params[ FBTEXTURE ]->GetFloatValue();
			fFbTextureVal[ 2 ] = params[ BLURTEXTURE ]->GetFloatValue();
			pShaderAPI->SetVertexShaderConstant( 0, fFbTextureVal );
			
			float fNvTextureVal[ 2 ];
			fNvTextureVal[ 0 ] = params[ NVTEXTURE ]->GetFloatValue();
			fNvTextureVal[ 1 ] = params[ NOISETEXTURE ]->GetFloatValue();
			pShaderAPI->SetPixelShaderConstant( 1, fNvTextureVal );
			
			SetVertexShaderTextureTransform( VERTEX_SHADER_SHADER_SPECIFIC_CONST_0, NOISETRANSFORM );
		}
 
		Draw();
	}
 
END_SHADER