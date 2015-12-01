/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

===== h_export.cpp ========================================================

  Entity classes exported by Halflife.

*/

#include "extdll.h"
#include "util.h"

#include "cbase.h"

// Holds engine functionality callbacks
enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;
StringBuf		pool;//Declare it here


#ifdef _WIN32

// Required DLL entry point
BOOL WINAPI DllMain(
   HINSTANCE hinstDLL,
   DWORD fdwReason,
   LPVOID lpvReserved)
{
	if      (fdwReason == DLL_PROCESS_ATTACH)
    {
    }
	else if (fdwReason == DLL_PROCESS_DETACH)
    {
    }
	return TRUE;
}

/*
#include "ss_error.h"

typedef int (*pfnModel) (char* s);
typedef int	(*pfnSound)	(char* s);
typedef void(*pfnSet) (edict_t *e, const char *m);

pfnSet pSet;
pfnSound pSnd;
pfnModel pMdl;

void MsgSetModel(edict_t *e, const char *m)
{
	char msg[666];
	sprintf(msg, "SetModel(\"%s\");", m);
	PrintSequence(msg, msg+strlen(msg));

	(*pSet)(e, m);
}

int MsgLoadSound(char *s)
{
	char msg[666];
	sprintf(msg, "PRECACHE_SOUND(\"%s\");", s);
	PrintSequence(msg, msg+strlen(msg));

	return (*pSnd)(s);
}

int MsgLoadModel(char *s)
{
	char msg[666];
	sprintf(msg, "PRECACHE_MODEL(\"%s\");", s);
	PrintSequence(msg, msg+strlen(msg));

	return (*pMdl)(s);
}
*/

void DLLEXPORT GiveFnptrsToDll(	enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
{
	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;

/*
	pSet = g_engfuncs.pfnSetModel;
	pSnd = g_engfuncs.pfnPrecacheSound;
	pMdl = g_engfuncs.pfnPrecacheModel;

	g_engfuncs.pfnSetModel = MsgSetModel;
	g_engfuncs.pfnPrecacheSound = MsgLoadSound;
	g_engfuncs.pfnPrecacheModel = MsgLoadModel;
*/
}


#else

extern "C" {

void GiveFnptrsToDll(	enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
{
	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;


}

}

#endif
