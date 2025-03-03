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
	WeaponReset();
	m_flLastAccuracyCheck = 0.f;
	m_flAccuracyCheckTime = 0.f;
	m_bAttack2 = false;
	m_bReloadDown = false;
	m_bReloading = false;

}

//-----------------------------------------------------------------------------
// Purpose: Primary fire.
//-----------------------------------------------------------------------------
void CTFMimicgun::PrimaryAttack(void)
{
	// Get the player owning the weapon.
	CTFPlayer* pPlayer = ToTFPlayer(GetPlayerOwner());
	if (!pPlayer)
		return;

	// Are we capable of firing again?
	if (m_flNextPrimaryAttack > gpGlobals->curtime)
		return;

	if (!CanAttack())
		return;

	BaseClass::PrimaryAttack();
}

//-----------------------------------------------------------------------------
// Purpose: Secondary fire. 
// (Mimic deployed animations if the class we're disguised as has them)
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

void CTFMimicgun::FakeReload(void)
{
    // Play first-person reload animation
    SendWeaponAnim(ACT_VM_RELOAD);

    // Get the player who owns this weapon
    CBasePlayer* pOwner = ToBasePlayer(GetOwner());
    CTFPlayer* pPlayer = ToTFPlayer(GetOwner());
    if (pOwner && pOwner->IsPlayer())
    {
        // Force third-person reload animation for other players
        pOwner->SetAnimation(PLAYER_RELOAD);
        pPlayer->DoAnimationEvent(PLAYERANIMEVENT_RELOAD);
		//float flReloadTime = SequenceDuration();
		//m_flNextSecondaryAttack = gpGlobals->curtime + flReloadTime;
		//if (m_flNextSecondaryAttack == gpGlobals->curtime) pPlayer->DoAnimationEvent(PLAYERANIMEVENT_RELOAD_END);
    }

    // Set reloading state
    m_bReloading = true;
}

void CTFMimicgun::ItemPostFrame(void)
{
    CBasePlayer* pOwner = ToBasePlayer(GetOwner());

    if (!pOwner)
        return;

    // Detect fresh RELOAD button press (mimicking Medigun)
    if (pOwner->m_nButtons & IN_RELOAD && !m_bReloadDown)
    {
        FakeReload();
        m_bReloadDown = true;
    }
    else if (!(pOwner->m_nButtons & IN_RELOAD) && m_bReloadDown)
    {
        m_bReloadDown = false;
    }


    BaseClass::ItemPostFrame();
}
