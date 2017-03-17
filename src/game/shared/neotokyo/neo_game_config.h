#ifndef NEO_GAME_CONFIG_H
#define NEO_GAME_CONFIG_H
#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
#include "c_baseentity.h"
#else
#include "baseentity.h"
#endif

#ifdef CLIENT_DLL
#define CNeoGameConfig C_NeoGameConfig
#endif


class CNeoGameConfig : public CBaseEntity
{	 
public:
	DECLARE_CLASS( CNeoGameConfig, CBaseEntity );
	DECLARE_NETWORKCLASS();

	CNeoGameConfig();

#ifdef GAME_DLL
	virtual int UpdateTransmitState();

	virtual bool KeyValue( const char *szKeyName, const char *szValue );
#endif

	int GetGameType();

protected:
	CNetworkVar( int, m_GameType );
};			


extern CNeoGameConfig* g_pNeoGameConfig;

inline CNeoGameConfig* NeoGameConfig()
{
	return g_pNeoGameConfig;
}


inline int CNeoGameConfig::GetGameType()
{
	return m_GameType;
}

#endif // NEO_GAME_CONFIG_H