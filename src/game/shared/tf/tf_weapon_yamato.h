//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_YAMATO_H
#define TF_WEAPON_YAMATO_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"
#ifdef GAME_DLL
#include "GameEventListener.h"
#else
#include "tf_viewmodel.h"
#include "bone_setup.h"
#include "tf_wearable_weapons.h"
#include "tf_weapon_sword.h"
#endif

#ifdef CLIENT_DLL
#define CTFYamato C_TFYamato
#endif

//=============================================================================
//
// Sword class. (Consumes heads for speed/health bonus.)
//
class CTFYamato : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFYamato, CTFWeaponBaseMelee);
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFYamato();
	virtual ~CTFYamato() {}

	virtual void		PrimaryAttack(void);
	virtual int			GetSwingRange( void );
	virtual void		DoRapidSlash( void );
	void				WeaponReset(void);
	
	virtual bool		Deploy( void );

	float				GetProgress( void ) { return 0.f; }
	const char*			GetEffectLabelText( void ) { return "#TF_BERZERK"; }
	float		m_flRapidSlashDuration = 0.5f;

private:

	CTFYamato( const CTFYamato& ) {}
};
#endif // TF_WEAPON_YAMATO_H
