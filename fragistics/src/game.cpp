/* game.cpp
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

#include "game.h"
#include "htmlfile.h"


extern char kill_strings[26][32];


char* toucase( char* ptr){
	char *ptr2=ptr;
 	while(*ptr2!='\0'){ 
		*ptr2=toupper(*ptr2);
		ptr2++;
	}
	return ptr;
}

Game::Game ( char* map, int type, int startsec, int frag_limit, int time_limit, int capture_limit, Stats *st_)
{
    st=st_;
    mapname=toucase(map);
    gametype=type;
    
    //		total_kills=0;
    total_items=0;
    total_time_sec=-startsec;
    msgs=0;
    msgs_team=0;
    gameover=false;
    gameoverreason=0;
    cont1v1=false;
    gamenumber=0;
    
    fraglimit=frag_limit;
    timelimit=time_limit;
    capturelimit=capture_limit;
    
    int i=0;
    for(i=0;i<GAME_MAX_SIMU_CLIENTS;i++)
	pindex[i]=NULL;
    for(i=0;i<TEAM_MAX;i++)
	teamscores[i]=0;
    for(i=0;i<MAX_KILLTYPE;i++)
	all_kills[i]=0;
    for(i=0;i<ITEM_LAST;i++)
	all_items[i]=0;
}

Game::~Game()
{
    // NOP
}

int Game::GetType()
{
    return gametype;
}

int Game::GetGameOverReason ()
{
    return gameoverreason;
}

//only valid after endgame event;
int Game::GetTotalTime()
{
    return total_time_sec;
}

int Game::GetTotalItems()
{
    return total_items;
}

PList* Game::GetPlayers()
{
    return (&players);
}

int Game::GetTeamScore(int team)
{
    if(team>=0 && team<TEAM_MAX)
	return teamscores[team];
    return 0;
}
    
string Game::GetMapname()
{
    return mapname;
}

int Game::GetKills(int type=0)
{
    return all_kills[type];
}

void Game::GetTimeStr(char *buf)
{
    int min,sec;
    min=total_time_sec/60;
    sec=total_time_sec%60;
    
    if(sec<10){
	sprintf(buf,"%d:0%d",min,sec);
    }else{
	sprintf(buf,"%d:%d",min,sec);
    }
}

Player* Game::GetPlayerFromIndex(int index)
{
    return pindex[index];
}

bool Game::isGameOver()
{
    return gameover;
}

int Game::GetMsgs()
{
    return msgs;
}

int Game::GetMsgsTeam()
{
    return msgs_team;
}

int Game::GetItem(int type)
{
    return all_items[type];
}

int Game::GetGameNumber()
{
    return gamenumber;
}

void Game::SetGameNumber(int gn)
{
    gamenumber=gn;
}

CrossTable* Game::GetKillTable(int num)
{
    return &kills[num];
}

//used for continueing 1V1 games
void Game::GameReset1v1(int time)
{
    gameover=false;
    gameoverreason=0;
    total_time_sec-=time;
    cont1v1=true;
    Player *p;
    p=players.GetFirst();
    if(p!=NULL){
	do{
	    p->AddTime((-time));
	}while((p=players.GetNext())!=NULL);
    }
}


//
// --------------------------------------------------------------------
//

void Game::HandleEvent(GameEvent *evt){
	switch(evt->event){

		case EVENT_PLAYERJOIN:{
				if(pindex[evt->player]==NULL && !gameover){
					pindex[evt->player]=new Player();
					//pindex[evt->player]->AddTime(-(evt->time_min*60+evt->time_sec));
				}else{
					if(!cont1v1 && !gameover)
						printf("Bad Player connect - %d still connected\n",evt->player);
				}
				break;
			}
		case EVENT_PLAYERLEAVE:{
				if(pindex[evt->player]!=NULL && !gameover){
					pindex[evt->player]->PlayerLeave(evt->time_min*60+evt->time_sec);
					if(pindex[evt->player]->GetTime()<0){
						printf("Bad Enter or leave time in Player leave\n");
					}
					pindex[evt->player]=NULL;
				}else{
					if(!gameover)
						printf("Bad Player in Player leave\n");
				}
				break;
			}
		case EVENT_ITEMPICKUP:{
				if( !gameover){
					total_items++;
					all_items[evt->other]++;
					if(pindex[evt->player]!=NULL){
						pindex[evt->player]->AddItem(evt->other);
					}else{
						printf("Bad Player in item pickup\n");
					}
				}
				break;
			}
		case EVENT_KILL:{
				if( !gameover){
//					total_kills++;

					all_kills[evt->how]++;
					all_kills[0]++;
					
					if(pindex[evt->player]!=NULL){
						Player  *killer,*killee;
					
						killer=pindex[evt->player];

						if(pindex[evt->other]!=NULL){
							killee=pindex[evt->other];

/*							kills[0].AddValue(killer,killee);
							if(evt->how<CROSSTABLE_MAX_KILL){
								kills[evt->how].AddValue(killer,killee);
							}*/
							AddKillToTables(kills,evt->how,killer,killee);

							if(evt->player == evt->other ){
								pindex[evt->player]->AddSuicide(evt->how);
							}else{
								pindex[evt->player]->AddKill(pindex[evt->other]->GetName(),evt->how);
								pindex[evt->other]->AddDeath(pindex[evt->player]->GetName(),evt->how);

								//calc new rank
								int delta, diff, keerank, kerrank;
								kerrank=pindex[evt->player]->GetRank();
								keerank=pindex[evt->other]->GetRank();
								diff=kerrank-keerank;
								delta=keerank/100-diff/200;
								if(delta<1) delta=1;
								pindex[evt->player]->AddRank(delta);
								pindex[evt->other]->AddRank((-delta));

								//custom rank calc;
								st->SetKillerKillee(pindex[evt->player],pindex[evt->other],evt->how);
								delta=st->DoCustMath();
								pindex[evt->player]->AddCustRank(delta);
								pindex[evt->other]->AddCustRank((-delta));

							}

						}else{
							pindex[evt->player]->AddKill("UNKNOWN",evt->how);
							printf("Bad Killee\n");
						}
					}else{
						printf("Bad Killer");
					}
				}
				break;
			}
		case EVENT_GAMEEND:{
				{	
					if(!gameover){
						total_time_sec+=evt->time_min*60+evt->time_sec;
						for(int i=0;i<GAME_MAX_SIMU_CLIENTS;i++){
							if(pindex[i]!=NULL){
								pindex[i]->AddTime(evt->time_min*60+evt->time_sec);
							}	
						}
					}
					if(!cont1v1){
						Player *p;
						p=players.GetFirst();
						if(p!=NULL){
							do{
								p->AddGame();
							}while((p=players.GetNext())!=NULL);
						}
					}
				}
				break;
			}
		case EVENT_SAY:{
				msgs++;
				char name[50],*cptr;
				cptr=strchr(evt->msg.c_str(),':');
				int dist=cptr-evt->msg.c_str();
				if(dist<48){
					memcpy(name,evt->msg.c_str(),dist);
					name[dist]='\0';
					Player *p;
					p=players.FindPlayer(name);
					if(p!=NULL){
						p->AddMsg();
					}else{
					  // TODO: players in msg: are stripped
					  printf("Bad Player in msg: %s\n", name); // ss5
					}
				}else{
					printf("Name too long in msg\n");
				}
				break;
			}
		case EVENT_SAY_TEAM:{
				msgs_team++;
				char name[50],*cptr;
				cptr=strchr(evt->msg.c_str(),':');
				int dist=cptr-evt->msg.c_str();
				if(dist<48){
					memcpy(name,evt->msg.c_str(),dist);
					name[dist]='\0';
					Player *p;
					p=players.FindPlayer(name);
					if(p!=NULL){
						p->AddMsgTeam();
					}else{
					  printf("Bad Player in msg: %s\n", name); // ss5
					}
				}else{
					printf("Name too long in msg\n");
				}
				break;
			}
		case EVENT_PLAYERINFO:{
				if(pindex[evt->player]!=NULL && !gameover){

					if(pindex[evt->player]->GetName()==NULL || *(pindex[evt->player]->GetName())=='\0'){
						//first set info - check for reconnect 
						bool found=false;
						Player *p,*pp;
						p=players.GetFirst();
						if(p!=NULL){
							do{
								if(!strcmp(p->GetName(),evt->name)){
									//already player of same name
									if(!strcmp(p->GetModel().c_str(),evt->model)){
										//already player with same model
										pp=p;
										found=true;
										for(int i=0;i<GAME_MAX_SIMU_CLIENTS;i++){
											if(pindex[i]==p){
												//new player - player with same name still playing
												found=false;
											}
										}
									}
									if(found==true)
										break;
								}
							}while((p=players.GetNext())!=NULL);
						}

						if(found &&pp!=NULL){
							//player reconnected
							int temptime=pindex[evt->player]->GetTime();
							delete pindex[evt->player];
							pindex[evt->player]=pp;
							pp->AddTime(temptime);
						}
					}

					pindex[evt->player]->SetInfo(evt->name,evt->model,evt->team,evt->how, evt->other, evt->wins, evt->losses,evt->time_min*60+evt->time_sec,st);
					if(!players.ContainsPlayer(pindex[evt->player])){
						players.AddPlayer(pindex[evt->player],pindex[evt->player]->GetName());
					}

				}else{
					if(!gameover)
						printf("Bad Player in setinfo\n");
				}

				break;
			}
		case EVENT_LIMITHIT:{
				if(!gameover){
					gameoverreason=evt->how;
					gameover=true;
					total_time_sec+=evt->time_min*60+evt->time_sec;
					for(int i=0;i<GAME_MAX_SIMU_CLIENTS;i++){
						if(pindex[i]!=NULL){
							pindex[i]->AddTime(evt->time_min*60+evt->time_sec);
						}	
					}
				}
				break;
			}
		case EVENT_PLAYER_SCORE:{
				if(gameover){
					if(pindex[evt->player]!=NULL){
						pindex[evt->player]->SetScore(evt->other,evt->how);
					}
				}
				break;
			}
		case EVENT_TEAM_SCORE:{
				if(gameover){
					teamscores[TEAM_RED] =evt->player;
					teamscores[TEAM_BLUE]=evt->other;
				}
				break;
			}


		default: break;

	}
}

