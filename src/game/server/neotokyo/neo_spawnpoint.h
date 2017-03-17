#ifndef NEO_SPAWNPOINT_H
#define NEO_SPAWNPOINT_H
#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
#include "c_baseentity.h"
#else
#include "baseentity.h"
#include "entityoutput.h"
#endif

#ifdef CLIENT_DLL
#define CNEOSpawnPoint C_NEOSpawnPoint
#endif


class CNEOPlayer;

class CNEOSpawnPoint : public CPointEntity
{	 
public:
	DECLARE_CLASS( CNEOSpawnPoint, CPointEntity );
	DECLARE_SERVERCLASS_NOBASE();	

#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CNEOSpawnPoint();

	virtual void Activate();

	virtual bool CanSpawn( CNEOPlayer* pTarget );

	inline COutputEvent* GetUnknownEvent() { return &m_UnknownEvent; }

private:
	COutputEvent m_UnknownEvent;
	int m_Unknown872;
};	



#endif // NEO_SPAWNPOINT_H