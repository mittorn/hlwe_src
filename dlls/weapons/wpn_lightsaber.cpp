#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "decals.h"
#include "projectiles.h"

class CLightsaber : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	void EXPORT SwingAgain( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( void );
	void Holster( void );
	void WeaponIdle( void );
	void Reload( void );
	void ReloadPistol( void );

	void EXPORT FireBall( void );
	void SellWeapon( void );
	int AddDuplicate( CBasePlayerItem* );
	TraceResult m_trHit;
};
LINK_ENTITY_TO_CLASS( weapon_lightsaber, CLightsaber );

void CLightsaber::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_LIGHTSABER/2;
	m_iClip = m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] = 0;
}

int CLightsaber::AddDuplicate( CBasePlayerItem *pOriginal )
{
	return false;
}

enum lightsaber_e
{
	LIGHTSABER_IDLE,
	LIGHTSABER_DRAW,
	LIGHTSABER_HOLSTER,
	LIGHTSABER_ATTACK1,
	LIGHTSABER_ATTACK2,
	LIGHTSABER_ATTACK3,
	LIGHTSABER_PISTOL_ATTACK
};

void CLightsaber::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_lightsaber.mdl");
	m_iId = WEAPON_LIGHTSABER;
	m_iDefaultAmmo = 75;
	FallInit();
}

void CLightsaber::Precache( void )
{
	PRECACHE_MODEL("models/v_lightsaber.mdl");
	PRECACHE_MODEL("models/p_lightsaber.mdl");

	PRECACHE_SOUND("weapons/lightsaber_hit.wav");
	PRECACHE_SOUND("weapons/lightsaber_hit2.wav");
	PRECACHE_SOUND("weapons/lightsaber_hit3.wav");
	PRECACHE_SOUND("weapons/lightsaber_miss.wav");
	PRECACHE_SOUND("weapons/lightsaber_idle.wav");
	PRECACHE_SOUND("weapons/lightsaber_fireplasma.wav");
	PRECACHE_SOUND("weapons/explode5.wav");
}

int CLightsaber::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "energy";
	p->iMaxAmmo1 = 50;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 25;
	p->iSlot = 0;
	p->iPosition = 5;
	p->iId = WEAPON_LIGHTSABER;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY | ITEM_FLAG_NOAUTORELOAD;
	p->iWeight = LIGHTSABER_WEIGHT;
	p->iMaxspeed = LIGHTSABER_MAXSPEED;
	return 1;
}

void CLightsaber::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 50)
		return;

	while (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 50 && m_flRechargeTime < gpGlobals->time)
	{
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]++;
		m_flRechargeTime += 1.2;
	}
}

void CLightsaber::ReloadPistol( void )
{
	if (m_iClip >= 25)
	return;

	while (m_iClip < 25 && m_flRechargeTime < gpGlobals->time)
	{
		m_iClip++;
		m_flRechargeTime += 1.5;
	}
}


BOOL CLightsaber::Deploy( )
{
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_LIGHTSABER );
	return DefaultDeploy( "models/v_lightsaber.mdl", "models/p_lightsaber.mdl", LIGHTSABER_DRAW, "crowbar" );
}

void CLightsaber::Holster( )
{
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_REMOVE );
	STOP_SOUND( ENT(m_pPlayer->pev), CHAN_AUTO, "weapons/lightsaber_idle.wav" );
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.8;
	SendWeaponAnim( LIGHTSABER_HOLSTER );
	m_fInAttack = 0;
}

void FindHullL( const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity )
{
	int		i, j, k;
	float		distance;
	float		*minmaxs[2] = {mins, maxs};
	TraceResult tmpTrace;
	Vector		vecHullEnd = tr.vecEndPos;
	Vector		vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine( vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace );
	if ( tmpTrace.flFraction < 1.0 )
	{
		tr = tmpTrace;
		return;
	}

	for ( i = 0; i < 2; i++ )
	{
		for ( j = 0; j < 2; j++ )
		{
			for ( k = 0; k < 2; k++ )
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace );
				if ( tmpTrace.flFraction < 1.0 )
				{
					float thisDistance = (tmpTrace.vecEndPos - vecSrc).Length();
					if ( thisDistance < distance )
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}

void CLightsaber::FireBall()
{
	if ( m_fInAttack == 0 )
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
		return;
	}

	m_iClip--;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecSrc = m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 10 + gpGlobals->v_right * -8 + gpGlobals->v_up * -5;
	CPBolt::ShootPBolt( m_pPlayer->pev, vecSrc, gpGlobals->v_forward * 15000 );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_EPISTOL );

	strcpy( m_pPlayer->m_szAnimExtention, "crowbar" );
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flRechargeTime = gpGlobals->time + 2;
}

