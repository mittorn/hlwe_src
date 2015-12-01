#include "extdll.h"
#include "util.h"
#include "client.h"
#include "cbase.h"
#include "player.h"
#include "effects.h"
#include "weapons.h"
#include "soundent.h"
#include "gamerules.h"
#include "animation.h"
#include "bot.h"

// weapon firing delay based on skill (min and max delay for each weapon)
float primary_fire_delay[13+1][5][2] = 
{
   // WEAPON_NONE - NOT USED
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

//***************//
//*MELEE WEAPONS*//
//***************//

   // WEAPON_MEDKIT
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_CROWBAR
   {{0.0, 0.1}, {0.2, 0.3}, {0.3, 0.5}, {0.4, 0.6}, {0.6, 1.0}},

   // WEAPON_BANDSAW
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_TORCH
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_LIGTSABER
   {{0.0, 0.1}, {0.2, 0.3}, {0.3, 0.5}, {0.4, 0.6}, {0.6, 1.0}},

//***********//
//*HAND GUNS*//
//***********//

   // WEAPON_GLOCK
   {{0.0, 0.1}, {0.1, 0.2}, {0.2, 0.3}, {0.3, 0.4}, {0.4, 0.5}},

   // WEAPON_GLOCK_AKIMBO
   {{0.0, 0.1}, {0.1, 0.2}, {0.2, 0.3}, {0.3, 0.4}, {0.4, 0.5}},

   // WEAPON_USP
   {{0.0, 0.1}, {0.1, 0.2}, {0.2, 0.3}, {0.3, 0.4}, {0.4, 0.5}},

   // WEAPON_DEAGLE
   {{0.0, 0.1}, {0.1, 0.2}, {0.2, 0.3}, {0.3, 0.4}, {0.4, 0.5}},

   // WEAPON_UZI
   {{0.0, 0.1}, {0.1, 0.2}, {0.2, 0.3}, {0.3, 0.4}, {0.4, 0.5}},

   // WEAPON_UZI_AKIMBO
   {{0.0, 0.1}, {0.1, 0.2}, {0.2, 0.3}, {0.3, 0.4}, {0.4, 0.5}},

   // WEAPON_SHIELDGUN
   {{0.0, 0.1}, {0.1, 0.2}, {0.2, 0.3}, {0.3, 0.4}, {0.4, 0.5}},

/*
   // WEAPON_MP5 (9mmAR)
   {{0.0, 0.1}, {0.1, 0.3}, {0.3, 0.5}, {0.4, 0.6}, {0.5, 0.8}},
   // WEAPON_MINIGUN
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},
   // WEAPON_CROSSBOW
   {{0.0, 0.25}, {0.2, 0.4}, {0.5, 0.7}, {0.8, 1.0}, {1.0, 1.3}},
   // WEAPON_SHOTGUN
   {{0.0, 0.25}, {0.2, 0.5}, {0.4, 0.8}, {0.6, 1.2}, {0.8, 2.0}},
   // WEAPON_RPG
   {{1.0, 3.0}, {2.0, 4.0}, {3.0, 5.0}, {4.0, 6.0}, {5.0, 7.0}},
   // WEAPON_GAUSS
   {{0.0, 0.1}, {0.2, 0.3}, {0.3, 0.5}, {0.5, 0.8}, {1.0, 1.2}},
   // WEAPON_EGON
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},
   // WEAPON_BIORIFLE
   {{0.0, 0.1}, {0.25, 0.4}, {0.4, 0.7}, {0.6, 1.0}, {1.0, 1.5}},
   // WEAPON_HANDGRENADE
   {{1.0, 1.4}, {1.4, 2.0}, {1.8, 2.6}, {2.0, 3.0}, {2.5, 3.8}},
   // WEAPON_TRIPMINE
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},
   // WEAPON_SATCHEL
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},
   // WEAPON_BFG
   {{0.0, 0.1}, {0.1, 0.2}, {0.2, 0.5}, {0.5, 0.7}, {0.6, 1.0}},
*/
};

float secondary_fire_delay[13+1][5][2] =
{
   // WEAPON_NONE - NOT USED
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

//***************//
//*MELEE WEAPONS*//
//***************//

   // WEAPON_MEDKIT
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_CROWBAR (not used)
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_BANDSAW
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_TORCH (not used)
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_LIGTSABER
   {{0.0, 0.1}, {0.2, 0.3}, {0.3, 0.5}, {0.5, 0.8}, {0.7, 1.2}},

//***********//
//*HAND GUNS*//
//***********//

   // WEAPON_GLOCK (Not used)
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_GLOCK_AKIMBO (not used)
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_USP (not used)
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_DEAGLE (not used)
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_UZI (not used)
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_UZI_AKIMBO (not used)
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},

   // WEAPON_SHIELDGUN
   {{0.0, 1.0}, {1.5, 2.0}, {2.0, 2.5}, {2.5, 3.0}, {3.0, 3.5}},




