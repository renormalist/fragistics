/* crosstable.h
 Fragistics  -  Game Statistics program for Quake 3 Arena
 Copyright (C) 2000   Brian Risinger

see crosstable.cpp or main.cpp for license info
*/


#ifndef CROSSTABLE_INCLUDED
#define CROSSTABLE_INCLUDED

#include <string>
#include "player.h"
#include "settings.h"

extern Settings settings;



/* Implements a table that displays killer / killee info

 
*/


class SimpleKillNode {
	 SimpleKillNode *next;

	 Player *killer;
	 Player *killee;
	 
public:
	SimpleKillNode(Player* killer_,Player* killee_){
		killer=killer_;
		killee=killee_;
		next=NULL;
	};

	~SimpleKillNode(){
//		if(next!=NULL)		//this was overflowing stack - see SimpleKillList deconstructor
//			delete next;
	};

	Player* GetKiller(){ return killer;};
	Player* GetKillee(){ return killee;};

	SimpleKillNode* GetNext(){return next;};
	void SetNext(SimpleKillNode* next_){next=next_;};
};



class SimpleKillList {
	SimpleKillNode *head;
	SimpleKillNode *iter;

public:
	SimpleKillList(){
		head=NULL;
		iter=NULL;
	};
	~SimpleKillList(){
		//was if head != NULL  delete head;  cause satck overflow on large lists
		while(head!=NULL){
			iter=head->GetNext();
			delete head;
			head=iter;
		}
	};

	void AddKill(Player* killer_,Player* killee_){
		SimpleKillNode *skn = new SimpleKillNode(killer_,killee_);
		skn->SetNext(head);
		head=skn;
	}


	SimpleKillNode* GetFirst(){
		iter=head;
		return iter;
	};

	SimpleKillNode* GetNext(){
		if(iter->GetNext()!=NULL){
			iter=iter->GetNext();
			return iter;
		}
		iter=NULL;
		return NULL;
	};

	Player* GetKiller(){
		if(iter!=NULL){
			return iter->GetKiller();
		}
		return NULL;
	};
	Player* GetKillee(){
		if(iter!=NULL){
			return iter->GetKillee();
		}
		return NULL;
	};

	bool empty(){ return(head==NULL)?true:false;};
};


class SimplePlayerListNode {
	//points to players in another list
	
	 Player *data;
	 SimplePlayerListNode *next;

	 string name;
	 
public:
	SimplePlayerListNode(Player *play,char* name_){
		name=name_;
		data=play;
		next=NULL;
	};

	~SimplePlayerListNode(){
		delete next;
	};

	Player* GetPlayer(){ return data;};

	SimplePlayerListNode* GetNext(){return next;};
	void SetNext(SimplePlayerListNode* next_){next=next_;};

	string GetName(){return name;};
	
};

class SimplePList {
	SimplePlayerListNode *head;

	SimplePlayerListNode *iter;
	int count;

public:
	SimplePList(){
		head=NULL;
		iter=NULL;
		count=0;
	};
	~SimplePList(){
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

	bool DupeList(SimplePList *list);

	//void ReSortList(int type);
};



class CrossTable{
private:
	SimplePList players;
	SimpleKillList kl;

	int topscore;
	int hilitelevels;
	int *hilitevalues;
	char **hilitecolors;
	int minhilite;


	

public:
	CrossTable(){
		hilitelevels= settings.GetKilltableHilitelevels();
		hilitevalues= new int[hilitelevels+1];
		hilitecolors= new char*[hilitelevels+1];

		string temp;
		for(int i=0;i<hilitelevels;i++){
			hilitevalues[i] = settings.GetKilltableHilightPercent(i);
			settings.GetKilltableHilightColor(i,&temp);
			hilitecolors[i] = new char[temp.length()+1];
			strcpy(hilitecolors[i],temp.c_str());
		}
		
		minhilite=settings.GetKilltableMinhilite();
			
/*			
		hilitelevels= 4;
		hilitevalues= new int[5];
		hilitecolors= new char*[5];

		hilitevalues[0] = 100;
		hilitecolors[0] = "#00ff00";

		hilitevalues[1] = 90;
		hilitecolors[1] = "#d0d000";

		hilitevalues[2] = 75;
		hilitecolors[2] = "#d07000";

		hilitevalues[3] = 50;
		hilitecolors[3] = "#f08080";

		minhilite=2;
*/
		
	};
	~CrossTable(){
		for(int i=0;i<hilitelevels;i++)
			delete [] hilitecolors[i];
		delete [] hilitevalues;
		delete [] hilitecolors;

	};

	bool AddPlayer(Player* p);
	bool AddValue(Player* killer,Player* killee);

	void Write(string* str);
	void WriteRowCompress(string *str);

	bool empty(){ return kl.empty();};

	void ColorScore(int score, string *str);
	
};



void AddKillToTables(CrossTable *killarray,int type, Player *killer,Player *killee);


#endif //CROSSTABLE_INCLUDED