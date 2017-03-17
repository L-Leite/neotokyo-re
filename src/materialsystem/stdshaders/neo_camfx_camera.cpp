#include "BaseVSShader.h"
#include "neotokyo_camfx_camera_ps20.inc"
#include "neotokyo_camfx_camera_vs20.inc"

BEGIN_SHADER( neo_camfx_camera, "Help for neo_camfx_camera" )
 
	BEGIN_SHADER_PARAMS
		SHADER_PARAM( FBTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
		SHADER_PARAM( BLURTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
		SHADER_PARAM( PIXELSIZEX, SHADER_PARAM_TYPE_FLOAT, "", "" );
		SHADER_PARAM( PIXELSIZEY, SHADER_PARAM_TYPE_FLOAT, "", "" );
		SHADER_PARAM( BLOOMLVL, SHADER_PARAM_TYPE_FLOAT, "0.55", "" );
		SHADER_PARAM( FINALGAMMA, SHADER_PARAM_TYPE_FLOAT, "0.75", "" );
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
	}
 
	SHADER_DRAW
	{
		SHADOW_STATE
		{								 
			pShaderShadow->EnableDepthWrites( false );
			pShaderShadow->DepthFunc( SHADER_DEPTHFUNC_NEVER );
			pShaderShadow->EnableAlphaWrites( true );
			pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
			pShaderShadow->EnableTexture( SHADER_SAMPLER1, true );
			int fmt = VERTEX_POSITION;
			pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 );
 
			DECLARE_STATIC_VERTEX_SHADER( neotokyo_camfx_camera_vs20 );
			SET_STATIC_VERTEX_SHADER( neotokyo_camfx_camera_vs20 );
 
			DECLARE_STATIC_PIXEL_SHADER( neotokyo_camfx_camera_ps20 );
			SET_STATIC_PIXEL_SHADER( neotokyo_camfx_camera_ps20 );
		}
 
		DYNAMIC_STATE
		{
			BindTexture( SHADER_SAMPLER0, FBTEXTURE );
			BindTexture( SHADER_SAMPLER1, BLURTEXTURE );

			float fFbTextureVal[ 2 ];
			fFbTextureVal[ 0 ] = params[ BASETEXTURE ]->GetFloatValue();	
			fFbTextureVal[ 1 ] = params[ FBTEXTURE ]->GetFloatValue();	  
			pShaderAPI->SetPixelShaderConstant( 0, fFbTextureVal );
			
			float fBlurTextureVal = params[ BLURTEXTURE ]->GetFloatValue();	  
			pShaderAPI->SetPixelShaderConstant( 1, &fBlurTextureVal );
		}
 
		Draw();
	}
 
END_SHADER