void CLightsaber::SecondaryAttack()
{
	if ( m_fInAttack == 0)
	{
		if (m_iClip <= 0)
	 	{
			WeaponIdle();
			return;
		}

		m_fInAttack = 1;
		SendWeaponAnim( LIGHTSABER_PISTOL_ATTACK );
		strcpy( m_pPlayer->m_szAnimExtention, "plasmapistol" );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.8;
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

void CLightsaber::PrimaryAttack()
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 1)
	return;

	if (m_fInAttack!=0)
	return;

	int fDidHit = FALSE;
	UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	TraceResult tr;
	Vector vecSrc	= m_pPlayer->GetGunPosition( );
	Vector vecEnd	= vecSrc + gpGlobals->v_forward * 100;
	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

	if ( tr.flFraction >= 1.0 )
	{
		UTIL_TraceHull( vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT( m_pPlayer->pev ), &tr );
		if ( tr.flFraction < 1.0 )
		{
			CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
			if ( !pHit || pHit->IsBSPModel() )
				FindHullL( vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict() );
			vecEnd = tr.vecEndPos;
		}
	}

	switch(RANDOM_LONG(0,2))
	{
		case 0: SendWeaponAnim( LIGHTSABER_ATTACK1 ); break;
		case 1: SendWeaponAnim( LIGHTSABER_ATTACK2 ); break;
		case 2: SendWeaponAnim( LIGHTSABER_ATTACK3 ); break;
	}

	if ( tr.flFraction >= 1.0 )
	{
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.33;
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/lightsaber_miss.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
	}
	else
	{
		fDidHit = TRUE;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 2;
		UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		pEntity->TakeDamage ( m_pPlayer->pev, m_pPlayer->pev, dmg_lightsaber.value* (mp_wpn_power.value/100), DMG_ENERGYBLAST | DMG_NEVERGIB);

		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.33;
		m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 5, 7 );
		int fHitWorld = TRUE;

		if (pEntity)
		{
			if (pEntity->Classify() != CLASS_NONE)
			{
				switch( RANDOM_LONG(0,2) )
				{
					case 0: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/lightsaber_hit.wav", 1, ATTN_NORM); break;
					case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/lightsaber_hit2.wav", 1, ATTN_NORM); break;
					case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/lightsaber_hit3.wav", 1, ATTN_NORM); break;
				}
				fHitWorld = FALSE;
			}
		}
			if (fHitWorld)
			{
				FX_Explosion( tr.vecEndPos+(tr.vecPlaneNormal * 12), EXPLOSION_LIGHTSABER );
				UTIL_DecalTrace( &tr, DECAL_SCRATCH1 + RANDOM_FLOAT(0,3));
			}
		m_trHit = tr;
	}
	m_flRechargeTime = gpGlobals->time + 1.5;
}

void CLightsaber::WeaponIdle( void )
{
	if (m_pPlayer->pev->waterlevel == 3)
	{
		if (CVAR_GET_FLOAT("mp_noselfdamage")==1)
		return;

		entvars_t *pevOwner = VARS( pev->owner );
		::WaterRadiusDamage( pev->origin, pev, pevOwner, (dmg_lightsaber.value*2) * (mp_wpn_power.value/100), (dmg_lightsaber.value*2) * (mp_wpn_power.value/100), CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB );
		FX_Explosion( pev->origin, EXPLOSION_BUBBLES);
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/shockgrenade_explo.wav", 0.55, ATTN_NORM);
		m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = 0;
	}

	Reload();
	ReloadPistol();

	if ( gpGlobals->time >= m_flShockTime )
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "weapons/lightsaber_idle.wav", 0.9, ATTN_NORM );
		m_flShockTime = gpGlobals->time +3;
	}

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	if (m_fInAttack==1)
	{
		FireBall();
		m_fInAttack = 0;
		return;
	}

	SendWeaponAnim( LIGHTSABER_IDLE );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 7);
}