/*
   // WEAPON_MP5 (9mmAR)
   {{0.0, 0.3}, {0.5, 0.8}, {0.7, 1.0}, {1.0, 1.6}, {1.4, 2.0}},
   // WEAPON_MINIGUN
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},
   // WEAPON_CROSSBOW
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},
   // WEAPON_SHOTGUN
   {{0.0, 0.25}, {0.2, 0.5}, {0.4, 0.8}, {0.6, 1.2}, {0.8, 2.0}},
   // WEAPON_RPG - Not applicable
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},
   // WEAPON_GAUSS
   {{0.2, 0.5}, {0.3, 0.7}, {0.5, 1.0}, {0.8, 1.5}, {1.0, 2.0}},
   // WEAPON_EGON - Not applicable
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},
   // WEAPON_BIORIFLE
   {{0.0, 0.1}, {0.2, 0.3}, {0.3, 0.5}, {0.5, 0.8}, {0.7, 1.2}},
   // WEAPON_HANDGRENADE - Not applicable
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},
   // WEAPON_TRIPMINE - Not applicable
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},
   // WEAPON_SATCHEL
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},
   // WEAPON_BFG
   {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}
*/
};   

ammo_check_t ammo_check[] =
{
   {"ammo_glock", "9mmP", 999},
   {"ammo_usp", "45ACP", 999},
   {"ammo_deagle", "50AE", 999},
   {"ammo_python", "357", 999},
   {"ammo_uzi", "8mm", 999},
   {"ammo_shieldgun", "12g", 999},

   {"ammo_mp5", "9mm", 999},
   {"ammo_mp5_sec", "ARgrenades", 999},
   {"ammo_shotgun", "buckshot", 999},
   {"ammo_crossbow", "bolts", 999},
   {"ammo_rpg", "rockets", 999},
   {"ammo_egon", "uranium238", 999},
   {"ammo_gauss", "uranium", 999},
   {"", 0, 0}
};

CBaseEntity * CBot::BotFindEnemy( void )
{
   Vector vecEnd;
   static BOOL flag=TRUE;

   if (pBotEnemy != NULL)  // does the bot already have an enemy?
   {
      vecEnd = pBotEnemy->EyePosition();

      // if the enemy is dead or has switched to Invisibility mode...
      if (!pBotEnemy->IsAlive() || (pBotEnemy->pev->effects & EF_NODRAW))
      {
         if (!pBotEnemy->IsAlive())  // is the enemy dead?, assume bot killed it
         {
            // the enemy is dead, jump for joy about 5% of the time
            if (RANDOM_LONG(1, 100) <= 5)
               pev->button |= IN_JUMP;
         }

         // don't have an enemy anymore so null out the pointer...
         pBotEnemy = NULL;
      }
      else if (FInViewCone( &vecEnd ) && FVisible( vecEnd ))
      {
         // if enemy is still visible and in field of view, keep it

         // face the enemy
         Vector v_enemy = pBotEnemy->pev->origin - pev->origin;
         Vector bot_angles = UTIL_VecToAngles( v_enemy );

         pev->ideal_yaw = bot_angles.y;

         // check for wrap around of angle...
         if (pev->ideal_yaw > 180)
            pev->ideal_yaw -= 360;
         if (pev->ideal_yaw < -180)
            pev->ideal_yaw += 360;

         return (pBotEnemy);
      }
   }

   int i;
   float nearestdistance = 10000;
   CBaseEntity *pNewEnemy = NULL;

   // search the world for players...
   for (i = 1; i <= gpGlobals->maxClients; i++)
   {
      CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );

      // skip invalid players and skip self (i.e. this bot)
      if ((!pPlayer) || (pPlayer == this))
         continue;

      // skip this player if not alive (i.e. dead or dying)
      if (pPlayer->pev->deadflag != DEAD_NO)
         continue;

      // skip players that are not bots in observer mode...
	if ( pPlayer->IsNetClient() && (m_afPhysicsFlags & PFLAG_OBSERVER))
         continue;

      // skip players that are in invisibility mode...
      if (pPlayer->pev->effects & EF_NODRAW)
         continue;

// BigGuy - START
      // is team play enabled?
      if (g_pGameRules->IsTeamplay())
      {
         // don't target your teammates if team names match...
         if (UTIL_TeamsMatch(g_pGameRules->GetTeamID(this),
                             g_pGameRules->GetTeamID(pPlayer)))
            continue;
      }
// BigGuy - END

      vecEnd = pPlayer->EyePosition();

      // see if bot can see the player...
      if (FInViewCone( &vecEnd ) && FVisible( vecEnd ))
      {
         float distance = (pPlayer->pev->origin - pev->origin).Length();
         if (distance < nearestdistance)
         {
            nearestdistance = distance;
            pNewEnemy = pPlayer;

            pBotUser = NULL;  // don't follow user when enemy found
         }
      }
   }

   if (pNewEnemy)
   {
      // face the enemy
      Vector v_enemy = pNewEnemy->pev->origin - pev->origin;
      Vector bot_angles = UTIL_VecToAngles( v_enemy );

      pev->ideal_yaw = bot_angles.y;

      // check for wrap around of angle...
      if (pev->ideal_yaw > 180)
         pev->ideal_yaw -= 360;
      if (pev->ideal_yaw < -180)
         pev->ideal_yaw += 360;
   }

   return (pNewEnemy);
}


