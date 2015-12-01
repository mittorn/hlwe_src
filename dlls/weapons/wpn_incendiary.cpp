#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "projectiles.h"

enum Incendiary_e
{
	INCENDIARY_IDLE_3ROCKETS,
	INCENDIARY_IDLE_2ROCKETS,
	INCENDIARY_IDLE_1ROCKETS,
	INCENDIARY_IDLE_0ROCKETS,
	INCENDIARY_FIRE_2ROCKETS,
	INCENDIARY_FIRE_1ROCKETS,
	INCENDIARY_FIRE_0ROCKETS,
	INCENDIARY_FIRE_2ROCKETS_SOLID,
	INCENDIARY_FIRE_1ROCKETS_SOLID,
	INCENDIARY_FIRE_0ROCKETS_SOLID,
	INCENDIARY_RELOAD,
	INCENDIARY_DRAW_3ROCKETS,
	INCENDIARY_DRAW_2ROCKETS,
	INCENDIARY_DRAW_1ROCKETS,
	INCENDIARY_DRAW_0ROCKETS,
	INCENDIARY_HOLSTER_3ROCKETS,
	INCENDIARY_HOLSTER_2ROCKETS,
	INCENDIARY_HOLSTER_1ROCKETS,
	INCENDIARY_HOLSTER_0ROCKETS
};

class CIncendiary : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	void Reload( void );
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy( void );
	void Holster( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};
LINK_ENTITY_TO_CLASS( weapon_incendiary, CIncendiary );

void CIncendiary::BuyPrimaryAmmo( void )
{
	BuyAmmo(3, (char*)pszAmmo1(), COST_LAUNCHER2_AMMO);
}

void CIncendiary::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_INCENDIARY/2;
}

void CIncendiary::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_incendiary.mdl");
	m_iId = WEAPON_INCENDIARY;
	m_iDefaultAmmo = 6;
	pev->weapons = CLIP_INCENDIARY;
	FallInit();
}

void CIncendiary::Precache( void )
{
	PRECACHE_MODEL("models/v_incendiary.mdl");
	PRECACHE_MODEL("models/p_incendiary.mdl");

	PRECACHE_MODEL("sprites/fire.spr");

	PRECACHE_SOUND("weapons/incendiary_fire.wav");
	PRECACHE_SOUND("weapons/incendiaryrocket.wav");
	PRECACHE_SOUND("weapons/incrocket_explode.wav" );
}

int CIncendiary::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "hellfire";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 3;
	p->iSlot = 5;
	p->iPosition = 1;
	p->iId = WEAPON_INCENDIARY;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = INCENDIARY_WEIGHT;
	p->iMaxspeed = INCENDIARY_MAXSPEED;
	return 1;
}

BOOL CIncendiary::Deploy( )
{
	int add = m_iClip > 2 ? 0 : 3 - m_iClip;
	return DefaultDeploy( "models/v_incendiary.mdl", "models/p_incendiary.mdl", INCENDIARY_DRAW_3ROCKETS+add, "gauss", 0.9 );
}

void CIncendiary::Holster( )
{
	int add = m_iClip > 2 ? 0 : 3 - m_iClip;
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.6;
	SendWeaponAnim( INCENDIARY_HOLSTER_3ROCKETS+add );
}

void CIncendiary::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}
	int add = m_iClip > 2 ? 0 : 3 - m_iClip;
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), m_pPlayer->m_fHeavyArmor?INCENDIARY_FIRE_2ROCKETS_SOLID+add:INCENDIARY_FIRE_2ROCKETS+add, 0, FIREGUN_INCENDIARY );

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Chellfire::ShootHellfire( m_pPlayer->pev, m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 28 + gpGlobals->v_right * 8 + gpGlobals->v_up * -8, gpGlobals->v_forward * 3000, FALSE);

	m_iClip--;
	m_iFiredAmmo++;
	pev->skin = m_iClip;
	m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->time + 1.8;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5,7);
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 10;
}

void CIncendiary::SecondaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		return;
	}
	int add = m_iClip > 2 ? 0 : 3 - m_iClip;
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), m_pPlayer->m_fHeavyArmor?INCENDIARY_FIRE_2ROCKETS_SOLID+add:INCENDIARY_FIRE_2ROCKETS+add, 0, FIREGUN_INCENDIARY );

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Chellfire::ShootHellfire( m_pPlayer->pev, m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 28 + gpGlobals->v_right * 8 + gpGlobals->v_up * -8, gpGlobals->v_forward * 300, TRUE);

	m_iFiredAmmo++;
	m_iClip--;
	pev->skin = m_iClip;
	m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->time + 1.8;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5,7);
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 10;
}

void CIncendiary::Reload( void )
{
	if (m_iClip)
	return;

	DefaultReload( 3, INCENDIARY_RELOAD, 4.3, 1.6 );
}

void CIncendiary::WeaponIdle( void )
{
	pev->skin = m_iClip;

	if ( m_flTimeWeaponIdle > gpGlobals->time )
	return;

	int iAnim;
	int add = m_iClip > 2 ? 0 : 3 - m_iClip;
	iAnim = INCENDIARY_IDLE_3ROCKETS + add;
	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}