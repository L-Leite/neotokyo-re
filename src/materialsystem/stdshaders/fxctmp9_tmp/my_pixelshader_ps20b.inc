#include "shaderlib/cshader.h"
class my_pixelshader_ps20b_Static_Index
{
public:
	my_pixelshader_ps20b_Static_Index( )
	{
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
#endif // _DEBUG
		return 0;
	}
};
#define shaderStaticTest_my_pixelshader_ps20b 0
class my_pixelshader_ps20b_Dynamic_Index
{
public:
	my_pixelshader_ps20b_Dynamic_Index()
	{
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
#endif // _DEBUG
		return 0;
	}
};
#define shaderDynamicTest_my_pixelshader_ps20b 0
