#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "effects.h"
#include "customentity.h"

#define BLASTER_PULSE_INTERVAL	0.1

enum blaster_e
{
	BLASTER_IDLE,
	BLASTER_IDLE2,
	BLASTER_IDLE3,
	BLASTER_FIDGET,
	BLASTER_FIRE,
	BLASTER_FIRE_SOLID,
	BLASTER_FIREBEAM,
	BLASTER_DRAW,
	BLASTER_HOLSTER
};

class CBlaster : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);
	void Reload( void );

	BOOL Deploy( void );
	void Holster( );

	void CreateEffect ( void );
	void UpdateEffect( const Vector &startPoint, const Vector &endPoint, float timeBlend );
	void DestroyEffect ( void );

	void EndAttack( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void SwitchFireMode( void );
	void WeaponIdle( void );

	void ZoomUpdate( void );
	void ZoomReset( void );

	float m_flAmmoUseTime;
	void Fire( const Vector &vecSrc, const Vector &vecDirShooting);
	void SellWeapon( void );
	int AddDuplicate( CBasePlayerItem* );

	enum BLASTER_FIRESTATE { FIRE_OFF, FIRE_CHARGE};
private:
	float				m_shootTime;
	CBeam				*m_pBeam;
	BLASTER_FIRESTATE		m_fireState;
};
LINK_ENTITY_TO_CLASS( weapon_blaster, CBlaster );

int CBlaster::AddDuplicate( CBasePlayerItem *pOriginal )
{
	return false;//do not pick up second one
}

void CBlaster::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_BLASTER/2;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
}

void CBlaster::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_blaster.mdl");
	m_iId = WEAPON_BLASTER;
	firemode=FIREMODE_PULSE;
	m_iDefaultAmmo = 50;
	FallInit();
}

void CBlaster::Precache( void )
{
	PRECACHE_MODEL("models/v_blaster.mdl");
	PRECACHE_MODEL("models/p_blaster.mdl");

	PRECACHE_SOUND("weapons/blaster_fire.wav");
	PRECACHE_SOUND("weapons/blaster_run.wav");
}

BOOL CBlaster::Deploy( void )
{
	m_fireState = FIRE_OFF;
	m_iLastFiremode = -1;
	return DefaultDeploy( "models/v_blaster.mdl", "models/p_blaster.mdl", BLASTER_DRAW, "rifle", 0.8 );
}

void CBlaster::Holster()
{
	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	if ( m_fireState != FIRE_OFF ) EndAttack();
	m_fInReload = FALSE;

	if (m_pPlayer->m_iFOV != 90)
	ZoomReset();

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( BLASTER_HOLSTER );
}

int CBlaster::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "energy";
	p->iMaxAmmo1 = 50;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 8;
	p->iPosition = 0;
	p->iId = WEAPON_BLASTER;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY | ITEM_FLAG_NOAUTORELOAD;
	p->iWeight = BLASTER_WEIGHT;
	p->iMaxspeed = BLASTER_MAXSPEED;
	return 1;
}

void CBlaster::PrimaryAttack( void )
{
	if ( m_pPlayer->pev->waterlevel == 3 || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		if ( m_fireState != FIRE_OFF || m_pBeam )
		EndAttack();
		else
		{	
			m_flNextPrimaryAttack = gpGlobals->time + 0.5;
			PlayEmptySound(2);
			SendWeaponAnim( BLASTER_IDLE);
			return;
		}
	}

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );

	if(firemode==FIREMODE_PULSE)
	{
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
		m_pPlayer->FireBeam(m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 2 + gpGlobals->v_up * -3, gpGlobals->v_forward, BEAM_BLASTER, 0, m_pPlayer->pev );

		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?BLASTER_FIRE_SOLID:BLASTER_FIRE, 0, FIREGUN_BLASTER );

		if (!m_pPlayer->m_fHeavyArmor)
		{
			m_pPlayer->pev->punchangle.x -= 1.5;
			m_pPlayer->pev->punchangle.y -= 1.0;
		}
		m_flNextPrimaryAttack = gpGlobals->time + 0.15;
		m_flTimeWeaponIdle = gpGlobals->time + 5;
		m_flRechargeTime = gpGlobals->time + 2;
	}
	else

	switch( m_fireState )
	{
		case FIRE_OFF:
		{
			if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
			{
				PlayEmptySound(2);
				return;
			}

			m_flAmmoUseTime = UTIL_WeaponTimeBase();
			SendWeaponAnim(BLASTER_FIREBEAM);

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
			m_shootTime = UTIL_WeaponTimeBase() + 2;
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/blaster_run.wav", 0.9, ATTN_NORM, 0, 100 );
			FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 1, FIREGUN_BLASTER );

			pev->dmgtime = UTIL_WeaponTimeBase();
			m_fireState = FIRE_CHARGE;
		}
		break;

		case FIRE_CHARGE:
		{
			Fire( vecSrc, vecAiming );

			if ( m_shootTime != 0 && UTIL_WeaponTimeBase() > m_shootTime )
			m_shootTime = 0;

			if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
			{
				EndAttack();
				m_fireState = FIRE_OFF;
			}
		}
		break;
	}
}

void CBlaster::SecondaryAttack( void )
{
	ZoomUpdate();
}