void Game::GetVariableValue(string *variable, string *value, Stats *stats){

//	(*value)="GAME VALUE";

	
	string begining,end;
	int pos;
	char numbuf[50];

	pos=variable->find_first_of('.');
	begining=variable->substr(0,pos);
	end=variable->substr(pos+1);
	value->erase();

	
	if(pos==-1){

		if(!strcmp(end.c_str(),"MAPNAME")){
			(*value)=mapname;
		}else if(!strcmp(end.c_str(),"MAPIMAGE")){
			(*value)=mapname;
			(*value)+=".jpg";
		}else if(!strcmp(end.c_str(),"PLAYERCOUNT")){
			(*value)=itoa(players.GetCount(),numbuf,10);
		}else if(!strcmp(end.c_str(),"TOTKILLS")){
			(*value)=itoa(all_kills[0],numbuf,10);
		}else if(!strcmp(end.c_str(),"TOTITEMS")){
			(*value)=itoa(total_items,numbuf,10);
		}else if(!strcmp(end.c_str(),"TOTMSGS")){
			(*value)=itoa(msgs,numbuf,10);
		}else if(!strcmp(end.c_str(),"TOTTEAMMSGS")){
			(*value)=itoa(msgs_team,numbuf,10);
		}else if(!strcmp(end.c_str(),"TOTSECONDS")){
			(*value)=itoa(total_time_sec,numbuf,10);
		}else if(!strcmp(end.c_str(),"GAMETYPE")){
			switch(gametype){
			case GAMETYPE_1V1:	(*value)="1V1"; break;
			case GAMETYPE_TDM:	(*value)="TDM"; break;
			case GAMETYPE_CTF:	(*value)="CTF"; break;
			default:			(*value)="FFA"; break;
			}
		}else if(!strcmp(end.c_str(),"GAMEOVERREASON")){
			switch(gameoverreason){
			case LIMIT_TIME:	(*value)="Time Limit"; break;
			case LIMIT_FRAG:	(*value)="Frag Limit"; break;
			case LIMIT_CAPTURE:	(*value)="Capture Limit"; break;
			default:			(*value)="UNKNOWN"; break;
			}
		}else if(!strcmp(end.c_str(),"WINNER")){
			switch(gametype){
			case GAMETYPE_1V1:{
									string sorttype;
									sorttype="PLAYER_WINS.1V1";
									PList templist;
									templist.DupeList(&players);
									templist.ReSortList(&sorttype,stats);
									Player *p;
									p=templist.GetFirst();
									if(p!=NULL){
										p->Write(value);
									}
									break;
							  }
			case GAMETYPE_TDM:
			case GAMETYPE_CTF:	if(teamscores[TEAM_RED]>teamscores[TEAM_BLUE]){
									(*value)="RED"; 
								}else if(teamscores[TEAM_RED]<teamscores[TEAM_BLUE]){
									(*value)="BLUE";
								}else{
									(*value)="TIE";
								}
								break;
			default:			{
									PList templist;
									templist.DupeList(&players);
									string sorttype;
									
									if(gameover){
										sorttype="PLAYER_SCORE";
									}else{
										sorttype="PLAYER_FRAGS.ALL";
									}
									templist.ReSortList(&sorttype,stats);
									Player *p;
									p=templist.GetFirst();
									if(p!=NULL){
										p->Write(value);
									}
								}
								break;
			}
		}

	}else{

		if(!strcmp(begining.c_str(),"TIME")){
			if(!strcmp(end.c_str(),"DAYS")){
				(*value)=itoa(total_time_sec/(60*60*24),numbuf,10);
			}else if(!strcmp(end.c_str(),"HOURS")){
				(*value)=itoa((total_time_sec/(60*60))%24,numbuf,10);
			}else if(!strcmp(end.c_str(),"MINUTES")){
				(*value)=itoa((total_time_sec/60)%60,numbuf,10);
			}else if(!strcmp(end.c_str(),"SECONDS")){
				(*value)=itoa(total_time_sec%60,numbuf,10);
			}else if(!strcmp(end.c_str(),"STR")){
				GetTimeStr(numbuf);
				(*value)=numbuf;
			}
		}else if(!strcmp(begining.c_str(),"LIMIT")){
			if(!strcmp(end.c_str(),"TIME")){
				(*value)=itoa(timelimit,numbuf,10);
			}else if(!strcmp(end.c_str(),"FRAG")){
				(*value)=itoa(fraglimit,numbuf,10);
			}else if(!strcmp(end.c_str(),"CAPTURE")){
				(*value)=itoa(capturelimit,numbuf,10);
			}
		}else if(!strcmp(begining.c_str(),"ALLKILLS")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			if(type>=0&&type<MAX_KILLTYPE){
				(*value)=itoa(all_kills[type],numbuf,10);
			}else if(type>=AGGREGATE_KILLS_MIN && type<AGGREGATE_KILLS_MAX){
				(*value)=itoa(GetAggregateKills(all_kills,type),numbuf,10);
			}
		}else if(!strcmp(begining.c_str(),"KILLTABLE")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			if(type>=0&&type<CROSSTABLE_MAX_KILL){
				kills[type].Write(value);
			}
		}else if(!strcmp(begining.c_str(),"KILLTABLE_C")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			if(type>=0&&type<CROSSTABLE_MAX_KILL){
				kills[type].WriteRowCompress(value);
			}
		}else if(!strcmp(begining.c_str(),"TEAMSCORE")){
			if(!strcmp(end.c_str(),"RED")){
				(*value)=itoa(teamscores[TEAM_RED],numbuf,10);
			}else if(!strcmp(end.c_str(),"BLUE")){
				(*value)=itoa(teamscores[TEAM_BLUE],numbuf,10);
			}
		}else if(!strcmp(begining.c_str(),"ITEM")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseItemStr(&end);
			}
			if(type>=0 && type<ITEM_LAST){
				(*value)=itoa(all_items[type],numbuf,10);
			}
		}

	}


}

