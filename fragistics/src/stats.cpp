/* stats.cpp
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


#include "stats.h"
#include "game.h"
#include "htmlfile.h"
#include "settings.h"
#include <time.h>
#include <math.h>

extern Settings settings;
extern OS os;


char kill_strings[26][32] = {	"All Kills",
				"Shotgun Kills",
				"Gauntlet Kills",
				"Machinegun Kills",
				"Direct Grenade Kills",
				"Grenade Splash Kills",
				"Direct Rocket Kills",
				"Rocket Splash Kills",
				"Direct Plasmagun Kills",
				"Plasmagun Splash Kills",
				"Railgun Kills",
				"Lightninggun Kills",
				"Direct BFG Kills",
				"BFG Splash Kills",
				"Drown",
				"Slime",
				"Lava",
				"Crushed",
				"TeleFrag",
				"Cratered",
				"Suicide",
				"unknown",
				"Trigger Hurt (fall out of map)",
				"other unknown",
				"Max Killtype",
				"\0\0\0" };

Stats::Stats () 
{
    int i;
    
    games=0;
    players=0;
    // kills=0;
    items=0;
    msgs=0;
    msgs_team=0;
    time_sec=0;
    currentplayer=NULL;
    currentploop=NULL;
    pgamelist=NULL;
    pklist=NULL;
    last_parsed_line=0;
    killer=NULL;
    killee=NULL;
    killtype=0;    
    
    for(i=0;i<MAX_KILLTYPE;i++) {
	all_kills[i]=0;
    }
    for(i=0;i<ITEM_LAST;i++) {
	all_items[i]=0;
    }
    currentgame=NULL;
    
    if(settings.GetUseCustRank()!=0){
	string temp,src,srcpath;
	
	settings.GetCustRankTemplate(&src);
	
	settings.GetTemplateSrcpath(&srcpath);
	if(srcpath.length()>0){
	    temp=srcpath;
	    temp+=os.PathSeparator;
	}
	temp+=src;
	if(strstr(src.c_str(),".htmlt")==NULL)
	    temp+=".htmlt";
	
	File *f = os.GetFile(temp.c_str());
	if(f->OpenRead()){
	    f->GetFile(&cust_math);
	}else{
	    printf("Error opening Custom Rank Template!\n");
	}
	f->Close();
    }
}

int Stats::GetGames()
{
    return games;
}

void Stats::SetCurrentFile (const char *file,int line)
{
    filename=file;
    last_parsed_line=line;
}

//used by player sort function
void Stats::SetCurrentPlayer (Player *p)
{
    currentplayer=p;
}

//used by player.setinfo
int Stats::GetPlayerRank (char *name)
{
    Player *p;
    p=player_list.FindPlayer(name);
    if(p!=NULL)
	return p->GetRank();
    return 10000;
};

//Used by custom ranking scheme code;
void Stats::SetKillerKillee (Player* killer_, Player *killee_, int kill)
{
    killer=killer_;
    killee=killee_;
    killtype=kill;
}

int Stats::GetCustRank (char *name) {
    Player *p;
    p=player_list.FindPlayer(name);
    if(p!=NULL)
	return p->GetCustRank();
    return settings.GetCustRankStart();
}

MapList *Stats::GetMaps()
{
    return &maps;
}

PList *Stats::GetPlayers()
{
    return &player_list;
}

int Stats::GetFileNameLine(string *file)
{
    (*file)=filename;
    return last_parsed_line;
}

void Stats::SetGames(int g)
{
    games=g;
}

void Stats::GetGamelist (string *s)
{
    (*s)=gamelisthtml;
}

void Stats::SetGamelist (string *s)
{
    gamelisthtml=*s;
}

void Stats::AddGameToTotal(Game *game, int lastline)
{
    //lastgame is for future use as to not have to reparse the whole log file,
    //to only parse the new games
    last_parsed_line=lastline;
    
    if( !( game->GetTotalTime()>=settings.GetGameMintime() &&
	   game->GetPlayers()->GetCount()>=settings.GetGameMinplayers() &&
	   game->GetKills() >= settings.GetGameMinkills() &&
	   ( settings.GetGameComplete()==0 || game->isGameOver()==true) 
	   ) ) return;
    
    
    //add stats to total
    currentgame=game;
    SetKillerKillee(NULL,NULL,0);
    
    //currentgame->WriteResults(games);
    game->SetGameNumber(++games);
    maps.AddGame(game);
    
    players+=currentgame->GetPlayers()->GetCount();
    msgs+=currentgame->GetMsgs();
    msgs_team+=currentgame->GetMsgsTeam();
    int i;
    for(i=0;i<MAX_KILLTYPE;i++){
	all_kills[i]+=currentgame->GetKills(i);
    }
    for(i=0;i<ITEM_LAST;i++){
	all_items[i]+=currentgame->GetItem(i);
	items+=currentgame->GetItem(i);
    }
    PList *gplayers = currentgame->GetPlayers();
    Player *p,*p2;
    string sorttype;
    if(game->isGameOver()){
	sorttype="PLAYER_SCORE";
    }else{
	sorttype="PLAYER_FRAGS.ALL";
    }
    gplayers->ReSortList(&sorttype,this); 
    p=gplayers->GetFirst();
    int place=1;
    if(p!=NULL){
	p->AddWin(currentgame->GetType());
	do{
	    if(currentgame->GetType()==GAMETYPE_FFA && place>1 && place<=settings.GetGamePlaced()){
		p->AddFFAPlace();
	    }
	    p->AddGameOfType(currentgame->GetType());
	    p2=player_list.FindPlayer(p->GetName());
	    if(p2==NULL){
		p2=new Player(p->GetName());
		player_list.AddPlayer(p2,p->GetName());
	    }
	    p2->AddStats(p);
	    p2->AddGameNum(games);
	    place++;
	}while((p=gplayers->GetNext())!=NULL);
    }
    char numbuf[50];
    
    //do web pages
    
    //add players to killtables
    PList* list_player=currentgame->GetPlayers();
    p=list_player->GetFirst();
    if(p!=NULL){
	do{
	    for(i=0;i<CROSSTABLE_MAX_KILL;i++){
		currentgame->GetKillTable(i)->AddPlayer(p);	
	    }
	}while((p=list_player->GetNext())!=NULL);
    }
    
    string file,filedest;
    HtmlTemplate *template_;
    
    //gamelistloop
    settings.GetTemplatesGamelistloop(&file);
    template_= new HtmlTemplate(file.c_str(), file.c_str(),this);
    if(template_->ParseFile(&filedest)==true){
	gamelisthtml+=filedest;
    }else{
	printf("ERROR parsing game list loop\n");
    }
    
    //game pages
    for(i=0;i<settings.GetTemplatesGameNumber();i++){
	settings.GetTemplatesGamePath(i,&file);
	filedest=file;
	filedest+="_";
	filedest+=itoa(games,numbuf,10);
	template_= new HtmlTemplate(file.c_str(), filedest.c_str(),this);
	printf("Writting template %s\n",filedest.c_str());
	bool ret=template_->Write();
	if(ret==false){
	    printf("Error writting template %s\n",file.c_str());
	}
	delete template_;
    }
    
    time_sec+=currentgame->GetTotalTime();
    currentgame=NULL;
}

void Stats::GetVariableValue(string *variable, string *value){
    
    string begining,end;
    int pos;
    char numbuf[50];
    
    pos=variable->find_first_of('_');
    begining=variable->substr(0,pos);
    end=variable->substr(pos+1);
    value->erase();
    
    if(!strcmp(variable->c_str(),"TEST")){
	(*value)="it works!";
    }else if(!strcmp(variable->c_str(),"CREDIT")){
	(*value)="<font size=-2>This file was generated by <a href=\"http://fragistics.sourceforge.net\">Fragistics</a></font>";
    }else if(!strcmp(variable->c_str(),"PQ")){
	(*value)="<a href=\"http://www.planetquake.com\">Planet Quake</a>";
    }else if(!strcmp(variable->c_str(),"GS")){
	(*value)="<a href=\"http://www.gamespy.com\">Game Spy</a>";
    }else if(!strcmp(begining.c_str(),"STATS")){
	
	(*variable)=end;
	pos=variable->find_first_of('.');
	if(pos==-1){
	    
	    if(!strcmp(end.c_str(),"GAMES")){
		(*value)=itoa(games,numbuf,10);
	    }else if(!strcmp(end.c_str(),"TOTPLAYERS")){
		(*value)=itoa(players,numbuf,10);
	    }else if(!strcmp(end.c_str(),"UNIQPLAYERS")){
		(*value)=itoa(player_list.GetCount(),numbuf,10);
	    }else if(!strcmp(end.c_str(),"LOGEDPLAYERS")){
		(*value)=itoa(limit_player_list.GetCount(),numbuf,10);
	    }else if(!strcmp(end.c_str(),"TOTKILLS")){
		(*value)=itoa(all_kills[0],numbuf,10);
	    }else if(!strcmp(end.c_str(),"TOTITEMS")){
		(*value)=itoa(items,numbuf,10);
	    }else if(!strcmp(end.c_str(),"TOTMSGS")){
		(*value)=itoa(msgs,numbuf,10);
	    }else if(!strcmp(end.c_str(),"TOTTEAMMSGS")){
		(*value)=itoa(msgs_team,numbuf,10);
	    }else if(!strcmp(end.c_str(),"TOTSECONDS")){
		(*value)=itoa(time_sec,numbuf,10);
	    }else if(!strcmp(end.c_str(),"GAMENUMBER")){
		(*value)=itoa(games,numbuf,10);
	    }else if(!strcmp(end.c_str(),"PREVGAMENUMBER")){
		(*value)=itoa(games-1,numbuf,10);
	    }else if(!strcmp(end.c_str(),"NEXTGAMENUMBER")){
		(*value)=itoa(games+1,numbuf,10);
	    }else if(!strcmp(end.c_str(),"GAMELISTLOOP")){
		(*value)=gamelisthtml;
	    }else if(!strcmp(end.c_str(),"CREATIONTIME")){
		(*value)=gamelisthtml;
	    }
	}else{
	    begining=variable->substr(0,pos);
	    end=variable->substr(pos+1);
	    if(!strcmp(begining.c_str(),"TOTTIME")){
		if(!strcmp(end.c_str(),"DAYS")){
		    (*value)=itoa(time_sec/(60*60*24),numbuf,10);
		}else if(!strcmp(end.c_str(),"HOURS")){
		    (*value)=itoa((time_sec/(60*60))%24,numbuf,10);
		}else if(!strcmp(end.c_str(),"MINUTES")){
		    (*value)=itoa((time_sec/60)%60,numbuf,10);
		}else if(!strcmp(end.c_str(),"SECONDS")){
		    (*value)=itoa(time_sec%60,numbuf,10);
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
		    killtypes[type].Write(value);
		}
	    }else if(!strcmp(begining.c_str(),"KILLTABLE_C")){
		int type;
		if(end.at(0)>='0' && end.at(0)<='9'){
		    type=atoi(end.c_str());
		}else{
		    type=ParseKilltypeStr(&end);
		}
		if(type>=0&&type<CROSSTABLE_MAX_KILL){
		    killtypes[type].WriteRowCompress(value);
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
	
    }else if(!strcmp(begining.c_str(),"GAME")){
	if(currentgame!=NULL)
	    currentgame->GetVariableValue(&end, value,this);
    }else if(!strcmp(begining.c_str(),"HTML")){
	if(!strcmp(end.c_str(),"BGIMAGE")){
	    settings.GetBgImage(value);
	}else if(!strcmp(end.c_str(),"ALINK")){
	    settings.GetColorAlink(value);
	}else if(!strcmp(end.c_str(),"BGCOLOR")){
	    settings.GetColorBg(value);
	}else if(!strcmp(end.c_str(),"LINK")){
	    settings.GetColorLink(value);
	}else if(!strcmp(end.c_str(),"TEXT")){
	    settings.GetColorText(value);
	}else if(!strcmp(end.c_str(),"VLINK")){
	    settings.GetColorVlink(value);
	}
    }else if(!strcmp(begining.c_str(),"SETTINGS")){
	if(!strcmp(end.c_str(),"PLAYERKILLLIMIT")){
	    (*value)=itoa(settings.GetMainPlayersMinKills(),numbuf,10);
	}else if(!strcmp(end.c_str(),"PLAYERGAMELIMIT")){
	    (*value)=itoa(settings.GetMainPlayersMinGames(),numbuf,10);
	}else if(!strcmp(end.c_str(),"PLAYERTIMELIMITSEC")){
	    (*value)=itoa(settings.GetMainPlayersMinTime(),numbuf,10);
	}else if(!strcmp(end.c_str(),"PLAYERTIMELIMIT.HOURS")){
	    (*value)=itoa((settings.GetMainPlayersMinTime()/(60*60))%24,numbuf,10);
	}else if(!strcmp(end.c_str(),"PLAYERTIMELIMIT.MINUTES")){
	    (*value)=itoa((settings.GetMainPlayersMinTime()/60)%60,numbuf,10);
	}else if(!strcmp(end.c_str(),"PLAYERTIMELIMIT.SECONDS")){
	    (*value)=itoa(settings.GetMainPlayersMinTime()%60,numbuf,10);
	    
	}else if(!strcmp(end.c_str(),"GAMEMINKILLS")){
	    (*value)=itoa(settings.GetGameMinkills(),numbuf,10);
	}else if(!strcmp(end.c_str(),"GAMEMINPLAYERS")){
	    (*value)=itoa(settings.GetGameMinplayers(),numbuf,10);
	}else if(!strcmp(end.c_str(),"GAMECOMPLETE")){
	    if(settings.GetGameComplete()){
		(*value)="TRUE";
	    }else{
		(*value)="FALSE";
	    }
	}else if(!strcmp(end.c_str(),"GAMEMINTIME")){
	    (*value)=itoa(settings.GetGameMintime(),numbuf,10);
	}else if(!strcmp(end.c_str(),"GAMEMINTIME.MINUTES")){
	    (*value)=itoa((settings.GetGameMintime()/60)%60,numbuf,10);
	}else if(!strcmp(end.c_str(),"GAMEMINTIME.SECONDS")){
	    (*value)=itoa(settings.GetGameMintime()%60,numbuf,10);
	}
    }else if(!strcmp(begining.c_str(),"USER")){
	settings.FindSetting(value,"USER VARS",end.c_str(),"UNDEFINED VARIABLE");
    }else if(!strcmp(begining.c_str(),"PLAYER")){
	if(currentplayer!=NULL)
	    currentplayer->GetVariableValue(&end, value);
	else
	    printf("LOOP VARIABLE outside of loop\n");
    }else if(!strcmp(begining.c_str(),"KILLER")){
	if(killer!=NULL)
	    killer->GetVariableValue(&end, value);
	else
	    printf("ILLEGAL template variable found\n");
    }else if(!strcmp(begining.c_str(),"KILLEE")){
	if(killee!=NULL)
	    killee->GetVariableValue(&end, value);
	else
	    printf("ILLEGAL template variable found\n");
    }else if(!strcmp(begining.c_str(),"KILLTYPE")){
	if(killtype!=0)
	    (*value)=itoa(killtype,numbuf,10);
	else
	    printf("ILLEGAL template variable found\n");
    }else if(!strcmp(variable->c_str(),"GAMELOOP.NUMBER")){
	if(pgamelist!=NULL)
	    (*value)=itoa(pgamelist->GetCurrentGame(),numbuf,10);
	else
	    printf("LOOP VARIABLE outside of loop\n");
    }else if(!strcmp(begining.c_str(),"PKLIST")){
	if(pklist!=NULL){
	    if(!strcmp(end.c_str(),"NAME")){
		pklist->WriteName(value);
	    }else{
		(*variable)=end;
		pos=variable->find_first_of('.');
		begining=variable->substr(0,pos);
		end=variable->substr(pos+1);
		int type;
		if(end.at(0)>='0' && end.at(0)<='9'){
		    type=atoi(end.c_str());
		}else{
		    type=ParseKilltypeStr(&end);
		}
		(*value)=itoa(pklist->GetKill(type),numbuf,10);
	    }
	}else
	    printf("LOOP VARIABLE outside of loop\n");
    }else if(!strcmp(begining.c_str(),"MAP")){
	if(currentmap!=NULL)
	    currentmap->GetVariableValue(&end, value);
	else
	    printf("LOOP VARIABLE outside of loop\n");
    }else if(!strcmp(begining.c_str(),"PLAYER.INDEX")){
	(*value)=itoa(currentploop->GetPos(currentplayer)+1,numbuf,10);//+1 because it is zero based
    }else if(!strcmp(begining.c_str(),"NOW")){
	char buf[512];
	settings.GetMainTimeFormat(&end);
	time_t now;
	now=time(NULL);
	strftime(buf,511,end.c_str(),localtime(&now));
	(*value)=buf;
    }
    
    
}

bool Stats::WriteMainStats(){
    
    HtmlTemplate *template_;
    string file,filedest;
    char numbuf[50];
    
    printf("\n\nInitalizing Overall Stats\n");
    //setup stats
    limit_player_list.DupeList(&player_list);
    limit_player_list.TrimPlayers();
    //add players to kill tables
    Player *p;
    int i;
    p=player_list.GetFirst();
    if(p!=NULL){
	do{
	    if(!settings.GetMainKilltables() || limit_player_list.FindPlayer(p->GetName())!=NULL){
		//add player
		for(i=0;i<CROSSTABLE_MAX_KILL;i++){
		    killtypes[i].AddPlayer(p);	
		}
		//add player's kills
		Player *killee;
		//loop through player's kills
		PlayerKillList *pk;
		pk=p->GetKillList();
		pk=pk->GetFirst();
		if(pk!=NULL){
		    do{
			killee=player_list.FindPlayer(pk->GetName());
			if(killee!=NULL){
			    for(i=1;i<MAX_KILLTYPE;i++){
				for(int j=0;j<pk->GetKill(i);j++){
				    AddKillToTables(killtypes,i,p,killee);
				}
			    }
			}
		    }while((pk=pk->GetNext())!=NULL);
		}
	    }
	}while((p=player_list.GetNext())!=NULL);
    }
    //setup Per Map Killtables
    if(maps.GetFirst()){
	do{
	    maps.GetMap()->InitKilltables();
	}while(maps.GetNext());
    }
    
    
    
    //build templates
    //	HtmlTemplate *test;
    //	test = new HtmlTemplate("test9.htmlt", "test9.html",this);
    //	return test->Write();
    //	test = new HtmlTemplate("test10.htmlt", "test10.html",this);
    //	return test->Write();
    
    
    bool ret,err=false;
    
    //lastgame template
    settings.GetTemplatesLastGame(&file);
    for(i=0;i<settings.GetTemplatesGameNumber();i++){
	settings.GetTemplatesGamePath(i,&filedest);
	filedest+="_0";
	template_= new HtmlTemplate(file.c_str(), filedest.c_str(),this);
	printf("Writting template %s\n",filedest.c_str());
	ret=template_->Write();
	if(ret==false){
	    printf("Error writting template %s\n",file.c_str());
	    err=true;
	}
	delete template_;
	
	settings.GetTemplatesGamePath(i,&filedest);
	filedest+="_";
	filedest+=itoa(games+1,numbuf,10);
	template_= new HtmlTemplate(file.c_str(), filedest.c_str(),this);
	printf("Writting template %s\n",filedest.c_str());
	ret=template_->Write();
	if(ret==false){
	    printf("Error writting template %s\n",file.c_str());
	    err=true;
	}
	delete template_;
    }
    
    //gamelist
    settings.GetTemplatesGamelist(&file);
    template_= new HtmlTemplate(file.c_str(), file.c_str(),this);
    printf("Writting template %s\n",file.c_str());
    ret=template_->Write();
    if(ret==false){
	printf("Error writting template %s\n",file.c_str());
	err=true;
    }
    delete template_;
    
    
    //per map templates
    SetupLoop(LOOP_MAPS);
    if(currentmap!=NULL){
	do{
	    if(currentmap->GetModified()){
		string mapname;
		currentmap->GetName(&mapname);
		for(i=0;i<settings.GetTemplatesMapNumber();i++){
		    settings.GetTemplatesMapPath(i,&file);
		    filedest=file;
		    filedest+="_";
		    filedest+=mapname;
		    template_= new HtmlTemplate(file.c_str(), filedest.c_str(),this);
		    printf("Writting template %s\n",filedest.c_str());
		    ret=template_->Write();
		    if(ret==false){
			printf("Error writting template %s\n",filedest.c_str());
			err=true;
		    }
		    delete template_;
		}
	    }
	    NextLoop(LOOP_MAPS);
	}while(currentmap!=NULL);
    }
    
    
    
    
    
    
    //player templates
    
    settings.GetTemplatesPlayer(&file);
    SetupLoop(LOOP_ALLPLAYER_FULL);
    if(currentplayer!=NULL){
	do{
	    if(currentplayer->GetModified()){
		currentplayer->GetFileName(&filedest);
		template_= new HtmlTemplate(file.c_str(), filedest.c_str(),this);
		printf("Writting template %s\n",filedest.c_str());
		ret=template_->Write();
		if(ret==false){
		    printf("Error writting template %s\n",filedest.c_str());
		    err=true;
		}
		delete template_;
	    }
	    NextLoop(LOOP_ALLPLAYER_FULL);
	}while(currentplayer!=NULL);
    }
    
    //main templates
    for(i=0;i<settings.GetTemplatesMainNumber();i++){
	settings.GetTemplatesMainPath(i,&file);
	template_= new HtmlTemplate(file.c_str(), file.c_str(),this);
	printf("Writting template %s\n",file.c_str());
	ret=template_->Write();
	if(ret==false){
	    printf("Error writting template %s\n",file.c_str());
	    err=true;
	}
	delete template_;
    }
    
    
    return err;
}


int Stats::DoCustMath(){
    string temp,buf;
    if(settings.GetUseCustRank()!=0){
	temp=cust_math;
	HtmlTemplate math(this);
	math.Parse(&temp,&buf);
	os.TrimStr(&buf);
	return atoi(buf.c_str());
    }
    return 0;
}


void Stats::AddPlayer(Player *player){
    player_list.AddPlayer(player,player->GetName());
    
}

//todo:
void Stats::AddMap(Map *map){
    maps.AddMap(map);
    
}

/*
  bool Stats::WriteMainStats(){
  
  HTMLFile *test;
  string temp,temp2;
  char numbuf[50];
  PList trimedPList;
  
  trimedPList.DupeList(&player_list);
  trimedPList.TrimPlayers();
  
  
  printf("\nGames List Page\n");
  
  //	temp="<HTML><HEAD>\n";
  //	temp+="<title>Last Game</title>\n</head>\n";
  //	temp+="<body bgcolor=\"#000000\" text=\"999999\" link=\"#0000ff\" vlink=\"#ff0000\" alink=\"00ff00\">";
  
  
  temp="You have gone past the last game.<P>Please choose one of the following<br>\n<a href=\"games.html\">Games List</a> <a href=\"stats.html\">Overall Stats</a> <a href=\"players.html\">Player stats</a>\n</body></html>";
  
  sprintf(numbuf,"game%d.html",games+1);
  test = new HTMLFile(numbuf);
  if(test->Open()){
  test->WriteHeader("Last Game");
  test->Write(&temp);
  }else{
  printf("failed to open %s html file",numbuf);
  return false;
  }
  delete test;
  test=NULL;
  temp.erase();
  
  
  test = new HTMLFile("games.html");
  
  if(test->Open()){
  test->WriteHeader("Games Played");
  test->Write(&gamelisthtml);
  test->Write("</table>");
  test->Write("<P><HR><P><font size=-2>This file was generated by <a href=\"http://members.xoom.com/TroZ_/q3logger/index.html\">Q3Logger</a></font>");
  test->Write("</body></html>");
  }else{
  printf("failed to open gamelist html file");
  return false;
  }
  
  delete test;
  test=NULL;
  
  printf("\nPlayer Stats\n");
  printf("%d players\n",player_list.GetCount());
  
  test = new HTMLFile("players.html");
  
  if(test->Open()){
  
  //		test->Write("<html><head><title>Player Stats</title></head>");
  //		//test->Write("<body bgcolor=\"#808080\"><CENTER>");
  //		test->Write("<body bgcolor=\"#000000\" text=\"999999\" link=\"#0000ff\" vlink=\"#ff0000\" alink=\"00ff00\"><center>");
  
  test->WriteHeader("Player Stats");
  
  test->Write("<h1>Player Stats</H1>");
  test->Write("<P><HR><P>");
  test->Write("<a href=\"stats.html\">Overall Stats</a> <a href=\"games.html\">Game Stats</a>");
  test->Write("<P><HR><P>");
  test->Write("<H3>Player Ranks</H3>");
  
  test->Write("<table border=1>");
  test->Write("<tr><td>&nbsp;</TD><td colspan=2>Games</td><td colspan=2>Time</td><td colspan=2>Kills</td><td colspan=2>Deaths</td><td colspan=2>Suicides</td><td colspan=2>Frags</td><td colspan=2>Score</td><td colspan=2>Efficiency</td><td colspan=2>Items</td><td colspan=2>FFA wins</td><td colspan=2>FFA wins or placed</td></TR>");
  
  PList game,time,kill,death,suicide,frag,score,eff,item,winsffa,topffa;
  game.DupeList(&trimedPList);
  time.DupeList(&trimedPList);
  kill.DupeList(&trimedPList);
  death.DupeList(&trimedPList);
  suicide.DupeList(&trimedPList);
  frag.DupeList(&trimedPList);
  eff.DupeList(&trimedPList);
  item.DupeList(&trimedPList);
  winsffa.DupeList(&trimedPList);
  topffa.DupeList(&trimedPList);
  score.DupeList(&trimedPList);
  game.ReSortList(0);
  time.ReSortList(1);
  kill.ReSortList(2);
  death.ReSortList(3);
  suicide.ReSortList(4);
  frag.ReSortList(5);
  eff.ReSortList(6);
  item.ReSortList(7);
  winsffa.ReSortList(9);
  topffa.ReSortList(14);
  score.ReSortList(8);
  
  
  
  Player *g,*t,*k,*d,*s,*f,*e,*i,*sc,*w,*tp;
  g=game.GetFirst();
  t=time.GetFirst();
  k=kill.GetFirst();
  d=death.GetFirst();
  s=suicide.GetFirst();
  f=frag.GetFirst();
  e=eff.GetFirst();
  i=item.GetFirst();
  sc=score.GetFirst();
  w=winsffa.GetFirst();
  tp=topffa.GetFirst();
  int c=0;
  if(g!=NULL && t!=NULL && k!=NULL && d!=NULL && s!=NULL && f!=NULL && e!=NULL && i!=NULL && sc!=NULL && w!=NULL && tp!=NULL ){
  do{
  c++;
  temp.erase();
  temp+="<tr><td>";
  temp+=itoa(c,numbuf,10);  //count
  temp+="</td><td>";
  temp2.erase();
  g->Write(&temp2);
  temp+=temp2;							//games name
  temp+="</td><td>";
  temp+=itoa(g->GetGames(),numbuf,10);	//games value
  temp+="</td><td>";
  temp2.erase();
  t->Write(&temp2);
  temp+=temp2;							//time name
  temp+="</td><td>";
  t->GetTimeStr(numbuf);
  temp+=numbuf;							//time value
  temp+="</td><td>";
  temp2.erase();
  k->Write(&temp2);
  temp+=temp2;							//kill name
  temp+="</td><td>";
  temp+=itoa(k->GetKill(0),numbuf,10);	//kill value
  temp+="</td><td>";
  temp2.erase();
  d->Write(&temp2);
  temp+=temp2;							//deaths name
  temp+="</td><td>";
  temp+=itoa(d->GetDeath(0),numbuf,10);	//deaths value
  temp+="</td><td>";
  temp2.erase();
  s->Write(&temp2);
  temp+=temp2;							//suicide name
  temp+="</td><td>";
  temp+=itoa(s->GetSuicide(0),numbuf,10);//suicide value
  temp+="</td><td>";
  temp2.erase();
  f->Write(&temp2);
  temp+=temp2;							//frag name
  temp+="</td><td>";
  temp+=itoa(f->GetFrags(0),numbuf,10);	//frag value
  temp+="</td><td>";
  temp2.erase();
  sc->Write(&temp2);
  temp+=temp2;							//score name
  temp+="</td><td>";
  temp+=itoa(sc->GetScore(),numbuf,10);	//score value
  temp+="</td><td>";
  temp2.erase();
  e->Write(&temp2);
  temp+=temp2;							//eff name
  temp+="</td><td>";
  e->GetEff(0,numbuf);
  temp+=numbuf;							//eff value
  temp+="</td><td>";
  temp2.erase();
  i->Write(&temp2);
  temp+=temp2;							//items name
  temp+="</td><td>";
  temp+=itoa(i->GetItemsTotal(),numbuf,10);//items value
  temp+="</td><td>";
  temp2.erase();
  w->Write(&temp2);
  temp+=temp2;							//winsffa name
  temp+="</td><td>";
  temp+=itoa(w->GetWins(GAMETYPE_FFA),numbuf,10);//winsffa value
  temp+="</td><td>";
  temp2.erase();
  tp->Write(&temp2);
  temp+=temp2;							//topffa name
  temp+="</td><td>";
  temp+=itoa((tp->GetWins(GAMETYPE_FFA)+tp->GetFFAPlaces()),numbuf,10);//topffa value
  temp+="</td></tr>";
  
  test->Write(&temp);
  
  g=game.GetNext();
  t=time.GetNext();
  k=kill.GetNext();
  d=death.GetNext();
  s=suicide.GetNext();
  f=frag.GetNext();
  e=eff.GetNext();
  i=item.GetNext();
  sc=score.GetNext();
  w=winsffa.GetNext();
  tp=topffa.GetNext();
  }while(g!=NULL && t!=NULL && k!=NULL && d!=NULL && s!=NULL && f!=NULL && e!=NULL && i!=NULL && sc!=NULL && w!=NULL && tp!=NULL );
  }
  test->Write("</table>");
  
  
  test->Write("<P><HR><P>");
  test->Write("<H3>Player Stats</H3>");
  Player *p;
  p=trimedPList.GetFirst();
  if(p!=NULL){
  do{
  temp.erase();
  p->WriteStats(&temp,false);
  test->Write(&temp);
  }while((p=trimedPList.GetNext())!=NULL);
  }
  
  test->Write("</table>");
  test->Write("<P><HR><P><font size=-2>This file was generated by <a href=\"http://members.xoom.com/TroZ_/q3logger/index.html\">Q3Logger</a></font>");
  test->Write("</body></html>");
  }else{
  printf("failed to open player html file");
  return false;
  }
  
  delete test;
  test=NULL;
  
  
  
  printf("\nOverall Stats\n");
  test = new HTMLFile("stats.html");
  
  if(test->Open()){
  //		test->Write("<html><head><title>Overall Stats</title></head>");
  //		//test->Write("<body bgcolor=\"#808080\"><CENTER>");
  //		test->Write("<body bgcolor=\"#000000\" text=\"999999\" link=\"#0000ff\" vlink=\"#ff0000\" alink=\"00ff00\">");
  
  test->WriteHeader("Overall Stats");
  
  temp="<center><h1>Overall Stats</H1>\n";
  temp+=itoa(games,numbuf,10);
  temp+=" games played<BR>\n";
  temp+=itoa(players,numbuf,10);
  temp+=" players / ";
  temp+=itoa(player_list.GetCount(),numbuf,10);
  temp+=" unique players<BR>\n";
  temp+=itoa(trimedPList.GetCount(),numbuf,10);
  temp+=" players meet requirements<BR>\n";
  temp+="<font size=-2>( ";
  temp+=itoa(settings.GetMainPlayersMinGames(),numbuf,10);
  temp+=" Games, ";
  temp+=itoa(settings.GetMainPlayersMinTime(),numbuf,10);
  temp+=" Minutes, ";
  temp+=itoa(settings.GetMainPlayersMinKills(),numbuf,10);
  temp+=" Kills )</font><BR>\n";
  temp+=itoa(kills,numbuf,10);
  temp+=" kills<BR>\n";
  temp+=itoa(items,numbuf,10);
  temp+=" items<BR>\n";
  int temptime=time_sec;
  if(temptime>(60*60*24)){
  temp+=itoa((temptime/(60*60*24)),numbuf,10);
  temp+="&nbsp;D, ";
  temptime%=(60*60*24);
  }
  if(temptime>(60*60)){
  temp+=itoa((temptime/(60*60)),numbuf,10);
  temp+="&nbsp;H, ";
  temptime%=(60*60);
  }
  temp+=itoa((temptime/(60)),numbuf,10);
  temp+="&nbsp;M, ";
  temptime%=(60);
  temp+=itoa((temptime),numbuf,10);
  temp+="&nbsp;S";
  test->Write(&temp);
  
  test->Write("<P><HR><P>");
  test->Write("<a href=\"games.html\">Game Stats</a> <a href=\"players.html\">Player stats</a>");
  test->Write("<P><HR><P>");
  
  test->Write("<H3>Kills by Type</H3><P>");
  test->Write("<TABLE border=1>");
  
  int i;
  for(i=0;i<MAX_KILLTYPE;i+=2){
  
  test->Write("<TR><TD align=right>");
  test->Write(kill_strings[i]);
  test->Write("</TD>");
  test->Write("<TD>");
  itoa(all_kills[i],numbuf,10);
  test->Write(numbuf);
  test->Write("</TD>\n");
  
  test->Write("<TD width=50></TD><TD align=right>");
  test->Write(kill_strings[i+1]);
  test->Write("</TD>");
  test->Write("<TD>");
  itoa(all_kills[i+1],numbuf,10);
  test->Write(numbuf);
  test->Write("</TD></TR>\n");
  }
  
  test->Write("</table>\n");
  
  
  test->Write("<P><HR><P>");
  test->Write("<h3>Kill Tables</H3><P>");
  
  Player *p;
  int maxkilltable=0;
  if(settings.GetMainKilltables()==1)
  maxkilltable=1;
  if(settings.GetMainKilltables()==2)
  maxkilltable=CROSSTABLE_MAX_KILL;
  for(i=0;i<maxkilltable;i++){
  //add players to table
  printf("rendering table %d\n",i);
  p=player_list.GetFirst();
  if(p!=NULL){
  do{
  if(trimedPList.FindPlayer(p->GetName())!=NULL)
  killtypes[i].AddPlayer(p);	
  }while((p=player_list.GetNext())!=NULL);
  }
  
  temp.erase();
  temp2.erase();
  temp2=kill_strings[i];
  temp2+="<BR>";
  test->Write(&temp2);
  killtypes[i].WriteRowCompress(&temp);
  test->Write(&temp);
  test->Write("<P>");
  
  }
  
  
  test->Write("</CENTER>");
  test->Write("<P><HR><P><font size=-2>This file was generated by <a href=\"http://members.xoom.com/TroZ_/q3logger/index.html\">Q3Logger</a></font>");
  test->Write("</body></html>");
  }else{
  printf("failed to open stats html file");
  return false;
  }
  
  
  return true;
  }
  
  
  
*/



