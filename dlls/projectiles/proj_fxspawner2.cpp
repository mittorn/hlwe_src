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

LINK_ENTITY_TO_CLASS( fx_spawner_freeze, CFXSpawner2 );

void CFXSpawner2:: Spawn( void )
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->renderfx = kRenderFxGlowShell;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 1;
	pev->scale = 2.0;
	pev->health = 300;
	pev->angles.x = -90;
	pev->body = 1;

	FX_Trail(pev->origin, entindex(), EFFECT_FREEZE);
	SetThink ( FollowThink );
	pev->nextthink = gpGlobals->time;
}

void CFXSpawner2:: FollowThink( void )
{
	entvars_t *pevOwner = VARS( pev->owner );

	if(pevOwner->health <=1 ||  pev->health <=0 || ( UTIL_PointContents(pevOwner->origin) == CONTENT_WATER ) || pevOwner->armorvalue)
		Destroy();
	else
		pev->health--;

	pev->angles.x = -90;

	pev->nextthink = gpGlobals->time+0.05;
}

void CFXSpawner2::Destroy( void )
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}