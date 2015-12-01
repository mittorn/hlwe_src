#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "gamerules.h"
#include "explode.h"

//=========================================================
// base ammo class
//=========================================================

void CBasePlayerAmmo::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/w_items.mdl");
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));
	UTIL_SetOrigin(pev, pev->origin);
	SetTouch(DefaultTouch);
}

CBaseEntity* CBasePlayerAmmo::Respawn(void)
{
	pev->effects |= EF_NODRAW;
	SetTouch(NULL);
	UTIL_SetOrigin(pev, g_pGameRules->VecAmmoRespawnSpot(this));// move to wherever I'm supposed to repawn.
	SetThink(Materialize);
	pev->nextthink = g_pGameRules->FlAmmoRespawnTime(this);
	return this;
}

void CBasePlayerAmmo::Materialize(void)
{
	if (pev->effects & EF_NODRAW)
	{
		// changing from invisible state to visible.
		EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "items/ammo_respawn.wav", 1, ATTN_NORM, 0, 100 );
		pev->effects &= ~EF_NODRAW;
		pev->effects |= EF_MUZZLEFLASH;
	}
	SetTouch(DefaultTouch);
}

void CBasePlayerAmmo::DefaultTouch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer())
		return;

	if (AddAmmo(pOther))
	{
		if (g_pGameRules->AmmoShouldRespawn(this) == GR_AMMO_RESPAWN_YES)
		{
			Respawn();
		}
		else
		{
			SetTouch(NULL);
			SetThink(SUB_Remove);
			pev->nextthink = gpGlobals->time + .1;
		}
	}
}

BOOL CBasePlayerAmmo::AddAmmo(CBaseEntity *pOther)
{ 
	if (pOther->GiveAmmo(m_iAmmoGive, (char *)STRING(pev->message), 999) != -1)
	{
		switch(RANDOM_LONG(0,1))
		{
			case 0:EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);break;
			case 1:EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip2.wav", 1, ATTN_NORM);break;
		}
		return TRUE;
	}
	return FALSE;
}

void CBasePlayerAmmo::Init(int ammo_give, char *name)
{
	m_iAmmoGive = ammo_give;
	pev->message = MAKE_STRING(name);
}

//=========================================================
// ammo classes
//=========================================================

class CAmmo30mmSG : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(4, "30mm");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS( ammo_30mmsg, CAmmo30mmSG);

class CAK74Ammo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(30, "762");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS( ammo_ak74, CAK74Ammo);

class CAK74Ammo2 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(3, "AK74grenades");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS( ammo_ak74_sec, CAK74Ammo2);

class CAkimbogunAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(60, "762Magnum");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS( ammo_akimbogun, CAkimbogunAmmo);

class CAutoshotgunAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(10, "10mmbuckshot");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS( ammo_autoshotgun, CAutoshotgunAmmo);

class CAmmoAwp : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(10, "338Magnum");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_awp, CAmmoAwp);

class CAmmoBarett : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(5, "127mm");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_barett, CAmmoBarett);

class CAmmoBfg : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(3, "BFG");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_bfg, CAmmoBfg);

class CAmmoBfg2 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(1, "SunOfGod");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_bfg_sec, CAmmoBfg2);

class CAmmoChronosceptor : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(4, "cpc");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_chronosceptor, CAmmoChronosceptor);

class CAmmoCrossbow : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(5, "bolts");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_crossbow, CAmmoCrossbow);

class CAmmoDeagle : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(7, "50AE");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_deagle, CAmmoDeagle);

class CAmmoDevastator : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(8, "Mmissiles");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_devastator, CAmmoDevastator);

class CAmmoDisplacer : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(20, "isotope");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_displacer, CAmmoDisplacer);

class CAmmoEgon : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(20, "uranium238");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_egon, CAmmoEgon);

class CAmmoFlakcannon : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(5, "shrapnel");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_flakcannon, CAmmoFlakcannon);

class CAmmoFlamethrower : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(100, "fuel");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_flamethrower, CAmmoFlamethrower);

class CAmmoFroster : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(50, "cell");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_froster, CAmmoFroster);

class CAmmoFroster2 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(2, "Cgrenade");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_froster_sec, CAmmoFroster2);

class CAmmoG11 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(50, "57mm");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_g11, CAmmoG11);

class CAmmoGauss : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(20, "uranium");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_gauss, CAmmoGauss);

class CAmmoGlock : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(17, "9mmP");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_glock, CAmmoGlock);

class CAmmoGluongun : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(20, "GluonCells");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_gluongun, CAmmoGluongun);

class CAmmoIncendiary : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(3, "hellfire");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_incendiary, CAmmoIncendiary);

class CAmmoM16 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(30, "762Nato");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_m16, CAmmoM16);

class CAmmoM162 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(2, "NGgrenades");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_m16_sec, CAmmoM162);

class CAmmoM249 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(100, "556Nato");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_m249, CAmmoM249);

class CAmmoBiorifle : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(18, "biocharge");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_biorifle, CAmmoBiorifle);

class CAmmoM72 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(20, "2mm");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_m72, CAmmoM72);

class CAmmoM722 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(5, "quad");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_m72_sec, CAmmoM722);

class CAmmoMinigun : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(120, "86mm");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_minigun, CAmmoMinigun);

class CAmmoMachinegun : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(90, "32mm");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_machinegun, CAmmoMachinegun);

class CAmmoMP5 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(30, "9mm");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_mp5, CAmmoMP5);

class CAmmoMP52 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(2, "ARgrenades");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_mp5_sec, CAmmoMP52);

class CAmmoNailgun : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(60, "nails");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_nailgun, CAmmoNailgun);

class CAmmoPhotongun : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(60, "uranium235");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_photongun, CAmmoPhotongun);

class CAmmoPlasmarifle : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(30, "MicroFusionCells");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_plasmarifle, CAmmoPlasmarifle);

class CAmmoPulserifle : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(10, "stormcell");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_pulserifle, CAmmoPulserifle);

class CAmmoPython : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(6, "357");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_python, CAmmoPython);

class CAmmoRedeemer : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(1, "nuke");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_redeemer, CAmmoRedeemer);

class CAmmoRpg : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(1, "rockets");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_rpg, CAmmoRpg);

class CAmmoShieldgun : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(6, "12g");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_shieldgun, CAmmoShieldgun);

class CAmmoShotgun : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(12, "buckshot");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_shotgun, CAmmoShotgun);

class CAmmoSvd : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(12, "762x54");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_svd, CAmmoSvd);

class CAmmoTaucannon : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(25, "EnergyCells");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_taucannon, CAmmoTaucannon);

class CAmmoTeslagun : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(80, "ShockCore");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_teslagun, CAmmoTeslagun);

class CAmmoTeslagun2 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(6, "ShockGrenade");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_teslagun_sec, CAmmoTeslagun2);

class CAmmoU2 : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(30, "556");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_u2, CAmmoU2);

class CAmmoU2Sec : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(2, "U2grenades");
		CBasePlayerAmmo::Spawn();
	}
};
LINK_ENTITY_TO_CLASS(ammo_u2_sec, CAmmoU2Sec);

class CAmmoUsp : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(12, "45ACP");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_usp, CAmmoUsp);

class CAmmoUzi : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(64, "8mm");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_uzi, CAmmoUzi);

class CAmmoWhl : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Init(1, "Dmissile");
		CBasePlayerAmmo::Spawn( );
	}
};
LINK_ENTITY_TO_CLASS(ammo_whl, CAmmoWhl);