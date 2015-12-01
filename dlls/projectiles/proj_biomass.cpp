#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "soundent.h"
#include "game.h"
#include "projectiles.h"
#include "decals.h"

#define SF_DETONATE	0x0001
LINK_ENTITY_TO_CLASS( biomass, CBiomass );

void CBiomass::Detonate( void )
{
	TraceResult tr;
	Vector vecEnd = pev->origin + pev->angles + gpGlobals->v_forward*20;
	UTIL_TraceLine( pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr );
	UTIL_DecalTrace( &tr, DECAL_OFSCORCH1 + RANDOM_FLOAT(0,5));

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*3.5, CLASS_NONE, DMG_ACID | DMG_POISON);

	FX_Explosion( tr.vecEndPos + (tr.vecPlaneNormal * 15), EXPLOSION_BIOMASS );
	UTIL_Remove( this );
}

void CBiomass::DetonateUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	Detonate( );
}

void CBiomass::Deactivate( void )
{
	Detonate( );
}

void CBiomass::SlideTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( this );
		return;
	}

	if (pOther->pev->takedamage && m_flNextAttack < gpGlobals->time)
	{
		entvars_t *pevOwner = VARS( pev->owner );
		switch (RANDOM_LONG(0,1))
		{
			case 0:	pOther->TakeDamage( pev, pevOwner, 1, DMG_POISON ); break;
			case 1:	pOther->TakeDamage( pev, pevOwner, 1, DMG_ACID ); break;
		}
		switch (RANDOM_LONG(0,1))
		{
			case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "debris/bustflesh1.wav", 1, ATTN_NORM); break;
			case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "debris/bustflesh2.wav", 1, ATTN_NORM); break;
		}
		m_flNextAttack = gpGlobals->time + 25;
	}
	else if ( pOther->pev->solid == SOLID_BSP || pOther->pev->movetype == MOVETYPE_PUSHSTEP )
	{
		FX_Explosion( pev->origin, EXPLOSION_BIOMASSIMPACT );
		switch (RANDOM_LONG(0,1))
		{
			case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "debris/bustflesh1.wav", 1, ATTN_NORM); break;
			case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "debris/bustflesh2.wav", 1, ATTN_NORM); break;
		}
	}

	pev->velocity = pev->velocity * 0.3;

	if (!b_attached && !pev->waterlevel)
	{
		b_attached = true;
		pev->velocity = pev->avelocity = g_vecZero;
		pev->movetype = MOVETYPE_FLY;
		pev->solid = SOLID_NOT;
		pev->enemy = pOther->edict();
		dist = pev->origin - pOther->pev->origin;

		if (pOther->IsPlayer())
		{
			angl_y = pOther->pev->v_angle.y;
		}
		else
		{
			angl_y = pOther->pev->angles.y;
			angl_x = pOther->pev->angles.x;
		}
	}
}

