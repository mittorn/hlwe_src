#include	"extdll.h"
#include	"util.h"
#include	"skill.h"

//===============================
//CVARS FOR WEAPON DAMAGE SETTINGS
//===============================

// Melee weapons
cvar_t	dmg_medkit	 	= {"dmg_medkit","10"};
cvar_t	dmg_crowbar	 	= {"dmg_crowbar","20"};
cvar_t	dmg_bandsaw	 	= {"dmg_bandsaw","23"};
cvar_t	dmg_torch	 	= {"dmg_torch","8"};
cvar_t	dmg_lightsaber	 	= {"dmg_lightsaber","125"};
cvar_t	dmg_lightsaber_pistol 	= {"dmg_lightsaber_pistol","41"};

// Hand Guns
cvar_t	dmg_glock	 	= {"dmg_glock","17"};
cvar_t	dmg_usp	 		= {"dmg_usp","24"};
cvar_t	dmg_deagle	 	= {"dmg_deagle","38"};
cvar_t	dmg_python	 	= {"dmg_python","40"};
cvar_t	dmg_uzi	 		= {"dmg_uzi","15"};
cvar_t	dmg_shieldgun	 	= {"dmg_shieldgun","20"};
cvar_t	dmg_knife	 	= {"dmg_knife","22"};

// Assault Weapons
cvar_t	dmg_shotgun	 	= {"dmg_shotgun","12"};
cvar_t	dmg_autoshotgun	 	= {"dmg_autoshotgun","12"};
cvar_t	dmg_30mmsg	 	= {"dmg_30mmsg","19"};
cvar_t	dmg_30mmgren	 	= {"dmg_30mmgren","190"};
cvar_t	dmg_mp5	 		= {"dmg_mp5","20"};
cvar_t	dmg_mp5_grenade	 	= {"dmg_mp5_grenade","200"};
cvar_t	dmg_m16	 		= {"dmg_m16","30"};
cvar_t	dmg_m16_grenade	 	= {"dmg_m16_grenade","12"};
cvar_t	dmg_ak74	 	= {"dmg_ak74","32"};
cvar_t	dmg_ak74_grenade 	= {"dmg_ak74_grenade","205"};

// Sniper Weapons
cvar_t	dmg_crossbow	 	= {"dmg_crossbow","50"};
cvar_t	dmg_crossbow_exp 	= {"dmg_crossbow_exp","120"};
cvar_t	dmg_g11	 		= {"dmg_g11","23"};
cvar_t	dmg_u2	 		= {"dmg_u2","25"};
cvar_t	dmg_u2shard	 	= {"dmg_u2shard","60"};
cvar_t	dmg_u2main	 	= {"dmg_u2main","220"};
cvar_t	dmg_svd	 		= {"dmg_svd","65"};
cvar_t	dmg_awp	 		= {"dmg_awp","115"};
cvar_t	dmg_barett 		= {"dmg_barett","150"};

// Heavy Weapons
cvar_t	dmg_m249 		= {"dmg_m249","28"};
cvar_t	dmg_minigun	 	= {"dmg_minigun","35"};
cvar_t	dmg_machinegun	 	= {"dmg_machinegun","75"};
cvar_t	dmg_nailgun	 	= {"dmg_nailgun","42"};
cvar_t	dmg_flamethrower	= {"dmg_flamethrower","35"};
cvar_t	dmg_froster 		= {"dmg_froster","63"};
cvar_t	dmg_cluster_momma 	= {"dmg_cluster_momma","170"};
cvar_t	dmg_cluster_baby 	= {"dmg_cluster_baby","85"};
cvar_t	dmg_biorifle	 	= {"dmg_biorifle","60"};
cvar_t	dmg_flak_shrapnel 	= {"dmg_flak_shrapnel","32"};
cvar_t	dmg_flak_bomb	 	= {"dmg_flak_bomb","180"};
cvar_t	dmg_bfg	 		= {"dmg_bfg","100"};
cvar_t	dmg_sunofgod 		= {"dmg_sunofgod","5000"};

