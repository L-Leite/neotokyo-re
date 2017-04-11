#include "cbase.h"
#include "weapon_ghost.h"

#if defined( CLIENT_DLL )

	#include "c_neo_player.h"

#else

	#include "neo_player.h"

#endif

CUtlVector< CWeaponGhost* > g_Ghosts;


IMPLEMENT_NETWORKCLASS_ALIASED( WeaponGhost, DT_WeaponGhost )

BEGIN_NETWORK_TABLE( CWeaponGhost, DT_WeaponGhost )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponGhost )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_ghost, CWeaponGhost );
PRECACHE_WEAPON_REGISTER( weapon_ghost );

/*#ifdef GAME_DLL				   
	BEGIN_DATADESC( CWeaponGhost )		  
		DEFINE_FUNCTION( DefaultTouch )	   
	END_DATADESC()		 
#endif*/


CWeaponGhost* GetGhost( int iIndex )
{
	if ( iIndex < 0 || iIndex >= GetNumberOfGhosts() )
		return nullptr;

	return g_Ghosts[ iIndex ];
}

CWeaponGhost* GetGhost()
{
	return GetGhost( 0 );
}

int GetNumberOfGhosts()
{
	return g_Ghosts.Size();
}

#ifdef CLIENT_DLL
CON_COMMAND_F( cl_listghosts, "List ghosts.", FCVAR_CHEAT )
{
	DevMsg( "Client list of ghosts:\n" );

	if ( g_Ghosts.IsEmpty() )
	{
		Warning( "\tNo ghosts found.\n" );
		return;
	}

	for ( int i = 0; i < g_Ghosts.Size(); i++ )
	{
		CWeaponGhost* pGhost = g_Ghosts[ i ];
		Vector vOrigin = pGhost->GetAbsOrigin();
		DevMsg( "\tGhost %i: pos %f %f %f\n", i, vOrigin.x, vOrigin.y, vOrigin.z );
	}
}
#elif GAME_DLL
CON_COMMAND_F( sv_listghosts, "List ghosts.", FCVAR_CHEAT )
{
	DevMsg( "Server list of ghosts:\n" );

	if ( g_Ghosts.IsEmpty() )
	{
		Warning( "\tNo ghosts found.\n" );
		return;
	}

	for ( int i = 0; i < g_Ghosts.Size(); i++ )
	{
		CWeaponGhost* pGhost = g_Ghosts[ i ];
		Vector vOrigin = pGhost->GetAbsOrigin();
		DevMsg( "\tGhost %i: pos %f %f %f\n", i, vOrigin.x, vOrigin.y, vOrigin.z );
	}
}
#endif

CWeaponGhost::CWeaponGhost()
{
	g_Ghosts.AddToTail( this );
}

#ifdef GAME_DLL
int CWeaponGhost::ShouldTransmit( const CCheckTransmitInfo *pInfo )
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}

int CWeaponGhost::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}

/*void CWeaponGhost::SetPickupTouch()
{
	SetTouch( &CWeaponGhost::DefaultTouch );
}

void CWeaponGhost::DefaultTouch( CBaseEntity *pOther )
{
	BaseClass::DefaultTouch( pOther );
}*/

void CWeaponGhost::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CNEOPlayer* pPlayer = (CNEOPlayer*) pCaller;

	if ( pPlayer && pPlayer->IsPlayer() )
	{
		if ( pPlayer->BumpWeapon( this ) )
		{
			pPlayer->FreeWeaponSlot();

			if ( !pPlayer->BumpWeapon( this ) )
				return;

			OnPickedUp( pPlayer );
		}
		else
			pPlayer->PickupObject( this );
	}
}
#endif

void CWeaponGhost::WeaponIdle()
{
	if ( HasWeaponIdleTimeElapsed() )
	{
		SendWeaponAnim( ACT_VM_IDLE );	
		SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );
	}		 	
}
