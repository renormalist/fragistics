/* settings.h
 Fragistics  -  Game Statistics program for Quake 3 Arena
 Copyright (C) 2000   Brian Risinger

see settings.cpp or main.cpp for license info
*/

#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED

#include <stdio.h>
#include <string>
#include "defines.h"
#include "os.h"

extern OS os;

using namespace std;


//utility classes for config file

class StringPair {
private:
	string name;
	string value;
	StringPair *next;
public:
	StringPair(char* name_, char* value_){
		name=name_;
		value=value_;
		next=NULL;
	};
	~StringPair(){
		if(next){
			delete next;
		}
	};

	string GetName(){return name;};
	string GetValue(){return value;};

	void SetValue(const char *value_){
		value.erase();
		value=value_;
	};

	void SetNext(StringPair *next_){
		next=next_;
	};
	StringPair* GetNext(){return next;};

};

class StringPairList {
private:
	StringPair *head;
	StringPair *last;
	//StringPair *ptr;  //was to be used for getfirst / get next functions
public:
	StringPairList(){
		head=NULL;
		//ptr=head;
		last=head;
	};
	~StringPairList(){
		if(head)
			delete head;
	};

	StringPair* AddPair(char *name_, char *value_){
		StringPair *sp = new StringPair(name_,value_);
		if(head==NULL){
			head=sp;
			last=sp;
		}else{
			last->SetNext(sp);
			last=sp;
		}
		return sp;
	};
	bool GetValue(const char *name, string *buf){
		StringPair *temp = head;
		(*buf)="";
		if(temp!=NULL){
			do{
				if(!strcmp(name,temp->GetName().c_str())){
					(*buf)=temp->GetValue();
					return true;
				}
				temp=temp->GetNext();
			}while(temp!=NULL);
		}
		return false;
	};

	StringPair* GetPair(const char*name){
		StringPair *temp = head;
		if(temp!=NULL){
			do{
				if(!strcmp(name,temp->GetName().c_str())){
					return temp;
				}
				temp=temp->GetNext();
			}while(temp!=NULL);
		}
		return NULL;
	};

	StringPair* GetFirst(){return head;};

};


class Line {
private:
	string line;
	StringPair *pair;
	Line *next;
public:

	Line(const char* line_){
		line=line_;
		next=NULL;
		pair=NULL;
	};
	~Line(){
		if(next!=NULL){
			delete next;
		}
	};

	string GetLine(){return line;};
	StringPair* GetPair(){return pair;};
	void SetPair(StringPair *pair_){pair=pair_;};

	void SetNext(Line *line_){next=line_;};
	Line* GetNext(){return next;};

};

class LineList {
private:
	Line* head;
	Line* last;
	Line* ptr;
public:
	LineList(){
		head=NULL;
		last=head;
		ptr=NULL;
	};
	~LineList(){
		if(head)
			delete head;
	};

	Line* AddLine(const char *line_){
		if(head==NULL){
			head=new Line(line_);
			last=head;
		}else{
			last->SetNext(new Line(line_));
			last=last->GetNext();
		}
		return last;
	};

	Line* GetFirst(){
		ptr=head;
		return ptr;
	};
	Line* GetNext(){
		if(ptr!=NULL){
			return ptr=ptr->GetNext();
		}else{
			return NULL;
		}
	};

};



class Section {
private:
	Section *next;
	Section *ptr;

	string name;
	LineList lines;
	StringPairList values;
public:
	Section(){
		next=NULL;
		ptr=NULL;
		name="";
	};
	Section(const char *name_){
		next=NULL;
		ptr=NULL;
		name=name_;
	};
	~Section(){
		if(next)
			delete next;
	}

	Section* GetNext(){return next;};
	void SetNext(Section *next_){next=next_;};

	string GetName(){return name;};
//	LineList *GetLines(){return &lines;};	//not needed
	bool GetValue(const char* name_,string *buf_){
		return values.GetValue(name_,buf_);
	};

	StringPair* GetStrings(){return values.GetFirst();};

	Line* AddLine(const char *line_){
		Line *l;
		l=NULL;
		char *ln = new char[strlen(line_)+2];
		strcpy(ln,line_);
		if(ln[0]!=';' && ln[0]!='/'){
			//not a comment
			string name, value;
			char *tmp;
			StringPair *sp=NULL;
			
			tmp=strchr(ln,'=');
			if(tmp!=NULL){
				//name = value pair
				*tmp='\0';
				name=os.TrimStr(ln);
				value=os.TrimStr(tmp+1);
				//see if pair already exists
				string temp;
				sp=values.GetPair(name.c_str());
				if(sp==NULL){
					//new pair
					l=lines.AddLine(line_);
					sp = values.AddPair((char*)name.c_str(),(char*)value.c_str());
					l->SetPair(sp);
				}else{
					//pair exists, just update pair - no new line
					sp->SetValue(value.c_str());
				}
			}else{
				l=lines.AddLine(line_);
				//blank line
			}
		}else{
			//comment
			l=lines.AddLine(ln);
		}
		return l;
	};

