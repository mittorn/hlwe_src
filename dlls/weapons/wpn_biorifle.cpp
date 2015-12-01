#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "projectiles.h"

class CBiorifle: public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};

void CBiorifle::BuyPrimaryAmmo( void )
{
	BuyAmmo(18, (char*)pszAmmo1(), COST_HEAVY_AMMO);
}

void CBiorifle::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_BIORIFLE/2;
}

enum biorifle_e
{
	BIORIFLE_IDLE,
	BIORIFLE_IDLE2,
	BIORIFLE_IDLE3,
	BIORIFLE_FIRE,
	BIORIFLE_FIRE_SOLID,
	BIORIFLE_RELOAD,
	BIORIFLE_DRAW,
	BIORIFLE_HOLSTER
};
LINK_ENTITY_TO_CLASS( weapon_biorifle, CBiorifle);

void CBiorifle::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_biorifle.mdl");
	m_iId = WEAPON_BIORIFLE;
	m_iDefaultAmmo = 36;
	pev->weapons = CLIP_BIORIFLE;
	FallInit();
}

void CBiorifle::Precache( void )
{
	PRECACHE_MODEL("models/v_biorifle.mdl");
	PRECACHE_MODEL("models/p_biorifle.mdl");
	PRECACHE_MODEL("models/w_biomass.mdl");
	PRECACHE_SOUND ("weapons/biorifle_fire.wav");
	PRECACHE_SOUND ("weapons/biomass_exp.wav");
}

int CBiorifle::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "biocharge";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 18;
	p->iSlot = 8;
	p->iPosition = 1;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_BIORIFLE;
	p->iWeight = BIORIFLE_WEIGHT;
	p->iMaxspeed = BIORIFLE_MAXSPEED;
	return 1;
}

BOOL CBiorifle::Deploy( )
{
	return DefaultDeploy( "models/v_biorifle.mdl", "models/p_biorifle.mdl", BIORIFLE_DRAW, "gauss", 0.9 );
}

void CBiorifle::PrimaryAttack()
{
	if (m_iClip <= 0)
 	{
		PlayEmptySound(4);
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	SendWeaponAnim((m_pPlayer->m_fHeavyArmor)?BIORIFLE_FIRE_SOLID:BIORIFLE_FIRE);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CBiomass::ShootBiomass( m_pPlayer->pev,	m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16 + gpGlobals->v_right * 7 + gpGlobals->v_up * -8, gpGlobals->v_forward * 3000, 400);
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/biorifle_fire.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));

	m_flNextPrimaryAttack = gpGlobals->time + 0.3;
	m_flNextSecondaryAttack = gpGlobals->time + 0.1;
	m_flTimeWeaponIdle = gpGlobals->time + 2;
	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(-2,5);
		m_pPlayer->pev->punchangle.y -= 1;
	}
}

void CBiorifle::SecondaryAttack( void )
{
	edict_t *pPlayer = m_pPlayer->edict( );
	CBaseEntity *pBioCharge = NULL;

	while ((pBioCharge = UTIL_FindEntityInSphere( pBioCharge, m_pPlayer->pev->origin, 16384)) != NULL)
	{
		if (FClassnameIs( pBioCharge->pev, "biomass"))
		{
			if (pBioCharge->pev->owner == pPlayer)
			{
				pBioCharge->Use( m_pPlayer, m_pPlayer, USE_ON, 0 );
			}
		}
	}
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.1;
}

void CBiorifle::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.9;
	SendWeaponAnim( BIORIFLE_HOLSTER );
}

void CBiorifle::Reload( void )
{
	DefaultReload( 18, BIORIFLE_RELOAD, 2.7, 0.87 );
}

void CBiorifle::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 2 ))
	{
	case 0:	
		iAnim = BIORIFLE_IDLE2;	
		break;
	case 1:	
		iAnim = BIORIFLE_IDLE3;	
		break;
	case 2:
		iAnim = BIORIFLE_IDLE;
		break;
	}

	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}

void DeactivateBiomass( CBasePlayer *pOwner )
{
	edict_t *pFind; 
	pFind = FIND_ENTITY_BY_CLASSNAME( NULL, "biomass" );

	while ( !FNullEnt( pFind ) )
	{
		CBaseEntity *pEnt = CBaseEntity::Instance( pFind );
		CBiomass *pBioCharge = (CBiomass *)pEnt;

		if ( pBioCharge)
		{
			if ( pBioCharge->pev->owner == pOwner->edict() )
				pBioCharge->Deactivate();
		}
		pFind = FIND_ENTITY_BY_CLASSNAME( pFind, "biomass" );
	}
}