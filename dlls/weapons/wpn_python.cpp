#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "monsters.h"
#include "player.h"
#include "gamerules.h"

class CPython : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);
	void PrimaryAttack( void );
	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );

	void EXPORT shellcasing( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};

enum python_e
{
	PYTHON_IDLE1,
	PYTHON_IDLE2,
	PYTHON_IDLE3,
	PYTHON_FIRE,
	PYTHON_FIRE_SOLID,
	PYTHON_RELOAD,
	PYTHON_DRAW,
	PYTHON_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_python, CPython );

void CPython::BuyPrimaryAmmo( void )
{
	BuyAmmo(6, (char*)pszAmmo1(), COST_HANDGUN_AMMO);
}

void CPython::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_PYTHON/2;
}

int CPython::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "357";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 6;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iSlot = 1;
	p->iPosition = 4;
	p->iId = WEAPON_PYTHON;
	p->iWeight = PYTHON_WEIGHT;
	p->iMaxspeed = PYTHON_MAXSPEED;
	return 1;
}

void CPython::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_python.mdl");
	m_iId = WEAPON_PYTHON;
	m_iDefaultAmmo = 12;
	FallInit();
}

void CPython::Precache( void )
{
	PRECACHE_MODEL("models/v_python.mdl");
	PRECACHE_MODEL("models/p_python.mdl");

	PRECACHE_SOUND("weapons/ammo_out.wav");
	PRECACHE_SOUND("weapons/357_shot1.wav");
}

BOOL CPython::Deploy( )
{
	return DefaultDeploy( "models/v_python.mdl", "models/p_python.mdl", PYTHON_DRAW, "onehanded" );
}

void CPython::Holster( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
	SendWeaponAnim( PYTHON_HOLSTER );
	m_fInReload = FALSE;
}

void CPython::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	if(! ( m_pPlayer->m_afButtonPressed & IN_ATTACK ) )
	return;

	m_iClip--;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_3DEGREES:VECTOR_CONE_4DEGREES, 16384, BULLET_357, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?PYTHON_FIRE_SOLID:PYTHON_FIRE, 0, FIREGUN_PYTHON );

	m_flNextPrimaryAttack = gpGlobals->time + 0.3;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 8;
}

void CPython::shellcasing() 
{ 
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 1, FIREGUN_PYTHON );
} 

void CPython::Reload( void )
{
	if (m_iClip>5)
	return;

	m_iClip=0;
	DefaultReload( 6, PYTHON_RELOAD, 3.0 );

	SetThink( shellcasing ); 
	pev->nextthink = gpGlobals->time + 1.5; 
}

void CPython::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 2))
	{
	case 0:	
		iAnim = PYTHON_IDLE1;	
		break;
	
	case 1:	
		iAnim = PYTHON_IDLE2;	
		break;
	default:
	case 2:
		iAnim = PYTHON_IDLE3;
		break;
	}
	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}
