#include "BaseVSShader.h"
#include "neotokyo_camfx_brightpass_ps20.inc"
#include "neotokyo_camfx_brightpass_vs20.inc"

BEGIN_SHADER( neo_camfx_brightpass, "Help for neo_camfx_brightpass" )
 
	BEGIN_SHADER_PARAMS
		SHADER_PARAM( FBTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
		SHADER_PARAM( PIXELSIZEX, SHADER_PARAM_TYPE_FLOAT, "", "" );
		SHADER_PARAM( PIXELSIZEY, SHADER_PARAM_TYPE_FLOAT, "", "" );
		SHADER_PARAM( BPCONTRAST, SHADER_PARAM_TYPE_FLOAT, "1.55", "" );
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
	}
 
	SHADER_DRAW
	{
		SHADOW_STATE
		{								 
			pShaderShadow->EnableDepthWrites( false );
			pShaderShadow->DepthFunc( SHADER_DEPTHFUNC_NEVER );
			pShaderShadow->EnableAlphaWrites( true );
			pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
			int fmt = VERTEX_POSITION;
			pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 );
 
			DECLARE_STATIC_VERTEX_SHADER( neotokyo_camfx_brightpass_vs20 );
			SET_STATIC_VERTEX_SHADER( neotokyo_camfx_brightpass_vs20 );
 
			DECLARE_STATIC_PIXEL_SHADER( neotokyo_camfx_brightpass_ps20 );
			SET_STATIC_PIXEL_SHADER( neotokyo_camfx_brightpass_ps20 );
		}
 
		DYNAMIC_STATE
		{
			BindTexture( SHADER_SAMPLER0, FBTEXTURE );

			float fTextureVal[ 2 ];
			fTextureVal[ 0 ] = params[ BASETEXTURE ]->GetFloatValue();
			fTextureVal[ 1 ] = params[ FBTEXTURE ]->GetFloatValue();	  
			pShaderAPI->SetPixelShaderConstant( 0, fTextureVal );
		}
 
		Draw();
	}
 
END_SHADER