void CBiomass :: StayInWorld( void )
{
	pev->nextthink = gpGlobals->time + 0.05;

	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	pev->frags--;
	if (pev->frags <= 0)
	{
		Detonate();
		return;
	}

	StudioFrameAdvance( );

	if (pev->enemy != NULL)
	{
		CBaseEntity *owner = CBaseEntity :: Instance ( pev->enemy );

		if (owner == NULL)
		{
			b_attached = false;
			pev->enemy = NULL;
			pev->movetype = MOVETYPE_TOSS;
			pev->solid = SOLID_BBOX;
			return;
		}

		if (owner->IsPlayer() && !owner->IsAlive())
		{
			Detonate( );
			return;
		}
		
		if (owner->IsRespawnable() && owner->pev->health <= 0)
		{
			Detonate( );
			return;
		}

		float alpha,theta;

		if (owner->IsPlayer())
		{
			alpha = angl_y - owner->pev->v_angle.y;
			theta = 0;
		}
		else
		{
			alpha = angl_y - owner->pev->angles.y;
			theta = angl_x - owner->pev->angles.x;
		}

		alpha *= M_PI/180.0;
		theta *= M_PI/180.0;

		//Vector offset (dist.x * cos(alpha) + dist.y * sin(alpha), dist.y * cos(alpha) - dist.x * sin(alpha), dist.z);
		Vector offset (dist.x * cos(alpha) * cos(theta) + dist.y * sin(alpha) - dist.z * cos(alpha) * sin(theta),
					   dist.y * cos(alpha) - dist.x * sin(alpha) * cos(theta) + dist.z * sin(alpha) * sin(theta),
					   dist.x * sin(theta) + dist.z * cos(theta));

		if (owner->IsPlayer() && owner->pev->waterlevel > 1)
			offset.z = 0;

		//pev->origin = owner->pev->origin + offset;
		pev->velocity = (owner->pev->origin + offset - pev->origin)/max(0.05, gpGlobals->frametime);
		return;
	}
	else if (b_attached)
	{
		b_attached = false;
		pev->enemy = NULL;
		pev->movetype = MOVETYPE_TOSS;
		pev->solid = SOLID_BBOX;
		return;
	}

	if (pev->waterlevel == 3)
	{
		b_attached = false;
		pev->enemy = NULL;
		pev->movetype = MOVETYPE_TOSS;
		pev->solid = SOLID_BBOX;
	}
	else if (pev->waterlevel == 0)
		pev->movetype = MOVETYPE_BOUNCE;
	else
		pev->velocity.z -= 8;
}

void CBiomass:: Spawn( void )
{
	SET_MODEL(ENT(pev), "models/w_biomass.mdl");
	ResetSequenceInfo();
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "biomass" );
	pev->solid = SOLID_BBOX;
	pev->rendermode = kRenderTransTexture;
	pev->renderamt = 150;
	pev->scale = 1.5;
	pev->enemy = NULL;
	dist = g_vecZero;
	angl_x = angl_y = 0;
	b_attached = false;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CBiomass *CBiomass :: ShootBiomass( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float Time)
{
	CBiomass *pBiomass = GetClassPtr( (CBiomass *)NULL );
	UTIL_SetOrigin( pBiomass->pev, vecStart );
	pBiomass->Spawn();
	pBiomass->pev->velocity = vecVelocity + gpGlobals->v_right * RANDOM_FLOAT(-50,50) + gpGlobals->v_up * RANDOM_FLOAT(-50,50);
	pBiomass->pev->owner = ENT(pevOwner);
	pBiomass->SetThink( StayInWorld );
	pBiomass->pev->nextthink = gpGlobals->time + 0.1;
	pBiomass->SetUse( DetonateUse );
	pBiomass->SetTouch( SlideTouch );
	pBiomass->pev->spawnflags = SF_DETONATE;
	pBiomass->pev->frags = Time;
	pBiomass->pev->dmg = dmg_biorifle.value * (mp_wpn_power.value/100);

	return pBiomass;
}

void CBiomass :: UseBiomass( entvars_t *pevOwner, BIOMASSCODE code )
{
	edict_t *pentFind;
	edict_t *pentOwner;

	if ( !pevOwner )
		return;

	CBaseEntity	*pOwner = CBaseEntity::Instance( pevOwner );
	pentOwner = pOwner->edict();

	pentFind = FIND_ENTITY_BY_CLASSNAME( NULL, "biomass" );
	while ( !FNullEnt( pentFind ) )
	{
		CBaseEntity *pEnt = Instance( pentFind );
		if ( pEnt )
		{
			if ( FBitSet( pEnt->pev->spawnflags, SF_DETONATE ) && pEnt->pev->owner == pentOwner )
			{
				if ( code == BIOMASS_DETONATE )
					pEnt->Use( pOwner, pOwner, USE_ON, 0 );
				else	
					pEnt->pev->owner = NULL;
			}
		}
		pentFind = FIND_ENTITY_BY_CLASSNAME( pentFind, "biomass" );
	}
}