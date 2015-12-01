#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "soundent.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( trip, CTrip );

void CTrip::Explode( TraceResult *pTrace, int bitsDamageType )
{
	entvars_t *pevOwner = VARS( pev->owner );
	RadiusDamage ( pev, pevOwner, pev->dmg, CLASS_NONE, bitsDamageType );
	FX_Explosion( pTrace->vecEndPos + (pTrace->vecPlaneNormal * 20), EXPLOSION_TRIPMINE );
	UTIL_Remove( this );
}