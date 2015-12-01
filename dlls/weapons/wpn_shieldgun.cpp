#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class Cshieldgun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void Range( void );
	void Melee( void );
	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );

	void SwitchFireMode( void );
	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
	int shield;
};

void Cshieldgun::BuyPrimaryAmmo( void )
{
	BuyAmmo(6, (char*)pszAmmo1(), COST_ASSAULT_AMMO);
}

void Cshieldgun::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_SHIELDGUN/2;
}

enum shieldgun_e
{
	SHIELDGUN_IDLE1,
	SHIELDGUN_IDLE2,
	SHIELDGUN_FIRE,
	SHIELDGUN_FIRE_SOLID,
	SHIELDGUN_KNIFE,
	SHIELDGUN_RELOAD,
	SHIELDGUN_DRAW,
	SHIELDGUN_HOLSTER,
	SHIELDGUN_SHIELD_IDLE,
	SHIELDGUN_SHIELD_UP,
	SHIELDGUN_SHIELD_DOWN
};

LINK_ENTITY_TO_CLASS( weapon_shieldgun, Cshieldgun );

void Cshieldgun::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_shieldgun.mdl");
	m_iId = WEAPON_SHIELDGUN;
	firemode=FIREMODE_SHOOT;
	shield=0;
	pev->weapons = CLIP_SHIELDGUN;
	m_iDefaultAmmo = 12;
	FallInit();
}

void Cshieldgun::Precache( void )
{
	PRECACHE_MODEL("models/v_shieldgun.mdl");
	PRECACHE_MODEL("models/p_shieldgun.mdl");

	PRECACHE_SOUND ("weapons/shieldgun_fire.wav");
	PRECACHE_SOUND ("weapons/knife_hitwall1.wav");
}

int Cshieldgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "12g";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 6;
	p->iSlot = 1;
	p->iPosition = 7;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_SHIELDGUN;
	p->iWeight = SHIELDGUN_WEIGHT;
	p->iMaxspeed = SHIELDGUN_MAXSPEED;
	return 1;
}

BOOL Cshieldgun::Deploy( )
{
	m_iLastFiremode = -1;
	return DefaultDeploy( "models/v_shieldgun.mdl", "models/p_shieldgun.mdl", SHIELDGUN_DRAW, "shieldgun", 1 );
}

void Cshieldgun::Holster( )
{
	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

        m_pPlayer->m_fShieldProtection = FALSE;
	m_fInReload = FALSE;
	shield=0;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.7;
	SendWeaponAnim( SHIELDGUN_HOLSTER );
}

void Cshieldgun::SwitchFireMode( void )
{
	if(firemode==FIREMODE_SHOOT)
	{
		firemode=FIREMODE_KNIFE;
		strcpy( m_pPlayer->m_szAnimExtention, "hive" );
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.1;
	}
	else
	{
		firemode=FIREMODE_SHOOT;
		strcpy( m_pPlayer->m_szAnimExtention, "shieldgun" );
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.1;
	}
}

void Cshieldgun::Range()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	m_iClip --;
	m_iFiredAmmo++;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	for (int i=0; i<4; i++)
		m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_14DEGREES:VECTOR_CONE_15DEGREES, 16384, BULLET_12G, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?SHIELDGUN_FIRE_SOLID:SHIELDGUN_FIRE, 0, FIREGUN_SHIELDGUN );

	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(4,6);
		m_pPlayer->pev->punchangle.y -= RANDOM_FLOAT(1,2);
	}
}

void Cshieldgun::Melee()
{
	SendWeaponAnim( SHIELDGUN_KNIFE );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, VECTOR_CONE_0DEGREES, 60, m_pPlayer->m_fHeavyArmor?BULLET_KNIFEQUAD:BULLET_KNIFE, m_pPlayer->pev);
}

void Cshieldgun::PrimaryAttack()
{
	if(shield==1)
	return;

	if(! ( m_pPlayer->m_afButtonPressed & IN_ATTACK ) )
	return;

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	if(firemode==FIREMODE_SHOOT)
		Range();
	if(firemode==FIREMODE_KNIFE)
		 Melee(); 

	m_flNextPrimaryAttack = gpGlobals->time + 0.25;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);
}

void Cshieldgun::SecondaryAttack()
{
	if(shield==1)
	{
		shield=0;
		SendWeaponAnim(SHIELDGUN_SHIELD_DOWN);
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
		strcpy( m_pPlayer->m_szAnimExtention, "shieldgun" );
	        m_pPlayer->m_fShieldProtection = FALSE;
	}
	else
	{
		shield=1;
		SendWeaponAnim(SHIELDGUN_SHIELD_UP);
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
		strcpy( m_pPlayer->m_szAnimExtention, "shield" );
	        m_pPlayer->m_fShieldProtection = TRUE;
	}
}

void Cshieldgun::Reload( void )
{
	if(shield==1)
		return;

	DefaultReload( 6, SHIELDGUN_RELOAD, 2.4, 0.84);
}


void Cshieldgun::WeaponIdle( void )
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

	if(shield!=0)
		SendWeaponAnim( SHIELDGUN_SHIELD_IDLE );
	else
	switch (RANDOM_LONG(0,1))
	{
		case 0:	SendWeaponAnim(SHIELDGUN_IDLE1); break;
		case 1:	SendWeaponAnim(SHIELDGUN_IDLE2); break;
	}

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}
