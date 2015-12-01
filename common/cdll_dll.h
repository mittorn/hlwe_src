// this file is included by both the game-dll and the client-dll,

#ifndef CDLL_DLL_H
#define CDLL_DLL_H

#define MAX_WEAPON_SLOTS	9	// hud item selection slots
#define MAX_ITEM_TYPES		9	// hud item selection slots
#define MAX_ITEMS		12	// hard coded item types

#define	HIDEHUD_WEAPONS		( 1<<0 )
#define	HIDEHUD_ALL		( 1<<1 )
#define HIDEHUD_HEALTH		( 1<<2 )

#define	MAX_AMMO_TYPES	128		// ???
#define MAX_AMMO_SLOTS  128		// not really slots

#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4

#define WEAPON_SUIT	256
#define MAX_WEAPONS	64		// max weapons

#endif