Vector CBot::BotBodyTarget( CBaseEntity *pBotEnemy )
{
   Vector target;
   float f_distance;
   float f_scale;
   int d_x, d_y, d_z;

   f_distance = (pBotEnemy->pev->origin - pev->origin).Length();

   if (f_distance > 1000)
      f_scale = 1.0;
   else if (f_distance > 100)
      f_scale = f_distance / 1000.0;
   else
      f_scale = 0.1;

   switch (bot_skill)
   {
      case 0:
         // VERY GOOD, same as from CBasePlayer::BodyTarget (in player.h)
         target = pBotEnemy->Center() + pBotEnemy->pev->view_ofs * RANDOM_FLOAT( 0.5, 1.1 );
         d_x = 0;  // no offset
         d_y = 0;
         d_z = 0;
         break;
      case 1:
         // GOOD, offset a little for x, y, and z
         target = pBotEnemy->Center() + pBotEnemy->pev->view_ofs;
         d_x = RANDOM_FLOAT(-5, 5) * f_scale;
         d_y = RANDOM_FLOAT(-5, 5) * f_scale;
         d_z = RANDOM_FLOAT(-9, 9) * f_scale;
         break;
      case 2:
         // FAIR, offset somewhat for x, y, and z
         target = pBotEnemy->Center() + pBotEnemy->pev->view_ofs;
         d_x = RANDOM_FLOAT(-9, 9) * f_scale;
         d_y = RANDOM_FLOAT(-9, 9) * f_scale;
         d_z = RANDOM_FLOAT(-15, 15) * f_scale;
         break;
      case 3:
         // POOR, offset for x, y, and z
         target = pBotEnemy->Center() + pBotEnemy->pev->view_ofs;
         d_x = RANDOM_FLOAT(-16, 16) * f_scale;
         d_y = RANDOM_FLOAT(-16, 16) * f_scale;
         d_z = RANDOM_FLOAT(-20, 20) * f_scale;
         break;
      case 4:
         // BAD, offset lots for x, y, and z
         target = pBotEnemy->Center() + pBotEnemy->pev->view_ofs;
         d_x = RANDOM_FLOAT(-20, 20) * f_scale;
         d_y = RANDOM_FLOAT(-20, 20) * f_scale;
         d_z = RANDOM_FLOAT(-27, 27) * f_scale;
         break;
   }

   target = target + Vector(d_x, d_y, d_z);

   return target;
}


