/* stats.h
 Fragistics  -  Game Statistics program for Quake 3 Arena
 Copyright (C) 2000   Brian Risinger

see stats.cpp or main.cpp for license info
*/



#ifndef STATS_H_INCLUDED
#define STATS_H_INCLUDED

#include <stdio.h>
#include <string>
#include "defines.h"
#include "os.h"
#include "game.h"
#include "player.h"
#include "crosstable.h"

using std::string;








class Stats {	

//public: //was public for tests, now private
private:
	int games;
	int players;
//	int kills;
	int items;
	int msgs;
	int msgs_team;
	int time_sec;

	string filename;
	int last_parsed_line;

	int all_kills[MAX_KILLTYPE];
	int all_items[ITEM_LAST];

	CrossTable killtypes[CROSSTABLE_MAX_KILL];

	PList player_list;
	PList limit_player_list;
	MapList maps;

	string gamelisthtml;

	Game *currentgame;

	Player *killer;
	Player *killee;
	int killtype;
	string cust_math;

	Player *currentplayer;
	PList  *currentploop;

	GameList *pgamelist;
	PlayerKillList *pklist;
	Map *currentmap;

	

public:

	Stats( ){
		games=0;
		players=0;
//		kills=0;
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

		int i;
		for(i=0;i<MAX_KILLTYPE;i++){
			all_kills[i]=0;
		}
		for(i=0;i<ITEM_LAST;i++)
			all_items[i]=0;

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

/*
		gamelisthtml="<html><head><title>Games Played\n</title></head>\n";
		//gamelisthtml+="<body bgcolor=\"#808080\" link=\"#0000ff\" vlink=\"#ff0000\" alink=\"00ff00\">\n";
		gamelisthtml+="<body bgcolor=\"#000000\" text=\"999999\" link=\"#0000ff\" vlink=\"#ff0000\" alink=\"00ff00\">\n";
*/
/*		gamelisthtml="<CENTER>\n<h2>Games Played</h2>\n\
						<a href=\"stats.html\">Overall Stats</a> <a href=\"players.html\">Player stats</a>\n\
						<TABLE border=1>\n\
						<TR><TD>Game</TD><TD>Map</TD><TD>Players</TD><TD>Kills</TD><TD>Time</TD></TR>\n";
*/	
	};
	~Stats(){};

	//functions that the FileParser call
	void AddGameToTotal(Game *game,int lastline=0);	//callback to add game to stats
	int GetGames(){return games;};


	//functions that the HtmlTemplate calls
	void GetVariableValue(string *variable, string *value);
	bool SetupLoop(int looptype,string *sort=NULL);
	bool NextLoop(int looptype);

	//functions called by the control program
	//function to call when all files have been parsed
	bool WriteMainStats();
	//to store curent file name for database
	void SetCurrentFile(const char *file,int line){
		filename=file;
		last_parsed_line=line;
	};

	//math function - called by htmltemplate and for player sort
	double DoMath(string *expression);

	//used by player sort function
	void SetCurrentPlayer(Player *p){currentplayer=p;};

	//used by player.setinfo
	int GetPlayerRank(char *name){
		Player *p;
		p=player_list.FindPlayer(name);
		if(p!=NULL)
			return p->GetRank();
		return 10000;
	};


	//Used by custom ranking scheme code;
	void SetKillerKillee(Player* killer_, Player *killee_, int kill){
		killer=killer_;
		killee=killee_;
		killtype=kill;
	};
	int GetCustRank(char *name){
		Player *p;
		p=player_list.FindPlayer(name);
		if(p!=NULL)
			return p->GetCustRank();
		return settings.GetCustRankStart();
	};
	int DoCustMath();

	//database functions
	void AddPlayer(Player *player);
	void AddMap(Map *map);
	MapList *GetMaps(){return &maps;};
	PList *GetPlayers(){return &player_list;};
	int GetFileNameLine(string *file){
		(*file)=filename;
		return last_parsed_line;
	};
	void SetGames(int g){games=g;};
	void GetGamelist(string *s){(*s)=gamelisthtml;};
	void SetGamelist(string *s){gamelisthtml=*s;};

	
};



class FileParser {
protected:
	string filename;
	Stats *stats;

public:
	FileParser(string *Filename,Stats *st){
		filename=(*Filename);
		stats=st;
	}
	~FileParser(){};

	virtual bool Parse(int startline=0)=0;  //returns true if parsing was successful /
	//should call stats->AddGameToTotal(game,lastline) after parsing each game
	//int the future, we will store cumulative stats in some sort of DB and continue
	//parsing from where we left off in the file, instead of reparsing the whole file,
	//that is why there is the startline param and the last line param in AddGameToTotal()


};

class GameslogParser : public FileParser {

public:
	GameslogParser(string *Filename,Stats *st):FileParser(Filename,st){};
	~GameslogParser();

	virtual bool Parse(int startline=0);

protected:
	GameEvent* ParseLine(char *line);


};




#endif //STATS_H_INCLUDED