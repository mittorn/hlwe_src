#ifndef __TF_DEFS_H
#define __TF_DEFS_H


// Defines for the playerclass
#define PC_UNDEFINED	0 
#define PC_SCOUT		1 
#define PC_SNIPER		2 
#define PC_SOLDIER		3 
#define PC_DEMOMAN		4 
#define PC_MEDIC		5 
#define PC_HVYWEAP		6 
#define PC_PYRO			7
#define PC_SPY			8
#define PC_ENGINEER		9

// Insert new class definitions here

// PC_RANDOM _MUST_ be the third last class
#define PC_RANDOM		10 		// Random playerclass
#define PC_CIVILIAN		11		// Civilians are a special class. They cannot
#define PC_LASTCLASS	12 		// Use this as the high-boundary for any loops
								// through the playerclass.

// Build state sent down to client
#define BS_BUILDING			(1<<0)
#define BS_HAS_DISPENSER	(1<<1)
#define BS_HAS_SENTRYGUN	(1<<2)
#define BS_CANB_DISPENSER	(1<<3)
#define BS_CANB_SENTRYGUN	(1<<4)

// Building metal costs
#define BUILD_COST_DISPENSER	100		// Metal needed to built 
#define BUILD_COST_SENTRYGUN	130		
#define BUILD_COST_MORTAR		150		
#define BUILD_COST_TELEPORTER	125		


// Legal Playerclass Handling
#define TF_ILL_SCOUT 		1
#define TF_ILL_SNIPER		2
#define TF_ILL_SOLDIER		4
#define TF_ILL_DEMOMAN		8
#define TF_ILL_MEDIC		16
#define TF_ILL_HVYWEP		32
#define TF_ILL_PYRO		64
#define TF_ILL_RANDOMPC		128
#define TF_ILL_SPY		256
#define TF_ILL_ENGINEER		512

/*======================*/
//      Menu stuff      //
/*======================*/

#define MENU_WEAPONS		0
#define MENU_DEFAULT		1
#define MENU_TEAM 		2
#define MENU_CLASS 		3
#define MENU_MAPBRIEFING	4
#define MENU_INTRO 		5
#define MENU_CLASSHELP		6
#define MENU_CLASSHELP2 	7
#define MENU_REPEATHELP 	8
#define MENU_SPECHELP		9
#define MENU_AMMO		10
#define MENU_FULL			11
#define MENU_T			12
#define MENU_JOIN			13
#define MENU_UNLOCK			14


#ifndef TF_DEFS_ONLY
#endif // TF_DEFS_ONLY
#endif // __TF_DEFS_H


