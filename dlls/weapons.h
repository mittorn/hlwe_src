#ifndef WEAPONS_H
#define WEAPONS_H

#include "effects.h"
#include "money.h"

class CBasePlayer;
extern int gmsgExplosion;
extern int gmsgWeapPickup;
extern int gmsgWorldExp;
extern int gmsgImpBullet;
extern int gmsgImpRocket;
extern int gmsgImpBeam;
extern int gmsgAurora;
extern int gmsgMSGManager;
extern int gmsgFireGun;
extern int gmsgFireBeam;
extern int gmsgBrassClip;
extern int gmsgPlrGib;
extern int gmsgBreakGib;
extern int gmsgTrail;
extern int gmsgSpecTank;

//Projectile trail and explosion effects
#define PROJ_REMOVE			0
#define PROJ_FLAME			1
#define PROJ_FLAME_DETONATE 		2
#define PROJ_FLAME_DETONATE_WATER	3
#define PROJ_ICE			4
#define PROJ_ICE_DETONATE 		5
#define PROJ_ICE_DETONATE_WATER		6
#define PROJ_NERVEGREN			7
#define PROJ_NERVEGREN_DETONATE 	8
#define PROJ_INCENDIARY2		9
#define PROJ_30MMGREN			10
#define PROJ_30MMGREN_DETONATE	 	11
#define PROJ_30MMGREN_DETONATE_WATER	12
#define PROJ_AK74			13
#define PROJ_AK74_DETONATE 		14
#define PROJ_AK74_DETONATE_WATER	15
#define PROJ_M203			16
#define PROJ_M203_DETONATE 		17
#define PROJ_M203_DETONATE_WATER	18
#define PROJ_MMISSILE			19
#define PROJ_MMISSILE_DETONATE 		20
#define PROJ_MMISSILE_DETONATE_WATER	21
#define PROJ_WARHEAD			22
#define PROJ_WARHEAD_DETONATE 		23
#define PROJ_WARHEAD_DETONATE_WATER	24
#define PROJ_DUMBFIRE			25
#define PROJ_DUMBFIRE_DETONATE 		26
#define PROJ_DUMBFIRE_DETONATE_WATER	27
#define PROJ_NUKE			28
#define PROJ_NUKE_DETONATE	 	29
#define PROJ_NUKE_DETONATE_WATER	30
#define PROJ_U2				31
#define PROJ_U2_DETONATE 		32
#define PROJ_U2_DETONATE_WATER		33
#define PROJ_U2_DETONATE_SHARDS		34
#define PROJ_U2_SHARD			35
#define PROJ_U2_SHARD_DETONATE 		36
#define PROJ_U2_SHARD_DETONATE_WATER	37
#define PROJ_FLAKBOMB			38
#define PROJ_FLAKBOMB_DETONATE 		39
#define PROJ_FLAKBOMB_DETONATE_WATER	40
#define PROJ_RPGROCKET			41
#define PROJ_RPGROCKET_DETONATE 	42
#define PROJ_RPGROCKET_DETONATE_WATER	43
#define PROJ_INCENDIARY			44
#define PROJ_INCENDIARY_DETONATE 	45
#define PROJ_INCENDIARY_DETONATE_WATER	46
#define PROJ_TESLAGREN			47
#define PROJ_TESLAGREN_DETONATE 	48
#define PROJ_TESLAGREN_DETONATE_WATER	49
#define PROJ_CLUSTERBOMB		50
#define PROJ_CLUSTERBOMB_DETONATE 	51
#define PROJ_CLUSTERBOMB_DETONATE_WATER	52
#define PROJ_CLUSTERBABY_DETONATE 	53
#define PROJ_CLUSTERBABY_DETONATE_WATER	54
#define PROJ_GLUON			55
#define PROJ_GLUON_DETONATE 		56
#define PROJ_GLUON_DETONATE_WATER	57
#define PROJ_GLUON2			58
#define PROJ_GLUON2_DETONATE 		59
#define PROJ_PLASMA	 		60
#define PROJ_DISPLACER			61
#define PROJ_DISPLACER_DETONATE 	62
#define PROJ_DISPLACER_DETONATE_WATER	63
#define PROJ_SHOCK			64
#define PROJ_SHOCK_DETONATE 		65
#define PROJ_SHOCK_DETONATE_WATER	66
#define PROJ_SUNOFGOD			67
#define PROJ_SUNOFGOD_DETONATE		68
#define PROJ_SUNOFGOD_DETONATE_WATER	69
#define PROJ_BLACKHOLE			70
#define PROJ_BLACKHOLE_DETONATE		71
#define PROJ_TELEENTER			72
#define PROJ_TELEENTER_DETONATE 	73
#define PROJ_TELEENTER_DETONATE_WATER	74
#define PROJ_DISPPOWER_DETONATE 	75
#define PROJ_SHOCKPOWER_DETONATE 	76
#define PROJ_GUTS			77
#define PROJ_GUTS_DETONATE		78
#define PROJ_ENERGYCHARGE_DETONATE 	79
#define PROJ_ENERGYCHARGE_DETONATE_WATER	80
#define EFFECT_BURN			81
#define EFFECT_FREEZE			82

// HUD Icons Message Manager
#define MSG_BUYZONE		1
#define MSG_MONEY		2
#define MSG_CLOAK		3	
#define MSG_PHK			4	
#define MSG_PHEV		5	
#define MSG_ANTIGRAV		6	
#define MSG_LONGJUMP		7	
#define MSG_PSHIELD		8	
#define MSG_PT			9	
#define MSG_TURRETSENTRY	10	
#define MSG_TURRETMISSILE	11	
#define MSG_TURRETION		12	
#define MSG_KEVLAR		13	
#define MSG_HARMOR		14	
#define MSG_BATTERY		15	
#define MSG_TRAIN		16	
#define MSG_WARHEAD		17	
#define MSG_BODY		18	
#define MSG_SKIN		19	
#define MSG_SCOPE		20
#define MSG_FIREMODE		21
#define MSG_CONCUSS		22
#define MSG_FLASHLIGHT		23
#define MSG_FLASHBATTERY	24
#define MSG_HEALTH		25
#define MSG_TIMER		26
#define MSG_GAMETITLE		27
#define MSG_FOV			28

