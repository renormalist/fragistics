/* gameslogparser.cpp
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


#include "gameslogparser.h"

//extern Settings settings;
//extern OS os;

// item strings - must be the same order as item defines
char item_strings[][30] = {	"item_unknown",
				"weapon_gauntlet",
				"weapon_machinegun",
				"weapon_shotgun",
				"weapon_grenadelauncher",
				"weapon_rocketlauncher",
				"weapon_lightning",
				"weapon_railgun",
				"weapon_plasmagun",
				"weapon_bfg",
				"item_armor_shard",
				"item_armor_combat",
				"item_armor_body",
				"item_health_small",
				"item_health",
				"item_health_large",
				"item_enviro",
				"item_flight",
				"item_haste",
				"item_invis",
				"item_health_mega",
				"item_quad",
				"item_regen",
				"holdable_medkit",
				"holdable_teleporter",
				"team_CTF_redflag",
				"team_CTF_blueflag",
				"ammo_bullets",
				"ammo_shells",
				"ammo_grenades",
				"ammo_rockets",
				"ammo_lightning",
				"ammo_slugs",
				"ammo_cells",
				"ammo_bfg",
				"\0\0\0"
};

bool GameslogParser::Parse(int startline){
    File *fp;
    char line[1024];
    GameEvent *evt;
    Game *currentgame=NULL;
    int linecount=0;
    
    fp=os.GetFile(filename.c_str());
    if(!fp->OpenRead()){
	printf("\nCouldn't open file %s!\n",filename.c_str());
	return false;
    }
    
    while(fp->ReadLine(line,1024)!=NULL){	
	linecount++;
	if(linecount<startline)
	    continue;
	
	//printf ("DBG: %d: %s", linecount, line); // ss5
	evt = ParseLine(line);
	//evt->print (); // ss5

	switch(evt->event){
	case EVENT_GAMESTART:{
	  //printf ("DBG: *** EVENT_GAMESTART\n");
	    evt->print (); // ss5
	    if(currentgame==NULL){
	      //printf ("DBG: *** a\n");
		currentgame= new Game(evt->name,
				      evt->how,
				      (evt->time_min*60+evt->time_sec),
				      evt->wins,
				      evt->losses,
				      evt->team,
				      stats);
		printf("Game %d\n", stats->GetGames());
	    }else{
	      //printf ("DBG: *** b\n");
		if(currentgame->GetType() == GAMETYPE_1V1 && evt->how == GAMETYPE_1V1){
		  //printf ("DBG: Weiterführung 1v1\n");
		    if( !strcmp(evt->name,currentgame->GetMapname().c_str()) ){
			//continuation of 1V1 game - different players
			currentgame->GameReset1v1(evt->time_min*60+evt->time_sec);
		    }else{
			stats->AddGameToTotal(currentgame,linecount);
			
			delete currentgame;
			currentgame= new Game(evt->name,
					      evt->how,
					      (evt->time_min*60+evt->time_sec),
					      evt->wins,
					      evt->losses,
					      evt->team,
					      stats);
			printf("Game %d\n", stats->GetGames());
		    }
		}else{
		  //printf ("DBG: *** c\n");
		    if(currentgame->GetType() != GAMETYPE_1V1)
			printf("ERROR: new game while current game not done!\n");
		    stats->AddGameToTotal(currentgame,linecount);
		    
		    delete currentgame;
		    currentgame= new Game(evt->name,
					  evt->how,
					  (evt->time_min*60+evt->time_sec),
					  evt->wins,
					  evt->losses,
					  evt->team,
					  stats);
		    printf("Game %d\n", stats->GetGames());
		}
	    }
	    break;
	}
	case EVENT_PLAYERJOIN:{
	    //players++;
	    //stats->AddPlayer();
	    break;
	}
	case EVENT_PLAYERLEAVE:{
	    break;
	}
	case EVENT_ITEMPICKUP:{
	    //items++;
	    //stats->AddItem(item->other);
	    break;
	}
	case EVENT_KILL:{
	    // kills++;
	    
	    if( evt->how==UNKNOWN_3 ||
		evt->how>=UNKNOWN_KILL ){
		printf("Unknown killtype: %d : %s",evt->how,evt->msg.c_str());
		if(evt->how>=UNKNOWN_KILL)
		    evt->how=UNKNOWN_KILL;
	    }
	    if( evt->player>1000){
		evt->player=evt->other;
	    }
	    
	    /* oops, my bad!
	       once I implemented that not all games counted, this
	       was counting some kills by a player in the overall
	       stats that weren't being counted in the player stats
			
	    all_kills[evt->how]++;
	    all_kills[0]++;
	    
	    if(currentgame!=NULL){
		//find players in game
		if(currentgame->GetPlayerFromIndex(evt->player)!=NULL){
		    Player *killer,*killee;
		    
		    killer=currentgame->GetPlayerFromIndex(evt->player);
		    
		    if(currentgame->GetPlayerFromIndex(evt->other)!=NULL){
			killee=currentgame->GetPlayerFromIndex(evt->other);
			
			//find cooresponding player in all players list
			Player *ker,*kee;
			ker=player_list.FindPlayer(killer->GetName());
			if(ker==NULL){
			    ker=new Player(killer->GetName());
			    player_list.AddPlayer(ker,ker->GetName());
			}
			kee=player_list.FindPlayer(killee->GetName());
			if(kee==NULL){
			    kee=new Player(killee->GetName());
			    player_list.AddPlayer(kee,kee->GetName());
			}
			
			//add to crosstable
			AddKillToTables(killtypes,evt->how,ker,kee);
			//killtypes[0].AddValue(ker,kee);
			//if(evt->how<CROSSTABLE_MAX_KILL){
			//killtypes[evt->how].AddValue(ker,kee);
			//}
			
		    }else{
			printf("Bad Killee\n");
		    }
		}else{
		    printf("Bad Killer\n");
		}
	    }else{
		printf("Kill without Game\n");
	    }
	    */
	    break;
	}
	case EVENT_GAMEEND:{
	    if(currentgame!=NULL){
		currentgame->HandleEvent(evt);
		
		if(currentgame->GetType()!=GAMETYPE_1V1){
		    stats->AddGameToTotal(currentgame,linecount);
		    delete currentgame;
		    currentgame=NULL;
		}
	    }
	    break;
	}
	case EVENT_SAY:{
	    //msgs++;
	    break;
	}
	case EVENT_SAY_TEAM:{
	    //msgs_team++;
	    break;
	}
	case EVENT_PLAYERINFO:{
	    break;
	}
	    
	default: break;
	    
	}
	if(currentgame!=NULL && evt->event!=EVENT_GAMEEND){
	    currentgame->HandleEvent(evt);
	}
	
	delete evt;
    }
    if(currentgame!=NULL){
	printf("last game still in progress - not adding to gamelist\n");
	delete currentgame;
    }
    
    delete fp;
    return true;
}

