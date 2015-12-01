#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "soundent.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( monster_satchel, CSCharge );

// SCharges flagged with this will be triggered when the owner calls detonateSatchelCharges
#define SF_DETONATE		0x0001

void CSCharge::Detonate( void )
{
	TraceResult tr;
	Vector vecSpot = pev->origin + Vector (0 ,0 ,8);
	Vector vecEnd =  pev->origin + Vector (0 ,0 ,-40);
	UTIL_TraceLine(vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr);

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_SMALEXP, (float)tex );

	FX_Explosion( pev->origin+Vector(0,0,25), EXPLOSION_SATCHEL );

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*2, CLASS_NONE, DMG_BLAST );
	UTIL_Remove( this );
}

void CSCharge::DetonateUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	Detonate( );
}

void CSCharge::Deactivate( void )
{
	Detonate( );
}

void CSCharge::SlideTouch( CBaseEntity *pOther )
{
	if ( pOther->edict() == pev->owner )
		return;

	// HACKHACK - On ground isn't always set, so look for ground underneath
	TraceResult tr1;
	UTIL_TraceLine( pev->origin, pev->origin - Vector(0,0,10), ignore_monsters, edict(), &tr1 );

	if ( tr1.flFraction < 1.0 )
	{
		pev->velocity = pev->velocity * 0.95;
		pev->avelocity = pev->avelocity * 0.9;
	}
	if ( !(pev->flags & FL_ONGROUND) && pev->velocity.Length2D() > 10 )
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce1.wav", 0.5, ATTN_NORM);

	StudioFrameAdvance( );
	pev->angles.x = pev->angles.z = 0;
	UTIL_MakeVectors ( pev->angles );

	Vector right, forward, up = Vector( 0, 0, 20 );
	TraceResult tr2;
	float angle_z;

	UTIL_TraceLine ( pev->origin + gpGlobals->v_right * 5 + up, pev->origin - up + gpGlobals->v_right * 5, ignore_monsters, ENT(pev), &tr2 );

	if ( tr1.flFraction < 1.0 && tr2.flFraction < 1.0 )
	{
		up = tr1.vecPlaneNormal;
		right = ( tr2.vecEndPos - tr1.vecEndPos ).Normalize ();
		forward = CrossProduct ( up, right );

		//hack: acos ranges from 0 to M_PI
		int sgn = ( gpGlobals->v_right.z > right.z ) ? 1 : -1;

		angle_z = ( acos ( DotProduct ( gpGlobals->v_right, right ) ) * 180.0f ) / M_PI;

		pev->angles = UTIL_VecToAngles ( forward );
		pev->angles.z = angle_z * sgn;
	}
}

void CSCharge :: SatchelThink( void )
{
	StudioFrameAdvance( );

	pev->frags--;
	if (pev->frags <= 0)
		Detonate();

	pev->nextthink = gpGlobals->time + 0.5;
}

void CSCharge:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "monster_satchel" );
	pev->solid = SOLID_BBOX;
	SET_MODEL(ENT(pev), "models/p_satchel.mdl");
	pev->gravity = 2;
	pev->friction = 0.8;
	pev->scale = 1.5;
	pev->frags = 240;

	pev->dmg = dmg_satchel.value * (mp_wpn_power.value/100);
	pev->sequence = 1;
	UTIL_SetSize(pev, Vector( -4, -4, -4), Vector(4, 4, 4));
	UTIL_SetOrigin( pev, pev->origin );
}

CSCharge * CSCharge :: ShootSCharge( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CSCharge *pSCharge = GetClassPtr( (CSCharge *)NULL );
	UTIL_SetOrigin( pSCharge->pev, vecStart );
	pSCharge->Spawn();
	pSCharge->pev->velocity = vecVelocity;
	pSCharge->pev->angles = g_vecZero;
	pSCharge->pev->owner = ENT(pevOwner);
	pSCharge->pev->avelocity.y = 400;
	
	pSCharge->SetThink( SatchelThink );
	pSCharge->pev->nextthink = gpGlobals->time + 0.1;
	pSCharge->SetUse( DetonateUse );
	pSCharge->SetTouch( SlideTouch );
	pSCharge->pev->spawnflags = SF_DETONATE;
	return pSCharge;
}

void CSCharge :: UseSatchelCharges( entvars_t *pevOwner, SATCHELCODE code )
{
	edict_t *pentFind;
	edict_t *pentOwner;

	if ( !pevOwner )
		return;

	CBaseEntity	*pOwner = CBaseEntity::Instance( pevOwner );
	pentOwner = pOwner->edict();

	pentFind = FIND_ENTITY_BY_CLASSNAME( NULL, "scharge" );
	while ( !FNullEnt( pentFind ) )
	{
		CBaseEntity *pEnt = Instance( pentFind );
		if ( pEnt )
		{
			if ( FBitSet( pEnt->pev->spawnflags, SF_DETONATE ) && pEnt->pev->owner == pentOwner )
			{
				if ( code == SATCHEL_DETONATE )
					pEnt->Use( pOwner, pOwner, USE_ON, 0 );
				else	// SATCHEL_RELEASE
					pEnt->pev->owner = NULL;
			}
		}
		pentFind = FIND_ENTITY_BY_CLASSNAME( pentFind, "scharge" );
	}
}