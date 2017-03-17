#include "cbase.h"
#include "neo_game_config.h"
			  

IMPLEMENT_NETWORKCLASS_ALIASED( NeoGameConfig, DT_NeoGameConfig )

BEGIN_NETWORK_TABLE( CNeoGameConfig, DT_NeoGameConfig )
#ifdef CLIENT_DLL
RecvPropInt( RECVINFO( m_GameType ) ),
#else													
SendPropInt( SENDINFO( m_GameType ) ),
#endif
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( neo_game_config, CNeoGameConfig );


CNeoGameConfig* g_pNeoGameConfig = nullptr;

CNeoGameConfig::CNeoGameConfig()
{
	Assert( !g_pNeoGameConfig );
	g_pNeoGameConfig = this;
}

#ifdef GAME_DLL
int CNeoGameConfig::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}

bool CNeoGameConfig::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq( szKeyName, "gametype" ) )
	{
		int gameType = atoi( szValue );

		if ( gameType < 0 || gameType >= 3 )
			gameType = 0;

		m_GameType = gameType;
	}	
	else
		return BaseClass::KeyValue( szKeyName, szValue );

	return true;
}
#endif