/*
void Game::WriteResults(int num){
	string temp,temp2;
	char numbuf[50];
	HTMLFile *file;

	temp="game";
	temp+=itoa(num,numbuf,10);
	temp+=".html";

	file = new HTMLFile((char*)temp.c_str());

	if(file->Open()){


//		file->Write("<html><head><title>");
//		file->Write(&mapname);	
//		file->Write(" - Game ");
//		file->Write(itoa(num,numbuf,10));
//		file->Write("</title></head>");
		//file->Write("<body bgcolor=\"#808080\" link=\"#0000ff\" vlink=\"#ff0000\" alink=\"00ff00\">");
//		file->Write("<body bgcolor=\"#000000\" text=\"999999\" link=\"#0000ff\" vlink=\"#ff0000\" alink=\"00ff00\">");

		temp = mapname;
		temp+= " - Game ";
		temp+=itoa(num,numbuf,10);
		file->WriteHeader(temp.c_str());
		
		temp="<CENTER><h2>Game ";
		temp+=itoa(num,numbuf,10);
		temp+="</h2>\n<h1>";
		temp+=mapname;
		temp+="</H1>\n";

		if(num!=1){
			temp+="<a href=\"game";
			temp+=itoa(num-1,numbuf,10);
			temp+=".html\">Previous game</A>\n";
		}else{
			temp+="<a href=\"games.html\">Games List</A>\n";
		}


		temp+="<img src=\"images/levelshots/";
		temp+=mapname;
		temp+=".jpg\" width=\"256\" height=\"256\">\n";

		temp+="<a href=\"game";
		temp+=itoa(num+1,numbuf,10);
		temp+=".html\">Next game</A><BR>\n";

		temp+="<a href=\"games.html\">Game List</a> <a href=\"stats.html\">Overall Stats</a> <a href=\"players.html\">Player stats</a>\n";

		temp+="<P></CENTER><P><HR><P>\n<table><tr><td>\n";

		

		temp+="Time ";
		GetTimeStr(numbuf);
		temp+=numbuf;
		temp+="<BR>\n";
		

		temp+=itoa(players.GetCount(),numbuf,10);
		temp+=" players<BR>\n";
		temp+=itoa(total_kills,numbuf,10);
		temp+=" kills<BR>\n";
		temp+=itoa(total_items,numbuf,10);
		temp+=" items<BR>\n";
		temp+=itoa(msgs,numbuf,10);
		temp+=" messages<BR>\n";
		temp+=itoa(msgs_team,numbuf,10);
		temp+=" team messages<P>\n";
		if(gameoverreason!=0){
			if(gameoverreason==1){
				temp+="Time Limit Hit\n";
			}
			if(gameoverreason==2){
				temp+="Frag Limit Hit\n";
			}
		}

		temp+="</td><td>\n";
		file->Write(&temp);


		file->Write("Scores:<BR>\n<TABLE border=1>\n");
		file->Write("<TR><TD>Name</TD><TD>Model</TD><TD>Ping</TD><TD>HC</TD><TD>Team</TD><TD>Kills</TD><TD>Deaths</TD><TD>Suicides</TD><TD>Frags</TD><TD>Score</TD><TD>Kill Dif.</TD><TD>Eff %</TD><TD>Skill</TD></TR>\n");
		Player *p;
		p=players.GetFirst();
		if(p!=NULL){
			do{
				temp.erase();
				temp2.erase();
				temp="<TR><TD>";
				p->Write(&temp2,0);
				temp+=temp2;
				temp+="</TD><TD>";
				temp+=p->GetModel();
				temp+="</TD><TD>";
				temp+=itoa(p->GetAvePing(),numbuf,10);
				temp+="</TD><TD>";
				temp+=itoa(p->GetAveHc(),numbuf,10);
				if(p->GetAveSkill()>=0){
					temp+=" B:";
					temp+=itoa(p->GetAveSkill()/100,numbuf,10);
				}
				temp+="</TD><TD>";
				switch(p->GetTeam()){
				case TEAM_RED:
					temp+="<font color=red>RED</font>";
					break;
				case TEAM_BLUE:
					temp+="<font color=blue>BLUE</font>";
					break;
				case TEAM_SPECTATOR:
					temp+="SPECT";
					break;
				default: break;
				}
				temp+="</TD><TD>";
				temp+=itoa(p->GetKill(TOTAL_KILLS),numbuf,10);
				temp+="</TD><TD>";
				temp+=itoa(p->GetDeath(TOTAL_KILLS),numbuf,10);
				temp+="</TD><TD>";
				temp+=itoa(p->GetSuicide(TOTAL_KILLS),numbuf,10);
				temp+="</TD><TD>";
				temp+=itoa((p->GetFrags(TOTAL_KILLS)),numbuf,10);
				temp+="</TD><TD>";
				temp+=itoa(p->GetScore(),numbuf,10);
				temp+="</TD><TD>";
				temp+=itoa((p->GetKillDiff(TOTAL_KILLS)),numbuf,10);
				temp+="</TD><TD>";
				p->GetEff(TOTAL_KILLS,numbuf);
				temp+=numbuf;
				temp+="</TD><TD>";
				p->GetWepSkill(TOTAL_KILLS,numbuf);
				temp+=numbuf;
				temp+="</TD></TR>";
				file->Write(&temp);
			}while((p=players.GetNext())!=NULL);
		}
		file->Write("</table>");


		file->Write("</td></tr></table>\n<P><HR><P>");

		file->Write("<h3>Kill Tables</H3><P>");

		file->Write("<table>\n");

		int c=0;
		for(int i=0;i<CROSSTABLE_MAX_KILL;i++){
			if(!kills[i].empty() || i==0){

				//add players to table
				p=players.GetFirst();
				if(p!=NULL){
					do{
						kills[i].AddPlayer(p);	
					}while((p=players.GetNext())!=NULL);
				}

				if(c%2==0){
					file->Write("<TR><TD>");
				}else{
					file->Write("<td>");
				}
				temp.erase();
				temp2.erase();
				temp2=kill_strings[i];
				temp2+="<BR>";
				file->Write(&temp2);
				kills[i].Write(&temp);
				file->Write(&temp);
				file->Write("<P>");
				if(c%2==0){
					file->Write("</TD><TD width=50>&nbsp;</TD>");
				}else{
					file->Write("</td></tr>");
				}
				c++;
			}
				
		}
		if(c%2)
			file->Write("<td>&nbsp;</td></tr>");
		file->Write("</table>");

		file->Write("<P><HR><P>");
		file->Write("<h3>Player Stats</H3><P>");
		p=players.GetFirst();
		if(p!=NULL){
			do{
				temp.erase();
				p->WriteStats(&temp,true);
				file->Write(&temp);
			}while((p=players.GetNext())!=NULL);
		}
		file->Write("<P><HR><P><font size=-2>This file was generated by <a href=\"http://members.xoom.com/TroZ_/q3logger/index.html\">Q3Logger</a></font>");

		file->Write("</body></html>");
	}else{
		printf("failed to open game %d html file",num);
	}

	delete file;
	
}
*/



