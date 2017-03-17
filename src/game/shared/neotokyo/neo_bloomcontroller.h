#ifndef NEO_BLOOMCONTROLLER_H
#define NEO_BLOOMCONTROLLER_H
#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
#include "c_baseentity.h"
#else
#include "baseentity.h"
#endif

#ifdef CLIENT_DLL
#define CNeoBloomController C_NeoBloomController
#endif


class CNeoBloomController : public CBaseEntity
{	 
public:
	DECLARE_CLASS( CNeoBloomController, CBaseEntity );
	DECLARE_NETWORKCLASS();

	CNeoBloomController();
	virtual ~CNeoBloomController();

#ifdef GAME_DLL
	virtual int UpdateTransmitState();	 

	virtual bool KeyValue( const char *szKeyName, const char *szValue );
#endif	  	

	float GetBloomAmount();

protected:
	CNetworkVar( float, m_BloomAmount );
};	

inline float CNeoBloomController::GetBloomAmount()
{
	return m_BloomAmount;
}


extern CNeoBloomController* g_pMapBloomController;
extern CNeoBloomController* GetMapBloomController();



#endif // NEO_BLOOMCONTROLLER_H