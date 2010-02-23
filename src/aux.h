/* aux.h
   Fragistics  -  Game Statistics program for Quake 3 Arena
   Copyright (C) 2000   Brian Risinger
   
   see main.cpp for license info
*/

#ifndef AUX_H_INCLUDED
#define AUX_H_INCLUDED

/**
   Auxiliary methods
*/

class Aux 
{
public:
    static const char *item2str (int item);
    static const char *event2str (int event);
    static const char *team2str (int team); 
};


#endif //AUX_H_INCLUDED
