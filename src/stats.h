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
    
    Stats( );
    ~Stats(){};
    
    //functions that the FileParser calls
    //callback to add game to stats
    void AddGameToTotal (Game *game,int lastline=0);
    int GetGames ();
    
    
    //functions that the HtmlTemplate calls
    void GetVariableValue (string *variable, string *value);
    bool SetupLoop (int looptype,string *sort=NULL);
    bool NextLoop (int looptype);
    
    //functions called by the control program
    //function to call when all files have been parsed
    bool WriteMainStats ();
    
    //to store curent file name for database
    void SetCurrentFile (const char *file,int line);
    
    //math function - called by htmltemplate and for player sort
    double DoMath (string *expression);
    
    //used by player sort function
    void SetCurrentPlayer (Player *p);
    
    //used by player.setinfo
    int GetPlayerRank (char *name);
    
    //Used by custom ranking scheme code;
    void SetKillerKillee (Player* killer_, Player *killee_, int kill);
    
    int GetCustRank (char *name);
    int DoCustMath ();
    
    //database functions
    void AddPlayer (Player *player);
    
    void AddMap (Map *map);
    MapList *GetMaps ();
    PList *GetPlayers ();
    int GetFileNameLine (string *file);
    void SetGames (int g);
    void GetGamelist (string *s);
    void SetGamelist (string *s);
    
};

#endif //STATS_H_INCLUDED
