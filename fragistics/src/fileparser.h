/* fileparser.h
 Fragistics  -  Game Statistics program for Quake 3 Arena
 Copyright (C) 2000   Brian Risinger

see fileparser.cpp or main.cpp for license info
*/



#ifndef FILEPARSER_H_INCLUDED
#define FILEPARSER_H_INCLUDED

#include <string>
#include "stats.h"

using std::string;

class FileParser {
protected:
	string filename;
	Stats *stats;

public:
	FileParser(string *Filename,Stats *st);
	~FileParser();

    //returns true if parsing was successful /
    //should call stats->AddGameToTotal(game,lastline) after parsing each game
    //int the future, we will store cumulative stats in some sort of
    //DB and continue parsing from where we left off in the file,
    //instead of reparsing the whole file, that is why there is the
    //startline param and the last line param in AddGameToTotal()
    virtual bool Parse (int startline=0) = 0;  
};

#endif //FILEPARSER_H_INCLUDED
