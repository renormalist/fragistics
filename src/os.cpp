/* os.cpp
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

Brian Risinger contact info:

troz_@xoommail.com		preferred
troz_@hotmail.com		also ok
risinger@hoflink.com	if unable to reach otherwise

98 Stone Lane
Levittown, NY  11756
USA

*/


//This file has functions for all operating system specific calls.
//if you find something that is operaing system specific, please move it
// into this file to make porting easier.  That way only os.h and os.cpp
// need to be ported to have Fragistics run on another operating system.
// if something here doesn't work, put an #ifdef WIN32 around it and add an
// ifdef for your OS.  Then send the update to troz_@xoommail.com for future releases 

#include "defines.h"
#include "os.h"
#include <stdio.h>
#include <stdlib.h>



#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif



//UTIL functions

#ifndef WIN32
// No itoa() in libc
char *itoa(int value, char *string, int radix){
	if (radix != 10){
		fprintf(stderr, "This version of itoa() only supports a radix of 10\n");
		exit(1);
	}
	sprintf(string, "%d", value);
	return string;
}

char getch(){
	char a='\0';
	while(a=='\0'){
		a=getchar();
	}
}
#endif

int GetAggregateKills(int *killarray, int type){
	//must match AddKillToTables in crosstable.cpp
	if(type>=0&&type<MAX_KILLTYPE){
		return killarray[type];
	}else if(type>=AGGREGATE_KILLS_MIN && type<AGGREGATE_KILLS_MAX){
		switch(type){
		case A_ROCKET_KILL:		return killarray[ROCKET_KILL_DIRECT] +	killarray[ROCKET_KILL_SPLASH];
		case A_GRENADE_KILL:	return killarray[GRENADE_KILL_DIRECT] +	killarray[GRENADE_KILL_SPLASH];
		case A_PLASMA_KILL:		return killarray[PLASMA_KILL_DIRECT] +	killarray[PLASMA_KILL_SPLASH];
		case A_BFG_KILL:		return killarray[BFG_KILL_DIRECT] +		killarray[BFG_KILL_SPLASH];
		case A_EXPLOSIVE_KILL:	return killarray[GRENADE_KILL_DIRECT] + killarray[GRENADE_KILL_SPLASH]+ killarray[ROCKET_KILL_DIRECT] + killarray[ROCKET_KILL_SPLASH];
		case A_BULLET_KILL:		return killarray[SHOTGUN_KILL] +		killarray[MACHINEGUN_KILL];
		case A_ENERGY_KILL:		return killarray[PLASMA_KILL_DIRECT] + 	killarray[PLASMA_KILL_SPLASH] + killarray[BFG_KILL_DIRECT] +	killarray[BFG_KILL_SPLASH];
		case A_OTHER_KILL:		return killarray[GAUNTLET_KILL] +		killarray[RAILGUN_KILL] +		killarray[LIGHTNING_KILL] +		killarray[TELEFRAG_KILL];
		case A_HITSCAN_KILL:	return killarray[SHOTGUN_KILL] +		killarray[GAUNTLET_KILL] +		killarray[MACHINEGUN_KILL] +	killarray[RAILGUN_KILL] +		killarray[LIGHTNING_KILL];
		case A_NONHITSCAN_KILL: return killarray[GRENADE_KILL_DIRECT] + killarray[GRENADE_KILL_SPLASH]+ killarray[ROCKET_KILL_DIRECT] + killarray[ROCKET_KILL_SPLASH] + killarray[PLASMA_KILL_DIRECT] + killarray[PLASMA_KILL_SPLASH] + killarray[BFG_KILL_DIRECT] +	killarray[BFG_KILL_SPLASH];
		case A_SUICIDE_KILL:	return killarray[SELF_WATER] +			killarray[SELF_SLIME] +			killarray[SELF_LAVA] +			killarray[SELF_CRUSH] +			killarray[SELF_FALLING] +		killarray[SELF_SUICIDE] +		killarray[SELF_TRIGGER_HURT];
		case A_NONWEP_KILL:		return killarray[SELF_WATER] +			killarray[SELF_SLIME] +			killarray[SELF_LAVA] +			killarray[SELF_CRUSH] +			killarray[TELEFRAG_KILL] +		killarray[SELF_FALLING] +		killarray[SELF_SUICIDE] +		killarray[SELF_TRIGGER_HURT];
		default: return -1;
		}
	}
	return -1;
}



