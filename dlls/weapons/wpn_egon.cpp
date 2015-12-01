#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "effects.h"
#include "customentity.h"

#define EGON_PULSE_INTERVAL		0.1
#define EGON_DISCHARGE_INTERVAL		0.1

enum egon_e
{
	EGON_IDLE,
	EGON_IDLE2,
	EGON_FIDGET,
	EGON_ALTFIREON,
	EGON_ALTFIREOFF,
	EGON_FIRE,
	EGON_FIRE2,
	EGON_FIRE3,
	EGON_DRAW,
	EGON_HOLSTER,
	EGON_RELOAD,
	EGON_SPIN,
	EGON_FIRENOVA
};

class CEgon : public CBasePlayerWeapon
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
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );

	float m_flAmmoUseTime;
	void Fire( const Vector &vecOrigSrc, const Vector &vecDir );

	enum EGON_FIRESTATE { FIRE_OFF, FIRE_CHARGE };
private:
	float			m_shootTime, m_shakeTime;
	CBeam			*m_pBeam;
	CBeam			*m_pNoise;
	EGON_FIRESTATE		m_fireState;
};

LINK_ENTITY_TO_CLASS( weapon_egon, CEgon );

void CEgon::BuyPrimaryAmmo( void )
{
	BuyAmmo(60, (char*)pszAmmo1(), COST_ENERGY2_AMMO);
}

void CEgon::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_EGON/2;
}

void CEgon::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_egon.mdl");
	m_iId = WEAPON_EGON;
	firemode=FIREMODE_WIDE;
	m_iDefaultAmmo = 120;
	pev->weapons = CLIP_EGON;
	FallInit();
}

void CEgon::Precache( void )
{
	PRECACHE_MODEL("models/v_egon.mdl");
	PRECACHE_MODEL("models/p_egon.mdl");

	PRECACHE_SOUND( "weapons/egon_off1.wav" );
	PRECACHE_SOUND( "weapons/egon_run3.wav" );
	PRECACHE_SOUND( "weapons/egon_windup2.wav" );
}

BOOL CEgon::Deploy( void )
{
	m_fireState = FIRE_OFF;
	m_iLastFiremode = -1;
	return DefaultDeploy( "models/v_egon.mdl", "models/p_egon.mdl", EGON_DRAW, "egon" );
}

void CEgon::Holster()
{
	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	if ( m_fireState != FIRE_OFF ) EndAttack();
	m_fInReload = FALSE;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.7;
	SendWeaponAnim( EGON_HOLSTER );
}

int CEgon::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium238";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 60;
	p->iSlot = 6;
	p->iPosition = 2;
	p->iId = WEAPON_EGON;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = EGON_WEIGHT;
	p->iMaxspeed = EGON_MAXSPEED;
	return 1;
}

void CEgon::PrimaryAttack( void )
{
	if ( m_pPlayer->pev->waterlevel== 3 || m_iClip <= 0)
	{
	if ( m_fireState != FIRE_OFF || m_pBeam ) EndAttack();
		else
	  PlayEmptySound( );
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.08;
	SendWeaponAnim( EGON_IDLE);
	return;
	}

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );

	switch( m_fireState )
	{
		case FIRE_OFF:
		{
			if (m_iClip <= 0)
			{
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.08;
				PlayEmptySound( );
				return;
			}

			m_flAmmoUseTime = UTIL_WeaponTimeBase();
			SendWeaponAnim( EGON_FIRE + RANDOM_LONG(0,2));
		       	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_EGON );

			m_shakeTime = 0;

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
			m_shootTime = UTIL_WeaponTimeBase() + 2;

			if ( firemode == FIREMODE_WIDE )
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/egon_windup2.wav", 0.98, ATTN_NORM, 0, 130 );
			else
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/egon_windup2.wav", 0.9, ATTN_NORM, 0, 100 );

			pev->dmgtime = UTIL_WeaponTimeBase() + EGON_PULSE_INTERVAL;
			m_fireState = FIRE_CHARGE;
		}
		break;

		case FIRE_CHARGE:
		{
			Fire( vecSrc, vecAiming );

			if ( m_shootTime != 0 && UTIL_WeaponTimeBase() > m_shootTime )
			{
				if ( firemode == FIREMODE_WIDE )
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/egon_run3.wav", 0.98, ATTN_NORM, 0, 130 );
				else
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/egon_run3.wav", 0.9, ATTN_NORM, 0, 100 );
				m_shootTime = 0;
			}

			if (m_iClip <= 0)
			{
				EndAttack();
				m_fireState = FIRE_OFF;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
			}

		}
		break;
	}
}