// HUD-firemode icons
#define FIREMODE_SCOPE		1
#define FIREMODE_CHARGE		2
#define FIREMODE_PULSE		3
#define FIREMODE_BEAM		4
#define FIREMODE_NARROW		5
#define FIREMODE_WIDE		6
#define FIREMODE_BOLT		7
#define FIREMODE_HALF		8
#define FIREMODE_FULL		9
#define FIREMODE_SHOOT		10
#define FIREMODE_KNIFE		11
#define FIREMODE_ACCELBOLT	12
#define FIREMODE_EXPBOLT	13
#define FIREMODE_SENTRYTURRET	14
#define FIREMODE_MISSILETURRET	15
#define FIREMODE_IONTURRET	16
#define FIREMODE_QUAD		17
#define FIREMODE_GLAUNCHER	18

//Weapon fire effects
#define FIREGUN_REMOVE		0
#define FIREGUN_GLOCK		1
#define FIREGUN_GLOCKAKIMBO	2
#define FIREGUN_USP		3
#define FIREGUN_DEAGLE		4
#define FIREGUN_PYTHON		5
#define FIREGUN_UZI		6
#define FIREGUN_UZIAKIMBO	7
#define FIREGUN_SHIELDGUN	8
#define FIREGUN_SHOTGUN		9
#define FIREGUN_SHOTGUN_BR_SHELL 10
#define FIREGUN_AUTOSHOTGUN	11
#define FIREGUN_30MMSG		12
#define FIREGUN_MP5		13
#define FIREGUN_M16		14
#define FIREGUN_AKIMBOGUN_LEFT	15
#define FIREGUN_AKIMBOGUN_RIGHT	16
#define FIREGUN_AKIMBOGUN_BOTH	17
#define FIREGUN_AK74		18
#define FIREGUN_G11		19
#define FIREGUN_U2		20
#define FIREGUN_SVD		21
#define FIREGUN_AWP		22
#define FIREGUN_TORCH		23
#define FIREGUN_M249		24
#define FIREGUN_MINIGUN		25
#define FIREGUN_BFG		26
#define FIREGUN_BARETT		27
#define FIREGUN_MACHINEGUN	28
#define FIREGUN_FLAKCANNON	29
#define FIREGUN_PLASMARIFLE	30
#define FIREGUN_FROSTER		31
#define FIREGUN_EPISTOL		32
#define FIREGUN_DEVASTATOR	33
#define FIREGUN_RPG		34
#define FIREGUN_GLUONGUN	35
#define FIREGUN_DISPLACER	36
#define FIREGUN_SMARTGUN	37
#define FIREGUN_INCENDIARY	38
#define FIREGUN_IONTURRET	39
#define FIREGUN_M72		40
#define FIREGUN_BANDSAW		41
#define FIREGUN_CROSSBOW	42
#define FIREGUN_NAILGUN		43
#define FIREGUN_TESLAGUN	44
#define FIREGUN_BLASTER		45
#define FIREGUN_GAUSS		46
#define FIREGUN_PHOTONGUN	47
#define FIREGUN_PHOTONGUN_EXP	48
#define FIREGUN_PULSERIFLE	49
#define FIREGUN_TAUCANNON	50
#define FIREGUN_TURRETKIT	51
#define FIREGUN_CHRONOSCEPTOR	52
#define FIREGUN_LIGHTSABER	53
#define FIREGUN_FTHROWER	54
#define FIREGUN_BFG_SEC		55
#define FIREGUN_EGON		56

//Beam impact effects
#define IMPBEAM_BLASTER		1
#define IMPBEAM_GAUSS		2
#define IMPBEAM_GAUSSCHARGED	3
#define IMPBEAM_PHOTONGUN	4
#define IMPBEAM_TAUCANNON	5
#define IMPBEAM_IONTURRET	6
#define IMPBEAM_BLASTERBEAM	7
#define IMPBEAM_EGON		8
#define IMPBEAM_PLASMABALL	9
#define IMPBEAM_PBOLT		10

//Impact surfaces
#define SURFACE_NONE		0
#define SURFACE_WORLDBRUSH	1
#define SURFACE_BREAKABLE	2
#define SURFACE_ARMOR		3
#define SURFACE_ENERGYARMOR	4
#define SURFACE_FLESH		5

//Weapon beam effects
#define BEAM_BLASTER		1
#define BEAM_BLASTER_EXIT	2
#define BEAM_GAUSS		3
#define BEAM_GAUSS_EXIT		4
#define BEAM_GAUSSCHARGED	5
#define BEAM_GAUSSCHARGED_EXIT	6
#define BEAM_PHOTONGUN		7
#define BEAM_PHOTONGUN_EXIT	8
#define BEAM_TAUCANNON		9
#define BEAM_TAUCANNON_EXIT	10
#define BEAM_IONTURRET		11
#define BEAM_IONTURRET_EXIT	12
#define BEAM_IONTURRET_EXIT	12
#define BEAM_PHOTONGUN_EXP	13
#define BEAM_TESLAGUN		14
#define BEAM_PULSERIFLE		15
#define BEAM_PSP		16
#define BEAM_LGTNGBALL		17
#define BEAM_PLASMABALL		18
#define BEAM_BLOODSTREAM	19