// Missile Weapons
cvar_t	dmg_rpg	 		= {"dmg_rpg","250"};
cvar_t	dmg_incendiary	 	= {"dmg_incendiary","210"};
cvar_t	dmg_inc_flame 		= {"dmg_inc_flame","30"};
cvar_t	dmg_whl	 		= {"dmg_whl","420"};
cvar_t	dmg_devastator	 	= {"dmg_devastator","190"};
cvar_t	dmg_redeemer 		= {"dmg_redeemer","1500"};

// Energy Weapons
cvar_t	dmg_smartgun 		= {"dmg_smartgun","32"};
cvar_t	dmg_lgtng_ball 		= {"dmg_lgtng_ball","150"};
cvar_t	dmg_teslagun	 	= {"dmg_teslagun","38"};
cvar_t	dmg_tesla_grenade 	= {"dmg_tesla_grenade","160"};
cvar_t	dmg_egon_narrow 	= {"dmg_egon_narrow","25"};
cvar_t	dmg_egon_wide	 	= {"dmg_egon_wide","32"};
cvar_t	dmg_photon_beam		= {"dmg_photon_beam","60"};
cvar_t	dmg_photon_ball		= {"dmg_photon_ball","62"};
cvar_t	dmg_gauss 		= {"dmg_gauss","80"};
cvar_t	dmg_gauss_secondary	= {"dmg_gauss_secondary","420"};
cvar_t	dmg_plasma	 	= {"dmg_plasma","52"};
cvar_t	dmg_plasma_secondary	= {"dmg_plasma_secondary","160"};
cvar_t	dmg_gluon	 	= {"dmg_gluon","50"};
cvar_t	dmg_taucannon 		= {"dmg_taucannon","200"};
cvar_t	dmg_displacer 		= {"dmg_displacer","425"};
cvar_t	dmg_teleenter 		= {"dmg_teleenter","283"};

// Explosives
cvar_t	dmg_flashbang	 	= {"dmg_flashbang","95"};
cvar_t	dmg_handgrenade	 	= {"dmg_handgrenade","220"};
cvar_t	dmg_tripmine	 	= {"dmg_tripmine","280"};
cvar_t	dmg_satchel	 	= {"dmg_satchel","300"};
cvar_t	dmg_c4	 		= {"dmg_c4","700"};

// Experimental
cvar_t	dmg_blaster 		= {"dmg_blaster","50"};
cvar_t	dmg_blaster_beam	= {"dmg_blaster_beam","15"};
cvar_t	dmg_pulserifle 		= {"dmg_pulserifle","320"};
cvar_t	dmg_m72	 		= {"dmg_m72","112"};
cvar_t	dmg_satellite 		= {"dmg_satellite","8000"};
cvar_t	dmg_chrono_radius	= {"dmg_chrono_radius","1500"};
cvar_t	dmg_chrono_clip_min	= {"dmg_chrono_clip_min","350"};
cvar_t	dmg_chrono_clip_max	= {"dmg_chrono_clip_max","2000"};
cvar_t	dmg_pds			= {"dmg_pds","38"};

// Other damages
cvar_t	dmg_shielddet	 	= {"dmg_shielddet","300"};

// WORLD WEAPONS
cvar_t	dmg_9mm_bullet 		= {"dmg_9mm_bullet","25"};
cvar_t	dmg_12mm_bullet	 	= {"dmg_12mm_bullet","40"};
cvar_t	dmg_14mm_bullet 	= {"dmg_14mm_bullet","45"};
cvar_t	dmg_tank_cannon	 	= {"dmg_tank_cannon","1000"};
cvar_t	dmg_dumbfire_missile 	= {"dmg_dumbfire_missile","200"};
cvar_t	dmg_ionturret 		= {"dmg_ionturret","300"};