void CEgon::SecondaryAttack( void )
{
	if ( m_fireState != FIRE_OFF )
	EndAttack();

	if(firemode==FIREMODE_WIDE)
	{
		SendWeaponAnim(EGON_FIDGET);
		firemode = FIREMODE_NARROW;
		m_flTimeUpdate = UTIL_WeaponTimeBase() + 1.1;
	}
	else if(firemode==FIREMODE_NARROW)
	{
		SendWeaponAnim(EGON_FIDGET);
		firemode = FIREMODE_WIDE;
		m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.7;
	}
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.8;
}

void CEgon::Fire( const Vector &vecOrigSrc, const Vector &vecDir )
{
	TraceResult tr;
	Vector vecDest;
	float timedist;

	if ( firemode == FIREMODE_WIDE )
		vecDest = vecOrigSrc + vecDir * 2000;
	else
		vecDest = vecOrigSrc + vecDir * 3500;

	Vector tmpSrc = vecOrigSrc + gpGlobals->v_up * -8 + gpGlobals->v_right * 3;
	UTIL_TraceLine( vecOrigSrc, vecDest, dont_ignore_monsters, m_pPlayer->edict(), &tr );
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

	if (tr.fAllSolid)
	return;

	if (pEntity == NULL)
	return;

	if (UTIL_PointContents(tr.vecEndPos) != CONTENTS_SKY)
	{
		if (tr.flFraction != 1.0)
			FX_ImpBeam( tr.vecEndPos, tr.vecPlaneNormal, 1, IMPBEAM_EGON );
	}
	switch ( firemode )
	{
	case FIREMODE_NARROW:
		if ( pev->dmgtime < UTIL_WeaponTimeBase() )
		{
			// Narrow mode only does damage to the entity it hits
			if (pEntity->pev->takedamage)
			{
				ClearMultiDamage();
				pEntity->TraceAttack( m_pPlayer->pev, dmg_egon_narrow.value * (mp_wpn_power.value/100), vecDir, &tr, DMG_ENERGYBEAM  | DMG_NEVERGIB);
				ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
			}
			if ( gpGlobals->time >= m_flAmmoUseTime )
			{
				m_iClip--;
				m_iFiredAmmo++;
				m_flAmmoUseTime = gpGlobals->time + 0.3;
			}
			pev->dmgtime = UTIL_WeaponTimeBase() + EGON_PULSE_INTERVAL;
		}
		timedist = ( pev->dmgtime - UTIL_WeaponTimeBase() ) / EGON_PULSE_INTERVAL;
	break;
	
	case FIREMODE_WIDE:
		if ( pev->dmgtime < UTIL_WeaponTimeBase() )
		{
			// wide mode does damage to the ent, and radius damage
			::RadiusDamage( tr.vecEndPos, pev, m_pPlayer->pev, dmg_egon_wide.value * (mp_wpn_power.value/100), 128, CLASS_NONE, DMG_PLASMA | DMG_NEVERGIB);
			if (pEntity->pev->takedamage)
			{
				ClearMultiDamage();
				pEntity->TraceAttack( m_pPlayer->pev, dmg_egon_wide.value * (mp_wpn_power.value/100), vecDir, &tr, DMG_PLASMA | DMG_NEVERGIB);
				ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
			}

			if ( !m_pPlayer->IsAlive() )
			return;

			if ( gpGlobals->time >= m_flAmmoUseTime )
			{
				m_iClip--;
				m_iFiredAmmo++;
				m_flAmmoUseTime = gpGlobals->time + 0.2;
			}

			pev->dmgtime = UTIL_WeaponTimeBase() + EGON_DISCHARGE_INTERVAL;
			if ( m_shakeTime < UTIL_WeaponTimeBase() )
			{
				UTIL_ScreenShake( tr.vecEndPos, 5.0, 150.0, 0.75, 250.0 );
				m_shakeTime = UTIL_WeaponTimeBase() + 1.5;
			}
		}
		timedist = ( pev->dmgtime - UTIL_WeaponTimeBase() ) / EGON_DISCHARGE_INTERVAL;
		break;
	}

	if ( timedist < 0 )      timedist = 0;
	else if ( timedist > 1 ) timedist = 1;
	timedist = 1-timedist;

	UpdateEffect( tmpSrc, tr.vecEndPos, timedist );
}