// explosions
#define EXPLOSION_C4		1
#define EXPLOSION_BIOMASS	2 
#define EXPLOSION_TURRET	3
#define EXPLOSION_PSHIELD	4 
#define EXPLOSION_TRIPMINE	5 
#define EXPLOSION_GRENADE	6 
#define EXPLOSION_FLASHBANG	7 
#define EXPLOSION_CHRONOCLIP	8 
#define EXPLOSION_BOLT		9
#define EXPLOSION_SATCHEL	10
#define EXPLOSION_TANKPROJ	11
#define EXPLOSION_SPARKSHOWER	12
#define EXPLOSION_SHIELDIMPACT	13
#define EXPLOSION_ARMORIMPACT	14
#define EXPLOSION_BUBBLES	15
#define EXPLOSION_WHITESMOKE	16
#define EXPLOSION_BIOMASSIMPACT	17 
#define EXPLOSION_TANK		18
#define EXPLOSION_BLOODDRIPS	19 
#define EXPLOSION_SATELLITE	20
#define EXPLOSION_MORTAR	21
#define EXPLOSION_TORCH		22
#define EXPLOSION_PTELEPORT	23
#define EXPLOSION_DISPTELEPORT	24
#define EXPLOSION_PLASMABALL2	25
#define EXPLOSION_PLASMABALL2_SPARKS	26
#define EXPLOSION_ENERGY_INWATER_S	27
#define EXPLOSION_ENERGY_INWATER_M	28
#define EXPLOSION_ENERGY_INWATER_L	29
#define EXPLOSION_PBOLT			30
#define EXPLOSION_WHL_SHARD		31
#define EXPLOSION_SPARKSHOWER_SND	32
#define EXPLOSION_LIGHTSABER		33
#define EXPLOSION_LAVA			34
#define EXPLOSION_LAVA_FLAME		35
#define EXPLOSION_CLUSTERBABY		36
#define EXPLOSION_MEDKIT		37
#define EXPLOSION_HEVCHARGER		38
#define EXPLOSION_BEAM_FLESHIMPACT	39
#define EXPLOSION_TURRET_SPAWN		40
#define EXPLOSION_MACHINEGUN		41

//Body Gibbage
#define GIBBED_BODY	1
#define GIBBED_HEAD	2
#define GIBBED_FROZEN	3
#define GIBBED_IGNITE	4
#define GIBBED_ELECTRO	5

// Different bodyes for gun clips
#define CLIP_GLOCK		0
#define CLIP_USP		1
#define CLIP_DEAGLE		2
#define CLIP_UZI		3
#define CLIP_SHIELDGUN		4
#define CLIP_MP5		5
#define CLIP_M16		6
#define CLIP_AKIMBOGUN_SG552	7
#define CLIP_AKIMBOGUN_AUG	8
#define CLIP_AK74		9
#define CLIP_CROSSBOW		10
#define CLIP_G11		11
#define CLIP_U2			12
#define CLIP_SVD		13
#define CLIP_AWP		14
#define CLIP_BARETT		15
#define CLIP_M249		16
#define CLIP_MINIGUN		17
#define CLIP_NAILGUN		18
#define CLIP_FLAMETHROWER	19
#define CLIP_FROSTER		20
#define CLIP_FLAKCANNON		21
#define CLIP_BFG		22
#define CLIP_INCENDIARY		23
#define CLIP_TESLAGUN		24
#define CLIP_EGON		25
#define CLIP_PLASMARIFLE	26
#define CLIP_PHOTONGUN		27
#define CLIP_GAUSS		28
#define CLIP_TAUCANNON		29
#define CLIP_GLUONGUN		30
#define CLIP_DISPLACER		31
#define CLIP_BIORIFLE		32
#define CLIP_PULSERIFLE		33
#define CLIP_M72		34
#define CLIP_CHRONOSCEPTOR	35
#define CLIP_MACHINEGUN		36

#define CLIP_GLOCK_DUAL		37
#define CLIP_UZI_RIGHT		38
#define CLIP_UZI_LEFT		39
#define CLIP_MACHINEGUN_LEFT	40
#define CLIP_NAILGUN_LEFT	41
#define CLIP_EGON_LEFT		42
#define CLIP_EGON_MIDDLE	43
#define CLIP_CHRONOSCEPTOR_LEFT 44
#define CLIP_GLUONGUN_LEFT	45

//SLOT 1
#define WEAPON_NONE			0
#define WEAPON_MEDKIT			1
#define WEAPON_CROWBAR			2
#define WEAPON_BANDSAW			3
#define WEAPON_TORCH			4
#define WEAPON_LIGHTSABER		5

//SLOT 2
#define	WEAPON_GLOCK			6
#define	WEAPON_GLOCK_AKIMBO		7
#define	WEAPON_USP			8
#define	WEAPON_DEAGLE			9
#define WEAPON_PYTHON			10
#define WEAPON_UZI			11
#define WEAPON_UZI_AKIMBO		12
#define WEAPON_SHIELDGUN		13

//SLOT 3
#define WEAPON_SHOTGUN			14
#define WEAPON_AUTOSHOTGUN		15
#define WEAPON_30MMSG			16
#define WEAPON_MP5			17
#define	WEAPON_M16			18
#define	WEAPON_AKIMBOGUN		19
#define	WEAPON_AK74			20

//SLOT 4
#define	WEAPON_G11			21
#define	WEAPON_U2			22
#define WEAPON_CROSSBOW			23
#define	WEAPON_SVD			24
#define	WEAPON_AWP			25
#define WEAPON_BARETT			26

//SLOT 5
#define	WEAPON_M249			27
#define WEAPON_MINIGUN			28
#define WEAPON_NAILGUN			29
#define	WEAPON_FROSTER			30
#define WEAPON_FLAMETHROWER		31
#define WEAPON_FLAKCANNON		32
#define WEAPON_MACHINEGUN		33
#define WEAPON_BFG			34

// SLOT 6
#define WEAPON_RPG			35
#define WEAPON_INCENDIARY		36
#define WEAPON_WHL			37
#define	WEAPON_DEVASTATOR		38
#define WEAPON_REDEEMER			39

//SLOT 7
#define WEAPON_SMARTGUN			40
#define	WEAPON_TESLAGUN			41
#define WEAPON_EGON			42
#define WEAPON_PLASMARIFLE		43
#define WEAPON_PHOTONGUN		44
#define WEAPON_GAUSS			45
#define WEAPON_TAUCANNON		46
#define WEAPON_GLUONGUN			47
#define WEAPON_DISPLACER		48