void CBot::BotWeaponInventory( void )
{
   int i;

   // initialize the elements of the weapons arrays...
   for (i = 0; i < MAX_WEAPONS; i++)
   {
      weapon_ptr[i] = NULL;
      primary_ammo[i] = 0;
      secondary_ammo[i] = 0;
   }

   // find out which weapons the bot is carrying...
   for (i = 0; i < MAX_ITEM_TYPES; i++)
   {
      CBasePlayerItem *pItem = NULL;

      if (m_rgpPlayerItems[i])
      {
         pItem = m_rgpPlayerItems[i];
         while (pItem)
         {
            weapon_ptr[pItem->m_iId] = pItem;  // store pointer to item

            pItem = pItem->m_pNext;
         }
      }
   }

   // find out how much ammo of each type the bot is carrying...
   for (i = 0; i < MAX_AMMO_SLOTS; i++)
   {
      if (!CBasePlayerItem::AmmoInfoArray[i].pszName)
         continue;

      if (strcmp("gasoline", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         primary_ammo[WEAPON_BANDSAW] = m_rgAmmo[i];

	else if (strcmp("9mmP", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
	{
		primary_ammo[WEAPON_GLOCK] = m_rgAmmo[i];
		primary_ammo[WEAPON_GLOCK_AKIMBO] = m_rgAmmo[i];
	}

      else if (strcmp("45ACP", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         primary_ammo[WEAPON_USP] = m_rgAmmo[i];

      else if (strcmp("50AE", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         primary_ammo[WEAPON_DEAGLE] = m_rgAmmo[i];

      else if (strcmp("357", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         primary_ammo[WEAPON_PYTHON] = m_rgAmmo[i];

      else if (strcmp("8mm", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
	{
         primary_ammo[WEAPON_UZI] = m_rgAmmo[i];
         primary_ammo[WEAPON_UZI_AKIMBO] = m_rgAmmo[i];
	}

      else if (strcmp("12g", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         primary_ammo[WEAPON_SHIELDGUN] = m_rgAmmo[i];




      else if (strcmp("9mm", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
      {
         primary_ammo[WEAPON_MP5] = m_rgAmmo[i];
      }
      else if (strcmp("ARgrenades", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         secondary_ammo[WEAPON_MP5] = m_rgAmmo[i];

      else if (strcmp("bolts", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
      {
         primary_ammo[WEAPON_CROSSBOW] = m_rgAmmo[i];
      }
      else if (stricmp("buckshot", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         primary_ammo[WEAPON_SHOTGUN] = m_rgAmmo[i];

      else if (stricmp("rockets", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         primary_ammo[WEAPON_RPG] = m_rgAmmo[i];

      else if (strcmp("uranium238", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         primary_ammo[WEAPON_EGON] = m_rgAmmo[i];

      else if (strcmp("uranium", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         primary_ammo[WEAPON_GAUSS] = m_rgAmmo[i];

      else if (stricmp("Hand Grenade", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         primary_ammo[WEAPON_HANDGRENADE] = m_rgAmmo[i];

      else if (stricmp("Trip Mine", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         primary_ammo[WEAPON_TRIPMINE] = m_rgAmmo[i];

      else if (stricmp("Satchel Charge", CBasePlayerItem::AmmoInfoArray[i].pszName) == 0)
         primary_ammo[WEAPON_SATCHEL] = m_rgAmmo[i];

   }

}

// specifing a weapon_choice allows you to choose the weapon the bot will
// use (assuming enough ammo exists for that weapon)
// BotFireWeapon will return TRUE if weapon was fired, FALSE otherwise
// primary is used to indicate whether you want primary or secondary fire
// if you have specified a weapon using weapon_choice

BOOL CBot::BotFireWeapon( Vector v_enemy_origin, int weapon_choice, BOOL primary )
{
   CBasePlayerItem *new_weapon;
   BOOL enemy_below;

   // is it time to check weapons inventory yet?
   if (f_weapon_inventory_time <= gpGlobals->time)
   {
      // check weapon and ammo inventory then update check time...
      BotWeaponInventory();
      f_weapon_inventory_time = gpGlobals->time + 2.0;
   }

   Vector v_enemy = v_enemy_origin - GetGunPosition( );
   float distance = v_enemy.Length();  // how far away is the enemy?

   // is enemy at least 45 units below bot? (for handgrenades and snarks)
   if (v_enemy_origin.z < (pev->origin.z - 45))
      enemy_below = TRUE;
   else
      enemy_below = FALSE;

//***************//
//*MELEE WEAPONS*//
//***************//

//CROWBAR
   if (pev->weapons & (1<<WEAPON_CROWBAR))
   {
      // No ammo?  close to enemy? use the crowbar
//Надо поправить, чтобы доставать лом в случае отсутствия каких-либо патронов, а не только Глоковских...
      if (( (primary_ammo[WEAPON_GLOCK] <= 0) && (weapon_choice == 0)) || (weapon_choice == WEAPON_CROWBAR))
      {
         new_weapon = weapon_ptr[WEAPON_CROWBAR];

         // check if the bot isn't already using this item...
         if (m_pActiveItem != new_weapon)
            SelectItem("weapon_crowbar");  // select the crowbar

	         pev->button |= IN_ATTACK;  // use primary attack (whack! whack!)

	         // set next time to "shoot"
	         f_shoot_time = gpGlobals->time + 0.2 + RANDOM_FLOAT(primary_fire_delay[WEAPON_CROWBAR][bot_skill][0], primary_fire_delay[WEAPON_CROWBAR][bot_skill][1]);
         return TRUE;
      }
   }

//BANDSAW
   if (pev->weapons & (1<<WEAPON_BANDSAW) && (pev->waterlevel != 3) )
   {
      if (( (weapon_choice == 0)) || (weapon_choice == WEAPON_BANDSAW))
      {
         new_weapon = weapon_ptr[WEAPON_BANDSAW];
         if (primary_ammo[WEAPON_BANDSAW] > 0)
         {
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_bandsaw");

		if(distance < 80)
		{
	               pev->button |= IN_ATTACK;
	               f_shoot_time = gpGlobals->time + RANDOM_FLOAT(primary_fire_delay[WEAPON_BANDSAW][bot_skill][0], primary_fire_delay[WEAPON_BANDSAW][bot_skill][1]);
		}
            }
            return TRUE;
         }
      }

// LIGHTSABER
   if (pev->weapons & (1<<WEAPON_LIGHTSABER) && (pev->waterlevel != 3))
   {
      if (( (weapon_choice == 0)) || (weapon_choice == WEAPON_LIGHTSABER))
      {
         new_weapon = weapon_ptr[WEAPON_LIGHTSABER];

            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_lightsaber");

            if (distance < 125) 
            {
               pev->button |= IN_ATTACK;
               f_shoot_time = gpGlobals->time + 0.2 + RANDOM_FLOAT(primary_fire_delay[WEAPON_LIGHTSABER][bot_skill][0], primary_fire_delay[WEAPON_LIGHTSABER][bot_skill][1]);
            }
            else
            {
               pev->button |= IN_ATTACK2;
               f_shoot_time = gpGlobals->time + 0.7 + RANDOM_FLOAT(secondary_fire_delay[WEAPON_LIGHTSABER][bot_skill][0], secondary_fire_delay[WEAPON_LIGHTSABER][bot_skill][1]);
            }
            return TRUE;
         }
      }

//***********//
//*HAND GUNS*//
//***********//

//GLOCK

   if (pev->weapons & (1<<WEAPON_GLOCK))
   {
      // if nothing else was selected, try the good ol' 9mm glock...
      if (((distance < 2000) && (weapon_choice == 0)) || (weapon_choice == WEAPON_GLOCK))
      {
         new_weapon = weapon_ptr[WEAPON_GLOCK];

         // check if the bot has any ammo left for this weapon...
         if (primary_ammo[WEAPON_GLOCK] > 0)
         {
            // check if the bot isn't already using this item...
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_glock");  // select the trusty 9mm glock

               pev->button |= IN_ATTACK;  // use primary attack (bang! bang!)
               // set next time to shoot
               f_shoot_time = gpGlobals->time + 0.1 + RANDOM_FLOAT(primary_fire_delay[WEAPON_GLOCK][bot_skill][0], primary_fire_delay[WEAPON_GLOCK][bot_skill][1]);
            }
            return TRUE;
         }
      }

//GLOCK AKIMBO
   if (pev->weapons & (1<<WEAPON_GLOCK_AKIMBO))
   {
      if (((distance < 1800) && (weapon_choice == 0)) || (weapon_choice == WEAPON_GLOCK_AKIMBO))
      {
         new_weapon = weapon_ptr[WEAPON_GLOCK_AKIMBO];
         if (primary_ammo[WEAPON_GLOCK_AKIMBO] > 0)
         {
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_glock_akimbo");
               pev->button |= IN_ATTACK;
               f_shoot_time = gpGlobals->time + 0.1 + RANDOM_FLOAT(primary_fire_delay[WEAPON_GLOCK_AKIMBO][bot_skill][0], primary_fire_delay[WEAPON_GLOCK_AKIMBO][bot_skill][1]);
            }
            return TRUE;
         }
      }

//USP
   if (pev->weapons & (1<<WEAPON_USP) && (pev->waterlevel != 3))
   {
      if (((distance < 1800) && (weapon_choice == 0)) || (weapon_choice == WEAPON_USP))
      {
         new_weapon = weapon_ptr[WEAPON_USP];
         if (primary_ammo[WEAPON_USP] > 0)
         {
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_usp");
               pev->button |= IN_ATTACK;
               f_shoot_time = gpGlobals->time + 0.1 + RANDOM_FLOAT(primary_fire_delay[WEAPON_USP][bot_skill][0], primary_fire_delay[WEAPON_USP][bot_skill][1]);
            }
            return TRUE;
         }
      }

//Deagle
   if (pev->weapons & (1<<WEAPON_DEAGLE) && (pev->waterlevel != 3))
   {
      if (((distance < 1800) && (weapon_choice == 0)) || (weapon_choice == WEAPON_DEAGLE))
      {
         new_weapon = weapon_ptr[WEAPON_DEAGLE];
         if (primary_ammo[WEAPON_DEAGLE] > 0)
         {
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_deagle");
               pev->button |= IN_ATTACK;
               f_shoot_time = gpGlobals->time + 0.1 + RANDOM_FLOAT(primary_fire_delay[WEAPON_DEAGLE][bot_skill][0], primary_fire_delay[WEAPON_DEAGLE][bot_skill][1]);
            }
            return TRUE;
         }
      }

//Python
   // if the bot is carrying the 357/PYTHON, (can't use underwater)...
   if ((pev->weapons & (1<<WEAPON_PYTHON)) && (pev->waterlevel != 3))
   {
      // if close enough for 357 shot...
      if (( (distance > 30) && (distance < 2300) && (weapon_choice == 0)) || (weapon_choice == WEAPON_PYTHON))
      {
         new_weapon = weapon_ptr[WEAPON_PYTHON];

         if (primary_ammo[WEAPON_PYTHON] > 0)
         {
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_python");

            pev->button |= IN_ATTACK;

            // set next time to shoot
            f_shoot_time = gpGlobals->time + 0.1 + RANDOM_FLOAT(primary_fire_delay[WEAPON_PYTHON][bot_skill][0],primary_fire_delay[WEAPON_PYTHON][bot_skill][1]);
            return TRUE;
         }
      }
   }

//UZI
   if (pev->weapons & (1<<WEAPON_UZI) && (pev->waterlevel != 3))
   {
      if (((distance < 1500) && (weapon_choice == 0)) || (weapon_choice == WEAPON_UZI))
      {
         new_weapon = weapon_ptr[WEAPON_UZI];
         if (primary_ammo[WEAPON_UZI] > 0)
         {
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_uzi");
               pev->button |= IN_ATTACK;
               f_shoot_time = gpGlobals->time + 0.1 + RANDOM_FLOAT(primary_fire_delay[WEAPON_UZI][bot_skill][0], primary_fire_delay[WEAPON_UZI][bot_skill][1]);
            }
            return TRUE;
         }
      }

//UZI AKIMBO
   if (pev->weapons & (1<<WEAPON_UZI_AKIMBO) && (pev->waterlevel != 3))
   {
      if (((distance < 1400) && (weapon_choice == 0)) || (weapon_choice == WEAPON_UZI_AKIMBO))
      {
         new_weapon = weapon_ptr[WEAPON_UZI_AKIMBO];
         if (primary_ammo[WEAPON_UZI_AKIMBO] > 0)
         {
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_uzi_akimbo");
               pev->button |= IN_ATTACK;
               f_shoot_time = gpGlobals->time + 0.1 + RANDOM_FLOAT(primary_fire_delay[WEAPON_UZI_AKIMBO][bot_skill][0], primary_fire_delay[WEAPON_UZI_AKIMBO][bot_skill][1]);
            }
            return TRUE;
         }
      }

// SHIELDGUN
   if (pev->weapons & (1<<WEAPON_SHIELDGUN) && (pev->waterlevel != 3))
   {
      if (((weapon_choice == 0)) || (weapon_choice == WEAPON_SHIELDGUN))
      {
         new_weapon = weapon_ptr[WEAPON_SHIELDGUN];

         if (primary_ammo[WEAPON_SHIELDGUN] > 0)
         {
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_shieldgun");

	    if ((distance > 70) && (distance < 850))
            {
               pev->button |= IN_ATTACK;
               f_shoot_time = gpGlobals->time + 0.2 + RANDOM_FLOAT(primary_fire_delay[WEAPON_SHIELDGUN][bot_skill][0], primary_fire_delay[WEAPON_SHIELDGUN][bot_skill][1]);
            }
	    else if (distance < 50)
     	   {
               pev->button |= IN_ATTACK;
               f_shoot_time = gpGlobals->time + 0.2 + RANDOM_FLOAT(primary_fire_delay[WEAPON_SHIELDGUN][bot_skill][0], primary_fire_delay[WEAPON_SHIELDGUN][bot_skill][1]);
	   }
            else
            {
               pev->button |= IN_ATTACK2;
               f_shoot_time = gpGlobals->time + 2.0 + RANDOM_FLOAT(secondary_fire_delay[WEAPON_SHIELDGUN][bot_skill][0], secondary_fire_delay[WEAPON_SHIELDGUN][bot_skill][1]);
            }
            return TRUE;
         }
      }
   }



   // if bot is carrying any hand grenades and enemy is below bot...
   if ((pev->weapons & (1<<WEAPON_HANDGRENADE)) && (enemy_below))
   {
      long use_grenade = RANDOM_LONG(1,100);

      // use hand grenades about 30% of the time...
      if (((distance > 150) && (distance < 750) && (weapon_choice == 0) && (use_grenade <= 30)) || (weapon_choice == WEAPON_HANDGRENADE))
      {
// BigGuy - START
         new_weapon = weapon_ptr[WEAPON_HANDGRENADE];

         // check if the bot isn't already using this item...
         if (m_pActiveItem != new_weapon)
            SelectItem("weapon_handgrenade");  // select the hand grenades

         pev->button |= IN_ATTACK;  // use primary attack (boom!)

         // set next time to "shoot"
         f_shoot_time = gpGlobals->time + 0.1 + 
            RANDOM_FLOAT(primary_fire_delay[WEAPON_HANDGRENADE][bot_skill][0],
                         primary_fire_delay[WEAPON_HANDGRENADE][bot_skill][1]);
         return TRUE;
// BigGuy - END
      }
   }

   // if the bot is carrying the egon gun (can't use underwater)...
   if ((pev->weapons & (1<<WEAPON_EGON)) && (pev->waterlevel != 3))
   {
      if ((weapon_choice == 0) || (weapon_choice == WEAPON_EGON))
      {
         new_weapon = weapon_ptr[WEAPON_EGON];

         // check if the bot has any ammo left for this weapon...
         if (primary_ammo[WEAPON_EGON] > 0)
         {
            // check if the bot isn't already using this item...
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_egon");  // select the egon gun

            pev->button |= IN_ATTACK;  // use primary attack (bang! bang!)

            // set next time to shoot
            f_shoot_time = gpGlobals->time;

            return TRUE;
         }
      }
   }

   // if the bot is carrying the gauss gun (can't use underwater)...
   if ((pev->weapons & (1<<WEAPON_GAUSS)) && (pev->waterlevel != 3))
   {
      if ((weapon_choice == 0) || (weapon_choice == WEAPON_GAUSS))
      {
         new_weapon = weapon_ptr[WEAPON_GAUSS];

         // check if the bot has any ammo left for this weapon...
         if (primary_ammo[WEAPON_GAUSS] > 1)
         {
            // check if the bot isn't already using this item...
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_gauss");  // select the gauss gun

            long use_secondary = RANDOM_LONG(1,100);

            // are we charging the gauss gun?
            if (f_fire_gauss > 0)
            {
               // is it time to fire the charged gauss gun?
               if (f_fire_gauss >= gpGlobals->time)
               {
                  // we DON'T set pev->button here to release the secondary
                  // fire button which will fire the charged gauss gun

                  f_fire_gauss = -1;  // -1 means not charging gauss gun

                  // set next time to shoot
                  f_shoot_time = gpGlobals->time + 1.0 +
                     RANDOM_FLOAT(secondary_fire_delay[WEAPON_GAUSS][bot_skill][0],
                                  secondary_fire_delay[WEAPON_GAUSS][bot_skill][1]);
               }
               else
               {
                  pev->button |= IN_ATTACK2;  // charge the gauss gun
                  f_shoot_time = gpGlobals->time;  // keep charging
               }
            }
            else if ((use_secondary <= 20) &&
                     (primary_ammo[WEAPON_GAUSS] >= 10))
            {
               // release secondary fire in 0.5 seconds...
               f_fire_gauss = gpGlobals->time + 0.5;

               pev->button |= IN_ATTACK2;  // charge the gauss gun
               f_shoot_time = gpGlobals->time; // keep charging
            }
            else
            {
               pev->button |= IN_ATTACK;  // use primary attack (bang! bang!)

               // set next time to shoot
               f_shoot_time = gpGlobals->time + 0.2 +
                  RANDOM_FLOAT(primary_fire_delay[WEAPON_GAUSS][bot_skill][0],
                               primary_fire_delay[WEAPON_GAUSS][bot_skill][1]);
            }

            return TRUE;
         }
      }
   }

   // if the bot is carrying the shotgun (can't use underwater)...
   if ((pev->weapons & (1<<WEAPON_SHOTGUN)) && (pev->waterlevel != 3))
   {
      // if close enough for good shotgun blasts...
      if (((distance > 30) && (distance < 150) && (weapon_choice == 0)) ||
          (weapon_choice == WEAPON_SHOTGUN))
      {
         new_weapon = weapon_ptr[WEAPON_SHOTGUN];

         // check if the bot has any ammo left for this weapon...
         if (primary_ammo[WEAPON_SHOTGUN] > 0)
         {
            // check if the bot isn't already using this item...
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_shotgun");  // select the shotgun

            long use_secondary = RANDOM_LONG(1,100);

            // use secondary attack about 30% of the time
            if ((use_secondary <= 30) && (primary_ammo[WEAPON_SHOTGUN] >= 2))
            {
// BigGuy - START
               pev->button |= IN_ATTACK2;  // use secondary attack (bang! bang!)

               // set next time to shoot
               f_shoot_time = gpGlobals->time + 1.5 +
                  RANDOM_FLOAT(secondary_fire_delay[WEAPON_SHOTGUN][bot_skill][0],
                               secondary_fire_delay[WEAPON_SHOTGUN][bot_skill][1]);
            }
// BigGuy - END
            else
            {
               pev->button |= IN_ATTACK;  // use primary attack (bang! bang!)

               // set next time to shoot
               f_shoot_time = gpGlobals->time + 0.75 +
                  RANDOM_FLOAT(primary_fire_delay[WEAPON_SHOTGUN][bot_skill][0],
                               primary_fire_delay[WEAPON_SHOTGUN][bot_skill][1]);
            }

            return TRUE;
         }
      }
   }


   // if the bot is carrying the MP5 (can't use underwater)...
   if ((pev->weapons & (1<<WEAPON_MP5)) && (pev->waterlevel != 3))
   {
      long use_secondary = RANDOM_LONG(1,100);

      // use secondary attack about 10% of the time...
      if (((distance > 300) && (distance < 600) &&
           (weapon_choice == 0) && (use_secondary <= 10)) ||
          ((weapon_choice == WEAPON_MP5) && (primary == FALSE)))
      {
         // at some point we need to fire upwards in the air slightly
         // for long distance kills.  for right now, just fire the
         // grenade at the poor sucker.

// BigGuy - START
         new_weapon = weapon_ptr[WEAPON_MP5];

         // check if the bot has any ammo left for this weapon...
         if (secondary_ammo[WEAPON_MP5] > 0)
         {
            // check if the bot isn't already using this item...
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_9mmAR");  // select the 9mmAR (MP5)

            pev->button |= IN_ATTACK2;  // use secodnary attack (boom!)

            // set next time to shoot
            f_shoot_time = gpGlobals->time + 1.0 +
               RANDOM_FLOAT(secondary_fire_delay[WEAPON_MP5][bot_skill][0],
                            secondary_fire_delay[WEAPON_MP5][bot_skill][1]);

            return TRUE;
         }
// BigGuy - END
      }

      // if close enough for good MP5 shot...
      if (((distance < 250) && (weapon_choice == 0)) ||
          (weapon_choice == WEAPON_MP5))
      {
         new_weapon = weapon_ptr[WEAPON_MP5];

         // check if the bot has any ammo left for this weapon...
         if (primary_ammo[WEAPON_MP5] > 0)
         {
            // check if the bot isn't already using this item...
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_9mmAR");  // select the 9mmAR (MP5)

            pev->button |= IN_ATTACK;  // use primary attack (bang! bang!)

            // set next time to shoot
            f_shoot_time = gpGlobals->time + 0.1 +
               RANDOM_FLOAT(primary_fire_delay[WEAPON_MP5][bot_skill][0],
                            primary_fire_delay[WEAPON_MP5][bot_skill][1]);

            return TRUE;
         }
      }
   }

   // if the bot is carrying the crossbow...
   if (pev->weapons & (1<<WEAPON_CROSSBOW))
   {
      // if bot is not too close for crossbow and not too far...
      if (((distance > 100) && (distance < 1000) && (weapon_choice == 0)) ||
          (weapon_choice == WEAPON_CROSSBOW))
      {
         new_weapon = weapon_ptr[WEAPON_CROSSBOW];

         // check if the bot has any ammo left for this weapon...
         if (primary_ammo[WEAPON_CROSSBOW] > 0)
         {
            // check if the bot isn't already using this item...
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_crossbow");  // select the crossbow

            pev->button |= IN_ATTACK;  // use primary attack (bang! bang!)

            // set next time to shoot
            f_shoot_time = gpGlobals->time + 0.75 +
               RANDOM_FLOAT(primary_fire_delay[WEAPON_CROSSBOW][bot_skill][0],
                            primary_fire_delay[WEAPON_CROSSBOW][bot_skill][1]);

            return TRUE;
         }
      }
   }

   // if the bot is carrying the RPG...
   if (pev->weapons & (1<<WEAPON_RPG))
   {
      // don't use the RPG unless the enemy is pretty far away...
      if (((distance > 300) && (weapon_choice == 0)) ||
          (weapon_choice == WEAPON_RPG))
      {
         new_weapon = weapon_ptr[WEAPON_RPG];

         // check if the bot has any ammo left for this weapon...
         if (primary_ammo[WEAPON_RPG] > 0)
         {
            // check if the bot isn't already using this item...
            if (m_pActiveItem != new_weapon)
               SelectItem("weapon_rpg");  // select the RPG rocket launcher

            pev->button |= IN_ATTACK;  // use primary attack (bang! bang!)

            // set next time to shoot
            f_shoot_time = gpGlobals->time + 1.5 +
               RANDOM_FLOAT(primary_fire_delay[WEAPON_RPG][bot_skill][0],
                            primary_fire_delay[WEAPON_RPG][bot_skill][1]);

            return TRUE;
         }
      }
   }
   // didn't have any available weapons or ammo, return FALSE
   return FALSE;
}


void CBot::BotShootAtEnemy( void )
{
   float f_distance;

   // aim for the head and/or body
   Vector v_enemy = BotBodyTarget( pBotEnemy ) - GetGunPosition();
   pev->v_angle = UTIL_VecToAngles( v_enemy );
   pev->angles.x = 0;
   pev->angles.y = pev->v_angle.y;
   pev->angles.z = 0;
   pev->ideal_yaw = pev->v_angle.y;

   // check for wrap around of angle...
   if (pev->ideal_yaw > 180)
      pev->ideal_yaw -= 360;
   if (pev->ideal_yaw < -180)
      pev->ideal_yaw += 360;

   pev->v_angle.x = -pev->v_angle.x;  //adjust pitch to point gun

   // is it time to shoot yet?
   if (f_shoot_time <= gpGlobals->time)
   {
      // select the best weapon to use at this distance and fire...
      BotFireWeapon( pBotEnemy->pev->origin );
   }

   v_enemy.z = 0;  // ignore z component (up & down)
   f_distance = v_enemy.Length();  // how far away is the enemy scum?

   if (f_distance > 300)      // run if distance to enemy is far
      f_move_speed = f_max_speed;
   else if (f_distance > 35)  // walk if distance is closer
      f_move_speed = f_max_speed / 2;
   else                     // don't move if close enough
      f_move_speed = 0.0;
}