void CEgon::UpdateEffect( const Vector &startPoint, const Vector &endPoint, float timeBlend )
{
	if ( !m_pBeam ) CreateEffect();

	m_pBeam->SetStartPos( endPoint );
	m_pBeam->SetBrightness( 255 - (timeBlend*180) );
	m_pBeam->SetWidth( 40 - (timeBlend*20) );

	if ( firemode == FIREMODE_WIDE )
		m_pBeam->SetColor( 30 + (25*timeBlend), 30  + (30*timeBlend), 64 + 80*fabs(sin(UTIL_WeaponTimeBase()*10)) );
	else
		m_pBeam->SetColor( 60 + (25*timeBlend), 120 + (30*timeBlend), 64 + 80*fabs(sin(UTIL_WeaponTimeBase()*10)) );

	m_pNoise->SetStartPos( endPoint );
}

void CEgon::CreateEffect( void )
{
	DestroyEffect();

	m_pBeam = CBeam::BeamCreate( "sprites/rings_all.spr", 35 );
	m_pBeam->PointEntInit( pev->origin, m_pPlayer->entindex() );
	m_pBeam->SetFlags( BEAM_FSINE );
	m_pBeam->SetFrame( 6 );
	m_pBeam->SetEndAttachment( 1 );
	m_pBeam->pev->spawnflags |= SF_BEAM_TEMPORARY;// Flag these to be destroyed on save/restore or level transition

	m_pNoise = CBeam::BeamCreate( "sprites/rings_all.spr", 35 );
	m_pNoise->PointEntInit( pev->origin, m_pPlayer->entindex() );
	m_pNoise->SetFrame( 14 );
	m_pNoise->SetScrollRate( 25 );
	m_pNoise->SetBrightness( 250 );
	m_pNoise->SetEndAttachment( 1 );
	m_pNoise->pev->spawnflags |= SF_BEAM_TEMPORARY;

	if ( firemode == FIREMODE_WIDE )
	{
		m_pBeam->SetScrollRate( 50 );
		m_pBeam->SetNoise( 20 );
		m_pNoise->SetColor( 50, 50, 255 );
		m_pNoise->SetNoise( 8 );
	}
	else
	{
		m_pBeam->SetScrollRate( 110 );
		m_pBeam->SetNoise( 5 );
		m_pNoise->SetColor( 80, 120, 255 );
		m_pNoise->SetNoise( 2 );
	}
}

void CEgon::DestroyEffect( void )
{
	if ( m_pBeam )
	{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;
	}
	if ( m_pNoise )
	{
		UTIL_Remove( m_pNoise );
		m_pNoise = NULL;
	}
}

void CEgon::EndAttack( void )
{
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_REMOVE );
	STOP_SOUND(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/egon_run3.wav" );
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/egon_off1.wav", 0.98, ATTN_NORM, 0, 100); 

	m_fireState = FIRE_OFF;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	DestroyEffect();
}

void CEgon::WeaponIdle( void )
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

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
	return;

	if ( m_fireState != FIRE_OFF )
	EndAttack();

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = EGON_IDLE2;	
		break;
	
	default:
	case 1:
		iAnim = EGON_IDLE;
		break;
	}

	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + RANDOM_FLOAT(10,15);
}

void CEgon::Reload( void )
{
	if (m_iClip)
	return;

	DefaultReload( 60, EGON_RELOAD, 7.7, 1.6 );
}
