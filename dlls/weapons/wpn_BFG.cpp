#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "shake.h"
#include "projectiles.h"

class CBFG : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );

	void EXPORT FireSunOfGod( void );

	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void BuySecondaryAmmo( void );
	void SellWeapon( void );
	int SecondaryAmmoIndex( void );
};

int CBFG::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void CBFG::BuyPrimaryAmmo( void )
{
	BuyAmmo(3, (char*)pszAmmo1(), COST_LAUNCHER2_AMMO);
}

void CBFG::BuySecondaryAmmo( void )
{
	BuyAmmo(1, (char*)pszAmmo2(), COST_EXPERIMENTAL_AMMO);
}

void CBFG::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_BFG/2;
}

enum BFG_e
{
	BFG_IDLE1,
	BFG_IDLE2,
	BFG_FIRE,
	BFG_FIRE_SOLID,
	BFG_SPINUP,
	BFG_RELOAD,
	BFG_DRAW,
	BFG_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_bfg, CBFG );

void CBFG::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_BFG.mdl");
	m_iId = WEAPON_BFG;
	m_iDefaultAmmo = 6;
	m_iDefaultAmmo2 = 1;
	pev->weapons = CLIP_BFG;
	FallInit();
}

void CBFG::Precache( void )
{
	PRECACHE_MODEL("models/v_BFG.mdl");
	PRECACHE_MODEL("models/p_BFG.mdl");

	PRECACHE_SOUND("weapons/bfg_fire_sunofgod.wav");
	PRECACHE_SOUND("weapons/bfg_dryfire.wav");
	PRECACHE_SOUND("weapons/sunofgod_burn.wav");
	PRECACHE_SOUND("weapons/sunofgod_blast.wav");
	PRECACHE_SOUND("weapons/BFG_fire.wav");
}

int CBFG::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "BFG";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = "SunOfGod";
	p->iMaxAmmo2 = 999;
	p->iMaxClip = 3;
	p->iSlot = 4;
	p->iPosition = 7;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_BFG;
	p->iWeight = BFG_WEIGHT;
	p->iMaxspeed = BFG_MAXSPEED;
	return 1;
}

BOOL CBFG::Deploy( )
{
	return DefaultDeploy( "models/v_BFG.mdl", "models/p_BFG.mdl", BFG_DRAW, "twohanded" );
}

void CBFG::Holster( void )
{
	if (m_fInAttack != 0)
	{
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_REMOVE );
		m_fInAttack = 0;
	}
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.7;
	SendWeaponAnim( BFG_HOLSTER );
}

void CBFG::PrimaryAttack()
{
	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = gpGlobals->time + 0.15;
		return;
	}

	if (m_fInAttack!=0)
	return;

	UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 400;
		UTIL_ScreenShake( m_pPlayer->pev->origin, 25.0, 500.0, 2, 500.0 );
	}
	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	for (int i=0; i<15; i++)
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, VECTOR_CONE_40DEGREES, 16384, BULLET_BFG, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?BFG_FIRE_SOLID:BFG_FIRE, m_pPlayer->m_fHeavyArmor?1:0, FIREGUN_BFG );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 2.5;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}

void CBFG::FireSunOfGod( void )
{
	if ( m_fInAttack == 0 )
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
		return;
	}

	if (m_pPlayer->pev->waterlevel == 3)
	{
		entvars_t *pevOwner = VARS( pev->owner );
		::RadiusDamage( pev->origin, pev, pevOwner, (dmg_sunofgod.value/4) * (mp_wpn_power.value/100), (dmg_sunofgod.value/4) * (mp_wpn_power.value/100), CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB);
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/displacer_teleportblast.wav", 0.55, ATTN_NORM);
		m_iClip = 0;
		return;
	}

	TraceResult tr;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	UTIL_TraceLine(m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 200, dont_ignore_monsters, ENT(pev), &tr);
	if (tr.flFraction != 1.0)
	{
		PlayEmptySound(3);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Can't create Sun Of God here!"));
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
		return;
	}

	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	SendWeaponAnim((m_pPlayer->m_fHeavyArmor)?BFG_FIRE_SOLID:BFG_FIRE);
	m_iClip = 0;
	m_iFiredAmmo+=3;

	UTIL_ScreenFade( m_pPlayer, Vector(255,255,255), 2.5, 0.5, 128, FFADE_IN );
	UTIL_ScreenShake( m_pPlayer->pev->origin, 40.0, 600.0, 3, 750.0 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 195;
	CSunOfGod::ShootSunOfGod( m_pPlayer->pev, vecSrc);
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 3.0;
}

void CBFG::SecondaryAttack( void )
{
	if ( m_fInAttack == 0)
	{
		if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 2 || m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
	 	{
			PlayEmptySound(4);
			return;
		}

		TraceResult tr;
		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		UTIL_TraceLine(m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 200, dont_ignore_monsters, ENT(pev), &tr);
		if (tr.flFraction != 1.0)
		{
			PlayEmptySound(3);
			ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Can't create Sun Of God here!"));
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
			return;
		}

		m_fInAttack = 1;
		SendWeaponAnim( BFG_SPINUP );
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_BFG_SEC );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5;
	}
	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			WeaponIdle();
			return;
		}

	}
}

void CBFG::Reload( void )
{
	DefaultReload( 3, BFG_RELOAD, 4.3, 1.8);
}

void CBFG::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if (m_fInAttack==1)
	{
		UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
		FireSunOfGod();
		m_fInAttack = 0;
		return;
	}

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = BFG_IDLE1;	
		break;
	
	default:
	case 1:
		iAnim = BFG_IDLE2;
		break;
	}
	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}