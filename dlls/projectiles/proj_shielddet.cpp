#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( pshield_det, CPshieldDet );

void CPshieldDet :: Spawn( void )
{
	pev->effects |= EF_NODRAW;
	pev->dmg = dmg_shielddet.value * (mp_wpn_power.value/100);
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	SetThink ( DamageThink );
	pev->nextthink = gpGlobals->time + 0.1;
}

void CPshieldDet :: DamageThink( void )
{
	FX_Explosion( pev->origin, EXPLOSION_PSHIELD );
	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*3, CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB);
	UTIL_Remove(this);
}