#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "effects.h"
#include "projectiles.h"

class C30mmsg : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void Shoot( void );
	void GrenadeLauncher( void );
	void SwitchFireMode( void );

	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};

enum ssg_e
{
	SSG_IDLE1,
	SSG_IDLE2,
	SSG_RELOAD_3,
	SSG_RELOAD_2,
	SSG_RELOAD_1,
	SSG_RELOAD_0,
	SSG_SHOOT,
	SSG_SHOOT_SOLID,
	SSG_DRAW,
	SSG_HOLSTER
};
LINK_ENTITY_TO_CLASS( weapon_30mmsg, C30mmsg );

void C30mmsg::BuyPrimaryAmmo( void )
{
	BuyAmmo(4, (char*)pszAmmo1(), COST_ASSAULT_AMMO);
}

void C30mmsg::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_SG30MM/2;
}

void C30mmsg::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_30mmsg.mdl");
	m_iId = WEAPON_30MMSG;
	firemode=FIREMODE_SHOOT;
	m_iDefaultAmmo = 8;
	m_iDefaultAmmo2 = 4;
	FallInit();
}

void C30mmsg::Precache( void )
{
	PRECACHE_MODEL("models/v_30mmsg.mdl");
	PRECACHE_MODEL("models/p_30mmsg.mdl");
	PRECACHE_SOUND("weapons/30mmsg_fire.wav");
}

int C30mmsg::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "30mm";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 4;
	p->iSlot = 2;
	p->iPosition = 2;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_30MMSG;
	p->iWeight = SG30MM_WEIGHT;
	p->iMaxspeed = SG30MM_MAXSPEED;
	return 1;
}

BOOL C30mmsg::Deploy( )
{
	m_iLastFiremode = -1;
	return DefaultDeploy( "models/v_30mmsg.mdl", "models/p_30mmsg.mdl", SSG_DRAW, "rpg", 1.5);
}

void C30mmsg::Holster( )
{
	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 1.2;
	SendWeaponAnim( SSG_HOLSTER );
}

void C30mmsg::SwitchFireMode( void )
{
	if(firemode==FIREMODE_SHOOT)
	{
		firemode=FIREMODE_GLAUNCHER;
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.2;
	}
	else
	{
		firemode=FIREMODE_SHOOT;
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.2;
	}
}

void C30mmsg::Shoot()
{
	UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	for (int i=0; i<15; i++)
		m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_14DEGREES:VECTOR_CONE_15DEGREES, 16384, BULLET_30mm, m_pPlayer->pev);
	FX_FireGun( m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?SSG_SHOOT_SOLID:SSG_SHOOT, 0, FIREGUN_30MMSG );
}

void C30mmsg::GrenadeLauncher( void )
{
	UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	C30mmgren::Shoot30mmgren( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 24 + gpGlobals->v_right * 8 + gpGlobals->v_up * -10, gpGlobals->v_forward * 4500 );
	FX_FireGun( m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?SSG_SHOOT_SOLID:SSG_SHOOT, 0, FIREGUN_30MMSG );
}

void C30mmsg::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}
	UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	if (!m_pPlayer->m_fHeavyArmor)
	{
		float flZVel = m_pPlayer->pev->velocity.z;
		m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 200;
		m_pPlayer->pev->velocity.z = flZVel;
	}

	m_iClip --;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	UTIL_ScreenShake( m_pPlayer->pev->origin, 5.0, 150.0, 1, 250.0 );

	if(firemode==FIREMODE_SHOOT)
		Shoot();
	if(firemode==FIREMODE_GLAUNCHER)
		GrenadeLauncher();

	(m_pPlayer->m_fHeavyArmor)?m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6:m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.1;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);
	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(-7,8);
		m_pPlayer->pev->punchangle.y -= 2.5;
	}
}

void C30mmsg::SecondaryAttack( void )
{
	if(firemode==FIREMODE_GLAUNCHER)
		firemode=FIREMODE_SHOOT;
	else
		firemode=FIREMODE_GLAUNCHER;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.1;

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(firemode);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	m_iLastFiremode = firemode;
}

void C30mmsg::Reload( void )
{
	if (m_iClip >= 4)
	return;

	int ReloadAnim = SSG_RELOAD_3;
	int adjust;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 4 - m_iClip)
		adjust = 4 - m_iClip;
	else
		adjust = m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType];

	float time = adjust * 1.1 + 2.5;
	DefaultReload( 4, ReloadAnim + adjust - 1, time);
}

void C30mmsg::WeaponIdle( void )
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

	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	switch (RANDOM_LONG(0,1))
	{
		case 0:	SendWeaponAnim( SSG_IDLE1 ); break;
		case 1:	SendWeaponAnim( SSG_IDLE2 ); break;
	}
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}