	void AddLineToSection(const char *line_,const char *section,Section **sec=NULL){
		Section *s=this;
		do{
			if(!strcmp(section,s->name.c_str())){
				s->AddLine(line_);
				if(sec!=NULL) (*sec)=s;
				return;
			}
		}while(s->GetNext() && (s=s->GetNext()));
		Section *n = new Section(section);
		s->SetNext(n);
		n->AddLine(line_);
		if(sec!=NULL) (*sec)=n;
	};

	int FindValue(const char *section_,const char *name_, string *value_,Section **sec=NULL){
		//returns 2 if found   1 if section found but not value   and 0 if section not found
		(*value_).erase();
		Section *s=this;
		do{
			if(!strcmp(section_,s->name.c_str())){
				if(sec!=NULL) (*sec)=s;
				return s->GetValue(name_,value_)+1;
			}
		}while(s->GetNext() && (s=s->GetNext()));
		return 0;
	};

	void Write(string *buf){
		(*buf).erase();
		if(name.length() >1){
			(*buf)="[";
			(*buf)+=name;
			(*buf)+="]\n";
		}
		Line *l;
		l=lines.GetFirst();
		if(l!=NULL){
			do{
				StringPair *sp;
				sp=l->GetPair();
				if(sp!=NULL){
					(*buf)+=sp->GetName();
					(*buf)+="=";
					(*buf)+=sp->GetValue();
					(*buf)+="\n";
				}else{
					(*buf)+=l->GetLine();
					//(*buf)+="\n";
				}
			}while((l=lines.GetNext())!=NULL);
		}
		if((*buf)[(*buf).length()-1]!='\n'){
			(*buf)+="\n";
		}
	};
		
};



///////////////////////////////////////////////////////////////////////////////

class ConfigFile {
	//implements a rewritable config file for settings
	//config file has sections '[game]'
	// and name / value pairs in the sections 'name = value'
	// comments start a line with ; or /
	//program can add settings to the file and resave it.
	//writing out the file preserves comments in the file
private:
	Section head;
	string filename;
	bool modified;
	
	Section *lastaccess;	//used to speed up adds and lookups;

public:
	ConfigFile(const char *name, bool newfile=false);
	~ConfigFile(){
		if(modified)
			Write();
	}

	int GetValue(const char *section,const char *name, string *val){
		if(lastaccess!=NULL && !strcmp(lastaccess->GetName().c_str(),section)){
			return lastaccess->GetValue(name,val)+1;
		}
		return head.FindValue(section,name,val,&lastaccess);
	};

	int AddValue(const char *section, const char *name, const char *value){
		modified=true;
		string tmp;
		tmp=name;
		tmp+="=";
		tmp+=value;
		if(lastaccess!=NULL && !strcmp(lastaccess->GetName().c_str(),section)){
			lastaccess->AddLine(tmp.c_str());
			return true;
		}
		head.AddLineToSection((char*)tmp.c_str(),section,&lastaccess);
		return true;
	};


	Section* GetSettings(){return &head;};


	int Write(const char *name);
	int Write();		//overwrites previous file


};


///////////////////////////////////////////////////////////////////////////////

class Settings {
	//holds all settings for program
	//parses a config file and commandline params
	//commandline params override config file settings
	//commandline settings should look like:
	// \section/name value 


private:
	static const char initChar;  // '\\'

	ConfigFile	*cfgfile;
	int			argc;
	char		**argv;

	//html settings
	string		text_color;
	string		link_color;
	string		vlink_color;
	string		alink_color;
	string		bg_color;
	string		bg_image;
	string		namecolors[10];

	//kill table settings
	int			hilite_levels;
	int			hilite_percents[KILLTABLE_MAXHILITE];
	string		hilite_colors[KILLTABLE_MAXHILITE];
	int			hilite_minhilite;
	string		killtable_column[3];

	//game settings
	int			game_minplayers;
	int			game_mintime;
	int			game_minkills;
	int			game_complete;
	int			game_placed;

	//main settings
	int			main_overall_killtable;
	int			players_min_time;
	int			players_min_games;
	int			players_min_kills;
	string		time_format;
	int			use_cust_rank;
	int			cust_rank_start;
	string		cust_rank_template;
	string		dbpath;
	string		srcpath;
	string		destpath;
	int			use_db;

	//LOG settings
	int			logs_number;
	string		*logs_paths;
	int			*logs_type;

