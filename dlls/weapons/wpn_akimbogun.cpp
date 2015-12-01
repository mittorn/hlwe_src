#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class Cakimbogun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void DoubleAttack( void );
	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );

	void FireLeft( void );
	void FireRight( void );
	void FireBoth( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );

	int clipAug;
	int clipSG552;
};

enum akimbogun_e
{
	AKIMBOGUN_IDLE,
	AKIMBOGUN_IDLE2,
	AKIMBOGUN_IDLE3,
	AKIMBOGUN_FIDGET,
	AKIMBOGUN_DRAW,
	AKIMBOGUN_HOLSTER,
	AKIMBOGUN_RELOAD,
	AKIMBOGUN_SHOOT_LEFT,
	AKIMBOGUN_SHOOT_RIGHT,
	AKIMBOGUN_SHOOT_BOTH,
	AKIMBOGUN_SHOOT_LEFT_SOLID,
	AKIMBOGUN_SHOOT_RIGHT_SOLID,
	AKIMBOGUN_SHOOT_BOTH_SOLID
};
LINK_ENTITY_TO_CLASS( weapon_akimbogun, Cakimbogun );

void Cakimbogun::BuyPrimaryAmmo( void )
{
	BuyAmmo(60, (char*)pszAmmo1(), COST_ASSAULT_AMMO);
}

void Cakimbogun::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_AKIMBOGUN/2;
}

void Cakimbogun::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_akimbogun.mdl");
	m_iId = WEAPON_AKIMBOGUN;
	pev->weapons = CLIP_AKIMBOGUN_SG552;

	clipAug=30;
	clipSG552=30;

	m_iDefaultAmmo = 120;
	FallInit();
}

void Cakimbogun::Precache( void )
{
	PRECACHE_MODEL("models/v_akimbogun.mdl");
	PRECACHE_MODEL("models/p_akimbogun.mdl");

	PRECACHE_SOUND ("weapons/akimbogun_aug_fire.wav");
	PRECACHE_SOUND ("weapons/akimbogun_sg552_fire.wav");
}

int Cakimbogun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762Magnum";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 60;
	p->iSlot = 2;
	p->iPosition = 5;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_AKIMBOGUN;
	p->iWeight = AKIMBOGUN_WEIGHT;
	p->iMaxspeed = AKIMBOGUN_MAXSPEED;
	return 1;
}

BOOL Cakimbogun::Deploy( )
{
	return DefaultDeploy( "models/v_akimbogun.mdl", "models/p_akimbogun.mdl", AKIMBOGUN_DRAW, "twohanded", 1.1 );
}

void Cakimbogun::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
	SendWeaponAnim( AKIMBOGUN_HOLSTER );
}

void Cakimbogun::FireLeft()
{
	if(clipAug<1 && clipSG552 > 0)
	{
		FireRight();
		return;
	}
	m_iClip--;
	clipAug--;
	m_iFiredAmmo++;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_3DEGREES:VECTOR_CONE_4DEGREES, 16384, BULLET_762Nato, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?AKIMBOGUN_SHOOT_LEFT_SOLID:AKIMBOGUN_SHOOT_LEFT, m_pPlayer->m_fHeavyArmor?1:0, FIREGUN_AKIMBOGUN_LEFT );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.11;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}

void Cakimbogun::FireRight()
{
	if(clipSG552<1 && clipAug > 0)
	{
		FireLeft();
		return;
	}
	m_iClip--;
	clipSG552--;
	m_iFiredAmmo++;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_3DEGREES:VECTOR_CONE_4DEGREES, 16384, BULLET_762Nato, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?AKIMBOGUN_SHOOT_RIGHT_SOLID:AKIMBOGUN_SHOOT_RIGHT, m_pPlayer->m_fHeavyArmor?1:0, FIREGUN_AKIMBOGUN_RIGHT );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.12;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}

void Cakimbogun::FireBoth()
{
	if(clipSG552<=0)
	{
		FireLeft();
		return;
	}
	if(clipAug<=0)
	{
		FireRight();
		return;
	}
	m_iClip -=2;
	m_iFiredAmmo+=2;
	clipAug--;
	clipSG552--;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	for (int i=0; i<2; i++)
		m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_5DEGREES:VECTOR_CONE_6DEGREES, 16384, BULLET_762Nato, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?AKIMBOGUN_SHOOT_BOTH_SOLID:AKIMBOGUN_SHOOT_BOTH, m_pPlayer->m_fHeavyArmor?1:0, FIREGUN_AKIMBOGUN_BOTH );

	m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->time + 0.15;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}

void Cakimbogun::DoubleAttack()
{
	if (m_iClip <= 0)
	{
		m_iClip=0;
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		PlayEmptySound( );
		return;
	}
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	FireBoth();
}

void Cakimbogun::PrimaryAttack()
{
	if (m_iClip <= 0)
	{
		m_iClip=0;
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		PlayEmptySound( );
		return;
	}
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	FireLeft();
}

void Cakimbogun::SecondaryAttack()
{
	if (m_iClip <= 0)
	{
		m_iClip=0;
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		PlayEmptySound( );
		return;
	}
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	FireRight();
}

void Cakimbogun::Reload( void )
{
	DefaultReload( 60, AKIMBOGUN_RELOAD, 6.8, 1.2 );
	clipAug=30;
	clipSG552=30;
}

void Cakimbogun::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	switch (RANDOM_LONG(0,3))
	{
		case 0:	SendWeaponAnim( AKIMBOGUN_IDLE ); break;
		case 1:	SendWeaponAnim( AKIMBOGUN_IDLE2 ); break;
		case 2:	SendWeaponAnim( AKIMBOGUN_IDLE3 ); break;
		case 3:	SendWeaponAnim( AKIMBOGUN_FIDGET ); break;
	}
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}