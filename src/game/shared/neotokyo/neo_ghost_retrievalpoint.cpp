#include "cbase.h"
#include "neo_ghost_retrievalpoint.h"
#include "neo_shareddefs.h"	

CUtlVector< CNeoGhostRetrievalPoint* > g_GhostRetrievalPoints;


IMPLEMENT_NETWORKCLASS_ALIASED( NeoGhostRetrievalPoint, DT_NeoGhostRetrievalPoint )

BEGIN_NETWORK_TABLE( CNeoGhostRetrievalPoint, DT_NeoGhostRetrievalPoint )
#ifdef CLIENT_DLL
	RecvPropVector( RECVINFO( m_Position ) ),
	RecvPropInt( RECVINFO( m_OwningTeamNumber ) ),
	RecvPropInt( RECVINFO( m_Radius ) ),
#else													
	SendPropVector( SENDINFO( m_Position ) ),
	SendPropInt( SENDINFO( m_OwningTeamNumber ) ),
	SendPropInt( SENDINFO( m_Radius ) ),
#endif
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( neo_ghost_retrieval_point, CNeoGhostRetrievalPoint );


#ifdef GAME_DLL

	BEGIN_DATADESC( CNeoGhostRetrievalPoint )
	END_DATADESC()

#endif


CNeoGhostRetrievalPoint* GetGhostRetrievalPoint( int iIndex )
{
	if ( iIndex < 0 || iIndex >= GetNumberOfGhostRetrievalPoints() )
		return nullptr;

	return g_GhostRetrievalPoints[ iIndex ];
}

int GetNumberOfGhostRetrievalPoints()
{	 
	return g_GhostRetrievalPoints.Size();
}

#ifdef CLIENT_DLL
CON_COMMAND_F( cl_listghostretrievalpoints, "List ghost retrieval points.", FCVAR_CHEAT )
{
	DevMsg( "Client list of ghost retrieval points:\n" );

	if ( g_GhostRetrievalPoints.IsEmpty() )
	{
		Warning( "\tNo ghost retrieval points found.\n" );
		return;
	}

	for ( int i = 0; i < g_GhostRetrievalPoints.Size(); i++ )
	{
		CNeoGhostRetrievalPoint* pRetrievalPoint = g_GhostRetrievalPoints[ i ];
		Vector vOrigin = pRetrievalPoint->GetAbsOrigin();
		DevMsg( "\tRetrieval point %i: pos %f %f %f\n", i, vOrigin.x, vOrigin.y, vOrigin.z );
	}
}
#elif GAME_DLL
CON_COMMAND_F( sv_listghostretrievalpoints, "List ghost retrieval points.", FCVAR_CHEAT )
{
	DevMsg( "Server list of ghost retrieval points:\n" );

	if ( g_GhostRetrievalPoints.IsEmpty() )
	{
		Warning( "\tNo ghost retrieval points found.\n" );
		return;
	}

	for ( int i = 0; i < g_GhostRetrievalPoints.Size(); i++ )
	{
		CNeoGhostRetrievalPoint* pRetrievalPoint = g_GhostRetrievalPoints[ i ];
		Vector vOrigin = pRetrievalPoint->GetAbsOrigin();
		DevMsg( "\tRetrieval point %i: pos %f %f %f\n", i, vOrigin.x, vOrigin.y, vOrigin.z );
	}
}
#endif



CNeoGhostRetrievalPoint::CNeoGhostRetrievalPoint()
{
	g_GhostRetrievalPoints.AddToTail( this );
}

#ifdef GAME_DLL
int CNeoGhostRetrievalPoint::ShouldTransmit( const CCheckTransmitInfo *pInfo )
{
	return FL_EDICT_ALWAYS;
}

int CNeoGhostRetrievalPoint::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}	

void CNeoGhostRetrievalPoint::Spawn()
{
	SetMoveType( MOVETYPE_NONE );
	CollisionProp()->SetSolid( SOLID_NONE );
	UTIL_SetSize( this, Vector( -2.0f, -2.0f, -2.0f ), Vector( 2.0f, 2.0f, 2.0f ) );
	m_Position = GetAbsOrigin();
	BaseClass::Spawn();
}

void CNeoGhostRetrievalPoint::Activate()
{
	m_Position = GetAbsOrigin();
	BaseClass::Activate();
}  

bool CNeoGhostRetrievalPoint::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq( szKeyName, "team" ) )
	{		   
		int teamNum = atoi( szValue );

		if ( teamNum < 0 || teamNum > 1 )
			teamNum = 0;

		m_iDefaultTeam = teamNum;
	}
	else if ( FStrEq( szKeyName, "radius" ) )
	{		
		int radius = atoi( szValue );

		if ( radius < GHOST_RETRIEVAL_MIN_RADIUS_DIST || radius > GHOST_RETRIEVAL_MAX_RADIUS_DIST )
			radius = GHOST_RETRIEVAL_DEFAULT_RADIUS_DIST;
	}
	else
		return BaseClass::KeyValue( szKeyName, szValue );

	return true;
} 
#endif	