bool Stats::SetupLoop(int looptype,string *sort){
    
    
    switch(looptype){
    case LOOP_ALLPLAYER_TRIM:
	currentploop=new PList();
	currentploop->DupeList(&limit_player_list);
	if(sort!=NULL && sort->length()>0) currentploop->ReSortList(sort,this);
	currentplayer=currentploop->GetFirst();
	if(currentplayer!=NULL) return true;
	else{ 
	    delete currentploop;
	    currentploop = NULL;
	}
	break;
    case LOOP_ALLPLAYER_FULL:
	currentploop=new PList();
	currentploop->DupeList(&player_list);
	if(sort!=NULL && sort->length()>0) currentploop->ReSortList(sort,this);
	currentplayer=currentploop->GetFirst();
	if(currentplayer!=NULL) return true;
	else{ 
	    delete currentploop;
	    currentploop = NULL;
	}
	break;
    case LOOP_GAMEPLAYER_TRIM:
	if(currentgame!=NULL){
	    currentploop=new PList();
	    currentploop->DupeList(currentgame->GetPlayers());
	    currentploop->TrimPlayers();
	    if(sort!=NULL && sort->length()>0) currentploop->ReSortList(sort,this);
	    currentplayer=currentploop->GetFirst();
	    if(currentplayer!=NULL) return true;
	    else{ 
		delete currentploop;
		currentploop = NULL;
	    }
	}
	break;
    case LOOP_GAMEPLAYER_FULL:
	if(currentgame!=NULL){
	    currentploop=new PList();
	    currentploop->DupeList(currentgame->GetPlayers());
	    if(sort!=NULL && sort->length()>0) currentploop->ReSortList(sort,this);
	    currentplayer=currentploop->GetFirst();
	    if(currentplayer!=NULL) return true;
	    else{ 
		delete currentploop;
		currentploop = NULL;
	    }
	}
	break;
    case LOOP_MAPPLAYER_TRIM:
	if(currentmap!=NULL){
	    currentploop=new PList();
	    currentploop->DupeList(currentmap->GetPlayers());
	    currentploop->TrimPlayers();
	    if(sort!=NULL && sort->length()>0) currentploop->ReSortList(sort,this);
	    currentplayer=currentploop->GetFirst();
	    if(currentplayer!=NULL) return true;
	    else{ 
		delete currentploop;
		currentploop = NULL;
	    }
	}
	break;
    case LOOP_MAPPLAYER_FULL:
	if(currentmap!=NULL){
	    currentploop=new PList();
	    currentploop->DupeList(currentmap->GetPlayers());
	    if(sort!=NULL && sort->length()>0) currentploop->ReSortList(sort,this);
	    currentplayer=currentploop->GetFirst();
	    if(currentplayer!=NULL) return true;
	    else{ 
		delete currentploop;
		currentploop = NULL;
	    }
	}
	break;
	
    case LOOP_PLAYERGAMES:
	if(currentplayer!=NULL){
	    pgamelist=currentplayer->GetGameList();
	    if(pgamelist->GetFirst()==NULL){
		pgamelist=NULL;
	    }else{
		return true;
	    }
	    break;
	}
    case LOOP_PLAYERKILLS:
	if(currentplayer!=NULL){
	    pklist=currentplayer->GetKillList();
	    if((pklist=pklist->GetFirst())==NULL){
		pklist=NULL;
	    }else{
		return true;
	    }
	    break;
	}
    case LOOP_PLAYERDEATHS:
	if(currentplayer!=NULL){
	    pklist=currentplayer->GetDeathList();
	    if((pklist=pklist->GetFirst())==NULL){
		pklist=NULL;
	    }else{
		return true;
	    }
	    break;
	}
    case LOOP_MAPS:
	if(!maps.GetFirst()){
	    currentmap=NULL;
	}else{
	    currentmap=maps.GetMap();
	    return true;
	}
	break;
    case LOOP_MAPGAMES:
	if(currentmap!=NULL){
	    pgamelist=currentmap->GetGameList();
	    if(pgamelist->GetFirst()==NULL){
		pgamelist=NULL;
	    }else{
		return true;
	    }
	    break;
	}
	//implement other loop types here
    }
    return false;
}