// SLOT 8
#define WEAPON_FLASHBANG		49
#define WEAPON_HANDGRENADE		50
#define WEAPON_TRIPMINE			51
#define	WEAPON_SATCHEL			52
#define WEAPON_C4			53

// SLOT 9
#define WEAPON_BLASTER			54
#define	WEAPON_BIORIFLE			55
#define	WEAPON_PULSERIFLE		56
#define WEAPON_M72			57
#define WEAPON_SATELLITE		58
#define WEAPON_TURRETKIT		59
#define WEAPON_CHRONOSCEPTOR		60

#define WEAPON_ALLWEAPONS		(~(1<<WEAPON_SUIT))

#define MAX_WEAPONS			64
#define WEAPON_SUIT			256	// ?????

#define MAX_SHIELD_BATTERY	300
#define MAX_PHEV_BATTERY	300
#define MAX_SHIELD_PHEV_BATTERY	400
#define MAX_NORMAL_BATTERY	200


//Buy Items, when dead
#define DEAD_BUY_MEDKIT		1
#define DEAD_BUY_CROWBAR	2
#define DEAD_BUY_TURRETKIT	3
#define DEAD_BUY_BANDSAW	4
#define DEAD_BUY_LIGHTSABER	5
#define DEAD_BUY_GLOCK		6
#define DEAD_BUY_USP		7
#define DEAD_BUY_DEAGLE		8
#define DEAD_BUY_PYTHON		9
#define DEAD_BUY_UZI		10
#define DEAD_BUY_SHIELDGUN      11
#define DEAD_BUY_SHOTGUN	12
#define DEAD_BUY_AUTOSHOTGUN	13
#define DEAD_BUY_30MMSG		14
#define DEAD_BUY_MP5		15
#define DEAD_BUY_M16		16
#define DEAD_BUY_AKIMBOGUN	17
#define DEAD_BUY_AK74		18
#define DEAD_BUY_CROSSBOW	19
#define DEAD_BUY_G11		20
#define DEAD_BUY_U2		21
#define DEAD_BUY_SVD		22
#define DEAD_BUY_AWP		23
#define DEAD_BUY_BARETT		24
#define DEAD_BUY_M249		25
#define DEAD_BUY_MINIGUN	26
#define DEAD_BUY_NAILGUN	27
#define DEAD_BUY_FROSTER	28
#define DEAD_BUY_FLAMETHROWER	29
#define DEAD_BUY_FLAKCANNON	30
#define DEAD_BUY_BFG		31
#define DEAD_BUY_RPG		32
#define DEAD_BUY_INCENDIARY	33
#define DEAD_BUY_WHL		34
#define DEAD_BUY_DEVASTATOR	35
#define DEAD_BUY_REDEEMER	36
#define DEAD_BUY_SMARTGUN	37
#define DEAD_BUY_TESLAGUN	38
#define DEAD_BUY_EGON		39
#define DEAD_BUY_PLASMARIFLE	40
#define DEAD_BUY_PHOTONGUN	41
#define DEAD_BUY_GAUSS		42
#define DEAD_BUY_TAUCANNON	43
#define DEAD_BUY_GLUONGUN	44
#define DEAD_BUY_DISPLACER	45
#define DEAD_BUY_FLASHBANG	46
#define DEAD_BUY_HANDGRENADE	47
#define DEAD_BUY_TRIPMINE	48
#define DEAD_BUY_SATCHEL	49
#define DEAD_BUY_C4		50
#define DEAD_BUY_BLASTER	51
#define DEAD_BUY_BIORIFLE	52
#define DEAD_BUY_PULSERIFLE	53
#define DEAD_BUY_M72		54
#define DEAD_BUY_SATELLITE	55
#define DEAD_BUY_CHRONOSCEPTOR	56
#define DEAD_BUY_BATTERY	57
#define DEAD_BUY_KEVLAR		58
#define DEAD_BUY_HARMOR		59
#define DEAD_BUY_LONGJUMP	60
#define DEAD_BUY_ANTIGRAV	61
#define DEAD_BUY_CLOAK		62
#define DEAD_BUY_PT		63
#define DEAD_BUY_POWERSHIELD	64
#define DEAD_BUY_PORTABLEHEALTHKIT 65
#define DEAD_BUY_PORTABLEHEV	66
#define DEAD_BUY_MACHINEGUN	67
#define DEAD_BUY_TORCH		68

// weapon weight factors (for auto-switching)   (-1 = noswitch)

// Melee weapons
#define CROWBAR_WEIGHT		6
#define BANDSAW_WEIGHT		9
#define TORCH_WEIGHT		5
#define LIGHTSABER_WEIGHT	10
#define MEDKIT_WEIGHT		3

// Hand Guns
#define GLOCK_WEIGHT		10
#define GLOCK_AKIMBO_WEIGHT	13
#define USP_WEIGHT		14
#define PYTHON_WEIGHT		16
#define DEAGLE_WEIGHT		18
#define UZI_WEIGHT		20
#define UZI_AKIMBO_WEIGHT	22
#define SHIELDGUN_WEIGHT	23

// Assault weapons
#define SHOTGUN_WEIGHT		20
#define AUTOSHOTGUN_WEIGHT	25
#define SG30MM_WEIGHT           42
#define MP5_WEIGHT		19
#define AKIMBOGUN_WEIGHT	24
#define M16_WEIGHT		22
#define AK74_WEIGHT		26

// Sniper weapons
#define CROSSBOW_WEIGHT		14
#define G11_WEIGHT		23
#define U2_WEIGHT		28
#define SVD_WEIGHT		33
#define AWP_WEIGHT		40
#define BARETT_WEIGHT		45

// Heavy weapons
#define M249_WEIGHT		30
#define MINIGUN_WEIGHT		38
#define FLAMETHROWER_WEIGHT	40
#define NAILGUN_WEIGHT		42
#define FLAKCANNON_WEIGHT	47
#define BFG_WEIGHT		120
#define FROSTER_WEIGHT		40
#define M72_WEIGHT		70
#define MACHINEGUN_WEIGHT	98

