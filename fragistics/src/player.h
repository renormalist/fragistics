/* player.h
 Fragistics  -  Game Statistics program for Quake 3 Arena
 Copyright (C) 2000   Brian Risinger

see player.cpp or main.cpp for license info
*/

#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <stdio.h>
#include <string>
#include "defines.h"
#include "os.h"
#include "settings.h"

class Stats;

using namespace std;

class SimplePlayer {
	char name[50];
	string htmlname;
	string vertname;
	string plainname;

	void parsename();

public:
	SimplePlayer(const char *name_){
		strncpy(name,name_,48);
		name[49]='\0';
		parsename();
	};
	~SimplePlayer(){};

	void Write(string *str, int vert=0); //0=html 1=vert  2=plain
	char* GetName(){return name;};
};


class PlayerKillList {
	PlayerKillList *next;
	
	SimplePlayer *name;
	int kills[MAX_KILLTYPE];

public:
	PlayerKillList(){
		for(int i=0;i<MAX_KILLTYPE;i++)
			kills[i]=0;
		next=NULL;
		name=NULL;
	};
	~PlayerKillList(){
		if(next!=NULL)
			delete next;
		if(name!=NULL)
			delete name;
	}

	void AddKill(char *name, int killtype, int number=1){
		PlayerKillList *temp,*prev=NULL;
		temp=next;
		while(temp!=NULL){
			if(!strcmp(temp->name->GetName(),name)){
				//add middle
				temp->kills[killtype]+=number;
				temp->kills[TOTAL_KILLS]+=number;
				return;
			}
			prev=temp;
			temp=temp->next;
		}
		if(prev!=NULL){
			//add end
			prev->next=new PlayerKillList();
			prev->next->name=new SimplePlayer(name);
			prev->next->kills[killtype]+=number;
			prev->next->kills[TOTAL_KILLS]+=number;
		}else{
			//add begining
			next=new PlayerKillList();
			next->name=new SimplePlayer(name);
			next->kills[killtype]+=number;
			next->kills[TOTAL_KILLS]+=number;
		}
	};

	int GetKill(int type){
		return GetAggregateKills(kills,type);
	};

	char *GetName(){return name->GetName();};

	void SortList(){
		PlayerKillList *temp,*oldhead,*prev;
		oldhead=next;
		next=NULL;
		while(oldhead!=NULL){
			if(next==NULL || oldhead->GetKill(TOTAL_KILLS)>next->GetKill(TOTAL_KILLS)){
				//should be first
				temp=oldhead->next;
				oldhead->next=next;
				next=oldhead;
				oldhead=temp;
			}else{
				temp=next->next;
				prev=next;
				while(temp!=NULL){
					if(oldhead->GetKill(TOTAL_KILLS)>temp->GetKill(TOTAL_KILLS)){
						//oldhead goes between prev and temp
						prev->next=oldhead;
						oldhead=oldhead->next;
						prev->next->next=temp;
						break;
					}else{
						prev=temp;
						temp=temp->next;
					}
				}
				if(temp==NULL){
					//oldhdead goes at end
					prev->next=oldhead;
					oldhead=oldhead->next;
					prev->next->next=NULL;
				}
			}
		}
	};

	PlayerKillList *GetFirst(){
		SortList();
		return next;
	};

	PlayerKillList *GetNext(){
		if(next!=NULL)
			return next;
		return NULL;
	};

	void AddList(PlayerKillList *other){
		//yes, i know this is not efficent
		PlayerKillList *node=other->next;//get first
		while(node!=NULL){
			for(int i=1;i<MAX_KILLTYPE;i++){
				AddKill(node->GetName(),i,node->GetKill(i));
			}
			node=node->next;
		}

	};

	void WriteName(string *buf, int vert=0){name->Write(buf,vert);};

};



class GameList {
	GameList *head;
	GameList *current;
	GameList *last;

	int gamenumber;
	GameList *next;

public:
	GameList(){
		next=head=current=last=NULL;
		gamenumber=0;
	};
	~GameList(){
		if(head!=NULL)
			delete head;
		if(next!=NULL)
			delete next;
	}

	void AddGame(int gamenumber_){
		if(last==NULL){
			head=new GameList();
			head->gamenumber=gamenumber_;
			last=head;
		}else{
			last->next=new GameList();
			last=last->next;
			last->gamenumber=gamenumber_;
		}
	};

	GameList* GetFirst(){
		if(head!=NULL){
			current=head;
			return current;
		}
		return NULL;
	};
	GameList* GetNext(){
		if(current!=NULL){
			current=current->next;
			return current;
		}
		return NULL;
	};

	int GetCurrentGame(){
		if(current!=NULL){
			return current->gamenumber;
		}
		return 0;
	}

	void DupeList(GameList *other){
		if(other->GetFirst()!=NULL){
			do{
				AddGame(other->GetCurrentGame());
			}while(other->GetNext());
		}
	}

	void Write(string *buf){
		char temp[50];
		buf->erase();
		if(GetFirst()!=NULL){
			do{
				(*buf)+=" ";
				(*buf)+=itoa(GetCurrentGame(),temp,10);
			}while(GetNext());
		}
	};

			
};

