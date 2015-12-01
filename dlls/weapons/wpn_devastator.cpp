#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "projectiles.h"

enum devastator_e
{
	DEVASTATOR_IDLE,
	DEVASTATOR_IDLE2,
	DEVASTATOR_IDLE3,
	DEVASTATOR_IDLE4,
	DEVASTATOR_IDLE5,
	DEVASTATOR_FIRE,
	DEVASTATOR_FIRE2,
	DEVASTATOR_FIRE3,
	DEVASTATOR_FIRE4,
	DEVASTATOR_FIRE5,
	DEVASTATOR_FIRE_SOLID,
	DEVASTATOR_FIRE2_SOLID,
	DEVASTATOR_FIRE3_SOLID,
	DEVASTATOR_FIRE4_SOLID,
	DEVASTATOR_FIRE5_SOLID,
	DEVASTATOR_DRAW,
	DEVASTATOR_DRAW2,
	DEVASTATOR_DRAW3,
	DEVASTATOR_DRAW4,
	DEVASTATOR_DRAW5,
	DEVASTATOR_HOLSTER,
	DEVASTATOR_HOLSTER2,
	DEVASTATOR_HOLSTER3,
	DEVASTATOR_HOLSTER4,
	DEVASTATOR_HOLSTER5,
	DEVASTATOR_RELOAD
};

class CDevastator : public CBasePlayerWeapon
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
	int FiredRockets;
};
LINK_ENTITY_TO_CLASS( weapon_devastator, CDevastator );

void CDevastator::BuyPrimaryAmmo( void )
{
	BuyAmmo(8, (char*)pszAmmo1(), COST_LAUNCHER2_AMMO);
}

void CDevastator::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_DEVASTATOR/2;
}

void CDevastator::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_devastator.mdl");
	m_iId = WEAPON_DEVASTATOR;
	m_iDefaultAmmo = 16;
	FallInit();
}

void CDevastator::Precache( void )
{
	PRECACHE_MODEL("models/v_devastator.mdl");
	PRECACHE_MODEL("models/p_devastator.mdl");

	PRECACHE_SOUND("weapons/devastator_fire.wav");
	PRECACHE_SOUND("weapons/mmissile_explode.wav" );
}

int CDevastator::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Mmissiles";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 8;
	p->iSlot = 5;
	p->iPosition = 3;
	p->iId = WEAPON_DEVASTATOR;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = DEVASTATOR_WEIGHT;
	p->iMaxspeed = DEVASTATOR_MAXSPEED;
	return 1;
}

BOOL CDevastator::Deploy( )
{
	int add = m_iClip > 3 ? 0 : 4 - m_iClip;
	return DefaultDeploy( "models/v_devastator.mdl", "models/p_devastator.mdl", DEVASTATOR_DRAW + add, "gauss", 0.9 );
}

void CDevastator::Holster( )
{
	m_fInReload = FALSE;
	int add = m_iClip > 3 ? 0 : 4 - m_iClip;
	SendWeaponAnim( DEVASTATOR_HOLSTER + add);
	m_pPlayer->m_flNextAttack = gpGlobals->time + 1;
}

void CDevastator::PrimaryAttack()
{
	if (m_iClip <= 0)
	{
		PlayEmptySound();
		return;
	}

	m_iClip--;
	int add = m_iClip > 3 ? 0 : 4 - m_iClip;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	CMmissile::ShootMmissile( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 24 + gpGlobals->v_right * 3 + gpGlobals->v_up * -8, gpGlobals->v_forward * 5250 );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?(DEVASTATOR_FIRE_SOLID+add):(DEVASTATOR_FIRE+add), 0, FIREGUN_DEVASTATOR );

	m_flNextPrimaryAttack = gpGlobals->time + 0.6;
	m_flTimeWeaponIdle = gpGlobals->time + 5;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 8;
}

void CDevastator::SecondaryAttack()
{
	if (m_iClip <= 0)
	{
		PlayEmptySound();
		return;
	}

	if (m_iClip == 1)
	{
		PrimaryAttack();
		return;
	}

	if (m_iClip >4)
		FiredRockets = 4;
	else if (m_iClip <= 4)
		FiredRockets = m_iClip;

	m_iClip -= FiredRockets;
	int add = m_iClip > 3 ? 0 : 4 - m_iClip;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?(DEVASTATOR_FIRE_SOLID+add):(DEVASTATOR_FIRE+add), 0, FIREGUN_DEVASTATOR );

	UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 24 + gpGlobals->v_right * 3 + gpGlobals->v_up * -8;

	float angle = -M_PI/2;
	for (int k = 0; k < FiredRockets; k++)
	{
		CMmissile::ShootMmissile( m_pPlayer->pev, vecSrc + gpGlobals->v_up*(8*sin(angle)) + gpGlobals->v_right*(8*cos(angle)), gpGlobals->v_forward*5250 + gpGlobals->v_up*(200*sin(angle))+gpGlobals->v_right*(200*cos(angle)));
		angle += 2*M_PI/FiredRockets;
	}

	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward*((m_iClip-1)*300);

	m_flNextSecondaryAttack = gpGlobals->time + 1.5;
	m_flTimeWeaponIdle = gpGlobals->time + 5;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= FiredRockets*5;
}

void CDevastator::WeaponIdle( void )
{
	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	int iAnim;
	int add = m_iClip > 3 ? 0 : 4 - m_iClip;
	iAnim = DEVASTATOR_IDLE + add;

	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}

void CDevastator::Reload( void )
{
	if (m_iClip)
	return;

	DefaultReload( 8, DEVASTATOR_RELOAD, 8.7 );
	m_flTimeWeaponIdle = gpGlobals->time + 10;
}