void CBlaster :: ZoomUpdate( void )
{
	if (m_pPlayer->pev->button & IN_ATTACK2)
	{
		if(m_iChargeLevel == 0)
		{
			if (m_flShockTime > UTIL_WeaponTimeBase()) return;
			m_iChargeLevel = 1;
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/tau_zoom.wav", 1, ATTN_NORM);

			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
			WRITE_BYTE(MSG_SCOPE);
			WRITE_BYTE(4);
			WRITE_ELSE(1+1);
			MESSAGE_END();

			m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.001;
		}
		if(m_iChargeLevel == 1)
		{
			m_pPlayer->m_iFOV = 80;
			m_iChargeLevel = 2;
		}

		if (m_flTimeUpdate > UTIL_WeaponTimeBase()) return;
		if (m_iChargeLevel == 2 && m_pPlayer->m_iFOV > 12)
		{
			m_pPlayer->m_iFOV-=5;
			m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.001;
		}
		if(m_iChargeLevel == 3) ZoomReset();
	}
	else if(m_iChargeLevel > 1) m_iChargeLevel = 3;
}

void CBlaster::ZoomReset( void )
{
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/tau_zoom.wav", 1, ATTN_NORM);
	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_SCOPE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	m_flShockTime = UTIL_WeaponTimeBase() + 0.5;
	m_pPlayer->m_iFOV = 90;
	m_iChargeLevel = 0;
}

void CBlaster::SwitchFireMode( void )
{
	if ( m_fireState != FIRE_OFF )
	EndAttack();

	if(firemode==FIREMODE_PULSE)
	{
		SendWeaponAnim(BLASTER_IDLE);
		firemode=FIREMODE_BEAM;
	}
	else if(firemode==FIREMODE_BEAM)
	{
		SendWeaponAnim(BLASTER_IDLE);
		firemode=FIREMODE_PULSE;
	}
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.1;
}

void CBlaster::Fire( const Vector &vecSrc, const Vector &vecDirShooting )
{
	TraceResult tr;
	float flDamage = dmg_blaster_beam.value * (mp_wpn_power.value/100);
	float timedist = (pev->dmgtime - UTIL_WeaponTimeBase())/BLASTER_PULSE_INTERVAL;
	Vector vecEnd = vecSrc + vecDirShooting * 16384;
	Vector tmpSrc = vecSrc + gpGlobals->v_up * -8 + gpGlobals->v_right * 3;
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pev), &tr);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	int surface = (int)SURFACETYPE_Trace(&tr, vecSrc, vecEnd);

	if (UTIL_PointContents(tr.vecEndPos) != CONTENTS_SKY)
		FX_ImpBeam( tr.vecEndPos, tr.vecPlaneNormal, surface, IMPBEAM_BLASTERBEAM );

	if ( pev->dmgtime < UTIL_WeaponTimeBase() )
	{
		if ( gpGlobals->time >= m_flAmmoUseTime )
		{
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
			m_flAmmoUseTime = gpGlobals->time + 0.4;
		}
		pev->dmgtime = UTIL_WeaponTimeBase() + BLASTER_PULSE_INTERVAL;

		if (pEntity->pev->takedamage)
		{
			ClearMultiDamage();
			pEntity->TraceAttack( m_pPlayer->pev, flDamage, vecEnd, &tr, DMG_ENERGYBEAM | DMG_NEVERGIB);
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
		}
	}

	if ( timedist < 0 )      timedist = 0;
	else if ( timedist > 1 ) timedist = 1;
	timedist = 1-timedist;
	UpdateEffect( tmpSrc, tr.vecEndPos, timedist );
}


void CBlaster::UpdateEffect( const Vector &startPoint, const Vector &endPoint, float timeBlend )
{
	if ( !m_pBeam ) CreateEffect();

	m_pBeam->SetStartPos( endPoint );
	m_pBeam->SetBrightness( 255 - (timeBlend*180) );
	m_pBeam->SetWidth(20 -(timeBlend*10));
	m_pBeam->SetColor(220 + (25*timeBlend), 0 + (30*timeBlend), 12 + 8*fabs(sin(UTIL_WeaponTimeBase()*10)));
}

void CBlaster::CreateEffect( void )
{
	DestroyEffect();

	m_pBeam = CBeam::BeamCreate("sprites/laserbeam.spr", 15);
	m_pBeam->PointEntInit( pev->origin, m_pPlayer->entindex());
	m_pBeam->SetEndAttachment(1);
	m_pBeam->pev->spawnflags |= SF_BEAM_TEMPORARY;
	m_pBeam->SetScrollRate(110);
	m_pBeam->SetNoise(0.1);
}

void CBlaster::DestroyEffect( void )
{
	if ( m_pBeam )
	{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;
	}
}

void CBlaster::EndAttack( void )
{
	SendWeaponAnim(BLASTER_IDLE);
	STOP_SOUND( ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/blaster_run.wav" );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_REMOVE );

	m_fireState = FIRE_OFF;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase();
	m_flRechargeTime = gpGlobals->time + 2;
	DestroyEffect();
}

void CBlaster::WeaponIdle( void )
{
	ZoomUpdate();
	Reload();

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

	if (m_fireState != FIRE_OFF)
	EndAttack();

	if (m_pPlayer->m_iFOV != 90)
		return;

	switch (RANDOM_LONG(0,3))
	{
		case 0:	SendWeaponAnim(BLASTER_IDLE); break;
		case 1:	SendWeaponAnim(BLASTER_IDLE2); break;
		case 2:	SendWeaponAnim(BLASTER_IDLE3); break;
		case 3:	SendWeaponAnim(BLASTER_FIDGET); break;
	}
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT(10,15);
}

void CBlaster::Reload( void )
{
	if ( m_fireState != FIRE_OFF )
		return;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 50)
		return;

	while (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 50 && m_flRechargeTime < gpGlobals->time)
	{
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]++;
		m_flRechargeTime += 0.8;
	}
}