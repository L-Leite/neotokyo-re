#include "cbase.h"
#include "neo_bloomcontroller.h"

CNeoBloomController* g_pMapBloomController = nullptr;


IMPLEMENT_NETWORKCLASS_ALIASED( NeoBloomController, DT_NeoBloomController )

BEGIN_NETWORK_TABLE( CNeoBloomController, DT_NeoBloomController )
#ifdef CLIENT_DLL
	RecvPropFloat( RECVINFO( m_BloomAmount ) ),
#else													
	SendPropFloat( SENDINFO( m_BloomAmount ) ),
#endif
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( neo_bloom_controller, CNeoBloomController );


inline CNeoBloomController* GetMapBloomController()
{
	return g_pMapBloomController;
}						 

CNeoBloomController::CNeoBloomController()
{
	g_pMapBloomController = this;
}

CNeoBloomController::~CNeoBloomController()
{
	if ( g_pMapBloomController == this )
		g_pMapBloomController = nullptr;
}

#ifdef GAME_DLL		   
int CNeoBloomController::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}	

bool CNeoBloomController::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq( szKeyName, "bloomamount" ) )
		m_BloomAmount = atof( szValue );
	else
		return BaseClass::KeyValue( szKeyName, szValue );

	return true;
} 
#endif	  
