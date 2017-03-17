#include "cbase.h"
#include "neo_dataaccesspoint.h"


IMPLEMENT_NETWORKCLASS_ALIASED( NeoDataAccessPoint, DT_NeoDataAccessPoint )

BEGIN_NETWORK_TABLE( CNeoDataAccessPoint, DT_NeoDataAccessPoint )
#ifdef CLIENT_DLL
	//RecvPropStringT( RECVINFO( m_Name ) ), // Do we need this? If so we gotta create RecvPropStringT ourselves
	RecvPropInt( RECVINFO( m_ID ) ),
	RecvPropVector( RECVINFO( m_Position ) ),
	RecvPropInt( RECVINFO( m_Icon ) ),
	RecvPropInt( RECVINFO( m_Status ) ),
	RecvPropInt( RECVINFO( m_Radius ) ),
#else					
	//SendPropStringT( SENDINFO( m_Name ) ),
	SendPropInt( SENDINFO( m_ID ) ),
	SendPropVector( SENDINFO( m_Position ) ),
	SendPropInt( SENDINFO( m_Icon ) ),
	SendPropInt( SENDINFO( m_Status ) ),
	SendPropInt( SENDINFO( m_Radius ) ),
#endif
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( neo_datapoint, CNeoDataAccessPoint );

#ifdef GAME_DLL
	BEGIN_DATADESC( CNeoDataAccessPoint )
	END_DATADESC()
#endif


CNeoDataAccessPoint::CNeoDataAccessPoint()
{
	m_Status = 0;
}

#ifdef GAME_DLL		   
int CNeoDataAccessPoint::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}	
#endif	  

void CNeoDataAccessPoint::Activate()
{
	DevMsg( "Server DAP %s Activated\n", m_Name );

#ifdef GAME_DLL
	m_Position = GetAbsOrigin();

	if ( m_Icon < 0 || m_Icon > 11 )
		m_Icon = 0;
#endif

	BaseClass::Activate();
}

