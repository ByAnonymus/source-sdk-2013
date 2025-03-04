#include "cbase.h"
#include "tf_weapon_mimicgun.h"
#include "tf_fx_shared.h"
#include "in_buttons.h"
#include "datamap.h"
#include "tf_weaponbase_gun.h"
#include "tf_weaponbase_melee.h"

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

	// Check if we can attack.
	if (!(gpGlobals->curtime>m_flNextPrimaryAttack))
		return;

	// Play first-person attack animation
	SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	 //Get the player who owns this weapon
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());
	if (pOwner && pOwner->IsPlayer())
	{
		// Force third-person attack animation for other players
		pOwner->SetAnimation(PLAYER_ATTACK1);
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
	}
	float flFireDelay = ApplyFireDelay(m_pWeaponInfo->GetWeaponData(m_iWeaponMode).m_flTimeFireDelay);
	int iProjectile = GetWeaponProjectileType();
	CBaseCombatWeapon* pDisguiseWeapon = pPlayer->m_Shared.GetDisguiseWeapon();
	if (pDisguiseWeapon)
	{
		// Cast to CTFWeaponBase to access TF2-specific functions
		CTFWeaponBase* pTFWeapon = dynamic_cast<CTFWeaponBase*>(pDisguiseWeapon);

		if (pTFWeapon)
		{
			const CTFWeaponInfo& weaponInfo = pTFWeapon->GetTFWpnData();

			// Use the getter function instead of directly accessing m_iWeaponMode
			int iWeaponMode = pTFWeapon->GetWeaponMode();

			// Apply fire delay
			flFireDelay = ApplyFireDelay(weaponInfo.GetWeaponData(iWeaponMode).m_flTimeFireDelay);
			
			//Find Disguise Weapon Projectile Type
			iProjectile = weaponInfo.GetWeaponData(iWeaponMode).m_iProjectile;
		}
	}
	//Msg("Fire Delay: %f\n", flFireDelay);
	//BaseClass::PrimaryAttack();
	// Set the next attack time.
	FireDisguisedProjectile(pPlayer, iProjectile);
	m_flNextPrimaryAttack = gpGlobals->curtime + flFireDelay;
}
CBaseEntity *CTFMimicgun::FireDisguisedProjectile(CTFPlayer* pPlayer, int iProjectile )
{
	CBaseEntity* pProjectile = NULL;

	// Anyone ever hear of a factory? This is a disgrace.
	switch (iProjectile)
	{
	case TF_PROJECTILE_BULLET:
		FireBullet(pPlayer);
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
		break;

	case TF_PROJECTILE_ROCKET:
		pProjectile = FireRocket(pPlayer, iProjectile);
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
		break;

	case TF_PROJECTILE_SYRINGE:
		pProjectile = FireNail(pPlayer, iProjectile);
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
		break;

	case TF_PROJECTILE_FLARE:
		pProjectile = FireFlare(pPlayer);
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
		break;

	case TF_PROJECTILE_PIPEBOMB:
	case TF_PROJECTILE_PIPEBOMB_REMOTE:
	case TF_PROJECTILE_PIPEBOMB_PRACTICE:
	case TF_PROJECTILE_CANNONBALL:
		pProjectile = FirePipeBomb(pPlayer, iProjectile);
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
		break;

	case TF_PROJECTILE_JAR:
	case TF_PROJECTILE_JAR_MILK:
	case TF_PROJECTILE_CLEAVER:
	case TF_PROJECTILE_THROWABLE:
	case TF_PROJECTILE_FESTIVE_JAR:
	case TF_PROJECTILE_BREADMONSTER_JARATE:
	case TF_PROJECTILE_BREADMONSTER_MADMILK:
	case TF_PROJECTILE_JAR_GAS:
		pProjectile = FireJar(pPlayer);
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
		break;
	case TF_PROJECTILE_ARROW:
	case TF_PROJECTILE_HEALING_BOLT:
	case TF_PROJECTILE_BUILDING_REPAIR_BOLT:
	case TF_PROJECTILE_FESTIVE_ARROW:
	case TF_PROJECTILE_FESTIVE_HEALING_BOLT:
	case TF_PROJECTILE_GRAPPLINGHOOK:
		pProjectile = FireArrow(pPlayer, ProjectileType_t(iProjectile));
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
		break;

	case TF_PROJECTILE_FLAME_ROCKET:
		pProjectile = FireFlameRocket(pPlayer);
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_SECONDARY);
		break;

	case TF_PROJECTILE_ENERGY_BALL:
		pProjectile = FireEnergyBall(pPlayer);
		if (ShouldPlayFireAnim())
		{
			pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
		}
		break;

	case TF_PROJECTILE_ENERGY_RING:
		pProjectile = FireEnergyBall(pPlayer, true);
		if (ShouldPlayFireAnim())
		{
			pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
		}
		break;

	case TF_PROJECTILE_NONE:
	default:
		// do nothing!
		DevMsg("Weapon does not have a projectile type set\n");
		break;
	}


	m_flLastFireTime = gpGlobals->curtime;
	m_iConsecutiveShots++;

	DoFireEffects();

	UpdatePunchAngles(pPlayer);

	return pProjectile;
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
	if (pOwner->m_nButtons & IN_ATTACK)
	{
		PrimaryAttack();
	}
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
//-----------------------------------------------------------------------------
// Purpose: Fire a bullet!
//-----------------------------------------------------------------------------
void CTFMimicgun::FireBullet(CTFPlayer* pPlayer)
{
	int iWeaponID = GetWeaponID();
	int iWeaponMode = m_iWeaponMode;
	float flWeaponSpread = GetWeaponSpread();
	float flDamage = GetProjectileDamage(); // Default to mimic gun's damage
	CBaseCombatWeapon* pDisguiseWeapon = pPlayer->m_Shared.GetDisguiseWeapon();

	CTFWeaponBase* pTFWeapon = nullptr;
	if (pDisguiseWeapon)
	{
		pTFWeapon = dynamic_cast<CTFWeaponBase*>(pDisguiseWeapon);
		if (pTFWeapon)
		{
			iWeaponMode = pTFWeapon->GetWeaponMode();
			iWeaponID = pTFWeapon->GetWeaponID();
			//pDisguiseWeapon->MyCombatWeaponPointer()->Spread; // Corrected: No parameter
			DevMsg("Weapon Alias: %s\n", WeaponIdToAlias(iWeaponID));
		}
	}

	// Play the correct shoot sound
	if (pTFWeapon)
	{
		//pTFWeapon->WeaponSound("Weapon.Shoot");
	}
	else
	{
		PlayWeaponShootSound();
	}

	FX_FireBullets(
		this,
		pPlayer->entindex(),
		pPlayer->Weapon_ShootPosition(),
		pPlayer->EyeAngles() + pPlayer->GetPunchAngle(),
		iWeaponID,
		iWeaponMode,
		CBaseEntity::GetPredictionRandomSeed(UseServerRandomSeed()) & 255,
		flWeaponSpread,
		flDamage, // Use the updated damage value
		IsCurrentAttackACrit()
	);
}