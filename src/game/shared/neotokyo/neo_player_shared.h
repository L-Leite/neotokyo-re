//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Shared Player Variables / Functions and variables that may or may not be networked
//
//===========================================================================================//

#ifndef NEO_PLAYER_SHARED_H
#define NEO_PLAYER_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "networkvar.h"
#include "weapon_neobase.h"

#ifdef CLIENT_DLL
class C_NEOPlayer;
#else
class CNEOPlayer;
#endif

class CNEOPlayerShared
{
public:
#ifdef CLIENT_DLL
	friend class C_NEOPlayer;
	typedef C_NEOPlayer OuterClass;
	DECLARE_PREDICTABLE();
#else
	friend class CNEOPlayer;
	typedef CNEOPlayer OuterClass;
#endif

	DECLARE_EMBEDDED_NETWORKVAR()
	DECLARE_CLASS_NOBASE( CNEOPlayerShared );

	CNEOPlayerShared();
	~CNEOPlayerShared();

#if defined ( NEO_USE_STAMINA ) || defined ( NEO_USE_SPRINTING )
	void	SetStamina( float stamina );
	float	GetStamina( void ) { return m_flStamina; }
#endif // NEO_USE_STAMINA || NEO_USE_SPRINTING

	void	Init( OuterClass *pOuter );

	bool	IsSniperZoomed( void ) const;
	bool	IsDucking( void ) const; 

#if defined ( NEO_USE_PLAYERCLASSES )
	void	SetDesiredPlayerClass( int playerclass );
	int		DesiredPlayerClass( void );

	void	SetPlayerClass( int playerclass );
	int		PlayerClass( void );
#endif

	CWeaponNEOBase* GetActiveNEOWeapon() const;

#if defined ( NEO_USE_PRONE )
	void	StartGoingProne( void );
	void	StandUpFromProne( void );
	bool	IsProne() const;
	bool	IsGettingUpFromProne() const;	
	bool	IsGoingProne() const;
	void	SetProne( bool bProne, bool bNoAnimation = false );
	bool	CanChangePosition( void );
#endif

	bool	IsJumping( void ) { return m_bJumping; }
	void	SetJumping( bool bJumping );

	void	ForceUnzoom( void );

#ifdef NEO_USE_SPRINTING
	bool	IsSprinting( void ) { return m_bIsSprinting; }

	void	SetSprinting( bool bSprinting );
	void	StartSprinting( void );
	void	StopSprinting( void );

	void ResetSprintPenalty( void );
#endif

	void ComputeWorldSpaceSurroundingBox( Vector *pVecWorldMins, Vector *pVecWorldMaxs );

private:

#if defined ( NEO_USE_PRONE )
	CNetworkVar( bool, m_bProne );
#endif

#if defined ( NEO_USE_PLAYERCLASSES )
	CNetworkVar( int, m_iPlayerClass );
	CNetworkVar( int, m_iDesiredPlayerClass );
#endif


#if defined ( NEO_USE_SPRINTING )
	CNetworkVar( bool, m_bIsSprinting );
	bool m_bGaveSprintPenalty;
#endif

#if defined ( NEO_USE_STAMINA ) || defined ( NEO_USE_SPRINTING )
	CNetworkVar( float, m_flStamina );
#endif // NEO_USE_STAMINA || NEO_USE_SPRINTING

public:

#ifdef NEO_USE_PRONE
	float m_flNextProneCheck; // Prevent it switching their prone state constantly.

	CNetworkVar( float, m_flUnProneTime );
	CNetworkVar( float, m_flGoProneTime );
	CNetworkVar( bool, m_bForceProneChange );
#endif

	bool m_bJumping;

	float m_flLastViewAnimationTime;

	//Tony; player speeds; at spawn server and client update both of these based on class (if any)
	float m_flRunSpeed;
	float m_flSprintSpeed;
	float m_flProneSpeed;

private:

	OuterClass *m_pOuter;
};			   




#endif //NEO_PLAYER_SHARED_H