int ParseKilltypeStr(string *str){

	if(!strcmp(str->c_str(),"TOTAL_KILLS")){
		return TOTAL_KILLS;
	}else if(!strcmp(str->c_str(),"ALL_KILLS")){
		return TOTAL_KILLS;
	}else if(!strcmp(str->c_str(),"ALL")){
		return TOTAL_KILLS;
	}else if(!strcmp(str->c_str(),"TOTAL")){
		return TOTAL_KILLS;
	}else if(!strcmp(str->c_str(),"SHOTGUN_KILL")){
		return SHOTGUN_KILL;
	}else if(!strcmp(str->c_str(),"GAUNTLET_KILL")){
		return GAUNTLET_KILL;
	}else if(!strcmp(str->c_str(),"MACHINEGUN_KILL")){
		return MACHINEGUN_KILL;
	}else if(!strcmp(str->c_str(),"GRENADE_KILL_DIRECT")){
		return GRENADE_KILL_DIRECT;
	}else if(!strcmp(str->c_str(),"GRENADE_KILL_SPLASH")){
		return GRENADE_KILL_SPLASH;
	}else if(!strcmp(str->c_str(),"ROCKET_KILL_DIRECT")){
		return ROCKET_KILL_DIRECT;
	}else if(!strcmp(str->c_str(),"ROCKET_KILL_SPLASH")){
		return ROCKET_KILL_SPLASH;
	}else if(!strcmp(str->c_str(),"PLASMA_KILL_DIRECT")){
		return PLASMA_KILL_DIRECT;
	}else if(!strcmp(str->c_str(),"PLASMA_KILL_SPLASH")){
		return PLASMA_KILL_SPLASH;
	}else if(!strcmp(str->c_str(),"RAILGUN_KILL")){
		return RAILGUN_KILL;
	}else if(!strcmp(str->c_str(),"LIGHTNING_KILL")){
		return LIGHTNING_KILL;
	}else if(!strcmp(str->c_str(),"BFG_KILL_DIRECT")){
		return BFG_KILL_DIRECT;
	}else if(!strcmp(str->c_str(),"BFG_KILL_SPLASH")){
		return BFG_KILL_SPLASH;
	}else if(!strcmp(str->c_str(),"SELF_WATER")){
		return SELF_WATER;
	}else if(!strcmp(str->c_str(),"SELF_SLIME")){
		return SELF_SLIME;
	}else if(!strcmp(str->c_str(),"SELF_LAVA")){
		return SELF_LAVA;
	}else if(!strcmp(str->c_str(),"SELF_CRUSH")){
		return SELF_CRUSH;
	}else if(!strcmp(str->c_str(),"TELEFRAG_KILL")){
		return TELEFRAG_KILL;
	}else if(!strcmp(str->c_str(),"SELF_FALLING")){
		return SELF_FALLING;
	}else if(!strcmp(str->c_str(),"SELF_SUICIDE")){
		return SELF_SUICIDE;
	}else if(!strcmp(str->c_str(),"SELF_TRIGGER_HURT")){
		return SELF_TRIGGER_HURT;
	}else if(!strcmp(str->c_str(),"A_ROCKET_KILL")){
		return A_ROCKET_KILL;
	}else if(!strcmp(str->c_str(),"A_GRENADE_KILL")){
		return A_GRENADE_KILL;
	}else if(!strcmp(str->c_str(),"A_PLASMA_KILL")){
		return A_PLASMA_KILL;
	}else if(!strcmp(str->c_str(),"A_BFG_KILL")){
		return A_BFG_KILL;
	}else if(!strcmp(str->c_str(),"A_EXPLOSIVE_KILL")){
		return A_EXPLOSIVE_KILL;
	}else if(!strcmp(str->c_str(),"A_BULLET_KILL")){
		return A_BULLET_KILL;
	}else if(!strcmp(str->c_str(),"A_ENERGY_KILL")){
		return A_ENERGY_KILL;
	}else if(!strcmp(str->c_str(),"A_OTHER_KILL")){
		return A_OTHER_KILL;
	}else if(!strcmp(str->c_str(),"A_HITSCAN_KILL")){
		return A_HITSCAN_KILL;
	}else if(!strcmp(str->c_str(),"A_NONHITSCAN_KILL")){
		return A_NONHITSCAN_KILL;
	}else if(!strcmp(str->c_str(),"A_SUICIDE_KILL")){
		return A_SUICIDE_KILL;
	}else if(!strcmp(str->c_str(),"A_NONWEP_KILL")){
		return A_NONWEP_KILL;
	}else{
		printf("UNKNOWN KILLTYPE IN TEMPLATE! :   %s\n",str->c_str());
		return UNKNOWN_KILL;
	}

}

