/* gameslogparser.h
   Fragistics  -  Game Statistics program for Quake 3 Arena
   Copyright (C) 2000   Brian Risinger
   
   see stats.cpp or main.cpp for license info
*/



#ifndef GAMESLOGPARSER_H_INCLUDED
#define GAMESLOGPARSER_H_INCLUDED

#include "fileparser.h"

using std::string;

class GameslogParser : public FileParser {
    
public:
    GameslogParser(string *Filename,Stats *st):FileParser(Filename,st){};
    ~GameslogParser();
    
    virtual bool Parse(int startline=0);
    
protected:
    GameEvent* ParseLine(char *line);
    
    
};

#endif //GAMESLOGPARSER_H_INCLUDED
