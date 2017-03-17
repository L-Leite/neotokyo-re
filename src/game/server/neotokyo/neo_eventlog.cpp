//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "../EventLog.h"
#include "KeyValues.h"

class CNEOEventLog : public CEventLog
{
private:
	typedef CEventLog BaseClass;

public:
	virtual ~CNEOEventLog() {};

public:
	bool PrintEvent( IGameEvent * event )	// override virtual function
	{
		if ( BaseClass::PrintEvent( event ) )
		{
			return true;
		}
	
		if ( Q_strcmp(event->GetName(), "neo_") == 0 )
		{
			return PrintNEOEvent( event );
		}

		return false;
	}

protected:

	bool PrintNEOEvent( IGameEvent * event )	// print Mod specific logs
	{
		//const char * name = event->GetName() + Q_strlen("sdk_"); // remove prefix
		return false;
	}

};

CNEOEventLog g_NEOEventLog;

//-----------------------------------------------------------------------------
// Singleton access
//-----------------------------------------------------------------------------
IGameSystem* GameLogSystem()
{
	return &g_NEOEventLog;
}

