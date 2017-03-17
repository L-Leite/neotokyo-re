#include "cbase.h"
#include "neo_playerclass_info.h"
#include "neo_shareddefs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


bool CPlayerClassInfo::idk( int i )
{
	switch ( i )
	{
		case 0:
			return true;
		case 1:
			return Unknown41;
		case 2:
			return Unknown42;
		case 3:
			return Unknown43;
		case 4:
			return Unknown44;
	}

	return false;
}

CDefaultClassInfo::CDefaultClassInfo()
{
	iClass = CLASS_UNASSIGNED;
	Unknown04 = 0;
	fMaxSpeed = 1.0f;
	Unknown12 = 1.0f;
	fDamageScale = 1.0f;
	Unknown20 = false;
	fMaxThermopticNRG = 0.0f;
	Unknown28 = 0.0f;
	Unknown32 = true;
	Unknown36 = 1.35f;
	Unknown40 = false;
	Unknown41 = false;
	Unknown42 = false;
	Unknown43 = false;
	Unknown44 = false;
}

CReconClassInfo::CReconClassInfo()
{
	iClass = CLASS_RECON;
	Unknown04 = 0;
	fMaxSpeed = 1.0f;
	Unknown12 = 1.3f;
	fDamageScale = 1.485f;
	Unknown20 = true;
	fMaxThermopticNRG = 13.0f;
	Unknown28 = 0.55f;
	Unknown32 = true;
	Unknown36 = 1.35f;
	Unknown40 = false;
	Unknown41 = false;
	Unknown42 = true;
	Unknown43 = false;
	Unknown44 = false;
}

CAssaultClassInfo::CAssaultClassInfo()
{
	iClass = CLASS_ASSAULT;
	Unknown04 = 0;
	fMaxSpeed = 0.8f;
	Unknown12 = 1.0f;
	fDamageScale = 1.2375f;
	Unknown20 = true;
	fMaxThermopticNRG = 8.0f;
	Unknown28 = 0.25f;
	Unknown32 = true;
	Unknown36 = 1.6f;
	Unknown40 = false;
	Unknown41 = false;
	Unknown42 = false;
	Unknown43 = false;
	Unknown44 = true;
}

CSupportClassInfo::CSupportClassInfo()
{
	iClass = CLASS_SUPPORT;
	Unknown04 = 0;
	fMaxSpeed = 0.8f;
	Unknown12 = 0.9f;
	fDamageScale = 0.66f;
	Unknown20 = false;
	fMaxThermopticNRG = 0.0f;
	Unknown28 = 0.0f;
	Unknown32 = false;
	Unknown36 = 1.35f;
	Unknown40 = false;
	Unknown41 = false;
	Unknown42 = false;
	Unknown43 = true;
	Unknown44 = false;
}

COperatorClassInfo::COperatorClassInfo()
{
	iClass = CLASS_OPERATOR;
	Unknown04 = 0;
	fMaxSpeed = 1.0f;
	Unknown12 = 1.0f;
	fDamageScale = 1.1f;
	Unknown20 = false;
	fMaxThermopticNRG = 15.0f;
	Unknown28 = 0.15f;
	Unknown32 = false;
	Unknown36 = 1.35f;
	Unknown40 = false;
	Unknown41 = false;
	Unknown42 = false;
	Unknown43 = false;
	Unknown44 = false;
}

CVipClassInfo::CVipClassInfo()
{
	iClass = CLASS_VIP;
	Unknown04 = 0;
	fMaxSpeed = 1.0f;
	Unknown12 = 1.0f;
	fDamageScale = 1.485f;
	Unknown20 = false;
	fMaxThermopticNRG = 0.0f;
	Unknown28 = 0.0f;
	Unknown32 = false;
	Unknown36 = 1.35f;
	Unknown40 = false;
	Unknown41 = false;
	Unknown42 = false;
	Unknown43 = false;
	Unknown44 = false;
}