// Launcher weapons
#define RPG_WEIGHT		60
#define INCENDIARY_WEIGHT	50
#define WHL_WEIGHT		72
#define DEVASTATOR_WEIGHT	75
#define REDEEMER_WEIGHT		110

// Energy weapons
#define SMARTGUN_WEIGHT		24
#define GAUSS_WEIGHT		37
#define EGON_WEIGHT		44
#define TESLAGUN_WEIGHT		39
#define TAUCANNON_WEIGHT	55
#define GLUONGUN_WEIGHT		75
#define PLASMARIFLE_WEIGHT	65
#define PHOTONGUN_WEIGHT	62
#define DISPLACER_WEIGHT	100
#define PULSERIFLE_WEIGHT	85

// Explosive weapons
#define HANDGRENADE_WEIGHT	11
#define SATCHEL_WEIGHT		18
#define TRIPMINE_WEIGHT		15
#define C4_WEIGHT		25
#define FLASHBANG_WEIGHT	5
#define SATELLITE_WEIGHT	200
#define CHRONOSCEPTOR_WEIGHT	185
#define BLASTER_WEIGHT		34
#define BIORIFLE_WEIGHT		36
#define TURRETKIT_WEIGHT	41

// maximum client movement speed, when carrying weapon...
// Melee weapons
#define CROWBAR_MAXSPEED	310
#define BANDSAW_MAXSPEED	265
#define TORCH_MAXSPEED		200
#define LIGHTSABER_MAXSPEED	315
#define MEDKIT_MAXSPEED		308

// Hand Guns
#define GLOCK_MAXSPEED		310
#define GLOCK_AKIMBO_MAXSPEED	295
#define USP_MAXSPEED		305
#define PYTHON_MAXSPEED		300
#define DEAGLE_MAXSPEED		300
#define UZI_MAXSPEED		295
#define UZI_AKIMBO_MAXSPEED	285
#define SHIELDGUN_MAXSPEED	200

// Assault weapons
#define SHOTGUN_MAXSPEED	260
#define AUTOSHOTGUN_MAXSPEED	260
#define SG30MM_MAXSPEED		240
#define MP5_MAXSPEED		290
#define AKIMBOGUN_MAXSPEED	250
#define M16_MAXSPEED		280
#define AK74_MAXSPEED		275

// Sniper weapons
#define CROSSBOW_MAXSPEED	280
#define G11_MAXSPEED		295
#define U2_MAXSPEED		275
#define SVD_MAXSPEED		260
#define AWP_MAXSPEED		245
#define BARETT_MAXSPEED		220

// Heavy weapons
#define M249_MAXSPEED		225
#define MINIGUN_MAXSPEED	175
#define FLAMETHROWER_MAXSPEED	225
#define FROSTER_MAXSPEED	230
#define NAILGUN_MAXSPEED	220
#define FLAKCANNON_MAXSPEED	190
#define MACHINEGUN_MAXSPEED	80
#define BFG_MAXSPEED		105

// Launcher weapons
#define INCENDIARY_MAXSPEED	230
#define RPG_MAXSPEED		215
#define WHL_MAXSPEED		180
#define DEVASTATOR_MAXSPEED	125
#define REDEEMER_MAXSPEED	110

// Energy weapons
#define SMARTGUN_MAXSPEED	305
#define GAUSS_MAXSPEED		260
#define EGON_MAXSPEED		200
#define TESLAGUN_MAXSPEED	275
#define TAUCANNON_MAXSPEED	235
#define GLUONGUN_MAXSPEED	225
#define PLASMARIFLE_MAXSPEED	250
#define PHOTONGUN_MAXSPEED	195
#define DISPLACER_MAXSPEED	185

// Explosive weapons
#define HANDGRENADE_MAXSPEED	310
#define SATCHEL_MAXSPEED	305
#define TRIPMINE_MAXSPEED	310
#define C4_MAXSPEED		300
#define FLASHBANG_MAXSPEED	315

// Experimental weapons
#define PULSERIFLE_MAXSPEED	250
#define M72_MAXSPEED		240
#define SATELLITE_MAXSPEED	318
#define CHRONOSCEPTOR_MAXSPEED	180
#define BLASTER_MAXSPEED	295
#define BIORIFLE_MAXSPEED	225
#define TURRETKIT_MAXSPEED	310

#define HARMOR_MAXSPEED		200
#define NORMAL_MAXSPEED		320

// the maximum amount of ammo each weapon's clip can hold
#define WEAPON_NOCLIP			-1

void DeactivateSatchels( CBasePlayer *pOwner );
void DeactivateBiomass( CBasePlayer *pOwner );
void DeactivateTurrets( CBasePlayer *pOwner );

// bullet types
typedef	enum
{
	BULLET_NONE = 0,
	BULLET_57mm, // g11
	BULLET_9mmP, // 9 mm Parabellum
	BULLET_8mm, // uzi
	BULLET_12G, // shieldgun
	BULLET_762Nato, //M16 
	BULLET_556Nato, //M249 
	BULLET_556, //u2 
	BULLET_762, //AK74 
	BULLET_762x54, //svd 
	BULLET_338Magnum, //AWP 
	BULLET_86mm, //minigun 
	BULLET_32mm, //machinegun 
	BULLET_MP5, // mp5
	BULLET_357, // python
	BULLET_BUCKSHOT, // shotgun
	BULLET_10MMBUCKSHOT, // automatic shotgun
	BULLET_BFG, // BFG
	BULLET_50AE, // Desert Eagle
	BULLET_45ACP, // usp
	BULLET_30mm, // 30mmsg
	BULLET_9MM,
	BULLET_12MM,
	BULLET_14MM,
	BULLET_CROWBAR,
	BULLET_KNIFE,
	BULLET_CROWBARQUAD,
	BULLET_KNIFEQUAD,
	BULLET_BANDSAW,
	BULLET_NAILGUN,
	BULLET_2MM,
	BULLET_2MM_QUAD,
	BULLET_127MM,
	BULLET_BOLT,
	BULLET_SMALEXP,
	BULLET_NORMEXP,
	BULLET_HIGHEXP,
	BULLET_MEGAEXP
} Bullet;