GameEvent* GameslogParser::ParseLine(char *line){
    GameEvent *evt=new GameEvent();
    char *ptr1,*ptr2;
    std::string eventMsg;
    line[1023]='\0';
    
    if(line!=NULL){
	ptr1=line;
	ptr2=strchr(ptr1,':');
	if(ptr2!=NULL){
	    *ptr2='\0';
	    evt->time_min=atoi(ptr1);
	    ptr1=++ptr2;
	    
	    evt->time_sec=atoi(ptr1);
	    
	    //find begining of 'info type'
	    //normal lines look like: 71:19 Item: 1 weapon_shotgun
	    //after 100 minutes		:127:02 Item: 3 weapon_shotgun
	    //after 1000 minutes	:1342:31Item: 3 weapon_shotgun
	    while(*ptr1>='0' && *ptr1<='9' || *ptr1==' ' ) ptr1++;
	    
	    ptr2=ptr1;
	    //find end of info type
	    while(*ptr2!='\0' && *ptr2!='\r' && *ptr2!='\n' && *ptr2!=' ' &&*ptr2!='\t') {
		ptr2++;
	    }
	    *ptr2='\0';
	    
	    if(ptr2!=NULL){

		if(!strcmp("InitGame:",ptr1)){
		    evt->event = EVENT_GAMESTART;
		    evt->msg = (ptr2+1);
		    eventMsg = evt->msg;
		    
		    ptr1=strstr(ptr2+1,"mapname\\");
		    if(ptr1!=NULL){
			ptr1+=8;
			ptr2=ptr1; // ss5: ?
			ptr2=strchr(ptr1,'\\');
			if(ptr2!=NULL){
			    *ptr2='\0';
			    strcpy(evt->name ,ptr1);
			    
			    //ptr1=strstr(ptr2+1,"g_gametype\\"); //ss5: WRONG
			    ptr1=strstr(eventMsg.c_str(),"g_gametype\\"); // ss5
			    if(ptr1!=NULL){
				ptr1+=11;
				evt->how=atoi(ptr1);
				//printf ("DBG: *** gametype gesetzt: %d\n", evt->how);
			    }
			    
			    //ptr1=strstr(ptr2+1,"capturelimit\\"); // ss5
			    ptr1=strstr(eventMsg.c_str(),"capturelimit\\"); // ss5
			    if(ptr1!=NULL){
				ptr1+=13;
				evt->team=atoi(ptr1);
			    }
			    
			    //ptr1=strstr(ptr2+1,"fraglimit\\"); // ss5
			    ptr1=strstr(eventMsg.c_str(),"fraglimit\\"); // ss5
			    if(ptr1!=NULL){
				ptr1+=10;
				evt->wins=atoi(ptr1);
			    }
			    
			    //ptr1=strstr(ptr2+1,"timelimit\\"); // ss5
			    ptr1=strstr(eventMsg.c_str(),"timelimit\\"); // ss5
			    if(ptr1!=NULL){
				ptr1+=10;
				evt->losses=atoi(ptr1);
			    }
			}
		    }
		}
		else if(!strcmp("ClientConnect:",ptr1)){
		    evt->event = EVENT_PLAYERJOIN;
		    evt->player = atoi(++ptr2);
		}
		else if(!strcmp("ClientDisconnect:",ptr1)){
		    evt->event = EVENT_PLAYERLEAVE;
		    evt->player = atoi(++ptr2);
		}
		else if(!strcmp("Item:",ptr1)){
		    evt->event = EVENT_ITEMPICKUP;
		    ptr1=ptr2+1;
		    ptr2=strchr(ptr1,' ');
		    if(ptr2!=NULL){
			*ptr2='\0';
			evt->player = atoi(ptr1);
			
			ptr1=++ptr2;
			
			evt->msg = ptr2;
			
			while(*ptr2!='\0' && *ptr2!='\r' && *ptr2!='\n' && *ptr2!=' ' &&*ptr2!='\t') ptr2++;
			*ptr2='\0';
			
			int i;
			for( i=0;i<ITEM_LAST;i++){
			    if(!strcmp(item_strings[i],ptr1))
				break;
			}
			if(i==ITEM_LAST)
			    i=ITEM_UNKNOWN;
			
			evt->other=i;
		    }
		}
		else if(!strcmp("Kill:",ptr1)){
		    evt->event = EVENT_KILL;
		    ptr1=ptr2+1;
		    ptr2=strchr(ptr1,' ');
		    if(ptr2!=NULL){
			*ptr2='\0';
			evt->player = atoi(ptr1);
			
			ptr1=ptr2+1;
			ptr2=strchr(ptr1,' ');
			if(ptr2!=NULL){
			    *ptr2='\0';
			    evt->other = atoi(ptr1);
			    
			    ptr1=ptr2+1;
			    ptr2=strchr(ptr1,':');
			    if(ptr2!=NULL){
				*ptr2='\0';
				evt->how = atoi(ptr1);
				
				evt->msg=ptr2+1;
			    }
			}
		    }
		}
		else if(!strcmp("ShutdownGame:",ptr1)){
		    evt->event = EVENT_GAMEEND;
		}
		else if(!strcmp("say:",ptr1)){
		    evt->event = EVENT_SAY;
		    evt->msg=(ptr2+1);
		}
		else if(!strcmp("sayteam:",ptr1)){
		    evt->event = EVENT_SAY_TEAM;
		    evt->msg=(ptr2+1);
		}
		else if(!strcmp("ClientUserinfoChanged:",ptr1)){
		    evt->event = EVENT_PLAYERINFO;
		    ptr1=ptr2+1;
		    ptr2=strchr(ptr1,' ');
		    if(ptr2!=NULL){
			*ptr2='\0';
			evt->player = atoi(ptr1);
			
			ptr1=ptr2+1;
			
			evt->msg = ptr1;
			
			ptr1=strstr(ptr1,"n\\");
			if(ptr1!=NULL){
			    ptr1+=2;
			    ptr2=strstr(ptr1,"\\t\\");
			    if(ptr2!=NULL){
				*ptr2='\0';
				strncpy(evt->name,ptr1,48);
				evt->name[49]='\0';
				
				ptr1=ptr2+3;
				ptr2=strchr(ptr1,'\\');
				if(ptr2!=NULL){
				    *ptr2='\0';
				    evt->team=atoi(ptr1);
				    ptr1=strstr(ptr2+1,"model\\");
				    if(ptr1!=NULL){
					ptr1+=6;
					ptr2=strchr(ptr1+1,'\\');
					if(ptr2!=NULL){
					    *ptr2='\0';
					    
					    strncpy(evt->model,ptr1,48);
					    evt->name[49]='\0';
					    
					    ptr1=strstr(ptr2+1,"hc\\");
					    if(ptr1!=NULL){
						ptr1+=3;
						ptr2=strchr(ptr1,'\\');
						if(ptr2!=NULL){
						    *ptr2='\0';
						    
						    evt->how = atoi(ptr1);
						    
						    ptr1=strstr(ptr2+1,"w\\");
						    if(ptr1!=NULL){
							ptr1+=2;
							
							evt->wins = atoi(ptr1);
							
							ptr1=strstr(ptr2+1,"l\\");
							if(ptr1!=NULL){
							    ptr1+=2;
							    
							    evt->losses = atoi(ptr1);
							    
							    ptr1=strstr(ptr2+1,"skill\\");
							    if(ptr1!=NULL){
								ptr1+=6;
								
								evt->other = atoi(ptr1);
							    }
							}
						    }
						}
					    }
					}
				    }
				}
			    }
			}
		    }
		}else if(!strcmp("Exit:",ptr1)){
		    evt->event = EVENT_LIMITHIT;
		    evt->how=LIMIT_UNKNOWN;
		    ptr1=ptr2+1;
		    ptr2=strstr(ptr1,"Time");
		    if(ptr2!=NULL){
			evt->how=LIMIT_TIME;
		    }else{
			ptr2=strstr(ptr1,"Frag");
			if(ptr2!=NULL){
			    evt->how=LIMIT_FRAG;
			}else{
			    ptr2=strstr(ptr1,"Capture");
			    if(ptr2!=NULL){
				evt->how=LIMIT_CAPTURE;
			    }	
			}
		    }
		}else if(!strcmp("score:",ptr1)){
		    evt->event = EVENT_PLAYER_SCORE;
		    evt->player=-1;
		    ptr1=ptr2+1;
		    ptr2=strchr(ptr1,' ');
		    if(ptr2!=NULL){
			*ptr2='\0';
			evt->other = atoi(ptr1);
			
			ptr1=ptr2+1;
			ptr1=strstr(ptr1,"ping:");
			if(ptr1!=NULL){
			    ptr1+=5;
			    
			    evt->how = atoi(ptr1);
			    
			    ptr1=strstr(ptr1,"client:");
			    if(ptr1!=NULL){
				ptr1+=7;
				ptr2=strchr(ptr1+1,' ');
				if(ptr2!=NULL)
				    *ptr2='\0';
				
				evt->player=atoi(ptr1);
				
				evt->msg=ptr2+1;
			    }
			}
		    }
		}else if(!strncmp("red:",ptr1,4)){
		    evt->event = EVENT_TEAM_SCORE;
		    evt->player=0;
		    evt->other=0;
		    ptr1+=4;
		    evt->player = atoi(ptr1);
		    ptr1=strstr(ptr2+1,"blue:");
		    if(ptr1!=NULL){
			ptr1+=5;
			evt->other = atoi(ptr1);
		    }
		}//end of else if
		
	    }
	}
    }
    
    return evt;
}

