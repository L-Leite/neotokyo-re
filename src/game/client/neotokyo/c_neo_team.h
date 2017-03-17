//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client side CNEOTeam class
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_NEO_TEAM_H
#define C_NEO_TEAM_H
#ifdef _WIN32
#pragma once
#endif

#include "c_team.h"
#include "shareddefs.h"

class C_BaseEntity;
class C_BaseObject;
class CBaseTechnology;

//-----------------------------------------------------------------------------
// Purpose: TF's Team manager
//-----------------------------------------------------------------------------
class C_NEOTeam : public C_Team
{
	DECLARE_CLASS( C_NEOTeam, C_Team );
	DECLARE_CLIENTCLASS();

public:

					C_NEOTeam();
	virtual			~C_NEOTeam();
};


#endif // C_NEO_TEAM_H
