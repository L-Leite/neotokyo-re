#include "BaseVSShader.h"
#include "neotokyo_pptest_ps20.inc"
#include "neotokyo_pptest_vs20.inc"

BEGIN_SHADER( NEO_PPTest, "Help for NEO_PPTest" )
 
	BEGIN_SHADER_PARAMS
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
	}
 
	SHADER_DRAW
	{
		SHADOW_STATE
		{								 
			pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
			int fmt = VERTEX_POSITION;
			pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 );
 
			DECLARE_STATIC_VERTEX_SHADER( neotokyo_pptest_vs20 );
			SET_STATIC_VERTEX_SHADER( neotokyo_pptest_vs20 );
 
			DECLARE_STATIC_PIXEL_SHADER( neotokyo_pptest_ps20 );
			SET_STATIC_PIXEL_SHADER( neotokyo_pptest_ps20 );
		}
 
		DYNAMIC_STATE
		{
			pShaderAPI->BindStandardTexture( SHADER_SAMPLER0, TEXTURE_FRAME_BUFFER_FULL_TEXTURE_0 );
		}
 
		Draw();
	}
 
END_SHADER