///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////



void Map::AddGame(Game *game){
	int i;

	games++;

	gamelist.AddGame(game->GetGameNumber());

	PList *oplayers;

	oplayers=game->GetPlayers();

	Player *p,*pl;
	p=oplayers->GetFirst();
	if(p!=NULL){
		do{
			pl=players.FindPlayer(p->GetName());
			if(pl==NULL){
				pl=new Player(p->GetName());
				players.AddPlayer(pl,pl->GetName());
			}
			pl->AddStats(p);
			pl->AddGameNum(game->GetGameNumber());
		}while((p=oplayers->GetNext())!=NULL);
	}

	total_items+=game->GetTotalItems();
	total_time_sec+=game->GetTotalTime();
	msgs+=game->GetMsgs();
	msgs_team+=game->GetMsgsTeam();

	for(i=0;i<TEAM_MAX;i++)
		teamscores[i]+=game->GetTeamScore(i);
	for(i=0;i<MAX_KILLTYPE;i++)
		all_kills[i]+=game->GetKills(i);
	for(i=0;i<ITEM_LAST;i++)
		all_items[i]+=game->GetItem(i);

	printf ("DBG: game->GetType() = %d\n", game->GetType()); // ss5
	gametype[game->GetType()]++;
	printf ("DBG: gametype[game->GetType()] = %d\n", gametype[game->GetType()]); // ss5
	
	gameoverreason[game->GetGameOverReason()];
	modified=true;

}