enum TANK_PROJECTILE
{
// fires bullets
	TANK_BULLET_9MM = 0,
	TANK_BULLET_12MM = 1,
	TANK_BULLET_14MM = 2,
	TANK_BULLET_BUCKSHOT = 3,
	TANK_BULLET_12G = 4,
	TANK_BULLET_57MM = 5,
	TANK_BULLET_762NATO = 6,
	TANK_BULLET_762X54 = 7,
	TANK_BULLET_338MAGNUM = 8,
	TANK_BULLET_86MM = 9,
	TANK_BULLET_32MM = 10,
	TANK_BULLET_127MM = 11,
	TANK_BULLET_BFG = 12,
	TANK_BULLET_NAIL = 13,
	TANK_BULLET_BOLT = 14,

// fires energy charges
	TANK_ENERGY_HOMINGSHOCK = 15,
	TANK_ENERGY_SHOCK = 16,
	TANK_ENERGY_PLASMA = 17,
	TANK_ENERGY_PLASMALARGE = 18,
	TANK_ENERGY_FLAME = 19,
	TANK_ENERGY_FROSTBALL = 20,
	TANK_ENERGY_GLUON = 21,
	TANK_ENERGY_GLUONLARGE = 22,
	TANK_ENERGY_DISPBALL_HALF = 23,
	TANK_ENERGY_DISPBALL_FULL = 24,
	TANK_ENERGY_PBOLT = 25,
	TANK_ENERGY_ECHARGE = 26,

//fires *explosive* projectiles
	TANK_ROCKET_XBOLT = 27,
	TANK_ROCKET_CLUSTER = 28,
	TANK_ROCKET_M203GREN = 29,
	TANK_ROCKET_AK74GREN = 30,
	TANK_ROCKET_HANDGREN = 31,
	TANK_ROCKET_U2 = 32,
	TANK_ROCKET_NGGREN = 33,
	TANK_ROCKET_TESLAGREN = 34,
	TANK_ROCKET_MMISSILE = 35,
	TANK_ROCKET_DEMOLITION = 36,
	TANK_ROCKET_RPG = 37,
	TANK_ROCKET_DUMBFIRE = 38,
	TANK_ROCKET_HELLFIRE = 39,
	TANK_ROCKET_FLAKBOMB = 40,
	TANK_ROCKET_NUKE = 41,
	TANK_ROCKET_NUKE_CONTROLLED = 42,
	TANK_MORTAR = 43,
	TANK_ROCKET_30MMGREN = 44,
	TANK_ROCKET_BIOMASS = 45,

//fires beam
	TANK_BEAM_BLASTER = 46,
	TANK_BEAM_GAUSS = 47,
	TANK_BEAM_GAUSSCHARGED = 48,
	TANK_BEAM_PHOTONGUN = 49,
	TANK_BEAM_PHOTONGUN_EXP = 50,
	TANK_BEAM_TAUCANNON = 51,
	TANK_BEAM_IONTURRET = 52,
	TANK_BEAM_PULSERIFLE = 53,
	TANK_BEAM_M72 = 54,
	TANK_BEAM_M72QUAD = 55,
	TANK_BEAM_TESLAGUN = 56
};

#define ITEM_FLAG_SELECTONEMPTY		1
#define ITEM_FLAG_NOAUTORELOAD		2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY	4
#define ITEM_FLAG_LIMITINWORLD		8
#define ITEM_FLAG_EXHAUSTIBLE		16 // A player can totally exhaust their ammo supply and lose this weapon

typedef struct
{
	int		iSlot;
	int		iPosition;
	const char	*pszAmmo1;	// ammo 1 type
	int		iMaxAmmo1;		// max ammo 1
	const char	*pszAmmo2;	// ammo 2 type
	int		iMaxAmmo2;		// max ammo 2
	const char	*pszName;
	int		iMaxClip;
	int		iId;
	int		iFlags;
	int		iWeight;// this value used to determine this weapon's importance in autoselection.
	int		iMaxspeed;
} ItemInfo;

typedef struct
{
	const char *pszName;
	int iId;
} AmmoInfo;

