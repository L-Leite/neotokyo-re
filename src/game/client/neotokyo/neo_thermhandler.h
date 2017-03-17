#ifndef NEO_THERMHANDLER
#define NEO_THERMHANDLER
#ifdef _WIN32
#pragma once
#endif

class IMaterial;


class CThermopticHandler
{
public:
	CThermopticHandler();

public:
	IMaterial* GetThermopticMaterial();

	void UpdateThermopticMaterial( float fNewTransparency );
	
private:
	bool CreateThermopticMaterial();

private:
	IMaterial* m_pThermMaterial;
	float m_fTransparency;		
	bool m_bTriedToInitMat;	 	
};

extern CThermopticHandler g_ThermopticHandler;


#endif // NEO_THERMHANDLER