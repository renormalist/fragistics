/* database.h
 Fragistics  -  Game Statistics program for Quake 3 Arena
 Copyright (C) 2000   Brian Risinger

see database.cpp or main.cpp for license info
*/

#include "stats.h"
#include "settings.h"

extern OS os;


class database{
	
	Stats *st;
	int startline;

	int nummaps;


	int readplayers();
	int writeplayers();
	
	int readmaps();
	int	writemaps();

	int readgamelist();
	int writegamelist();

	

public:

	database( Stats *s){
		st=s;
		startline=0;
	}
	~database(){
	}

	int read(){ // reads the database
		int result=0;
		result= readplayers();
		if(result==0){
			result= readmaps();
			if(result==0){
				result= readgamelist();
			}
		}
		return result;
	};
	int write(){ // reads the database
		int result=0;
		result= writeplayers();
		if(result==0){
			result= writemaps();
			if(result==0){
				result= writegamelist();
			}
		}
		return result;
	};

	int GetStartLine(){return startline;};

};