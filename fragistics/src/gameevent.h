/* gameevent.h
   Fragistics  -  Game Statistics program for Quake 3 Arena
   Copyright (C) 2000 Brian Risinger
   Copyright (c) 2002 Steffen Schwigon
   
   see main.cpp for license info
*/

#ifndef GAMEEVENT_H_INCLUDED
#define GAMEEVENT_H_INCLUDED

#include <string>

class GameEvent {
public:
    int  event;     // type of event
    int  time_min;  // minutes from start of game of event
    int  time_sec;  // seconds after minute of event  0 - 60
    int  realtime;  // time_t type, time of event
                    // new game, player info, kill, item, limit, score, teamscore, say / say team
    int  player;    // player, killer, pickuper, player, red score
    int  other;     // skill(bot), killee, item, score, blue score
    int  how;       // game type, hc, weapon, 1=time 2=frag, ping
    char name[50];  // map name, player name
    char model[50]; // model
    int  team;      // capture limit, team
    string msg;     // init str, info str, kill msg, item name, message
    int  wins;      // frag limit, 1v1 wins
    int  losses;    // time limit, 1v1 losses
    
    GameEvent();
    ~GameEvent();
    void print ();
};

#endif //GAMEEVENT_H_INCLUDED
