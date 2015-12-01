#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"

enum Torch_e
{
	TORCH_IDLE,
	TORCH_IDLE2,
	TORCH_USE,
	TORCH_STARTUSE,
	TORCH_ENDUSE,
	TORCH_DRAW,
	TORCH_HOLSTER,
	TORCH_VODKA
};

enum Torch_a
{
	ACT_NO = 0,
	ACT_DRINK,
	ACT_TAKEUP,
	ACT_PUTDOWN,
	ACT_BURN
};

#define ALCOHOL_MAXIMAL_POWER		75 //a bit more than concussion level
#define	ALCOHOL_CONCUSSION_DOZE		50	//100 gramms are concussing??? LOL!
#define ALCOHOL_DANGER_DOZE		25 //screen starts spinning too hardly

float AdjustedGaussFunction ( float x, float m, float s )
{
	return 1000 / ((sqrt(2 * M_PI) * s) * exp ( ((x - m)*(x - m))/(2*s*s) ));
}

class CTorch : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy( void );
	void Holster( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void DoubleAttack( void ) {PrimaryAttack();};
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void BuySecondaryAmmo( void );
	void SellWeapon( void );
	BOOL ShouldWeaponIdle( void ) {return TRUE;};
	int SecondaryAmmoIndex( void );

	int active;
	float m_flBuzzTime;
};
LINK_ENTITY_TO_CLASS( weapon_torch, CTorch );

void CTorch::BuyPrimaryAmmo( void )
{
	BuyAmmo(200, (char*)pszAmmo1(), COST_ENERGY_AMMO);
}

void CTorch::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_TORCH/2;
}

int CTorch::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void CTorch::BuySecondaryAmmo( void )
{
	BuyAmmo(2, (char*)pszAmmo2(), COST_ASSAULT_AMMO);
}

void CTorch::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_torch.mdl");
	m_iId = WEAPON_TORCH;
	m_iDefaultAmmo = 200;
	m_iDefaultAmmo2 = 4;
	active = ACT_NO;
	FallInit();
}

void CTorch::Precache( void )
{
	PRECACHE_MODEL("models/v_torch.mdl");
	PRECACHE_MODEL("models/p_torch.mdl");
	PRECACHE_SOUND("weapons/torch_use.wav");
}

BOOL CTorch::Deploy( void )
{
        m_pPlayer->m_fTorch = TRUE;
	return DefaultDeploy( "models/v_torch.mdl", "models/p_torch.mdl", TORCH_DRAW, "hive", 1.4 );
}

void CTorch::Holster( )
{
        m_pPlayer->m_fTorch = FALSE;
	if (active >= ACT_PUTDOWN)
	{
		STOP_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/rocket1.wav" );
		FX_FireGun(m_pPlayer->pev->angles, m_pPlayer->entindex(), 200, 0, FIREGUN_REMOVE );
	}
	active = ACT_NO;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 1.1;
	SendWeaponAnim( TORCH_HOLSTER );
}

int CTorch::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "propane";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = "Vodka";
	p->iMaxAmmo2 = 30;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 4;
	p->iId = WEAPON_TORCH;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = TORCH_WEIGHT;
	p->iMaxspeed = TORCH_MAXSPEED;
	return 1;
}

