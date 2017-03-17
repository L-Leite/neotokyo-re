#ifndef NEO_GHOST_SPAWNPOINT_H
#define NEO_GHOST_SPAWNPOINT_H
#ifdef _WIN32
#pragma once
#endif

#include "baseentity.h"


class CNeoGhostSpawnPoint : public CBaseEntity
{	 
public:
	DECLARE_CLASS( CNeoGhostSpawnPoint, CBaseEntity );
	DECLARE_SERVERCLASS_NOBASE();
	DECLARE_DATADESC();

	CNeoGhostSpawnPoint();

	virtual void Spawn();
	virtual void Activate();

	virtual bool KeyValue( const char *szKeyName, const char *szValue );		

	virtual int	ObjectCaps();
};	


extern CUtlVector< CNeoGhostSpawnPoint* > g_GhostSpawnPoints;
extern CNeoGhostSpawnPoint* GetGhostSpawnPoint( int iIndex );
extern int GetNumberOfGhostSpawnPoints();


#endif // NEO_GHOST_SPAWNPOINT_H