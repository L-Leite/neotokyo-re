#include "cbase.h"
#include "neo_spawnpoint.h" 
#include "neo_gamerules.h"


LINK_ENTITY_TO_CLASS( info_player_attacker, CNEOSpawnPoint );
LINK_ENTITY_TO_CLASS( info_player_defender, CNEOSpawnPoint );


#ifdef GAME_DLL	 
	BEGIN_DATADESC( CNEOSpawnPoint )
	END_DATADESC()	  
#endif


CNEOSpawnPoint::CNEOSpawnPoint()
{		
}

void CNEOSpawnPoint::Activate()
{
	BaseClass::Activate();
	NEOGameRules()->AddPlayerSpawnPoint( this );
}

bool CNEOSpawnPoint::CanSpawn( CNEOPlayer* pTarget )
{
	if ( m_Unknown872 == 1 )
		return false;

	CBaseEntity* pEntities[ 256 ];

	int count = UTIL_EntitiesInBox( pEntities, 256, GetAbsOrigin(), GetAbsOrigin() + Vector( 32.0f, 32.0f, 32.0f ), 0 );

	for ( int i = 0; i < count; i++ )
	{
		CNEOPlayer* pPlayer = (CNEOPlayer*) pEntities[ i ];

		if ( (pPlayer && pPlayer->IsPlayer()) || pPlayer == pTarget )
			continue;

		return true;
	}

	return false;
}