class Player {
	SimplePlayer* name;
	string model;
	int team;  //0=none,1=red,2=blue

	int items[ITEM_LAST];
	int kills[MAX_KILLTYPE];
	int deaths[MAX_KILLTYPE];
	int suicides[MAX_KILLTYPE];

	int games;
	int time_sec;
	int msgs;
	int msgs_team;
	int items_total;

	float eff;

	int hc;
	int skill;
	int ping;
	int pinggames;	//number of games for which we have the ping
	int score;

	int FFAGames;
	int TDMGames;
	int OneV1Games;
	int CTFGames;

	int winsFFA;
	int placedFFA;
	int winsTDM;
	int wins1v1;
	int loss1v1;
	int winsCTF;

	GameList gamelist;
	PlayerKillList pkill;
	PlayerKillList pdeath;

	int startrank;
	int rank;
	int custstartrank;
	int custrank;

	bool modified;

public:
	Player(){
		init();
	};
	Player(char* name_) {
		init();
		name=new SimplePlayer(name_);
		
	};
	Player(char* name_,char* model_){
		init();
		name=new SimplePlayer(name_);
		model=model_;
	};
	Player(char* name_,char* model_,int team_){
		init();
		name=new SimplePlayer(name_);
		model=model_;
		team=team_;
	};
	Player(Section *sec);

	~Player(){
		if(name)
			delete name;
	};

	void init(){
		name=NULL;
		model.erase();
		team=TEAM_SPECTATOR;
		msgs=0;
		msgs_team=0;
		games=0;
		time_sec=0;
		items_total=0;
		hc=0;
		skill=0;
		ping=0;
		pinggames=0;
		score=0;

		FFAGames=0;
		TDMGames=0;
		OneV1Games=0;
		CTFGames=0;

		winsFFA=0;
		placedFFA=0;
		winsTDM=0;
		wins1v1=0;
		winsCTF=0;
		loss1v1=0;

		rank=-1;
		startrank=-1;
		custrank=-1;
		custstartrank=-1;

		int i;
		for(i=0;i<ITEM_LAST;i++)
			items[i]=0;
		for(i=0;i<MAX_KILLTYPE;i++){
			kills[i]=0;
			suicides[i]=0;
			deaths[i]=0;
		}
		modified=false;
	};

	void SetInfo(char* name_, char* model_, int team_, int hc_, int skill_,int w1v1, int l1v1, int timesec, Stats *st);

	void AddItem(int item_){items[item_]++;items_total++;};
	void AddKill(char *player,int killtype){
		kills[killtype]++;
		kills[0]++;
		pkill.AddKill(player,killtype);
	};
	int GetKill(int killtype){return kills[killtype];};

	char* GetName(){return name->GetName();};
	void Write(string *str, int vert=0){name->Write(str,vert);};
	string GetModel(){return model;};
	int GetTeam(){return team;};


	void AddDeath(char *player,int killtype){
		deaths[killtype]++;
		deaths[0]++;
		pdeath.AddKill(player,killtype);
	};
	int GetDeath(int killtype){return GetAggregateKills(deaths, killtype);};
	void AddSuicide(int killtype){
		char *n,na[2];
		suicides[killtype]++;
		suicides[0]++;
		if(name!=NULL){
			n=name->GetName();
		}else{
			strcpy(na,"\0");
			n=na;
		}
		pdeath.AddKill(n,killtype);
		pkill.AddKill(n,killtype);
	};
	int GetSuicide(int killtype){return GetAggregateKills(suicides, killtype);};
	int GetFrags(int killtype){return (GetAggregateKills(kills, killtype) - GetAggregateKills(suicides, killtype));};
	int GetKillDiff(int killtype){return (GetAggregateKills(kills, killtype) - GetAggregateKills(deaths, killtype));};
	void GetEff(int killtype,char *buf){
		if(GetAggregateKills(kills, killtype)<=0)
			strcpy(buf,"0.00");
		else
			sprintf(buf,"%.2f",(((float)GetAggregateKills(kills, killtype)*100.0)/((float)GetAggregateKills(kills, killtype) + GetAggregateKills(deaths, killtype) + GetAggregateKills(suicides, killtype))) );
	};
	int GetEffInt(int killtype);
	int GetWepSkillInt(int killtype);
	void GetWepSkill(int killtype,char *buf){
		sprintf(buf,"%.2f",((float)GetAggregateKills(kills, killtype))/((float)(GetAggregateKills(deaths, killtype)+GetAggregateKills(suicides, killtype)+1)) );
	};

	void AddMsg(){msgs++;};
	void AddMsgTeam(){msgs_team++;};
	int GetMsgs(){return msgs;};
	int GetMsgsTeam(){return msgs_team;};

	void AddStats(Player *gameplay);
	void CopyStats(Player *gameplay);

//	void WriteStats(string *str, bool game);

	void AddTime(int sec){
		modified=true;
		if(team!=TEAM_SPECTATOR)		//don't change time if in spectator mode
			time_sec+=sec;
	};
	void PlayerLeave(int sec){
		modified=true;
		if(team!=TEAM_SPECTATOR)		//don't change time if in spectator mode
			time_sec+=sec;
		team=TEAM_SPECTATOR;
	};

