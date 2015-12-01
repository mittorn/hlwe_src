#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "decals.h"
#include "projectiles.h"

enum pulserifle_e
{
	PULSERIFLE_IDLE,
	PULSERIFLE_IDLE2,
	PULSERIFLE_FIDGET,
	PULSERIFLE_FIRE,
	PULSERIFLE_FIRE_SOLID,
	PULSERIFLE_RELOAD,
	PULSERIFLE_DRAW,
	PULSERIFLE_HOLSTER
};

class Cpulserifle : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( void );
	void Reload( void );
	void Holster( void );
	void WeaponIdle( void );

	void SwitchFireMode( void );
	void ZoomMode( void );
	void ChargeArmor( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
	void ZoomReset( void );
};
LINK_ENTITY_TO_CLASS( weapon_pulserifle, Cpulserifle );

void Cpulserifle::BuyPrimaryAmmo( void )
{
	BuyAmmo(10, (char*)pszAmmo1(), COST_ENERGY2_AMMO);
}

void Cpulserifle::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_PULSERIFLE/2;
}

void Cpulserifle::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_pulserifle.mdl");
	m_iId = WEAPON_PULSERIFLE;
	firemode=FIREMODE_CHARGE;
	m_iDefaultAmmo = 20;
	pev->weapons = CLIP_PULSERIFLE;
	FallInit();
}

void Cpulserifle::Precache( void )
{
	PRECACHE_MODEL("models/v_pulserifle.mdl");
	PRECACHE_MODEL("models/p_pulserifle.mdl");

	PRECACHE_SOUND ("weapons/pulserifle_fire.wav");
	PRECACHE_SOUND( "weapons/pulse_hitwall.wav" );
}

int Cpulserifle::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "stormcell";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 10;
	p->iSlot = 8;
	p->iPosition = 2;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_PULSERIFLE;
	p->iWeight = PULSERIFLE_WEIGHT;
	p->iMaxspeed = PULSERIFLE_MAXSPEED;
	return 1;
}

BOOL Cpulserifle::Deploy( )
{
	m_iLastFiremode = -1;
	return DefaultDeploy( "models/v_pulserifle.mdl", "models/p_pulserifle.mdl", PULSERIFLE_DRAW, "gauss", 0.8 );
}

void Cpulserifle::Holster( )
{
	m_fInReload = FALSE;
	ZoomReset();

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( PULSERIFLE_HOLSTER );
}

void Cpulserifle::ZoomReset(void)
{
	if (m_pPlayer->m_iFOV != 90)
	{
		m_fInZoom = 0;
		m_pPlayer->m_iFOV = 90;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_SCOPE);
		WRITE_BYTE(0);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}
}

void Cpulserifle::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}
	TraceResult tr;

	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	UTIL_TraceLine(m_pPlayer->GetGunPosition(), m_pPlayer->GetGunPosition() + gpGlobals->v_forward*16384, dont_ignore_monsters, m_pPlayer->edict(), &tr);
	if ( UTIL_PointContents(tr.vecEndPos) != CONTENTS_SKY && UTIL_PointContents(tr.vecEndPos) != CONTENT_WATER)
		Create( "pulse", tr.vecEndPos, tr.vecEndPos, m_pPlayer->edict() );

	m_pPlayer->FireBeam(m_pPlayer->GetGunPosition()+gpGlobals->v_forward * 16 + gpGlobals->v_right * 3 + gpGlobals->v_up * -3, gpGlobals->v_forward, BEAM_PULSERIFLE, 0, m_pPlayer->pev );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?PULSERIFLE_FIRE_SOLID:PULSERIFLE_FIRE, 0, FIREGUN_PULSERIFLE );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 1.5;
	m_flTimeWeaponIdle = gpGlobals->time + 5;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 4;
}

void Cpulserifle::SecondaryAttack()
{
	if(firemode==FIREMODE_SCOPE)
		ZoomMode();
	if(firemode==FIREMODE_CHARGE)
		ChargeArmor();
}

void Cpulserifle::ZoomMode( void )
{
	if (m_fInZoom == 0) 
	{
		m_pPlayer->m_iFOV = 40;
		m_fInZoom = 1;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_SCOPE);
		WRITE_BYTE(4);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}
	else if (m_fInZoom == 1) 
	{
		m_pPlayer->m_iFOV = 10;
		m_fInZoom = 2;
	}
	else if (m_fInZoom == 2) 
	{
		m_pPlayer->m_iFOV = 90;
		m_fInZoom = 0;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_SCOPE);
		WRITE_BYTE(0);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	m_flNextSecondaryAttack = gpGlobals->time + 0.5;
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom2.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
}

void Cpulserifle::ChargeArmor()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		return;
	}

	if (m_pPlayer->pev->armorvalue > 180)
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "items/suitchargeno1.wav", 0.9, ATTN_NORM );
		m_flNextSecondaryAttack = gpGlobals->time + 1.0;
		return;
	}

	m_iClip--;
	m_iFiredAmmo++;
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "items/suitchargeok1.wav", 1.0, ATTN_LOW);
	m_pPlayer->pev->armorvalue +=20;

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 2;
	m_flTimeWeaponIdle = gpGlobals->time + 5;
}

void Cpulserifle::SwitchFireMode( void )
{
	if(firemode==FIREMODE_CHARGE)
		firemode=FIREMODE_SCOPE;
	else
		firemode=FIREMODE_CHARGE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.1;
}

void Cpulserifle::Reload( void )
{
	if (m_iClip)
	return;

	ZoomReset();
	DefaultReload( 10, PULSERIFLE_RELOAD, 2.9, 1.5 );
}

void Cpulserifle::WeaponIdle( void )
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

	if (m_pPlayer->m_iFOV != 90)
		return;

	switch (RANDOM_LONG(0,2))
	{
		case 0:	SendWeaponAnim( PULSERIFLE_IDLE ); break;
		case 1:	SendWeaponAnim( PULSERIFLE_IDLE2 ); break;
		case 2:	SendWeaponAnim( PULSERIFLE_FIDGET ); break;
	}
	m_flTimeWeaponIdle = gpGlobals->time + 10;
}