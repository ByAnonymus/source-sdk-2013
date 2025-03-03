#include "cbase.h"
#include "tf_weapon_mimicgun.h"
#include "tf_fx_shared.h"
#include "in_buttons.h"
#include "datamap.h"
#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Mimicgun tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED(TFMimicgun, DT_WeaponMimicgun)

BEGIN_NETWORK_TABLE(CTFMimicgun, DT_WeaponMimicgun)
END_NETWORK_TABLE()
#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CTFMimicgun)
DEFINE_PRED_FIELD(m_flLastAccuracyCheck, FIELD_FLOAT, 0),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS(tf_weapon_mimicgun, CTFMimicgun);
PRECACHE_WEAPON_REGISTER(tf_weapon_mimicgun);

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC(CTFMimicgun)
END_DATADESC()
#endif


//=============================================================================
//
// Weapon Mimicgun functions.
//

CTFMimicgun::CTFMimicgun()
{
	m_flLastAccuracyCheck = 0.f;
	m_flAccuracyCheckTime = 0.f;
	m_bAttack2 = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFMimicgun::DefaultReload(int iClipSize1, int iClipSize2, int iActivity)
{
	// The the owning local player.
	CTFPlayer* pPlayer = GetTFPlayerOwner();
	if (!pPlayer)
		return false;

	if (pPlayer->IsPlayerClass(TF_CLASS_SPY))
	{
		if (pPlayer->m_Shared.InCond(TF_COND_STEALTHED))
		{
			return false;
		}
	}

	if (pPlayer->m_Shared.IsFeignDeathReady())
		return false; // Can't reload if our feign death arm is up.

	return BaseClass::DefaultReload(iClipSize1, iClipSize2, iActivity);

}

//-----------------------------------------------------------------------------
// Purpose: Primary fire.
//-----------------------------------------------------------------------------
void CTFMimicgun::PrimaryAttack(void)
{
	// Check for ammunition.
	if (m_iClip1 <= 0 && m_iClip1 != -1)
		return;

	// Are we capable of firing again?
	if (m_flNextPrimaryAttack > gpGlobals->curtime)
		return;

	// Get the player owning the weapon.
	CTFPlayer* pPlayer = ToTFPlayer(GetPlayerOwner());
	if (!pPlayer)
		return;

	if (!CanAttack())
		return;

	BaseClass::PrimaryAttack();
}

//-----------------------------------------------------------------------------
// Purpose: Secondary fire. (Mimic deployed animations if the class we're disguised as has them)
//-----------------------------------------------------------------------------
void CTFMimicgun::SecondaryAttack(void)
{
	CTFPlayer* pPlayer = ToTFPlayer(GetOwner());
	m_bAttack2 = !m_bAttack2;
	if (m_flNextSecondaryAttack > gpGlobals->curtime)
		return;
	if ((pPlayer->m_Shared.InCond( TF_COND_DISGUISED )) && ((pPlayer->m_Shared.GetDisguiseClass() == TF_CLASS_HEAVYWEAPONS) || (pPlayer->m_Shared.GetDisguiseClass() == TF_CLASS_SNIPER)) && (pPlayer->m_Shared.GetDisguiseWeapon()->GetSlot() == 0)) {
		if (m_bAttack2) {
			pPlayer->m_Shared.AddCond( TF_COND_AIMING );
			pPlayer->TeamFortress_SetSpeed();
		}

		else {
			pPlayer->m_Shared.RemoveCond( TF_COND_AIMING );
			pPlayer->TeamFortress_SetSpeed();
		}
		BaseClass::SecondaryAttack();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFMimicgun::Holster(CBaseCombatWeapon* pSwitchingTo)
{
	return BaseClass::Holster(pSwitchingTo);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFMimicgun::Deploy(void)
{
	return BaseClass::Deploy();
}
#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose: Reset revenge crits when the revolver is changed
//-----------------------------------------------------------------------------
void CTFMimicgun::Detach(void)
{
	BaseClass::Detach();
}
#endif

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
/*void CTFMimicgun::ItemPostFrame(void)
{
	CTFPlayer* pPlayer = ToTFPlayer(GetOwner());
	// Fire.
	if (pPlayer->m_nButtons & IN_ATTACK)
	{
		Fire();
	}

	// Idle.
	//if (!((pPlayer->m_nButtons & IN_ATTACK) || (pPlayer->m_nButtons & IN_ATTACK2)))
	//{
	//	// No fire buttons down or reloading
	//	if (!ReloadOrSwitchWeapons() && (m_bInReload == false))
	//	{
	//		WeaponIdle();
	//	}
	//}
	if (pPlayer->m_nButtons & IN_ATTACK2)
	{
		Fire2();
	}
}*/