#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "projectiles.h"

#define		TURRET_DEADZONE_FOV		45 //+- 45 degrees

class CTurretkit : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);
	void WeaponIdle( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( void );
	void Holster( void );
	void StartFire( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
	int SecondaryAmmoIndex( void );
};
LINK_ENTITY_TO_CLASS( weapon_turretkit, CTurretkit );

void CTurretkit::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_TURRETKIT/2;
}

int CTurretkit::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void CTurretkit::BuyPrimaryAmmo( void )
{
	BuyAmmo(80, (char*)pszAmmo1(), COST_ENERGY2_AMMO);
}

enum turretkit_e
{
	TURRETKIT_IDLE_P,
	TURRETKIT_IDLE_W,
	TURRETKIT_IDLE_V
};

void CTurretkit::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_turretkit.mdl");
	firemode=FIREMODE_SENTRYTURRET;
	m_iId = WEAPON_TURRETKIT;
	m_iDefaultAmmo = 80;
	m_iDefaultAmmo2 = 3;
	FallInit();
}

void CTurretkit::Precache( void )
{
	PRECACHE_MODEL("models/p_turretkit.mdl");

	PRECACHE_SOUND("items/turret_mount.wav");
	PRECACHE_SOUND("weapons/psp_fire.wav");
	PRECACHE_SOUND("weapons/psp_blast.wav");
}

int CTurretkit::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "energypack";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = "turret";
	p->iMaxAmmo2 = 3;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iSlot = 8;
	p->iPosition = 5;
	p->iId = WEAPON_TURRETKIT;
	p->iWeight = TURRETKIT_WEIGHT;
	p->iMaxspeed = TURRETKIT_MAXSPEED;
	return 1;
}

BOOL CTurretkit::Deploy( )
{
	m_iLastFiremode = -1;
	return DefaultDeploy( "models/p_turretkit.mdl", "models/p_turretkit.mdl", TURRETKIT_IDLE_V, "grenade" );
}

void CTurretkit::Holster( )
{
	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
}

void CTurretkit::PrimaryAttack()
{
	TraceResult	tr;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	UTIL_TraceLine(m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 100, dont_ignore_monsters, ENT(pev), &tr);

	if (tr.flFraction != 1.0 || m_pPlayer->pev->waterlevel == 3 || UTIL_PointContents(tr.vecEndPos) == CONTENTS_WATER)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Can't mount TURRET here!"));
		return;
	}
	if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
 	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("No TURRETS awailable!"));
		return;
	}

	Vector PlrOrig = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 50;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	if(firemode==FIREMODE_SENTRYTURRET)
	{
		if(m_pPlayer->m_fTurretSentry)
		{
			ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have SENTRY TURRET!"));
			return;
		}
		CBaseEntity *pSentry = CBaseEntity::Create( "monster_turret_sentry", PlrOrig, Vector(0,0,0), m_pPlayer->edict());
		m_pPlayer->m_fTurretSentry = 150;
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/turret_mount.wav", 1, ATTN_NORM);
	}
	else if(firemode==FIREMODE_MISSILETURRET)
	{
		if(m_pPlayer->m_fTurretMissile)
		{
			ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have MISSILE TURRET!"));
			return;
		}
		CBaseEntity *pSentry = CBaseEntity::Create( "monster_turret_missile", PlrOrig, Vector(0,0,0), m_pPlayer->edict());
		m_pPlayer->m_fTurretMissile = 200;
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/turret_mount.wav", 1, ATTN_NORM);
	}
	else if(firemode==FIREMODE_IONTURRET)
	{
		if(m_pPlayer->m_fTurretIon)
		{
			ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have ION TURRET!"));
			return;
		}
		CBaseEntity *pSentry = CBaseEntity::Create( "monster_turret_ion", PlrOrig, Vector(0,0,0), m_pPlayer->edict());
		m_pPlayer->m_fTurretIon = 250;
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/turret_mount.wav", 1, ATTN_NORM);
	}
	m_pPlayer->m_flNextAttack = gpGlobals->time + 1.5;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);
}

