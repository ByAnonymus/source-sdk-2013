//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_yamato.h"
#include "in_buttons.h"

// Client specific.
#ifdef CLIENT_DLL
#include "cam_thirdperson.h"
#include "c_tf_player.h"
#include "econ_entity.h"
// Server specific.
#else
#include "tf_player.h"
#endif
extern ConVar cam_idealpitch;


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
// Weapon Yamato functions.
//

CTFYamato::CTFYamato()
{

}

void CTFYamato::PrimaryAttack(void) 
{
	CTFPlayer* pOwner = ToTFPlayer(GetOwner());

	if (pOwner->m_nButtons & IN_FORWARD)
	{
		DoRapidSlash();
	}
	//m_flNextPrimaryAttack = gpGlobals->curtime;
	BaseClass::PrimaryAttack();
	//pOwner->m_Shared.RemoveCond(TF_COND_SPEED_BOOST);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int	CTFYamato::GetSwingRange( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
		//int iRange = 0;
		//CALL_ATTRIB_HOOK_INT( iRange, is_a_sword )
		//return 72;
		return 72;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFYamato::DoRapidSlash(void)
{
	CTFPlayer* pOwner = ToTFPlayer(GetOwner());
	// Store dash direction
	Vector vecForward;
	AngleVectors(pOwner->EyeAngles(), &vecForward);

#ifndef CLIENT_DLL
	// Set dash speed and duration
	pOwner->m_flNextRapidSlashTime = gpGlobals->curtime + m_flRapidSlashDuration; // 0.3 seconds of movement
	pOwner->ApplyAbsVelocityImpulse(Vector(0, 0, 250)); // Upward impulse
	pOwner->m_vecRapidSlashDir = vecForward * 2400.0f; // Dash speed

	//pOwner->m_Shared.AddCond(TF_COND_SWIMMING_NO_EFFECTS, m_flRapidSlashDuration);

	pOwner->m_Shared.AddCond(TF_COND_CRITBOOSTED_USER_BUFF, m_flRapidSlashDuration);

	pOwner->ApplyAbsVelocityImpulse(pOwner->m_vecRapidSlashDir);


#endif // !CLIENT_DLL
}
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFYamato::Deploy( void )
{
	bool res = BaseClass::Deploy();
	return res;
}
void CTFYamato::WeaponReset(void)
{
	BaseClass::WeaponReset();
#ifdef CLIENT_DLL
	CTFPlayer* pPlayer = GetTFPlayerOwner();
	g_ThirdPersonManager.SetForcedThirdPerson(true);
	Vector offset(150.0f, 25.0f, -10.0f);
	g_ThirdPersonManager.SetDesiredCameraOffset(offset);
	cam_idealpitch.SetValue(0.0f);

	::input->CAM_ToThirdPerson();

	pPlayer->ThirdPersonSwitch(true);
#endif // CLIENT_DLL
}