void Map::GetVariableValue(string *variable, string *value){
	string begining,end;
	int pos;
	char numbuf[50];

	pos=variable->find_first_of('.');
	begining=variable->substr(0,pos);
	end=variable->substr(pos+1);
	value->erase();

	
	if(pos==-1){

		if(!strcmp(end.c_str(),"MAPNAME")){
			(*value)=mapname;
		}else if(!strcmp(end.c_str(),"MAPIMAGE")){
			(*value)=mapname;
			(*value)+=".jpg";
		}else if(!strcmp(end.c_str(),"GAMES")){
			(*value)=itoa(games,numbuf,10);;
		}else if(!strcmp(end.c_str(),"PLAYERCOUNT")){
			(*value)=itoa(players.GetCount(),numbuf,10);
		}else if(!strcmp(end.c_str(),"TOTKILLS")){
			(*value)=itoa(all_kills[0],numbuf,10);
		}else if(!strcmp(end.c_str(),"TOTITEMS")){
			(*value)=itoa(total_items,numbuf,10);
		}else if(!strcmp(end.c_str(),"TOTMSGS")){
			(*value)=itoa(msgs,numbuf,10);
		}else if(!strcmp(end.c_str(),"TOTTEAMMSGS")){
			(*value)=itoa(msgs_team,numbuf,10);
		}else if(!strcmp(end.c_str(),"TOTSECONDS")){
			(*value)=itoa(total_time_sec,numbuf,10);
		}

	}else{

		if(!strcmp(begining.c_str(),"TIME")){
			if(!strcmp(end.c_str(),"DAYS")){
				(*value)=itoa(total_time_sec/(60*60*24),numbuf,10);
			}else if(!strcmp(end.c_str(),"HOURS")){
				(*value)=itoa((total_time_sec/(60*60))%24,numbuf,10);
			}else if(!strcmp(end.c_str(),"MINUTES")){
				(*value)=itoa((total_time_sec/60)%60,numbuf,10);
			}else if(!strcmp(end.c_str(),"SECONDS")){
				(*value)=itoa(total_time_sec%60,numbuf,10);
			}else if(!strcmp(end.c_str(),"STR")){
				GetTimeStr(numbuf);
				(*value)=numbuf;
			}
		}else if(!strcmp(begining.c_str(),"ALLKILLS")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			if(type>=0&&type<MAX_KILLTYPE){
				(*value)=itoa(all_kills[type],numbuf,10);
			}else if(type>=AGGREGATE_KILLS_MIN && type<AGGREGATE_KILLS_MAX){
				(*value)=itoa(GetAggregateKills(all_kills,type),numbuf,10);
			}
		}else if(!strcmp(begining.c_str(),"KILLTABLE")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			if(type>=0&&type<CROSSTABLE_MAX_KILL){
				kills[type].Write(value);
			}
		}else if(!strcmp(begining.c_str(),"KILLTABLE_C")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			if(type>=0&&type<CROSSTABLE_MAX_KILL){
				kills[type].WriteRowCompress(value);
			}
		}else if(!strcmp(begining.c_str(),"TEAMSCORE")){
			if(!strcmp(end.c_str(),"RED")){
				(*value)=itoa(teamscores[TEAM_RED],numbuf,10);
			}else if(!strcmp(end.c_str(),"BLUE")){
				(*value)=itoa(teamscores[TEAM_BLUE],numbuf,10);
			}
		}else if(!strcmp(begining.c_str(),"ITEM")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseItemStr(&end);
			}
			if(type>=0 && type<ITEM_LAST){
				(*value)=itoa(all_items[type],numbuf,10);
			}
		}else if(!strcmp(begining.c_str(),"GAMETYPE")){
			if(!strcmp(end.c_str(),"1V1")){
				(*value)=itoa(gametype[GAMETYPE_1V1],numbuf,10);
			}else if(!strcmp(end.c_str(),"TDM")){
				(*value)=itoa(gametype[GAMETYPE_TDM],numbuf,10);
			}else if(!strcmp(end.c_str(),"CTF")){
				(*value)=itoa(gametype[GAMETYPE_CTF],numbuf,10);
			}else if(!strcmp(end.c_str(),"FFA")){
				(*value)=itoa(gametype[GAMETYPE_FFA],numbuf,10);
			}
		}else if(!strcmp(begining.c_str(),"GAMEOVERREASON")){
			if(!strcmp(end.c_str(),"TIME")){
				(*value)=itoa(gameoverreason[LIMIT_TIME],numbuf,10);
			}else if(!strcmp(end.c_str(),"FRAG")){
				(*value)=itoa(gameoverreason[LIMIT_FRAG],numbuf,10);
			}else if(!strcmp(end.c_str(),"CAPTURE")){
				(*value)=itoa(gameoverreason[LIMIT_CAPTURE],numbuf,10);
			}else{
				(*value)=itoa(games-(gameoverreason[LIMIT_CAPTURE]+gameoverreason[LIMIT_CAPTURE]+
					gameoverreason[LIMIT_TIME]),numbuf,10);
			}

		}

	}



}