// Items that the player has in their inventory that they can use
class CBasePlayerItem : public CBaseAnimating
{
public:
	virtual void SetObjectCollisionBox( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	
	static	TYPEDESCRIPTION m_SaveData[];

	virtual int AddToPlayer( CBasePlayer *pPlayer );	// return TRUE if the item you want the item added to the player inventory
	virtual int AddDuplicate( CBasePlayerItem *pItem ) { return FALSE; }	// return TRUE if you want your duplicate removed from world
	void EXPORT DestroyItem( void );
	void EXPORT DefaultTouch( CBaseEntity *pOther );	// default weapon touch
	void EXPORT FallThink ( void );// when an item is first spawned, this think is run to determine when the object has hit the ground.
	void EXPORT Materialize( void );// make a weapon visible and tangible
	void EXPORT AttemptToMaterialize( void );  // the weapon desires to become visible and tangible, if the game rules allow for it
	CBaseEntity* Respawn ( void );// copy a weapon
	void FallInit( void );
	void CheckRespawn( void );
	virtual int GetItemInfo(ItemInfo *p) { return 0; };	// returns 0 if struct not filled out
	virtual BOOL CanDeploy( void ) { return TRUE; };
	virtual BOOL Deploy( )								// returns is deploy was successful
		 { return TRUE; };

	virtual BOOL CanHolster( void ) { return TRUE; };// can this weapon be put away right now?
	virtual void Holster( void );
	virtual void UpdateItemInfo( void ) { return; };

	virtual void ItemPreFrame( void )	{ return; }		// called each frame by the player PreThink
	virtual void ItemPostFrame( void ) { return; }		// called each frame by the player PostThink

	virtual void Drop( void );
	virtual void Kill( void );
	virtual void AttachToPlayer ( CBasePlayer *pPlayer );

	virtual int PrimaryAmmoIndex() { return -1; };
	virtual int SecondaryAmmoIndex() { return -1; };

	virtual int UpdateClientData( CBasePlayer *pPlayer ) { return 0; }

	virtual CBasePlayerItem *GetWeaponPtr( void ) { return NULL; };

	static ItemInfo ItemInfoArray[ MAX_WEAPONS ];
	static AmmoInfo AmmoInfoArray[ MAX_AMMO_SLOTS ];

	CBasePlayer	*m_pPlayer;
	CBasePlayerItem *m_pNext;
	int		m_iId;												// WEAPON_???

	int iItemSlot( void ) { return 0; }

	int		iItemPosition( void ) { return ItemInfoArray[ m_iId ].iPosition; }
	const char	*pszAmmo1( void )	{ return ItemInfoArray[ m_iId ].pszAmmo1; }
	int		iMaxAmmo1( void )	{ return ItemInfoArray[ m_iId ].iMaxAmmo1; }
	const char	*pszAmmo2( void )	{ return ItemInfoArray[ m_iId ].pszAmmo2; }
	int		iMaxAmmo2( void )	{ return ItemInfoArray[ m_iId ].iMaxAmmo2; }
	const char	*pszName( void )	{ return ItemInfoArray[ m_iId ].pszName; }
	int		iMaxClip( void )	{ return ItemInfoArray[ m_iId ].iMaxClip; }
	int		iWeight( void )		{ return ItemInfoArray[ m_iId ].iWeight; }
	int		iMaxspeed( void )	{ return ItemInfoArray[ m_iId ].iMaxspeed; }
	int		iFlags( void )		{ return ItemInfoArray[ m_iId ].iFlags; }

	int		modelindexsave; 
	string_t	modelsave;
};


// inventory items that 
class CBasePlayerWeapon : public CBasePlayerItem
{
public:
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	
	static	TYPEDESCRIPTION m_SaveData[];

	virtual void BuyPrimaryAmmo( void ) {return;}
	virtual void SellWeapon( void ) {return;}
	virtual void BuySecondaryAmmo( void ) {return;}
	virtual void BuyAmmo( int iCount, char *szName, int iCost );

	virtual void SwitchFireMode( void ) {return;}
	virtual void CompleteReload ( void );

	// generic weapon versions of CBasePlayerItem calls
	virtual int AddToPlayer( CBasePlayer *pPlayer );
	virtual int AddDuplicate( CBasePlayerItem *pItem );

	virtual int ExtractAmmo( CBasePlayerWeapon *pWeapon ); //{ return TRUE; };			// Return TRUE if you can add ammo to yourself when picked up
	virtual int ExtractClipAmmo( CBasePlayerWeapon *pWeapon );// { return TRUE; };			// Return TRUE if you can add ammo to yourself when picked up

	virtual int AddWeapon( void ) { ExtractAmmo( this ); return TRUE; };	// Return TRUE if you want to add yourself to the player

	// generic "shared" ammo handlers
	BOOL AddPrimaryAmmo( int iCount, char *szName, int iMaxClip, int iMaxCarry );
	BOOL AddSecondaryAmmo( int iCount, char *szName, int iMaxCarry );

	virtual void UpdateItemInfo( void ) {};	// updates HUD state

	BOOL PlayEmptySound( int iSoundType = 0 );
	int m_iPlayEmptySound;

	int m_fFireOnEmpty;		// True when the gun is empty and the player is still holding down the
	virtual void ResetEmptySound( void );

	virtual void SendWeaponAnim( int iAnim );

	virtual BOOL CanDeploy( void );
	virtual BOOL IsUseable( void );

	BOOL DefaultDeploy( char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt, float fDrawTime = 0.5 );
	int DefaultReload( int iClipSize, int iAnim, float fDelay, float fDrop = 0 );

	virtual void ItemPostFrame( void );	// called each frame by the player PostThink
	virtual void PrimaryAttack( void ) { return; }				// do "+ATTACK"
	virtual void SecondaryAttack( void ) { return; }			// do "+ATTACK2"
	virtual void DoubleAttack( void ) { SecondaryAttack(); }	// do "+ATTACK+ATTACK2" = ATTACK2 by default
	virtual void Reload( void ) { return; }						// do "+RELOAD"
	virtual void WeaponIdle( void ) { return; }					// called when no buttons pressed
	virtual int UpdateClientData( CBasePlayer *pPlayer );		// sends hud info to client dll, if things have changed
	virtual void RetireWeapon( void );
	virtual void Holster( void );
	virtual BOOL ShouldWeaponIdle( void ) {return FALSE; };
	
	int	PrimaryAmmoIndex(); 
	int	SecondaryAmmoIndex(); 

	virtual CBasePlayerItem *GetWeaponPtr( void ) { return (CBasePlayerItem *)this; };

	float	m_flPumpTime;
	int	m_fInSpecialReload;									// Are we in the middle of a reload for the shotguns
	float	m_flNextPrimaryAttack;								// soonest time ItemPostFrame will call PrimaryAttack
	float	m_flNextSecondaryAttack;							// soonest time ItemPostFrame will call SecondaryAttack
	float	m_flTimeWeaponIdle;									// soonest time ItemPostFrame will call WeaponIdle
	int	m_iPrimaryAmmoType;									// "primary" ammo index into players m_rgAmmo[]
	int	m_iSecondaryAmmoType;								// "secondary" ammo index into players m_rgAmmo[]
	int	m_iClip;											// number of shots left in the primary weapon clip, -1 it not used
	int	m_iClientClip;										// the last version of m_iClip sent to hud dll
	int	m_iClientWeaponState;								// the last version of the weapon state sent to hud dll (is current weapon, is on target)
	int	m_fInReload;										// Are we in the middle of a reload;
	int	m_iDefaultAmmo;// how much ammo you get when you pick up this weapon as placed by a level designer.
	int	m_iDefaultAmmo2;
	int 	WastedAmmo;
	float	m_flRechargeTime;
	int	m_fInAttack;

