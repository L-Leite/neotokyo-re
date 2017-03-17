#include "cbase.h"
#include "neo_ghost_spawnpoint.h"
#include "neo_gamerules.h"

CUtlVector< CNeoGhostSpawnPoint* > g_GhostSpawnPoints;		   


LINK_ENTITY_TO_CLASS( neo_ghostspawnpoint, CNeoGhostSpawnPoint );

					  
BEGIN_DATADESC( CNeoGhostSpawnPoint )
END_DATADESC()


CNeoGhostSpawnPoint* GetGhostSpawnPoint( int iIndex )
{
	if ( iIndex < 0 || iIndex >= GetNumberOfGhostSpawnPoints() )
		return nullptr;

	return g_GhostSpawnPoints[ iIndex ];
}

int GetNumberOfGhostSpawnPoints()
{	 
	return g_GhostSpawnPoints.Size();
}

CNeoGhostSpawnPoint::CNeoGhostSpawnPoint()
{
	g_GhostSpawnPoints.AddToTail( this );
}

void CNeoGhostSpawnPoint::Spawn()
{
	SetMoveType( MOVETYPE_NONE );
	BaseClass::Spawn();
}

void CNeoGhostSpawnPoint::Activate()
{
	NEOGameRules()->AddGhostSpawnPoint( this );
	BaseClass::Activate();
}

bool CNeoGhostSpawnPoint::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq( szKeyName, "mins" ) || FStrEq( szKeyName, "maxs" ) )
		Warning( "Warning! Can't specify mins/maxs for point entities! (%s)\n", GetClassname() );
	else
		return BaseClass::KeyValue( szKeyName, szValue );

	return true;
} 

int	CNeoGhostSpawnPoint::ObjectCaps()
{
	return BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION;
}