void CTorch::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	if (active == ACT_NO)
	{
		SendWeaponAnim (TORCH_STARTUSE);
		active = ACT_TAKEUP;
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		m_flTimeWeaponIdle = gpGlobals->time + 0.6;
	}
	else if (active == ACT_TAKEUP)
	{
		active = ACT_BURN;
		FX_FireGun(m_pPlayer->pev->angles, m_pPlayer->entindex(), 200, 0, FIREGUN_TORCH );
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/rocket1.wav", 0.3, ATTN_NORM );
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 1;
		m_flTimeWeaponIdle = gpGlobals->time + 1.1;
	}
	else if (active == ACT_BURN)
	{
		TraceResult tr;
		UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
		UTIL_TraceLine(m_pPlayer->GetGunPosition(), m_pPlayer->GetGunPosition() + (gpGlobals->v_forward*60), dont_ignore_monsters, m_pPlayer->edict(), &tr);

		if (!tr.pHit->v.takedamage)
			return;

		if ( gpGlobals->time >= m_flBuzzTime )
		{
			EMIT_SOUND( ENT(pev), CHAN_STATIC, "weapons/torch_use.wav", 0.8, ATTN_NORM );
			m_flBuzzTime = gpGlobals->time + 1.2;
		}

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		float damage = AdjustedGaussFunction(m_pPlayer->m_fVodkaShots, ALCOHOL_MAXIMAL_POWER, 100);

		if (pEntity->IsPlayer())
		{
			if ( g_pGameRules->PlayerRelationship(m_pPlayer, pEntity) == GR_TEAMMATE )
				pEntity->TakeDamage ( m_pPlayer->pev, m_pPlayer->pev, damage * 2, DMG_REGENARMOR);
			else
				pEntity->TakeDamage ( m_pPlayer->pev, m_pPlayer->pev, dmg_torch.value * (mp_wpn_power.value/100), DMG_BURN);
		}
		else if (pEntity->pev->takedamage)
		{
			pEntity->TakeDamage ( m_pPlayer->pev, m_pPlayer->pev, damage, DMG_REGENARMOR);
		}

		SendWeaponAnim( TORCH_USE );
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
		FX_Explosion( tr.vecEndPos + (gpGlobals->v_right * 6 + gpGlobals->v_up * -4), EXPLOSION_TORCH );

		m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.2;
		m_flTimeWeaponIdle = gpGlobals->time + 1;
	}
}

void CTorch::SecondaryAttack()
{
	if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
 	{
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		return;
	}
	if (active == ACT_NO)
	{
		active = ACT_DRINK;
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->time + 1;
		m_flTimeWeaponIdle = gpGlobals->time + 1;
		SendWeaponAnim(TORCH_VODKA);
	}
	else if (active == ACT_DRINK)
	{
		m_pPlayer->m_fVodkaShots += 50;
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;

		if (m_pPlayer->m_fVodkaShots > ALCOHOL_CONCUSSION_DOZE && m_pPlayer->m_fConcussLevel < ALCOHOL_DANGER_DOZE)
			m_pPlayer->m_fConcussLevel += 20;

		m_flNextSecondaryAttack = m_flNextPrimaryAttack = m_flTimeWeaponIdle = gpGlobals->time + 1.2;
		active = ACT_NO;
	}
}

void CTorch::WeaponIdle( void )
{
	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	int iAnim;

	if (active == ACT_DRINK)
	{
		m_pPlayer->m_fVodkaShots += 50;
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;

		if (m_pPlayer->m_fVodkaShots > ALCOHOL_CONCUSSION_DOZE && m_pPlayer->m_fConcussLevel < ALCOHOL_DANGER_DOZE)
			m_pPlayer->m_fConcussLevel += 20;

		m_flNextSecondaryAttack = m_flNextPrimaryAttack = m_flTimeWeaponIdle = gpGlobals->time + 1.2;
		active = ACT_NO;
		return;
	}
	else if (active == ACT_BURN)
	{
		iAnim = TORCH_ENDUSE;
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 1;
		m_flTimeWeaponIdle = gpGlobals->time + 0.5;
		active = ACT_PUTDOWN;
	}
	else if (active == ACT_PUTDOWN)
	{
		active = ACT_NO;
		STOP_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/rocket1.wav" );
		FX_FireGun(m_pPlayer->pev->angles, m_pPlayer->entindex(), 200, 0, FIREGUN_REMOVE );
		m_flTimeWeaponIdle = gpGlobals->time + 0.5;
		return;
	}
	else if (active == ACT_TAKEUP)
	{
		active = ACT_BURN;
		FX_FireGun(m_pPlayer->pev->angles, m_pPlayer->entindex(), 200, 0, FIREGUN_TORCH );
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/rocket1.wav", 0.3, ATTN_NORM );
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 1;
		m_flTimeWeaponIdle = gpGlobals->time + 1.1;
		return;
	}
	else
	{
		switch ( RANDOM_LONG( 0, 1 ) )
		{
		case 0:	
			iAnim = TORCH_IDLE;	
			break;
		
		default:
		case 1:
			iAnim = TORCH_IDLE2;
			break;
		}

		m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
	}

	SendWeaponAnim( iAnim );
}