#ifndef NEO_CONTROLPOINT_H
#define NEO_CONTROLPOINT_H
#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
#include "c_baseentity.h"
#else
#include "baseentity.h"
#endif

#ifdef CLIENT_DLL
#define CNeoControlPoint C_NeoControlPoint
#endif


class CNeoControlPoint : public CBaseEntity
{	 
public:
	DECLARE_CLASS( CNeoControlPoint, CBaseEntity );
	DECLARE_NETWORKCLASS();

#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CNeoControlPoint();

#ifdef GAME_DLL
	virtual int UpdateTransmitState();	 
#endif	  	

	virtual void Activate();

	bool UpdatePointOwner();

protected:
	CNetworkVar( string_t, m_Name );
	CNetworkVar( int, m_ID );
	CNetworkVector( m_Position );
	CNetworkVar( int, m_Icon );
	CNetworkVar( int, m_Status );
	CNetworkVar( int, m_Owner );
	CNetworkVar( int, m_Radius );
};	
				


#endif // NEO_CONTROLPOINT_H