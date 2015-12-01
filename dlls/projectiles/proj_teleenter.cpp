#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( teleenter, CTeleenter );

void CTeleenter :: Spawn( void )
{
	SET_MODEL(ENT(pev), "sprites/anim_spr8.spr");	
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 5;
	pev->movetype = MOVETYPE_NONE;
}

void CTeleenter :: MakeBlast( void )
{
	FX_Trail( pev->origin, entindex(), UTIL_PointContents(pev->origin)==CONTENTS_WATER?PROJ_TELEENTER_DETONATE_WATER:PROJ_TELEENTER_DETONATE);
	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*1.5, CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB);
	UTIL_Remove( this );
}

CTeleenter *CTeleenter::ShootTeleenter( entvars_t *pevOwner, Vector vecStart)
{
	CTeleenter *pTeleenter = GetClassPtr( (CTeleenter *)NULL );
	pTeleenter->Spawn();

	UTIL_SetOrigin( pTeleenter->pev, vecStart );
	pTeleenter->pev->owner = ENT(pevOwner);
	pTeleenter->SetThink ( MakeBlast );

	if (UTIL_PointContents(vecStart) == CONTENTS_WATER)
		pTeleenter->pev->nextthink = gpGlobals->time + 0.1;
	else
		pTeleenter->pev->nextthink = gpGlobals->time + 2.0;

	pTeleenter->pev->dmg = dmg_teleenter.value * (mp_wpn_power.value/100);
	FX_Trail(pTeleenter->pev->origin, pTeleenter->entindex(), PROJ_TELEENTER);
	return pTeleenter;
}
