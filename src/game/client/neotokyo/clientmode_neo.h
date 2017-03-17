//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef NEO_CLIENTMODE_H
#define NEO_CLIENTMODE_H
#ifdef _WIN32
#pragma once
#endif

#include "clientmode_shared.h"
#include "neoviewport.h"

class ClientModeNEONormal : public ClientModeShared 
{
public:
	DECLARE_CLASS( ClientModeNEONormal, ClientModeShared );

private:

// IClientMode overrides.
public:

					ClientModeNEONormal();
	virtual			~ClientModeNEONormal();

	virtual void	InitViewport();

	virtual float	GetViewModelFOV( void );

	int				GetDeathMessageStartHeight( void );

	virtual void	PostRenderVGui();

	bool IsNEODev();

	
private:
	bool m_bIsNEODev;
	
	//	void	UpdateSpectatorMode( void );

};

inline bool ClientModeNEONormal::IsNEODev()
{
	return m_bIsNEODev;
}


extern IClientMode *GetClientModeNormal();
extern ClientModeNEONormal* GetClientModeNEONormal();


#endif // NEO_CLIENTMODE_H
