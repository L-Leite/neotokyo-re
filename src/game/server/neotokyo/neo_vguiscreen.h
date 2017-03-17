#ifndef NEO_VGUISCREEN_H
#define NEO_VGUISCREEN_H

#ifdef _WIN32
#pragma once
#endif

#include "vguiscreen.h"


class CNEOVGuiScreen : public CVGuiScreen
{
public:
	DECLARE_CLASS( CNEOVGuiScreen, CVGuiScreen );	  	
	DECLARE_DATADESC();

	CNEOVGuiScreen();

	virtual void Activate();
};



#endif // NEO_VGUISCREEN_H