bool Stats::NextLoop(int looptype){
    if(looptype!=LOOP_MAPS && looptype!=LOOP_PLAYERGAMES && looptype!=LOOP_MAPGAMES &&
       looptype!=LOOP_PLAYERKILLS && looptype!=LOOP_PLAYERDEATHS ){
	if(currentploop==NULL){
	    return false;
	}else{
	    currentplayer=currentploop->GetNext();
	    if(currentplayer!=NULL) return true;
	    else{ 
		delete currentploop;
		currentploop = NULL;
	    }
	}
    }else if(looptype==LOOP_PLAYERGAMES || looptype==LOOP_MAPGAMES ){
	if(pgamelist!=NULL){
	    if(pgamelist->GetNext()==NULL){
		pgamelist=NULL;
	    }else{
		return true;
	    }
	}
    }else if( looptype==LOOP_PLAYERKILLS || looptype==LOOP_PLAYERDEATHS ){
	if(pklist!=NULL){
	    if((pklist=pklist->GetNext())==NULL){
		pklist=NULL;
	    }else{
		return true;
	    }
	}
    }else if(looptype==LOOP_MAPS){
	if(!maps.GetNext()){
	    currentmap=NULL;
	}else{
	    currentmap=maps.GetMap();
	    return true;
	}
    }
    
    return false;
}



