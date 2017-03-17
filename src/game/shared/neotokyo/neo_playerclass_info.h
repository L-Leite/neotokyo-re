#ifndef NEO_PLAYERCLASS_INFO_H
#define NEO_PLAYERCLASS_INFO_H
#ifdef _WIN32
#pragma once
#endif						


class CPlayerClassInfo
{
public:
	bool idk( int i );

public:
	int iClass;
	int Unknown04;
	float fMaxSpeed;
	float Unknown12;
	float fDamageScale;
	bool Unknown20;
	float fMaxThermopticNRG;
	float Unknown28;
	bool Unknown32;
	float Unknown36;
	bool Unknown40;
	bool Unknown41;
	bool Unknown42;
	bool Unknown43;
	bool Unknown44;
};

class CDefaultClassInfo : public CPlayerClassInfo
{
public:
	CDefaultClassInfo();
};

class CReconClassInfo : public CPlayerClassInfo
{
public:
	CReconClassInfo();
};

class CAssaultClassInfo : public CPlayerClassInfo
{
public:
	CAssaultClassInfo();
};

class CSupportClassInfo : public CPlayerClassInfo
{
public:
	CSupportClassInfo();
};

class COperatorClassInfo : public CPlayerClassInfo
{
public:
	COperatorClassInfo();
};

class CVipClassInfo : public CPlayerClassInfo
{
public:
	CVipClassInfo();
};

#endif // NEO_PLAYERCLASS_INFO_H
