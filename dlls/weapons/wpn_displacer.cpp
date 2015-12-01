#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "shake.h"
#include "projectiles.h"

enum displacer_e 
{
	DISPLACER_IDLE1,
	DISPLACER_IDLE2,
	DISPLACER_SPINUP,
	DISPLACER_SPIN,
	DISPLACER_FIRE,
	DISPLACER_FIRE_SOLID,
	DISPLACER_RELOAD,
	DISPLACER_DRAW,
	DISPLACER_HOLSTER
};

class CDisplacer : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	void Reload( void );
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy( void );
	void Holster( void );

	void EXPORT Teleport( void );
	void EXPORT FireBall( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );
	void SwitchFireMode( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};
LINK_ENTITY_TO_CLASS( weapon_displacer, CDisplacer );

void CDisplacer::BuyPrimaryAmmo( void )
{
	BuyAmmo(60, (char*)pszAmmo1(), COST_ENERGY2_AMMO);
}

void CDisplacer::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_DISPLACER/2;
}

void CDisplacer::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_displacer.mdl");
	m_iId = WEAPON_DISPLACER;
	firemode=FIREMODE_HALF;
	m_iDefaultAmmo = 120;
	pev->weapons = CLIP_DISPLACER;
	FallInit();
}

void CDisplacer::Precache( void )
{
	PRECACHE_MODEL("models/v_displacer.mdl");
	PRECACHE_MODEL("models/p_displacer.mdl");

	PRECACHE_SOUND("weapons/displacer_fire.wav");
	PRECACHE_SOUND("weapons/displacer_spin.wav");
	PRECACHE_SOUND("weapons/displacer_teleport.wav");
	PRECACHE_SOUND("weapons/displacer_teleportblast.wav");
}

BOOL CDisplacer::Deploy( void )
{
	m_iLastFiremode = -1;
	return DefaultDeploy( "models/v_displacer.mdl", "models/p_displacer.mdl", DISPLACER_DRAW, "gauss" );
}

void CDisplacer::Holster( void )
{
	if (m_fInAttack > 0)
	{
		m_fInAttack = 0;
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 2, FIREGUN_DISPLACER );
	}
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.7;
	SendWeaponAnim( DISPLACER_HOLSTER );

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();
}

int CDisplacer::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "isotope";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 60;
	p->iSlot = 6;
	p->iPosition = 8;
	p->iId = WEAPON_DISPLACER;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = DISPLACER_WEIGHT;
	p->iMaxspeed = DISPLACER_MAXSPEED;
	return 1;
}

void CDisplacer::SwitchFireMode( void )
{
	if(firemode==FIREMODE_FULL)
	{
		firemode=FIREMODE_HALF;
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.2;
	}
	else
	{
		firemode=FIREMODE_FULL;
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.2;
	}
}

void CDisplacer::FireBall()
{
	if ( m_fInAttack == 0 )
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
		return;
	}

	if (m_pPlayer->pev->waterlevel == 3)
	{
		entvars_t *pevOwner = VARS( pev->owner );
		::RadiusDamage( pev->origin, pev, pevOwner, (dmg_displacer.value/2) * (mp_wpn_power.value/100), (dmg_displacer.value/2) * (mp_wpn_power.value/100), CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB );
		FX_Explosion( pev->origin, EXPLOSION_ENERGY_INWATER_L);
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/displacer_teleportblast.wav", 0.55, ATTN_LOW_HIGH);
		m_iClip = 0;
		return;
	}
	if (m_iClip <= 9)
	{
		PlayEmptySound(1);
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}
	UTIL_ScreenFade( m_pPlayer, Vector(0,200,0), 2.5, 0.5, 128, FFADE_IN );
	FX_FireGun(m_pPlayer->pev->angles, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?DISPLACER_FIRE_SOLID:DISPLACER_FIRE, 0, FIREGUN_DISPLACER);
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 32 + gpGlobals->v_right * 4 + gpGlobals->v_up * -6;

	if(firemode==FIREMODE_FULL && m_iClip > 19)
	{
		m_iClip -= 20;
		m_iFiredAmmo+=20;
		CDispball::ShootDispball( m_pPlayer->pev, vecSrc, gpGlobals->v_forward * 1500, 2);
	}
	else
	{
		m_iClip -= 10;
		m_iFiredAmmo+=10;
		CDispball::ShootDispball( m_pPlayer->pev, vecSrc, gpGlobals->v_forward * 3000, 1);
	}
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 3.0;
}

