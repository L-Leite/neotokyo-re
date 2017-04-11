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

	string_t GetName();
	Vector	GetPosition();
	int		GetIcon();
	int		GetOwner();

protected:
	CNetworkVar( string_t, m_Name );
	CNetworkVar( int, m_ID );
	CNetworkVector( m_Position );
	CNetworkVar( int, m_Icon );
	CNetworkVar( int, m_Status );
	CNetworkVar( int, m_Owner );
	CNetworkVar( int, m_Radius );
};	

inline string_t CNeoControlPoint::GetName()
{
	return m_Name;
}

inline Vector CNeoControlPoint::GetPosition()
{
	return m_Position;
}

inline int CNeoControlPoint::GetIcon()
{
	return m_Icon;
}

inline int CNeoControlPoint::GetOwner()
{
	return m_Owner;
}
				


#endif // NEO_CONTROLPOINT_H