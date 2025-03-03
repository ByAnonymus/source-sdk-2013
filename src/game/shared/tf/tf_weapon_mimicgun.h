//========= Copyright Valve Corporation, All rights reserved. ============//
//
//
//=============================================================================
#ifndef TF_WEAPON_MIMICGUN_H
#define TF_WEAPON_MIMICGUN_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFMimicgun C_TFMimicgun
#endif

//=============================================================================
//
// TF Weapon Mimicgun.
//
class CTFMimicgun : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS(CTFMimicgun, CTFWeaponBaseGun);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CTFMimicgun();
	~CTFMimicgun() {}

	virtual int		GetWeaponID(void) const { return TF_WEAPON_MIMICGUN; }

	virtual void	ItemPostFrame(void);
	virtual void	PrimaryAttack(void);
	virtual void	SecondaryAttack(void);

	virtual void	FakeReload(void);

	virtual bool		Holster(CBaseCombatWeapon* pSwitchingTo = NULL);
	virtual bool		Deploy(void);

	float				GetProgress(void) { return 0.f; }
	bool				m_bAttack2;
	bool				m_bReloadDown;
	bool				m_bReloading;


private:

	CTFMimicgun(const CTFMimicgun&) {}

	float			m_flLastAccuracyCheck;
	float			m_flAccuracyCheckTime;
};

#endif // TF_WEAPON_MIMICGUN_H