	void GetModelImgName(char *buf);

	double GetSortVal(int type);

	void GetTimeStr(char *buf);

	void AddGame(){games++;};
	int GetGames(){return games;};
	int GetItemsTotal(){return items_total;};

	int GetTime(){return time_sec;};

	void SetScore(int score_, int ping_){
		ping+=ping_;
		pinggames++;
		score+=score_;
	};
	int GetScore(){return score;};
	int GetAvePing(){
		return (pinggames>0)?ping/pinggames:999;
	};
	int GetAveHc(){return (games>0)?hc/games:0;};
	int GetAveSkill(){return (games>0)?(skill*100)/games:0;};

	void AddGameOfType(int type){
		switch(type){
		case GAMETYPE_1V1: OneV1Games++; return;
		case GAMETYPE_TDM: TDMGames++; return;
		case GAMETYPE_CTF: CTFGames++; return;
		default:			FFAGames++; return;
		}
	};
	int GetGamesOfType(int type){
		switch(type){
		case GAMETYPE_1V1: return OneV1Games; 
		case GAMETYPE_TDM: return TDMGames;
		case GAMETYPE_CTF: return CTFGames;
		case GAMETYPE_FFA: return FFAGames;
		default:			return games;
		}
	};

	void AddWin(int type){
		switch(type){
		case GAMETYPE_1V1: /*wins1v1++;*/ return;
		case GAMETYPE_TDM: winsTDM++; return;
		case GAMETYPE_CTF: winsCTF++; return;
		default:			winsFFA++; return;
		}
	};
	int GetWins(int type){
		switch(type){
		case GAMETYPE_1V1: return wins1v1; 
		case GAMETYPE_TDM: return winsTDM;
		case GAMETYPE_CTF: return winsCTF;
		case GAMETYPE_FFA: return winsFFA;
		default:			return winsFFA+winsCTF+winsTDM+wins1v1;
		}
	};
	int GetLosses(int type){
		switch(type){
		case GAMETYPE_1V1: return loss1v1;
		case GAMETYPE_TDM: return TDMGames-winsTDM;
		case GAMETYPE_CTF: return CTFGames-winsCTF;
		case GAMETYPE_FFA: return FFAGames-winsFFA;
		default:			return (TDMGames+CTFGames+FFAGames)-(winsFFA+winsCTF+winsTDM)+loss1v1;
		}
	};
	int GetMatches(int type){
		switch(type){
		case GAMETYPE_1V1: return wins1v1+loss1v1; 
		case GAMETYPE_TDM: return TDMGames;
		case GAMETYPE_CTF: return CTFGames;
		case GAMETYPE_FFA: return FFAGames;
		default:			return games;
		}
	};
	void AddFFAPlace(){ placedFFA++;};
	int GetFFAPlaces(){ return placedFFA++;};
		

	bool isGood();//does player meet requirements of appearing in the stats

	void GetFileName(string *buf);

	void GetVariableValue(string *variable, string *value);
	void GetIntValue(string *variable,double ratio,string *value);

	void AddGameNum(int game){gamelist.AddGame(game);};
	GameList* GetGameList(){return &gamelist;};
	PlayerKillList *GetKillList(){return &pkill;};
	PlayerKillList *GetDeathList(){return &pdeath;};

	int GetRank(){return rank;};
	void AddRank(int rankdelta){rank+=rankdelta;};
	int GetCustRank(){return custrank;};
	void AddCustRank(int rankdelta){custrank+=rankdelta;};

	bool GetModified(){return modified;};
	void WriteToDb(ConfigFile *cf);
};




class PlayerListNode {
	
	 Player *data;
	 PlayerListNode *next;

	 string name;
	 
public:
	PlayerListNode(Player *play,char* name_){
		name=name_;
		data=play;
		next=NULL;
	};

	~PlayerListNode(){
		delete next;
		delete data;
	};

	Player* GetPlayer(){ return data;};

	PlayerListNode* GetNext(){return next;};
	void SetNext(PlayerListNode* next_){next=next_;};

	string GetName(){return name;};

	
};



class PList {
	PlayerListNode *head;

	PlayerListNode *iter;
	int count;

public:
	PList(){
		head=NULL;
		iter=NULL;
		count=0;
	};
	~PList(){
		if(head!=NULL)
			delete head;
	};

	void AddPlayer(Player *play,char* name_);

	Player* GetFirst(){
		iter=head;
		if(iter!=NULL){
			return iter->GetPlayer();
		}
		return NULL;
	};

	Player* GetNext(){
		if(iter->GetNext()!=NULL){
			iter=iter->GetNext();
			return iter->GetPlayer();
		}
		iter=NULL;
		return NULL;
	};

	Player* FindPlayer(char* name);
	bool ContainsPlayer(Player *p);

	int GetCount(){return count;};
	int GetPos(char *name);
	int GetPos(Player* Play);

	bool DupeList(PList *list);

	void ReSortList(string *type, Stats *stats);

	void TrimPlayers();
};



#endif  //PLAYER_H_INCLUDED
