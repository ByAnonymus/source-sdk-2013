//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_yamato.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include "econ_entity.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Sword tables.
//

// CTFYamato
IMPLEMENT_NETWORKCLASS_ALIASED( TFYamato, DT_TFWeaponYamato )

BEGIN_NETWORK_TABLE( CTFYamato, DT_TFWeaponYamato )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFYamato )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_yamato, CTFYamato );
PRECACHE_WEAPON_REGISTER( tf_weapon_yamato );
//=============================================================================
//
// Weapon Sword functions.
//

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFYamato::WeaponReset( void )
{
	BaseClass::WeaponReset();

#ifdef CLIENT_DLL
	m_flNextIdleWavRoll = 0;
	m_iPrevWavDecap = 0;
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int	CTFYamato::GetSwingRange( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	if ( pOwner && pOwner->m_Shared.InCond( TF_COND_SHIELD_CHARGE ) )
	{
		return 128;
	}
	else
	{
		//int iRange = 0;
		//CALL_ATTRIB_HOOK_INT( iRange, is_a_sword )
		//return 72;
		return 72;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFYamato::Deploy( void )
{
	bool res = BaseClass::Deploy();

#ifdef GAME_DLL
	if ( res )
#else
	// When we go active, there's a chance we immediately thirst for heads.
	if ( RandomInt( 1,4 ) == 1 )
	{
		m_flNextIdleWavRoll = gpGlobals->curtime + 3.0;
	}
	else
	{
		m_flNextIdleWavRoll = gpGlobals->curtime + RandomFloat( 5.0, 30.0 );
	}
#endif

	return res;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFYamato::GetCount( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	if ( !pOwner )
		return 0;

	return pOwner->m_Shared.GetDecapitations();
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFYamato::WeaponIdle( void )
{
	BaseClass::WeaponIdle();

	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	if ( !pOwner )
		return;

	// If we've decapped someone recently, we roll shortly afterwards
	int iDecaps = pOwner->m_Shared.GetDecapitations();
	if ( m_iPrevWavDecap < iDecaps )
	{
		 m_flNextIdleWavRoll = MIN( m_flNextIdleWavRoll, gpGlobals->curtime + RandomFloat(3,5) );
	}
	m_iPrevWavDecap = iDecaps;

	// Randomly play sounds to the local player. The more decaps we have, the more chance it's a good wav.
	if ( m_flNextIdleWavRoll < gpGlobals->curtime )
	{
		float flChance = RemapValClamped( iDecaps, 0, 10, 0.25, 0.9 );
		if ( RandomFloat() <= flChance )
		{
			// Chance to get the more powerful wav: 
			float flChanceForGoodWav = RemapValClamped( iDecaps, 0, 10, 0.1, 0.75 );
			if ( RandomFloat() <= flChanceForGoodWav )
			{
				WeaponSound( SPECIAL2 );
			}
			else
			{
				WeaponSound( SPECIAL1 );
			}
		}

		m_flNextIdleWavRoll = gpGlobals->curtime + RandomFloat( 30.0, 60.0 );
	}
}

#endif