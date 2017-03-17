#ifndef NEO_GHOST_RETRIEVALPOINT_H
#define NEO_GHOST_RETRIEVALPOINT_H
#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
#include "c_baseentity.h"
#else
#include "baseentity.h"
#endif

#ifdef CLIENT_DLL
#define CNeoGhostRetrievalPoint C_NeoGhostRetrievalPoint
#endif


class CNeoGhostRetrievalPoint : public CBaseEntity
{	 
public:
	DECLARE_CLASS( CNeoGhostRetrievalPoint, CBaseEntity );
	DECLARE_NETWORKCLASS();
	
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CNeoGhostRetrievalPoint();

#ifdef GAME_DLL
	virtual int ShouldTransmit( const CCheckTransmitInfo *pInfo );
	virtual int UpdateTransmitState();	 

	virtual void Spawn();
	virtual void Activate(); 	

	virtual bool KeyValue( const char *szKeyName, const char *szValue );
#endif	 

	int GetDefaultTeam();

	int GetOwningTeamNumber();
	void SetOwningTeamNumber( int team );

	int GetRadius();

protected:
	int m_iDefaultTeam;
	CNetworkVector( m_Position );
	CNetworkVar( int, m_OwningTeamNumber );
	CNetworkVar( int, m_Radius );
};	


inline int CNeoGhostRetrievalPoint::GetDefaultTeam()
{
	return m_iDefaultTeam;
}

inline int CNeoGhostRetrievalPoint::GetOwningTeamNumber()
{
	return m_OwningTeamNumber;
}

inline void CNeoGhostRetrievalPoint::SetOwningTeamNumber( int team )
{
	m_OwningTeamNumber = team;
}

inline int CNeoGhostRetrievalPoint::GetRadius()
{
	return m_Radius;
}

extern CUtlVector< CNeoGhostRetrievalPoint* > g_GhostRetrievalPoints;
extern CNeoGhostRetrievalPoint* GetGhostRetrievalPoint( int iIndex );
extern int GetNumberOfGhostRetrievalPoints();


#endif // NEO_GHOST_RETRIEVALPOINT_H