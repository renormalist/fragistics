/* gameevent.cpp
   Fragistics  -  Game Statistics program for Quake 3 Arena
   Copyright (C) 2000   Brian Risinger
   
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
   
*/

#include "defines.h"
#include "aux.h"

const char *Aux::item2str (int item)
{
    switch (item) {
    case ITEM_NONE: return "ITEM_NONE";
    case ITEM_UNKNOWN: return "ITEM_UNKNOWN";
    case ITEM_GAUNTLET: return "ITEM_GAUNTLET";
    case ITEM_MACHINEGUN: return "ITEM_MACHINEGUN";
    case ITEM_SHOTGUN: return "ITEM_SHOTGUN";
    case ITEM_GRENADEL: return "ITEM_GRENADEL";
    case ITEM_ROCKETL: return "ITEM_ROCKETL";
    case ITEM_LIGHTNINGGUN: return "ITEM_LIGHTNINGGUN";
    case ITEM_RAILGUN: return "ITEM_RAILGUN";
    case ITEM_PLASMAGUN: return "ITEM_PLASMAGUN";
    case ITEM_BFG: return "ITEM_BFG";
    case ITEM_ARMORSHARD: return "ITEM_ARMORSHARD";
    case ITEM_YELLOWARMOR: return "ITEM_YELLOWARMOR";
    case ITEM_REDARMOR: return "ITEM_REDARMOR";
    case ITEM_5HEALTH: return "ITEM_5HEALTH";
    case ITEM_25HEALTH: return "ITEM_25HEALTH";
    case ITEM_50HEALTH: return "ITEM_50HEALTH";
    case ITEM_BATTLESUIT: return "ITEM_BATTLESUIT";
    case ITEM_FLIGHT: return "ITEM_FLIGHT";
    case ITEM_HASTE: return "ITEM_HASTE";
    case ITEM_INVISIBILITY: return "ITEM_";
    case ITEM_MEGAHEALTH: return "ITEM_MEGAHEALTH";
    case ITEM_QUADDAMAGE: return "ITEM_QUADDAMAGE";
    case ITEM_REGENERATION: return "ITEM_REGENERATION";
    case ITEM_MEDKIT: return "ITEM_MEDKIT";
    case ITEM_TELEPORTER: return "ITEM_TELEPORTER";
    case ITEM_REDFLAG: return "ITEM_REDFLAG";
    case ITEM_BLUEFLAG: return "ITEM_BLUEFLAG";
    case ITEM_BULLETS: return "ITEM_BULLETS";
    case ITEM_SHELLS: return "ITEM_SHELLS";
    case ITEM_GRENADES: return "ITEM_GRENADES";
    case ITEM_ROCKETS: return "ITEM_ROCKETS";
    case ITEM_LIGHTNING: return "ITEM_LIGHTNING";
    case ITEM_SLUGS: return "ITEM_SLUGS";
    case ITEM_PLASMAAMMO: return "ITEM_PLASMAAMMO";
    case ITEM_BFGAMMO: return "ITEM_BFGAMMO";
    case ITEM_LAST: return "ITEM_LAST";
    default: return "ITEM_??";
    }
}

const char *Aux::event2str (int event)
{
    switch (event) {
    case EVENT_NONE: return "EVENT_NONE"; 
    case EVENT_GAMESTART: return "EVENT_GAMESTART"; 
    case EVENT_PLAYERJOIN: return "EVENT_PLAYERJOIN";
    case EVENT_PLAYERLEAVE: return "EVENT_PLAYERLEAVE"; 
    case EVENT_ITEMPICKUP: return "EVENT_ITEMPICKUP"; 
    case EVENT_KILL: return "EVENT_KILL"; 
    case EVENT_GAMEEND: return "EVENT_GAMEEND"; 
    case EVENT_SAY: return "EVENT_SAY"; 
    case EVENT_SAY_TEAM: return "EVENT_SAY_TEAM"; 
    case EVENT_PLAYERINFO: return "EVENT_PLAYERINFO"; 
    case EVENT_LIMITHIT: return "EVENT_LIMITHIT"; 
    case EVENT_PLAYER_SCORE: return "EVENT_PLAYER_SCORE"; 
    case EVENT_TEAM_SCORE: return "EVENT_TEAM_SCORE";
    default: return "EVENT_??";
    }
}


const char *Aux::team2str (int team)
{
    switch (team) {
    case TEAM_FFA: return "TEAM_FFA";
    case TEAM_RED: return "TEAM_RED";
    case TEAM_BLUE: return "TEAM_BLUE";
    case TEAM_SPECTATOR: return "TEAM_SPECTATOR";
    case TEAM_MAX: return "TEAM_MAX";
    default: return "TEAM_??";
    }
}
