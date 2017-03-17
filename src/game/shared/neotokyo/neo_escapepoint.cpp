#include "cbase.h"
#include "neo_escapepoint.h"

CUtlVector< CNeoEscapePoint* > g_EscapePoints;


IMPLEMENT_NETWORKCLASS_ALIASED( NeoEscapePoint, DT_NeoEscapePoint )

BEGIN_NETWORK_TABLE( CNeoEscapePoint, DT_NeoEscapePoint )
#ifdef CLIENT_DLL
	RecvPropVector( RECVINFO( m_Position ) ),
	RecvPropInt( RECVINFO( m_Radius ) ),
#else					
	SendPropVector( SENDINFO( m_Position ) ),
	SendPropInt( SENDINFO( m_Radius ) ),
#endif
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( neo_escape_point, CNeoEscapePoint );

#ifdef GAME_DLL
	BEGIN_DATADESC( CNeoEscapePoint )
	END_DATADESC()
#endif


CNeoEscapePoint* GetEscapePoint( int index )
{
	if ( index < 0 || index >= GetNumOfEscapePoints() )
		return nullptr;

	return g_EscapePoints[ index ];
}

int GetNumOfEscapePoints()
{			
	return g_EscapePoints.Count();
}


CNeoEscapePoint::CNeoEscapePoint()
{
	g_EscapePoints.AddToTail( this );
}

CNeoEscapePoint::~CNeoEscapePoint()
{
	g_EscapePoints.FindAndRemove( this );
}


#ifdef GAME_DLL		   
int CNeoEscapePoint::ShouldTransmit( const CCheckTransmitInfo *pInfo )
{
	return FL_EDICT_ALWAYS;
}

int CNeoEscapePoint::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}	  

bool CNeoEscapePoint::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq( szKeyName, "radius" ) )
	{
		int iRadius = atoi( szValue );

		if ( iRadius < 48 || iRadius > 256 )
			iRadius = 128;

		m_Radius = iRadius;
	}
	else
	{
		return BaseClass::KeyValue( szKeyName, szValue );
	}

	return true;
}

void CNeoEscapePoint::Activate()
{
	m_Position = GetAbsOrigin(); 
	BaseClass::Activate();
}
#endif	
