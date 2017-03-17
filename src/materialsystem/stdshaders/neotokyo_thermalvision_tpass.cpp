#include "BaseVSShader.h"
#include "neotokyo_thermalvision_tpass_ps20.inc"
#include "neotokyo_thermalvision_tpass_vs20.inc"

BEGIN_VS_SHADER( neo_thermalvision_tpass, "Help for neotokyo_thermalvision_tpass" )
 
	BEGIN_SHADER_PARAMS
		SHADER_PARAM( FBTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
		SHADER_PARAM( NVTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
		SHADER_PARAM( SLTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" );
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
		if ( params[ NVTEXTURE ]->IsDefined() )
		{
			LoadTexture( NVTEXTURE );
		}
		if ( params[ SLTEXTURE ]->IsDefined() )
		{
			LoadTexture( SLTEXTURE );
		}
	}
 
	SHADER_DRAW
	{
		SHADOW_STATE
		{		
			pShaderShadow->EnableDepthTest( false );	
			pShaderShadow->EnableDepthWrites( false );
			pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
			pShaderShadow->EnableTexture( SHADER_SAMPLER1, true );
			pShaderShadow->EnableTexture( SHADER_SAMPLER2, true );
			int fmt = VERTEX_POSITION;			
			pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 );		
			
 
			DECLARE_STATIC_VERTEX_SHADER( neotokyo_thermalvision_tpass_vs20 );
			SET_STATIC_VERTEX_SHADER( neotokyo_thermalvision_tpass_vs20 );
 
			DECLARE_STATIC_PIXEL_SHADER( neotokyo_thermalvision_tpass_ps20 );
			SET_STATIC_PIXEL_SHADER( neotokyo_thermalvision_tpass_ps20 );
		}
 
		DYNAMIC_STATE
		{
			BindTexture( SHADER_SAMPLER0, FBTEXTURE );
			BindTexture( SHADER_SAMPLER1, NVTEXTURE );
			BindTexture( SHADER_SAMPLER2, SLTEXTURE );
			
			SetVertexShaderTextureTransform( VERTEX_SHADER_SHADER_SPECIFIC_CONST_0, NOISETRANSFORM );
		}
 
		Draw();
	}
 
END_SHADER