void Map::InitKilltables(){
	//add players to kill tables
	Player *p;
	int i;
	p=players.GetFirst();
	if(p!=NULL){
		do{
			//add player
			for(i=0;i<CROSSTABLE_MAX_KILL;i++){
				kills[i].AddPlayer(p);	
			}
			//add player's kills
			Player *killee;
			//loop through player's kills
			PlayerKillList *pk;
			pk=p->GetKillList();
			pk=pk->GetFirst();
			if(pk!=NULL){
				do{
					killee=players.FindPlayer(pk->GetName());
					if(killee!=NULL){
						for(i=1;i<MAX_KILLTYPE;i++){
							for(int j=0;j<pk->GetKill(i);j++){
								AddKillToTables(kills,i,p,killee);
							}
						}
					}
				}while((pk=pk->GetNext())!=NULL);
			}
		}while((p=players.GetNext())!=NULL);
	}
}



void Map::WriteToDb(ConfigFile *cf){
	char temp[500],temp2[50];

	cf->AddValue("","map",mapname.c_str());
	cf->AddValue("","games",itoa(games,temp,10));
	cf->AddValue("","total_time_sec",itoa(total_time_sec,temp,10));
	cf->AddValue("","total_items",itoa(total_items,temp,10));
	cf->AddValue("","msgs",itoa(msgs,temp,10));
	cf->AddValue("","msgs_team",itoa(msgs_team,temp,10));

	int i;
	for(i=0;i<ITEM_LAST;i++){
		sprintf(temp2,"item_%d",i);
		cf->AddValue("",temp2,itoa(all_items[i],temp,10));
	}
	for(i=0;i<MAX_KILLTYPE;i++){
		sprintf(temp2,"kill_%d",i);
		cf->AddValue("",temp2,itoa(all_kills[i],temp,10));
	}
	for(i=0;i<LIMIT_MAX;i++){
		sprintf(temp2,"gameover_%d",i);
		cf->AddValue("",temp2,itoa(gameoverreason[i],temp,10));
	}
	for(i=0;i<GAMETYPE_MAX;i++){
		sprintf(temp2,"gametype_%d",i);
		cf->AddValue("",temp2,itoa(gametype[i],temp,10));
	}
	for(i=0;i<TEAM_MAX;i++){
		sprintf(temp2,"teamscore_%d",i);
		cf->AddValue("",temp2,itoa(teamscores[i],temp,10));
	}
	
	string buf;
	gamelist.Write(&buf);
	cf->AddValue("","gamelist",buf.c_str());

	Player *p;
	//players
	p=players.GetFirst();
	if(p!=NULL){
		do{
			p->WriteToDb(cf);
		}while((p=players.GetNext())!=NULL);
	}

}


