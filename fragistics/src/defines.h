/* defines.h
 Fragistics  -  Game Statistics program for Quake 3 Arena
 Copyright (C) 2000   Brian Risinger

see main.cpp for license info
*/

#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

#include <string>

using namespace std;


//kill types - id defined - cant be changed
#define TOTAL_KILLS			0

#define SHOTGUN_KILL		1
#define GAUNTLET_KILL		2
#define MACHINEGUN_KILL		3
#define GRENADE_KILL_DIRECT	4
#define GRENADE_KILL_SPLASH	5
#define ROCKET_KILL_DIRECT	6
#define ROCKET_KILL_SPLASH	7
#define PLASMA_KILL_DIRECT	8
#define PLASMA_KILL_SPLASH	9
#define RAILGUN_KILL		10
#define LIGHTNING_KILL		11
#define BFG_KILL_DIRECT		12
#define BFG_KILL_SPLASH		13

#define SELF_WATER			14
#define SELF_SLIME			15
#define SELF_LAVA			16
#define SELF_CRUSH			17
#define TELEFRAG_KILL		18
#define SELF_FALLING		19
#define SELF_SUICIDE		20
	#define UNKNOWN_3		21
#define SELF_TRIGGER_HURT	22


#define UNKNOWN_KILL		23
#define MAX_KILLTYPE		24
//max_killtype must be last of normal kill types

//aggregate kill types - can be changed
#define AGGREGATE_KILLS_MIN	25

#define A_ROCKET_KILL		25		//6+7
#define A_GRENADE_KILL		26		//4+5
#define A_PLASMA_KILL		27		//8+9
#define A_BFG_KILL			28		//12+13

#define A_EXPLOSIVE_KILL	29		//4+5+6+7
#define A_BULLET_KILL		30		//1+3
#define A_ENERGY_KILL		31		//8+9+12+13
#define A_OTHER_KILL		32		//2+10+11+18

#define A_HITSCAN_KILL		33		//1+2+3+10   ?+11?
#define A_NONHITSCAN_KILL	34		//4+5+6+7+8+9+12+13

#define A_SUICIDE_KILL		35		//14+15+16+17+19+20+22
#define A_NONWEP_KILL		36		//14+15+16+17+18+19+20+22

#define AGGREGATE_KILLS_MAX	37
#define CROSSTABLE_MAX_KILL	37//was 14 origionally



//item types - can be changed
#define ITEM_NONE			-1
#define ITEM_UNKNOWN		0
#define ITEM_GAUNTLET		1
#define ITEM_MACHINEGUN		2
#define ITEM_SHOTGUN		3
#define ITEM_GRENADEL		4
#define ITEM_ROCKETL		5
#define ITEM_LIGHTNINGGUN	6
#define ITEM_RAILGUN		7
#define ITEM_PLASMAGUN		8
#define ITEM_BFG			9
#define ITEM_ARMORSHARD		10
#define ITEM_YELLOWARMOR	11
#define ITEM_REDARMOR		12
#define ITEM_5HEALTH		13
#define ITEM_25HEALTH		14
#define ITEM_50HEALTH		15
#define ITEM_BATTLESUIT		16
#define ITEM_FLIGHT			17
#define ITEM_HASTE			18
#define ITEM_INVISIBILITY	19
#define ITEM_MEGAHEALTH		20
#define ITEM_QUADDAMAGE		21
#define ITEM_REGENERATION	22
#define ITEM_MEDKIT			23
#define ITEM_TELEPORTER		24
#define ITEM_REDFLAG		25
#define ITEM_BLUEFLAG		26
#define ITEM_BULLETS		27
#define ITEM_SHELLS			28
#define ITEM_GRENADES		29
#define ITEM_ROCKETS		30
#define ITEM_LIGHTNING		31
#define ITEM_SLUGS			32
#define ITEM_PLASMAAMMO		33
#define ITEM_BFGAMMO		34
#define ITEM_LAST			35
//ITEM_LAST must be last



							

//events
#define EVENT_NONE			0
#define EVENT_GAMESTART		1
#define EVENT_PLAYERJOIN	2
#define EVENT_PLAYERLEAVE	3
#define EVENT_ITEMPICKUP	4
#define EVENT_KILL			5
#define EVENT_GAMEEND		6
#define EVENT_SAY			7
#define EVENT_SAY_TEAM		8
#define EVENT_PLAYERINFO	9
#define EVENT_LIMITHIT		10
#define EVENT_PLAYER_SCORE	11
#define EVENT_TEAM_SCORE	12



