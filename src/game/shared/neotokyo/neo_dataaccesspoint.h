#ifndef NEO_DATAACCESSPOINT_H
#define NEO_DATAACCESSPOINT_H
#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
#include "c_baseentity.h"
#else
#include "baseentity.h"
#endif

#ifdef CLIENT_DLL
#define CNeoDataAccessPoint C_NeoDataAccessPoint
#endif


class CNeoDataAccessPoint : public CBaseEntity
{	 
public:
	DECLARE_CLASS( CNeoDataAccessPoint, CBaseEntity );
	DECLARE_NETWORKCLASS();

#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CNeoDataAccessPoint();

#ifdef GAME_DLL
	virtual int UpdateTransmitState();	 
#endif	  	

	virtual void Activate();

protected:
	CNetworkVar( string_t, m_Name );
	CNetworkVar( int, m_ID );
	CNetworkVector( m_Position );
	CNetworkVar( int, m_Icon );
	CNetworkVar( int, m_Status );
	CNetworkVar( int, m_Radius );
};	
				


#endif // NEO_DATAACCESSPOINT_H