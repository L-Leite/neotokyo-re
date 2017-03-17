#include "BaseVSShader.h"
#include "neotokyo_nightvision_brightpass_ps20.inc"
#include "neotokyo_nightvision_brightpass_vs20.inc"

BEGIN_SHADER( neo_nightvision_brightpass, "Help for neo_nightvision_brightpass" )
 
	BEGIN_SHADER_PARAMS
		SHADER_PARAM( FBTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
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
 
			DECLARE_STATIC_VERTEX_SHADER( neotokyo_nightvision_brightpass_vs20 );
			SET_STATIC_VERTEX_SHADER( neotokyo_nightvision_brightpass_vs20 );
 
			DECLARE_STATIC_PIXEL_SHADER( neotokyo_nightvision_brightpass_ps20 );
			SET_STATIC_PIXEL_SHADER( neotokyo_nightvision_brightpass_ps20 );
		}
 
		DYNAMIC_STATE
		{
			BindTexture( SHADER_SAMPLER0, FBTEXTURE );
		}
 
		Draw();
	}
 
END_SHADER