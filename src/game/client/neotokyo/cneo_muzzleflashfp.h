#ifndef NEO_MUZZLEFLASHFP_H
#define NEO_MUZZLEFLASHFP_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseanimating.h"
#include "neo_viewmodel.h"


class CIdk
{
public:
	bool m_Unknown00;
	float m_Unknown04;
	float m_Unknown08;
	float m_Unknown12;
	float m_Unknown16;
	int m_Unknown20;
	float m_Unknown24;
	int m_Unknown28;
};

class CNEO_MuzzleflashFP : public C_BaseAnimating
{	 
public:
	DECLARE_CLASS( CNEO_MuzzleflashFP, C_BaseAnimating );

	CNEO_MuzzleflashFP();

	virtual RenderGroup_t GetRenderGroup();

	virtual void CreateMuzzleFlashModel( CNeoViewModel* pViewModel, int iAttachment );	   
	virtual void DrawMuzzleFlash();
	virtual void something();
	virtual void something2( float a2, float a3, float a4, int a5 );
	virtual void something3();

	virtual void SetAttachment( int attachment );

	virtual void SetUnknown1784( bool bSet );
	virtual bool GetUnknown1784();

private:
	Vector m_vecSomething;
	QAngle m_angSomething;

	bool m_Unknown1784;

	const model_t* m_pMuzzleFlashModel;
	int m_iAttachment;

	CIdk m_Idks[ 5 ];

	int m_iMfMaterialIndex;
};	
				


#endif // NEO_MUZZLEFLASHFP_H