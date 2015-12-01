#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "shake.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( detpack, CDetpack );

void CDetpack::DrawFX( void )
{
	pev->takedamage = 0;
	TraceResult tr;
	Vector vecSpot = pev->origin + Vector (0,0,12);
	Vector vecEnd =  pev->origin + Vector (0,0,-50);
	UTIL_TraceLine(vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr);

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_MEGAEXP, (float)tex );
	FX_Explosion( pev->origin+Vector(0,0,50), EXPLOSION_C4 );
	UTIL_ScreenShake( pev->origin, 30, 2, 3, pev->dmg*3 );
}

void CDetpack::Explode( entvars_t *pevAttacker )
{
	if (pev->takedamage)
	{
		DrawFX();
		entvars_t *pevOwner = VARS( pevAttacker );
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*2.2, CLASS_NONE, DMG_BLAST );
		UTIL_Remove( this );
	}
}

void CDetpack::Detonate( void )
{
	if (pev->takedamage)
	{
		DrawFX();
		entvars_t *pevOwner = VARS( pev->enemy );
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*2.2, CLASS_NONE, DMG_BLAST );
		UTIL_Remove( this );
	}
}

void CDetpack::BounceTouch( CBaseEntity *pOther )
{
	pev->velocity = g_vecZero;
	pev->angles.x = pev->angles.z = 0;
	UTIL_MakeVectors ( pev->angles );

	Vector right, forward, up = Vector( 0, 0, 20 );
	TraceResult tr1, tr2;
	float angle_z;

	UTIL_TraceLine ( pev->origin + up, pev->origin - up, ignore_monsters, ENT(pev), &tr1 );
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

void CDetpack :: BeepThink( void )
{
	if (pev->dmgtime <= gpGlobals->time)
		SetThink( Detonate );

	if ( gpGlobals->time >= m_flSoundTime )
	{
		EMIT_SOUND(ENT(pev), CHAN_STATIC, "weapons/c4_arm.wav", 1, ATTN_NORM);
		m_flSoundTime = gpGlobals->time + (m_flBeepTime/10);
	}
	m_flBeepTime-=0.1;
	pev->nextthink = gpGlobals->time + 0.1;
}

void CDetpack:: Spawn( void )
{
	pev->sequence = 2;
	pev->health = 1;
	pev->movetype = MOVETYPE_BOUNCE;
	pev->takedamage = DAMAGE_YES;
	pev->classname = MAKE_STRING( "detpack" );
	pev->solid = SOLID_BBOX;

	if (pev->dmgtime - gpGlobals->time <= 10.0)
		m_flBeepTime = 5;
	else
		m_flBeepTime = 45;

	SET_MODEL(ENT(pev), "models/p_c4.mdl");
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CDetpack * CDetpack:: ShootTimedDetpack( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time )
{
	CDetpack *pDetpack = GetClassPtr( (CDetpack *)NULL );
	pDetpack->pev->dmgtime = gpGlobals->time + time;
	pDetpack->Spawn();
	UTIL_SetOrigin( pDetpack->pev, vecStart );
	pDetpack->pev->velocity = vecVelocity;
	pDetpack->pev->enemy = ENT(pevOwner);
	
	pDetpack->SetTouch( BounceTouch );	
	pDetpack->SetThink( BeepThink );
	pDetpack->pev->nextthink = gpGlobals->time + 0.1;
	pDetpack->pev->dmg = dmg_c4.value * (mp_wpn_power.value/100);
	return pDetpack;
}

void CDetpack :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (!(bitsDamageType & (DMG_BLIND|DMG_RADIATION|DMG_NERVEGAS)) && flDamage > 5)
		Explode( pevAttacker );

	CBaseMonster::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}

int CDetpack :: RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	if (!(bitsDamageType & (DMG_BLIND|DMG_NERVEGAS|DMG_RADIATION)) && flDamage > 5)
		Explode( pevAttacker );

	return 0;
}