	int 	m_iChargeLevel;
	float	m_flTimeUpdate;									// special time for additional effects
	float	m_flShockTime;									// shock time - play sound effects. don't save

	virtual void RestoreBody ( void );
	int m_iLastSkin;
	int m_iLastBody;
	BOOL b_Restored;//restore body and skin after save/load
	BOOL m_fInZoom;
	int m_fSpotActive;

	int firemode;
	int m_iLastFiremode;

	int m_iFiredAmmo;
	void EXPORT ClipCasing ( void );
	void EXPORT ClipCasingGluongun ( void );
	void EXPORT ClipCasingAug ( void );
	void EXPORT ClipCasingUzi ( void );
	void EXPORT ClipCasingNailgun ( void );
	void EXPORT ClipCasingMachinegun ( void );
	void EXPORT ClipCasingEgon ( void );
	void EXPORT ClipCasingEgonLast ( void );
	void EXPORT ClipCasingChronosceptor ( void );
};

class CBasePlayerAmmo : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual BOOL AddAmmo(CBaseEntity *pOther);
	void EXPORT DefaultTouch(CBaseEntity *pOther);
	void EXPORT Materialize(void);
	void Init(int ammo_give, char *name);
	CBaseEntity* Respawn(void);
	int	m_iAmmoGive;
};

extern DLL_GLOBAL	short	g_sModelIndexLaser;// holds the index for the laser beam
extern DLL_GLOBAL	const char *g_pModelNameLaser;
extern DLL_GLOBAL	short	g_sModelIndexSmoke;

extern void ClearMultiDamage(void);
extern void ApplyMultiDamage(entvars_t* pevInflictor, entvars_t* pevAttacker );
extern void AddMultiDamage( entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType);

extern void RadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType );
extern void WaterRadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType );
extern void FullRadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType );

typedef struct 
{
	CBaseEntity		*pEntity;
	float			amount;
	int				type;
} MULTIDAMAGE;

extern MULTIDAMAGE gMultiDamage;

//SNIPER GUNS, HAND GUNS
#define VECTOR_CONE_0DEGREES	Vector( 0.00000, 0.00000, 0.00000 )
#define VECTOR_CONE_1DEGREES	Vector( 0.00218, 0.00218, 0.00218 )
#define VECTOR_CONE_2DEGREES	Vector( 0.00436, 0.00436, 0.00436 )

//HAND GUNS, SUBMACHINE GUNS, ASSAULT RIFLES
#define VECTOR_CONE_3DEGREES	Vector( 0.00654, 0.00654, 0.00654 )
#define VECTOR_CONE_4DEGREES	Vector( 0.00872, 0.00872, 0.00872 )
#define VECTOR_CONE_5DEGREES	Vector( 0.01090, 0.01090, 0.01090 )
#define VECTOR_CONE_6DEGREES	Vector( 0.01308, 0.01308, 0.01308 )

// HEAVY GUNS, SHOTGUNS
#define VECTOR_CONE_7DEGREES	Vector( 0.01526, 0.01526, 0.01526 )
#define VECTOR_CONE_8DEGREES	Vector( 0.01744, 0.01744, 0.01744 )
#define VECTOR_CONE_9DEGREES	Vector( 0.01961, 0.01961, 0.01961 )

//SHOTGUNS
#define VECTOR_CONE_10DEGREES	Vector( 0.02179, 0.02179, 0.02179 )
#define VECTOR_CONE_11DEGREES	Vector( 0.02397, 0.02397, 0.02397 )
#define VECTOR_CONE_12DEGREES	Vector( 0.02615, 0.02615, 0.02615 )
#define VECTOR_CONE_13DEGREES	Vector( 0.02833, 0.02833, 0.02833 )
#define VECTOR_CONE_14DEGREES	Vector( 0.03051, 0.03051, 0.03051 )
#define VECTOR_CONE_15DEGREES	Vector( 0.03269, 0.03269, 0.03269 )

// BFG
#define VECTOR_CONE_20DEGREES	Vector( 0.04341, 0.04341, 0.04341 )
#define VECTOR_CONE_40DEGREES	Vector( 0.08682, 0.08682, 0.08682 )

//=========================================================
// CWeaponBox - a single entity that can store weapons
// and ammo. 
//=========================================================
class CWeaponBox : public CBaseEntity
{
	void Spawn( void );
	void Touch( CBaseEntity *pOther );
	void KeyValue( KeyValueData *pkvd );
	BOOL IsEmpty( void );
	int  GiveAmmo( int iCount, char *szName, int iMax, int *pIndex = NULL );
	void SetObjectCollisionBox( void );
public:
	void EXPORT Kill ( void );
	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	HasWeapon( CBasePlayerItem *pCheckItem );
	BOOL PackWeapon( CBasePlayerItem *pWeapon );
	BOOL PackAmmo( int iszName, int iCount );
	
	CBasePlayerItem	*m_rgpPlayerItems[MAX_ITEM_TYPES];// one slot for each 

	int m_rgiszAmmo[MAX_AMMO_SLOTS];// ammo names
	int	m_rgAmmo[MAX_AMMO_SLOTS];// ammo quantities

	int m_cAmmoTypes;// how many ammo types packed into this box (if packed by a level designer)
};

bool bIsMultiplayer ( void );
void LoadVModel ( char *szViewModel, CBasePlayer *m_pPlayer );

class CLaserSpot : public CBaseEntity//laser spot for different weapons
{
	void Spawn( void );
	void SpawnNailgunspot( void );
	void SpawnDGlockspot( void );
	void SpawnEaglespot( void );
	int ObjectCaps( void ) { return FCAP_DONT_SAVE; }
public:
	void Suspend( float flSuspendTime );
	void EXPORT Revive( void );
	
	static CLaserSpot *CreateSpot( void );
	static CLaserSpot *CreateSpotRpg( void );
	static CLaserSpot *CreateSpotDGlock( void );
	static CLaserSpot *CreateSpotNailgun( void );
	static CLaserSpot *CreateSpotEagle( void );
};
#endif // WEAPONS_H
