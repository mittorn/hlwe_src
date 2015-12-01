/*================================================================================================

			All those basic elements, that permit us to design new entities from 0

================================================================================================*/


#include "extdll.h"
#include "util.h"
#include "cbase.h"

extern void AddMultiDamage( entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType);

#define SF_USEABLE	4//third checkbox

//======================== STUDIO-BASED EMPTY ENTITY =============================

class CInfoNull : public CBaseEntity
{
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	int RealTakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType ) { return 0; };
	void Spawn( void );
	int ObjectCaps( void ) { return CBaseEntity::ObjectCaps() | ((pev->spawnflags & SF_USEABLE)?(FCAP_IMPULSE_USE):(0)); }
};

LINK_ENTITY_TO_CLASS(info_empty, CInfoNull);

void CInfoNull::Spawn( void )
{
	pev->takedamage = DAMAGE_YES;
}

void CInfoNull :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (!pev->takedamage) 
		return;

	AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
}


//======================== BSP-BASED EMPTY ENTITY =============================

class CFuncNull : public CBaseEntity
{
public:

	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	int RealTakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType ) { return 0; };
	void Spawn( void );
	int ObjectCaps( void ) { return CBaseEntity::ObjectCaps() | ((pev->spawnflags & SF_USEABLE)?(FCAP_IMPULSE_USE):(0)); }
};

LINK_ENTITY_TO_CLASS(func_empty, CFuncNull);

void CFuncNull::Spawn( void )
{
	pev->takedamage = DAMAGE_YES;
}

void CFuncNull :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (!pev->takedamage) 
		return;

	AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
}