Map::Map(ConfigFile *cf){
	string temp;
	char temp2[50];

	cf->GetValue("","map",&mapname);

	printf("reading map %s\t",mapname.c_str());

	cf->GetValue("","games",&temp);
	games=atoi(temp.c_str());
	cf->GetValue("","total_time_sec",&temp);
	total_time_sec=atoi(temp.c_str());
	cf->GetValue("","total_items",&temp);
	total_items=atoi(temp.c_str());
	cf->GetValue("","msgs",&temp);
	msgs=atoi(temp.c_str());
	cf->GetValue("","msgs_team",&temp);
	msgs_team=atoi(temp.c_str());

	int i;
	for(i=0;i<ITEM_LAST;i++){
		sprintf(temp2,"item_%d",i);
		cf->GetValue("",temp2,&temp);
		all_items[i]=atoi(temp.c_str());
	}
	for(i=0;i<MAX_KILLTYPE;i++){
		sprintf(temp2,"kill_%d",i);
		cf->GetValue("",temp2,&temp);
		all_kills[i]=atoi(temp.c_str());
	}
	for(i=0;i<LIMIT_MAX;i++){
		sprintf(temp2,"gameover_%d",i);
		cf->GetValue("",temp2,&temp);
		gameoverreason[i]=atoi(temp.c_str());
	}
	for(i=0;i<GAMETYPE_MAX;i++){
		sprintf(temp2,"gametype_%d",i);
		cf->GetValue("",temp2,&temp);
		gametype[i]=atoi(temp.c_str());
	}
	for(i=0;i<TEAM_MAX;i++){
		sprintf(temp2,"teamscore_%d",i);
		cf->GetValue("",temp2,&temp);
		teamscores[i]=atoi(temp.c_str());
	}

	int pos;
	cf->GetValue("","gamelist",&temp);
	os.TrimStr(&temp);
	while(temp.length()>0){
		gamelist.AddGame(atoi(temp.c_str()));
		pos=temp.find_first_of(' ');
		if(pos>-1){
			temp=temp.substr(pos+1);
		}else{
			break;
		}
	}

	//now add players
	Section *s;
	Player *p;
	s=cf->GetSettings();
	if(s!=NULL){
		s=s->GetNext();
		while(s!=NULL){
			p=new Player(s);
			players.AddPlayer(p,p->GetName());
			s=s->GetNext();
		}
	}

	modified=false;

}


///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

void MapList::ReSort(int type){

}