//Games
#define GAMETYPE_FFA		0
#define GAMETYPE_1V1		1
#define GAMETYPE_TDM		3
#define GAMETYPE_CTF		4
#define GAMETYPE_ALL		-1
#define GAMETYPE_MAX		5


#define GAME_MAX_SIMU_CLIENTS		65


#define TEAM_FFA			0
#define TEAM_RED			1
#define TEAM_BLUE			2
#define TEAM_SPECTATOR		-1
#define TEAM_MAX			3


//kill tables
#define KILLTABLE_MAXHILITE 21


#define LOOP_NONE				0
#define LOOP_ALLPLAYER_TRIM		1
#define LOOP_ALLPLAYER_FULL		2
#define LOOP_GAMEPLAYER_TRIM	3
#define LOOP_GAMEPLAYER_FULL	4
#define LOOP_MAPPLAYER_TRIM		5
#define LOOP_MAPPLAYER_FULL		6
#define LOOP_MAPS				7
#define LOOP_PLAYERGAMES		8
#define LOOP_PLAYERKILLS		9
#define LOOP_PLAYERDEATHS		10
#define LOOP_MAPGAMES			11


#define LIMIT_UNKNOWN		0
#define LIMIT_TIME			1
#define LIMIT_FRAG			2
#define LIMIT_CAPTURE		3
#define LIMIT_MAX			4


#define PLAYERLIST_SORTTYPE_NORMAL	0
#define PLAYERLIST_SORTTYPE_PERGAME	1000
#define PLAYERLIST_SORTTYPE_PERHOUR	2000
#define PLAYERLIST_SORT_GAMES		0
#define PLAYERLIST_SORT_TIME		1
#define PLAYERLIST_SORT_KILLS		2
#define PLAYERLIST_SORT_DEATHS		3
#define PLAYERLIST_SORT_SUICIDES	4
#define PLAYERLIST_SORT_FRAGS		5
#define PLAYERLIST_SORT_EFF			6
#define PLAYERLIST_SORT_ITEMS		7
#define PLAYERLIST_SORT_SCORE		8
#define PLAYERLIST_SORT_FFAWINS		9
#define PLAYERLIST_SORT_1V1WINS		10
#define PLAYERLIST_SORT_TDMWINS		11
#define PLAYERLIST_SORT_CTFWINS		12
#define PLAYERLIST_SORT_FFAPLACE	13
#define PLAYERLIST_SORT_FFAWINPLACE	14
#define PLAYERLIST_SORT_SKILL		15
#define PLAYERLIST_SORT_AVEHC		16



#define LOGTYPE_GAMESLOG			0

//database settings
//0 dont store in database
//1 this is the active file - store in database - resume parsing this file from where parsing left off last time
//2 this file is stored in the database and not parsed
//3 this file is added to the database (with all 2 files which get reparsed) then the program exits - file is marked with 2
#define DB_DONTADD			0
#define DB_ACTIVEFILE		1
#define DB_FILESTORED		2
#define DB_STOREFILE		3
#define DB_MAX				4




class GameEvent {
public:
	int event;		//type of event
	int time_min;	//minutes from start of game of event
	int time_sec;	//seconds after minute of event  0 - 60
	int realtime;	//time_t type time of event
					//new game		player info		kill		item		limit			score	teamscore	say / say team
	int player;		//				player			killer		pickuper					player	red score
	int other;		//				skill(bot)		killee		item						score	blue score
	int how;		//game type		hc				weapon					1=time 2=frag	ping
	char name[50];	//map name		player name 
	char model[50];	//				model
	int team;		//capture limit	team
	string msg;		//init str		info str		kill msg	item name										message
	int wins;		//frag limit	1v1 wins
	int	losses;		//time limit	1v1 losses


	GameEvent(){
		event=EVENT_NONE;
		time_min=0;
		time_sec=0;
		player=-1;
		other=ITEM_NONE;
		name[0]='\0';
		model[0]='\0';
		msg.erase();
		team=TEAM_FFA;
		wins=0;
		losses=0;
	}
	~GameEvent(){};
};


class Games;
class PList;
class Stats;



#endif //DEFINES_H_INCLUDED