// HEALTH/CHARGE
cvar_t	give_charger		= {"give_charger","100"};		
cvar_t	give_battery		= {"give_battery","25"};			
cvar_t	give_healthkit		= {"give_healthkit","20"};		

// player damage adjusters
cvar_t	dmg_player_head		= {"dmg_player_head","4"};
cvar_t	dmg_player_chest	= {"dmg_player_chest","1"};
cvar_t	dmg_player_stomach	= {"dmg_player_stomach","1.5"};
cvar_t	dmg_player_arm		= {"dmg_player_arm","0.9"};
cvar_t	dmg_player_leg		= {"dmg_player_leg","0.9"};
cvar_t	dmg_player_armor	= {"dmg_player_armor","1"};

//MONSTERS!!
// Undead damage adjusters

cvar_t	dmg_undead_head		= {"dmg_undead_head","2"};
cvar_t	dmg_undead_chest	= {"dmg_undead_chest","1"};
cvar_t	dmg_undead_stomach	= {"dmg_undead_stomach","1"};
cvar_t	dmg_undead_arm		= {"dmg_undead_arm","0.9"};
cvar_t	dmg_undead_leg		= {"dmg_undead_leg","0.9"};

cvar_t	zombie_health		= {"zombie_health","200"};
cvar_t	zombie_dmg_melee	= {"zombie_dmg_melee","27"};
cvar_t	zombie_dmg_range	= {"zombie_dmg_range","20"};

