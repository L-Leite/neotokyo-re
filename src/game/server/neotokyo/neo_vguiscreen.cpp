#include "cbase.h"
#include "neo_vguiscreen.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


LINK_ENTITY_TO_CLASS( neo_vguiscreen, CNEOVGuiScreen );
PRECACHE_REGISTER( neo_vguiscreen );


BEGIN_DATADESC( CNEOVGuiScreen )
END_DATADESC()


CNEOVGuiScreen::CNEOVGuiScreen()
{
}							

void CNEOVGuiScreen::Activate()
{
	BaseClass::Activate();
}