void CDisplacer:: Teleport ( void )
{	
	if (m_fInAttack == 0)
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
		return;
	}
	edict_t *pSpot = EntSelectSpawnPoint(m_pPlayer);

	if (pSpot)
	{
		UTIL_ScreenFade( m_pPlayer, Vector(0, 200, 0), 0.5, 0.5, 255, FFADE_IN );
		CTeleenter::ShootTeleenter( m_pPlayer->pev, pev->origin);

		m_iClip -= 10;
		m_iFiredAmmo+=10;
		Vector tmp = pSpot->v.origin;
		tmp.z -= m_pPlayer->pev->mins.z;
		tmp.z++;
		UTIL_SetOrigin( m_pPlayer->pev, tmp );

		m_pPlayer->pev->angles = pSpot->v.angles;
		m_pPlayer->pev->velocity = m_pPlayer->pev->basevelocity = g_vecZero;
		FX_Explosion( tmp, EXPLOSION_DISPTELEPORT );

		SendWeaponAnim((m_pPlayer->m_fHeavyArmor)?DISPLACER_FIRE_SOLID:DISPLACER_FIRE);
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 3.0;
	}
}

void CDisplacer::PrimaryAttack( void )
{
	if (m_iClip <= 9)
	{
		PlayEmptySound(1);
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	if ( m_fInAttack == 0)
	{
		if (m_pPlayer->pev->waterlevel == 3)
		{
			entvars_t *pevOwner = VARS( pev->owner );
			::RadiusDamage( pev->origin, pev, pevOwner, (dmg_displacer.value/2) * (mp_wpn_power.value/100), (dmg_displacer.value/2) * (mp_wpn_power.value/100), CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB );
			FX_Explosion( pev->origin, EXPLOSION_ENERGY_INWATER_L);
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/displacer_teleportblast.wav", 0.55, ATTN_LOW_HIGH);
			m_iClip = 0;
			return;
		}

		m_fInAttack = 1;
		FX_FireGun(m_pPlayer->pev->angles, m_pPlayer->entindex(), DISPLACER_SPINUP, 1, FIREGUN_DISPLACER);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.3;
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

void CDisplacer::SecondaryAttack( void )
{
	if ( m_fInAttack == 0)
	{
		if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 9)
		{
			PlayEmptySound(1);
			m_flNextSecondaryAttack = gpGlobals->time + 0.5;
			return;
		}

		m_fInAttack = 2;
		FX_FireGun(m_pPlayer->pev->angles, m_pPlayer->entindex(), DISPLACER_SPINUP, 1, FIREGUN_DISPLACER);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.3;
	}
	else if (m_fInAttack == 2)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			WeaponIdle();
			return;
		}

	}
}

void CDisplacer::WeaponIdle( void )
{
	if( m_iLastFiremode != firemode)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_FIREMODE);
		WRITE_BYTE(firemode);
		WRITE_ELSE(1+1);
		MESSAGE_END();

		m_iLastFiremode = firemode;
	}

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	if (m_fInAttack==1)
	{
		FireBall();
		m_fInAttack = 0;
		return;
	}
	else if (m_fInAttack==2)
	{
		Teleport();
		m_fInAttack = 0;
		return;
	}

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = DISPLACER_IDLE1;	
		break;
	
	default:
	case 1:
		iAnim = DISPLACER_IDLE2;
		break;
	}

	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 5, 8 );
}

void CDisplacer::Reload( void )
{
	if (m_iClip)
	return;

	DefaultReload( 60, DISPLACER_RELOAD, 6.3, 2.2 );
}