	//template settings
	int			main_number;
	string		*main_templates;
	int			game_number;
	string		*game_templates;
	string		lastgame_template;
	string		gamelist_template;
	string		gamelistloop_template;
	string		player_template;
	int			map_number;
	string		*map_templates;
	

public:
	Settings(){
		cfgfile=NULL;
		argc=0;
		argv=NULL;

		hilite_levels=0;
		for(int i=0;i<KILLTABLE_MAXHILITE;i++)
			hilite_percents[i]=101;
		logs_number=0;

	};
	~Settings(){
		if(cfgfile){
			delete cfgfile;
			delete [] logs_paths;
			delete [] main_templates;
			delete [] game_templates;
		}
	};

	int ParseSettings(char *filename, int argc_, char **argv_);

	bool FindSetting(string *value, const char* section,const char* name, const char* default_);

	int GetKilltableHilitelevels(){return hilite_levels;};
	int GetKilltableHilightPercent(int num){
		if(num>=0 &&num<KILLTABLE_MAXHILITE)
			return hilite_percents[num];
		return 101;
	}
	void GetKilltableHilightColor(int num,string *buf){
		buf->erase();
		if(num>=0 &&num<KILLTABLE_MAXHILITE)
			(*buf)=hilite_colors[num];
	};
	int GetKilltableMinhilite(){return hilite_minhilite;};
	char *GetKilltableColumn(int i){
		if(i<3){
			return (char*)killtable_column[i].c_str();
		}
		return (char*)killtable_column[0].c_str();
	};

	int GetGameMinplayers(){return game_minplayers;};
	int GetGameMintime(){return game_mintime;};
	int GetGameMinkills(){return game_minkills;};
	int GetGameComplete(){return game_complete;};
	int GetGamePlaced(){ return game_placed;};

	void GetNameColor(int num,string *buf){
		buf->erase();
		if(num>=0 &&num<10)
			(*buf)=namecolors[num];
	};
	void GetColorLink(string *buf){
		buf->erase();
		(*buf)=link_color;
	};
	void GetColorVlink(string *buf){
		buf->erase();
		(*buf)=vlink_color;
	};
	void GetColorAlink(string *buf){
		buf->erase();
		(*buf)=alink_color;
	};
	void GetColorText(string *buf){
		buf->erase();
		(*buf)=text_color;
	};
	void GetColorBg(string *buf){
		buf->erase();
		(*buf)=bg_color;
	};
	void GetBgImage(string *buf){
		buf->erase();
		(*buf)=bg_image;
	};
	
	void GetTemplateSrcpath(string *buf){
		buf->erase();
		(*buf)=srcpath;
	};
	void GetTemplateDestpath(string *buf){
		buf->erase();
		(*buf)=destpath;
	};
	void GetDatabasepath(string *buf){
		buf->erase();
		(*buf)=dbpath;
	};

	int GetMainKilltables(){return main_overall_killtable;};
	int GetMainPlayersMinTime(){return players_min_time;};
	int GetMainPlayersMinGames(){return players_min_games;};
	int GetMainPlayersMinKills(){return players_min_kills;};
	void GetMainTimeFormat(string *buf){
		buf->erase();
		(*buf)=time_format;
	};
	int GetUseDB(){return use_db;};

	int GetLogsNumber(){return logs_number;};
	void GetLogsPath(int num, string *path){
		path->erase();
		if(num>=0 &&num<logs_number){
			(*path)=logs_paths[num];
		}
	};
	int GetLogsType(int num){
		if(num>=0 &&num<logs_number){
			return logs_type[num];
		}
		return 0;
	}
	void SetLogsType(int num, int type){
		if(num>=0 &&num<logs_number){
			char temp[20],temp2[20];
			sprintf(temp,"logtype%d",num);
			cfgfile->AddValue("LOGS",temp, itoa(type,temp2,10));
		}
	}


	int GetTemplatesMainNumber(){return main_number;};
	void GetTemplatesMainPath(int num, string *path){
		path->erase();
		if(num>=0 &&num<main_number){
			(*path)=main_templates[num];
		}
	};
	int GetTemplatesGameNumber(){return game_number;};
	void GetTemplatesGamePath(int num, string *path){
		path->erase();
		if(num>=0 &&num<game_number){
			(*path)=game_templates[num];
		}
	};
	void GetTemplatesLastGame(string *path){(*path)=lastgame_template;};
	void GetTemplatesGamelist(string *path){(*path)=gamelist_template;};
	void GetTemplatesGamelistloop(string *path){(*path)=gamelistloop_template;};
	void GetTemplatesPlayer(string *path){(*path)=player_template;};
	int GetTemplatesMapNumber(){return map_number;};
	void GetTemplatesMapPath(int num, string *path){
		path->erase();
		if(num>=0 &&num<map_number){
			(*path)=map_templates[num];
		}
	};
	

	int	GetUseCustRank(){return use_cust_rank;};
	int	GetCustRankStart(){return cust_rank_start;};
	void GetCustRankTemplate(string *val){(*val)=cust_rank_template;};
};







#endif //SETTINGS_H_INCLUDED