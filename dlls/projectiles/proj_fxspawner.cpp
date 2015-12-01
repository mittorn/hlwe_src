//player attached entity
//used to make "burn" and "freeze" effects work properly

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( fx_spawner_burn, CFXSpawner );

void CFXSpawner:: Spawn( void )
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 16;
	pev->renderamt = 0;
	pev->scale = 0.1;
	pev->health = 200;

	FX_Trail(pev->origin, entindex(), EFFECT_BURN);
	SetThink ( FollowThink );
	pev->nextthink = gpGlobals->time;
}

void CFXSpawner:: FollowThink( void )
{
	entvars_t *pevOwner = VARS( pev->owner );

	if(pev->health <=0 || ( UTIL_PointContents(pevOwner->origin) == CONTENT_WATER ) || pevOwner->armorvalue)
		Destroy();
	else
		pev->health--;

	UTIL_SetOrigin(pev, pevOwner->origin);
	pev->nextthink = gpGlobals->time+0.05;
}

void CFXSpawner::Destroy( void )
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}