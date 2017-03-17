#include "cbase.h"
#include "neo_controlpoint.h"
#include "neo_gamerules.h"


IMPLEMENT_NETWORKCLASS_ALIASED( NeoControlPoint, DT_NeoControlPoint )

BEGIN_NETWORK_TABLE( CNeoControlPoint, DT_NeoControlPoint )
#ifdef CLIENT_DLL
	//RecvPropStringT( RECVINFO( m_Name ) ),
	RecvPropInt( RECVINFO( m_ID ) ),
	RecvPropVector( RECVINFO( m_Position ) ),
	RecvPropInt( RECVINFO( m_Icon ) ),
	RecvPropInt( RECVINFO( m_Status ) ),
	RecvPropInt( RECVINFO( m_Owner ) ),
	RecvPropInt( RECVINFO( m_Radius ) ),
#else											
	//SendPropStringT( SENDINFO( m_Name ) ),
	SendPropInt( SENDINFO( m_ID ) ),
	SendPropVector( SENDINFO( m_Position ) ),
	SendPropInt( SENDINFO( m_Icon ) ),
	SendPropInt( SENDINFO( m_Status ) ),
	SendPropInt( SENDINFO( m_Owner ) ),
	SendPropInt( SENDINFO( m_Radius ) ),
#endif
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( neo_controlpoint, CNeoControlPoint );

#ifdef GAME_DLL
	BEGIN_DATADESC( CNeoControlPoint )
	END_DATADESC()
#endif


CNeoControlPoint::CNeoControlPoint()
{
	m_Status = 0;
	m_Owner = 0;
}

#ifdef GAME_DLL		   
int CNeoControlPoint::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}	
#endif	  

void CNeoControlPoint::Activate()
{
	DevMsg( "Server CP %s Activated\n", m_Name );

#ifdef GAME_DLL
	m_Position = GetAbsOrigin();

	if ( m_Icon < 0 || m_Icon > 11 )
		m_Icon = 0;
#endif

	NEOGameRules()->AddControlPoint( this );
	BaseClass::Activate();
}

bool CNeoControlPoint::UpdatePointOwner()
{
#ifdef GAME_DLL
	CBaseEntity* pEntities[ 256 ];

	int count = UTIL_EntitiesInBox( pEntities, 256, GetAbsOrigin(), m_Position, 0 );

	for ( int i = 0; i < count; i++ )
	{
		CNEOPlayer* pPlayer = (CNEOPlayer*) pEntities[ i ];

		if ( !pPlayer || !pPlayer->IsPlayer() )
			continue;

		m_Owner = pPlayer->GetTeamNumber();
		return true;
	}
#endif

	return false;
}
