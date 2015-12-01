#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "gamerules.h"
#include "game.h"
#include "projectiles.h"
#include "customentity.h"

LINK_ENTITY_TO_CLASS( energycharge, CEnergycharge );

void CEnergycharge::ExplodeTouch( CBaseEntity *pOther )
{
	BlastOff();

	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( this );
		return;
	}
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine( vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr );
	UTIL_DecalTrace(&tr, DECAL_OFSMSCORCH1 + RANDOM_FLOAT(0,2));

	entvars_t *pevOwner = VARS( pev->owner );

	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		FX_Trail( pev->origin, entindex(), PROJ_ENERGYCHARGE_DETONATE_WATER);
		::WaterRadiusDamage( pev->origin, pev, pevOwner, (dmg_smartgun.value*2) * (mp_wpn_power.value/100), (dmg_smartgun.value*2) * (mp_wpn_power.value/100), CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB);
		UTIL_Remove( this );
	return;
	}

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack( pevOwner, pev->dmg, (pOther->pev->origin - pev->origin).Normalize(), &tr, DMG_ENERGYBLAST | DMG_NEVERGIB );
		ApplyMultiDamage( pev, pevOwner );
	}

	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 10), entindex(), PROJ_ENERGYCHARGE_DETONATE);
	UTIL_Remove( this );
}

void CEnergycharge::BlastOn ( void )
{
	Vector	posGun, angleGun;
	TraceResult trace;

	m_pBeam = CBeam::BeamCreate( "sprites/plasma.spr", 30 );
	GetAttachment( 1, posGun, angleGun );
	GetAttachment( 2, posGun, angleGun );

	Vector vecEnd = (gpGlobals->v_forward * 60) + posGun;
	UTIL_TraceLine( posGun, vecEnd, dont_ignore_monsters, edict(), &trace );

	m_pBeam->EntsInit( entindex(), entindex() );
	m_pBeam->SetStartAttachment( 1 );
	m_pBeam->SetEndAttachment( 2 );
	m_pBeam->SetBrightness( 190 );
	m_pBeam->SetScrollRate( 20 );
	m_pBeam->SetNoise( 20 );
	m_pBeam->SetFlags( BEAM_FSHADEOUT );
	m_pBeam->SetColor( 35, 214, 177 );

	m_pNoise = CBeam::BeamCreate( "sprites/plasma.spr", 30 );

	GetAttachment( 1, posGun, angleGun );
	GetAttachment( 2, posGun, angleGun );

	UTIL_TraceLine( posGun, vecEnd, dont_ignore_monsters, edict(), &trace );

	m_pNoise->EntsInit( entindex(), entindex() );
	m_pNoise->SetStartAttachment( 1 );
	m_pNoise->SetEndAttachment( 2 );
	m_pNoise->SetBrightness( 190 );
	m_pNoise->SetScrollRate( 20 );
	m_pNoise->SetNoise( 65 );
	m_pNoise->SetFlags( BEAM_FSHADEOUT );
	m_pNoise->SetColor( 255, 255, 173 );
	EXPORT RelinkBeam();
}

void CEnergycharge::BlastOff ( void )
{
	UTIL_Remove( m_pBeam );
	m_pBeam = NULL;
	UTIL_Remove( m_pNoise );
	m_pNoise = NULL;
}

void CEnergycharge::Spawn( )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "Energycharge" );
	pev->solid = SOLID_BBOX;
	SET_MODEL(ENT(pev), "models/energycharge_effect.mdl");

	BlastOn();
	pev->nextthink = gpGlobals->time + 0.1;
}

CEnergycharge *CEnergycharge::ShootEnergycharge( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CEnergycharge*pEnergycharge= GetClassPtr( (CEnergycharge*)NULL );
	pEnergycharge->Spawn();

	UTIL_SetOrigin( pEnergycharge->pev, vecStart );
	pEnergycharge->pev->velocity = vecVelocity;
	pEnergycharge->pev->angles = UTIL_VecToAngles (pEnergycharge->pev->velocity);
	pEnergycharge->pev->owner = ENT(pevOwner);
	pEnergycharge->pev->nextthink = gpGlobals->time;
	pEnergycharge->SetTouch( ExplodeTouch );
	pEnergycharge->pev->dmg = 50;
	return pEnergycharge;
}