void SkillRegisterCvars(void)
{
// Melee weapons
	CVAR_REGISTER(&dmg_medkit);
	CVAR_REGISTER(&dmg_crowbar);
	CVAR_REGISTER(&dmg_bandsaw);
	CVAR_REGISTER(&dmg_lightsaber);
	CVAR_REGISTER(&dmg_torch);
	CVAR_REGISTER(&dmg_lightsaber_pistol);

// Hand Guns
	CVAR_REGISTER(&dmg_glock);
	CVAR_REGISTER(&dmg_usp);
	CVAR_REGISTER(&dmg_deagle);
	CVAR_REGISTER(&dmg_python);
	CVAR_REGISTER(&dmg_uzi);
	CVAR_REGISTER(&dmg_shieldgun);
	CVAR_REGISTER(&dmg_knife);

// Assault Weapons
	CVAR_REGISTER(&dmg_shotgun);
	CVAR_REGISTER(&dmg_autoshotgun);
	CVAR_REGISTER(&dmg_30mmsg);
	CVAR_REGISTER(&dmg_30mmgren);
	CVAR_REGISTER(&dmg_mp5);
	CVAR_REGISTER(&dmg_mp5_grenade);
	CVAR_REGISTER(&dmg_m16);
	CVAR_REGISTER(&dmg_m16_grenade);
	CVAR_REGISTER(&dmg_ak74);
	CVAR_REGISTER(&dmg_ak74_grenade);

// Sniper Weapons
	CVAR_REGISTER(&dmg_crossbow);
	CVAR_REGISTER(&dmg_crossbow_exp);
	CVAR_REGISTER(&dmg_g11);
	CVAR_REGISTER(&dmg_u2);
	CVAR_REGISTER(&dmg_u2main);
	CVAR_REGISTER(&dmg_u2shard);
	CVAR_REGISTER(&dmg_svd);
	CVAR_REGISTER(&dmg_awp);
	CVAR_REGISTER(&dmg_barett);

// Heavy Weapons
	CVAR_REGISTER(&dmg_m249);
	CVAR_REGISTER(&dmg_machinegun);
	CVAR_REGISTER(&dmg_minigun);
	CVAR_REGISTER(&dmg_nailgun);
	CVAR_REGISTER(&dmg_flamethrower);
	CVAR_REGISTER(&dmg_froster);
	CVAR_REGISTER(&dmg_cluster_momma);
	CVAR_REGISTER(&dmg_cluster_baby);
	CVAR_REGISTER(&dmg_biorifle);
	CVAR_REGISTER(&dmg_flak_shrapnel);
	CVAR_REGISTER(&dmg_flak_bomb);
	CVAR_REGISTER(&dmg_bfg);
	CVAR_REGISTER(&dmg_sunofgod);

// Missile Weapons
	CVAR_REGISTER(&dmg_rpg);
	CVAR_REGISTER(&dmg_incendiary);
	CVAR_REGISTER(&dmg_inc_flame);
	CVAR_REGISTER(&dmg_whl);
	CVAR_REGISTER(&dmg_devastator);
	CVAR_REGISTER(&dmg_redeemer);

// Energy Weapons
	CVAR_REGISTER(&dmg_smartgun);
	CVAR_REGISTER(&dmg_lgtng_ball);
	CVAR_REGISTER(&dmg_teslagun);
	CVAR_REGISTER(&dmg_tesla_grenade);
	CVAR_REGISTER(&dmg_egon_narrow);
	CVAR_REGISTER(&dmg_egon_wide);
	CVAR_REGISTER(&dmg_photon_beam);
	CVAR_REGISTER(&dmg_photon_ball);
	CVAR_REGISTER(&dmg_gauss);
	CVAR_REGISTER(&dmg_gauss_secondary);
	CVAR_REGISTER(&dmg_plasma);
	CVAR_REGISTER(&dmg_plasma_secondary);
	CVAR_REGISTER(&dmg_gluon);
	CVAR_REGISTER(&dmg_taucannon);
	CVAR_REGISTER(&dmg_displacer);
	CVAR_REGISTER(&dmg_teleenter);

// Explosives
	CVAR_REGISTER(&dmg_flashbang);
	CVAR_REGISTER(&dmg_handgrenade);
	CVAR_REGISTER(&dmg_tripmine);
	CVAR_REGISTER(&dmg_satchel);
	CVAR_REGISTER(&dmg_c4);

// Experimental
	CVAR_REGISTER(&dmg_blaster);
	CVAR_REGISTER(&dmg_blaster_beam);
	CVAR_REGISTER(&dmg_pulserifle);
	CVAR_REGISTER(&dmg_m72);
	CVAR_REGISTER(&dmg_satellite);
	CVAR_REGISTER(&dmg_chrono_radius);
	CVAR_REGISTER(&dmg_chrono_clip_min);
	CVAR_REGISTER(&dmg_chrono_clip_max);
	CVAR_REGISTER(&dmg_pds);

// Other damages
	CVAR_REGISTER(&dmg_shielddet);

// World Weapons
	CVAR_REGISTER(&dmg_9mm_bullet);
	CVAR_REGISTER(&dmg_12mm_bullet);
	CVAR_REGISTER(&dmg_14mm_bullet);
	CVAR_REGISTER(&dmg_tank_cannon);
	CVAR_REGISTER(&dmg_dumbfire_missile);
	CVAR_REGISTER(&dmg_ionturret);

// HEALTH/CHARGE
	CVAR_REGISTER(&give_charger);
	CVAR_REGISTER(&give_healthkit);
	CVAR_REGISTER(&give_battery);

// player damage adjusters
	CVAR_REGISTER(&dmg_player_head);
	CVAR_REGISTER(&dmg_player_chest);
	CVAR_REGISTER(&dmg_player_stomach);
	CVAR_REGISTER(&dmg_player_arm);
	CVAR_REGISTER(&dmg_player_leg);
	CVAR_REGISTER(&dmg_player_armor);

//MONSTERS!!
// undead damage adjusters
	CVAR_REGISTER(&dmg_undead_head);
	CVAR_REGISTER(&dmg_undead_chest);
	CVAR_REGISTER(&dmg_undead_stomach);
	CVAR_REGISTER(&dmg_undead_arm);
	CVAR_REGISTER(&dmg_undead_leg);

	CVAR_REGISTER(&zombie_health);
	CVAR_REGISTER(&zombie_dmg_melee);
	CVAR_REGISTER(&zombie_dmg_range);
}