int ParseItemStr(string *str){

	if(!strcmp(str->c_str(),"SHOTGUN")){
		return ITEM_SHOTGUN;
	}else if(!strcmp(str->c_str(),"GAUNTLET")){
		return ITEM_GAUNTLET;
	}else if(!strcmp(str->c_str(),"MACHINEGUN")){
		return ITEM_MACHINEGUN;
	}else if(!strcmp(str->c_str(),"GRENADEL")){
		return ITEM_GRENADEL;
	}else if(!strcmp(str->c_str(),"ROCKETL")){
		return ITEM_ROCKETL;
	}else if(!strcmp(str->c_str(),"LIGHTNING")){
		return ITEM_LIGHTNINGGUN;
	}else if(!strcmp(str->c_str(),"RAILGUN")){
		return ITEM_RAILGUN;
	}else if(!strcmp(str->c_str(),"PLASMA")){
		return ITEM_PLASMAGUN;
	}else if(!strcmp(str->c_str(),"PLASMA_KILL_SPLASH")){
		return PLASMA_KILL_SPLASH;
	}else if(!strcmp(str->c_str(),"BFG")){
		return ITEM_BFG;
	}else if(!strcmp(str->c_str(),"ARMOR_SHARD")){
		return ITEM_ARMORSHARD;
	}else if(!strcmp(str->c_str(),"ARMOR_YELLOW")){
		return ITEM_YELLOWARMOR;
	}else if(!strcmp(str->c_str(),"ARMOR_RED")){
		return ITEM_REDARMOR;
	}else if(!strcmp(str->c_str(),"HEALTH_5")){
		return ITEM_5HEALTH;
	}else if(!strcmp(str->c_str(),"HEALTH_25")){
		return ITEM_25HEALTH;
	}else if(!strcmp(str->c_str(),"HEALTH_50")){
		return ITEM_50HEALTH;
	}else if(!strcmp(str->c_str(),"BATTLESUIT")){
		return ITEM_BATTLESUIT;
	}else if(!strcmp(str->c_str(),"FLIGHT")){
		return ITEM_FLIGHT;
	}else if(!strcmp(str->c_str(),"HASTE")){
		return ITEM_HASTE;
	}else if(!strcmp(str->c_str(),"INVIS")){
		return ITEM_INVISIBILITY;
	}else if(!strcmp(str->c_str(),"HEALTH_MEGA")){
		return ITEM_MEGAHEALTH;
	}else if(!strcmp(str->c_str(),"QUAD")){
		return ITEM_QUADDAMAGE;
	}else if(!strcmp(str->c_str(),"REGEN")){
		return ITEM_REGENERATION;
	}else if(!strcmp(str->c_str(),"MEDKIT")){
		return ITEM_MEDKIT;
	}else if(!strcmp(str->c_str(),"TELE")){
		return ITEM_TELEPORTER;
	}else if(!strcmp(str->c_str(),"FLAG_RED")){
		return ITEM_REDFLAG;
	}else if(!strcmp(str->c_str(),"FLAG_BLUE")){
		return ITEM_BLUEFLAG;
	}else if(!strcmp(str->c_str(),"MACHINEGUN_A")){
		return ITEM_BULLETS;
	}else if(!strcmp(str->c_str(),"SHOTGUN_A")){
		return ITEM_SHELLS;
	}else if(!strcmp(str->c_str(),"GRENADEL_A")){
		return ITEM_GRENADES;
	}else if(!strcmp(str->c_str(),"ROCKETL_A")){
		return ITEM_ROCKETS;
	}else if(!strcmp(str->c_str(),"LIGHTNING_A")){
		return ITEM_LIGHTNING;
	}else if(!strcmp(str->c_str(),"RAILGUN_A")){
		return ITEM_SLUGS;
	}else if(!strcmp(str->c_str(),"PLASMA_A")){
		return ITEM_PLASMAAMMO;
	}else if(!strcmp(str->c_str(),"BFG_A")){
		return ITEM_BFGAMMO;
	}else{
		printf("UNKNOWN KILLTYPE IN TEMPLATE! :   %s\n",str->c_str());
		return UNKNOWN_KILL;
	}

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


bool File::Exists(){
	//not used yet
	return false;
}

int File::OpenRead(){
	fp=fopen(name,"r");
	if(fp==NULL){
		return false;
	}
	open=true;
	read=true;
	return true;
}

int File::OpenWrite(){
	fp=fopen(name,"w+");		//write and create
	if(fp==NULL){
		fp=NULL;
		return false;
	}
	open=true;
	write=true;
	return true;
}


int File::Close(){
	if(open==true && fp!=NULL){
		fclose(fp);
		open=false;
		read=false;
		write=false;
		fp=NULL;
		return true;
	}
	return false;
}


char *File::ReadLine(char *buf,int bufsize){
	if(open==true && read==true){
		return fgets(buf,bufsize,fp);
	}
	return NULL;
}

int File::WriteLine(const char *buf){
	if(open==true && write==true){
		fprintf(fp,"%s\n",buf);
		return true;
	}
	return false;
}
int File::WriteStr(const char *buf){
	if(open==true && write==true){
		fprintf(fp,"%s",buf);
		return true;
	}
	return false;
}

bool File::GetFile(string *buf){
	if(!OpenRead())
		return false;
	char tmp[1024];

	buf->erase();
	while(ReadLine(tmp,1023)!=NULL) {
	    (*buf)+=tmp;
	}
	
	Close();
	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////




int OS::SetLowPriority(){
	return false;
}


void OS::Delay(int seconds){
#ifdef WIN32
	Sleep(seconds*1000);
#endif //WINDOWS
}

#ifdef WIN32
const char OS::PathSeparator = '\\';
#else
const char OS::PathSeparator = '/';
#endif



int OS::GetVersion(char *ver,int size){
	//puts the program version into ver and return 0 or returns the size needed for ver if ver is too small
#ifdef WIN32
	//todo read from resource
	if(size>=10){
		sprintf(ver,"1.4");
		return 0;
	}
	return 10;
	
#else //WIN32

	if(size>=10){
		sprintf(ver,"1.5.2");
		return 0;
	}
	return 10;
#endif //WIN32

	
}