void CTurretkit::SecondaryAttack()
{
	if(firemode==FIREMODE_SENTRYTURRET)
		firemode = FIREMODE_MISSILETURRET;
	else if(firemode==FIREMODE_MISSILETURRET)
		firemode = FIREMODE_IONTURRET;
	else if(firemode==FIREMODE_IONTURRET)
		firemode = FIREMODE_SENTRYTURRET;

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(firemode);
	WRITE_ELSE(1+1);
	MESSAGE_END();
	m_iLastFiremode = firemode;
	m_flNextSecondaryAttack = gpGlobals->time + 0.2;
}

void CTurretkit::StartFire (void)
{
	entvars_t *pevOwner = VARS(pev->owner);
	CBaseEntity *pOther = NULL;
	CBaseEntity *pPlayer = CBaseEntity::Instance(pev->owner);

	CBaseEntity *pTarget = NULL;
	int dist = 4096;

	TraceResult tr, tr2;

	while ((pOther = UTIL_FindEntityInSphere( pOther, pev->origin, 4096 )) != NULL)
	{
		if ( pOther->edict() != pev->owner && pOther->pev->takedamage == DAMAGE_AIM && pOther->pev->deadflag != DEAD_DEAD && pOther->pev->health >=1 && g_pGameRules->PlayerRelationship(pPlayer, pOther) != GR_TEAMMATE && pOther->Classify() != CLASS_MACHINE )
		{
			UTIL_MakeVectors( m_pPlayer->pev->v_angle );

			UTIL_TraceLine( m_pPlayer->pev->origin + gpGlobals->v_forward * -8 + gpGlobals->v_right * 8 + gpGlobals->v_up * 28, pOther->pev->origin+Vector(0,0,30), dont_ignore_monsters, ENT(pev), &tr );

			if (tr.pHit != pOther->edict()) continue;//check borders between gun and target

			float my_angle = UTIL_VecToAngles( pOther->pev->origin+Vector(0,0,30) - m_pPlayer->pev->origin - gpGlobals->v_forward * -8 - gpGlobals->v_right * 8 - gpGlobals->v_up * 28 ).y;

			if (fabs(UTIL_AngleDiff (m_pPlayer->pev->angles.y + 90, my_angle)) < TURRET_DEADZONE_FOV) continue;//don't shoot at player's head

			int tmp;

			if ((tmp = (pOther->pev->origin - pev->origin).Length()) < dist)//choose nearest target
			{
				pTarget = pOther;
				dist = tmp;
				tr2 = tr;
			}
		}
	}

	if (pTarget != NULL)
	{
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		FX_FireBeam(m_pPlayer->GetGunPosition()+gpGlobals->v_forward * -8 + gpGlobals->v_right * 8 + gpGlobals->v_up * 3, tr2.vecEndPos, tr2.vecPlaneNormal, BEAM_PSP );
		FX_ImpBeam( tr2.vecEndPos, tr2.vecPlaneNormal, pTarget->pev->armorvalue?SURFACE_ENERGYARMOR:SURFACE_FLESH, 0);
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_TURRETKIT );

		ClearMultiDamage( );
		pTarget->TraceAttack( m_pPlayer->pev, dmg_pds.value * (mp_wpn_power.value/100), pev->velocity, &tr2, DMG_PLASMA | DMG_NEVERGIB);
		ApplyMultiDamage( pev, m_pPlayer->pev );
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
	}
}

void CTurretkit::WeaponIdle( void )
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

	if ( UTIL_PointContents(pev->origin) != CONTENT_WATER && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
	{
		if ( gpGlobals->time >= m_flShockTime )
		{
			StartFire();
			m_flShockTime = gpGlobals->time + 0.8;
		}
	}

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	SendWeaponAnim(TURRETKIT_IDLE_V);
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}