double Stats::DoMath(string *expression){
    double val1=1.0,val2=1.0,result=0.0;
    string var1,var2,oper,orig;
    int depth=0,pos=0,op=0;
    char cur='\0';
    
    os.TrimStr(expression);
    orig=(*expression);
    
    if(expression->length()<1){
	printf("Bad expression - empty\n");
	return result;
    }
    
    if(expression->at(0)=='(' && expression->at(expression->length()-1)==')' ){
	(*expression)=expression->substr(1,expression->length()-2);
	os.TrimStr(expression);
    }
    
    //is expression only one word
    if(expression->find_first_of(' ')==-1){
	//just a number or variable
	cur=expression->at(0);
	if(cur>='0' && cur<='9'){
	    val1=atof(expression->c_str());
	    return val1;
	}else{
	    GetVariableValue(expression,&var1);
	    val1=atof(var1.c_str());
	    return val1;
	}
    }
    
    //get first value
    while(pos<expression->length() && depth>=0 && !(depth==0 && cur==' ') ){
	cur=expression->at(pos);
	if(cur=='('){
	    depth++;
	}
	if(cur==')'){
	    depth--;
	}
	var1+=cur;	
	pos++;
    }
    if(depth!=0){
	printf("Bad Expression: %s\n",orig.c_str());
	return result;
    }
    (*expression)=expression->substr(pos);
    
    //get oper
    GetNextWord(expression,&oper);
    
    //get second value - the rest of the expression
    var2=(*expression);
    
    os.TrimStr(&oper);
    if(oper.at(0)=='+'){
	op=1;
    }else if(oper.at(0)=='-'){
	op=2;
    }else if(oper.at(0)=='*'){
	op=3;
    }else if(oper.at(0)=='/'){
	op=4;
    }else if(oper.at(0)=='%'){
	op=5;
    }else if(oper.at(0)=='^'){
	op=6;
    }else if(!strcmp(oper.c_str(),"0/")){
	op=7;
    }else if(!strcmp(oper.c_str(),"1/")){
	op=8;
    }
    
    if(op==0){
	printf("Bad operator %s in expression %s\n",oper.c_str(),orig.c_str());
	return result;
    }
    
    val1=DoMath(&var1);
    val2=DoMath(&var2);
    
    switch(op){
    case 1:
	result=val1+val2;
	break;
    case 2:
	result=val1-val2;
	break;
    case 3:
	result=val1*val2;
	break;
    case 4:
	result=val1/val2;
	break;
    case 5:
	result=((int)val1)%((int)val2);
	break;
    case 6:
	result=pow(val1,val2);
	break;
    case 7:
	if(val2==0.0)
	    result=0.0;
	else
	    result=val1/val2;
	break;
    case 8:
	if(val2==0.0)
	    result=val1;
	else
	    result=val1/val2;
	break;
    }
    
    return result;
}

