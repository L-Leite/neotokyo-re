#ifndef NEO_ESCAPEPOINT_H
#define NEO_ESCAPEPOINT_H
#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
#include "c_baseentity.h"
#else
#include "baseentity.h"
#endif

#ifdef CLIENT_DLL
#define CNeoEscapePoint C_NeoEscapePoint
#endif


class CNeoEscapePoint : public CBaseEntity
{	 
public:
	DECLARE_CLASS( CNeoEscapePoint, CBaseEntity );
	DECLARE_NETWORKCLASS();

#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CNeoEscapePoint();
	virtual ~CNeoEscapePoint();

#ifdef GAME_DLL
	virtual int ShouldTransmit( const CCheckTransmitInfo *pInfo );
	virtual int UpdateTransmitState();	 	

	virtual bool KeyValue( const char *szKeyName, const char *szValue );

	virtual void Activate();
#endif	  

protected:
	CNetworkVector( m_Position );
	CNetworkVar( int, m_Radius );
};	


extern CUtlVector< CNeoEscapePoint* > g_EscapePoints;
extern CNeoEscapePoint* GetEscapePoint( int index );
extern int GetNumOfEscapePoints();


#endif // NEO_ESCAPEPOINT_H