/* settimgs.cpp
 Fragistics  -  Game Statistics program for Quake 3 Arena
 Copyright (C) 2000   Brian Risinger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Brian Risinger contact info:

troz_@xoommail.com		preferred
troz_@hotmail.com		also ok
risinger@hoflink.com	if unable to reach otherwise

98 Stone Lane
Levittown, NY  11756
USA

*/



#include "settings.h"

extern OS os;

ConfigFile::ConfigFile(const char *name, bool newfile){
	filename=name;
	lastaccess=NULL;

	if(!newfile){
		File *fp=os.GetFile(filename.c_str());

		char line[1024];
		string section;
		section="";
		if(fp->OpenRead()){
			while(fp->ReadLine(line,1023)!=NULL){
				if(line[0]=='['){
					//new section
					char *ptr=strchr(line,']');
					if(ptr!=NULL)
						*ptr='\0';
					section=(&line[1]);
				}else{
					//normal line
					head.AddLineToSection(line,(char*)section.c_str());
				}
			}
		}
		delete fp;
	};

}

int ConfigFile::Write(){
	//todo: make this safer by first deleting filename.old if it exists
	//		then renaming filname to filename.old
	//		and finally writing out filename
	return Write(filename.c_str());
}

int ConfigFile::Write(const char *name){
	
	Section *s;
	s=&head;
	bool first=true;

	File *fp=os.GetFile(name);

	if(fp->OpenWrite()){

		while(s!=NULL){
			string str;
			s->Write(&str);
			fp->WriteStr((char*)str.c_str());
			s=s->GetNext();
		}
		modified=false;
		delete fp;
		return true;
	}else{
		printf("\nFailed to save config file %s\n",name);
		delete fp;
		return false;
	}
}




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



int Settings::ParseSettings(char *filename, int argc_, char **argv_){
	int i;
	char numbuf[50];
	string temp,temp1;

	cfgfile = new ConfigFile(filename);
	argc=argc_;
	argv=argv_;


	//settings

	
	//HTML
	FindSetting(&text_color, "HTML","text_color", "#c0c0c0");
	FindSetting(&link_color, "HTML","link_color", "#0000ff");
	FindSetting(&vlink_color, "HTML","vlink_color", "#ff0000");
	FindSetting(&alink_color, "HTML","alink_color", "#00ff00");
	FindSetting(&text_color, "HTML","text_color", "#c0c0c0");
	FindSetting(&bg_color, "HTML","bg_color", "#000000");
	FindSetting(&bg_image, "HTML","bg_image", "");

	FindSetting(&namecolors[0], "HTML","name0", "black");
	FindSetting(&namecolors[1], "HTML","name1", "red");
	FindSetting(&namecolors[2], "HTML","name2", "green");
	FindSetting(&namecolors[3], "HTML","name3", "yellow");
	FindSetting(&namecolors[4], "HTML","name4", "blue");
	FindSetting(&namecolors[5], "HTML","name5", "cyan");
	FindSetting(&namecolors[6], "HTML","name6", "magenta");
	FindSetting(&namecolors[7], "HTML","name7", "white");
	FindSetting(&namecolors[8], "HTML","name8", "white");
	FindSetting(&namecolors[9], "HTML","name9", "white");

	
	//KILL TABLES
	if(FindSetting(&temp, "KILL TABLES","hilite_levels", "4")==false){
		temp="4";
		cfgfile->AddValue("KILL TABLES","hilite_level0", "100");
		cfgfile->AddValue("KILL TABLES","hilite_color0", "#00ff00");

		cfgfile->AddValue("KILL TABLES","hilite_level1", "90");
		cfgfile->AddValue("KILL TABLES","hilite_color1", "#d0d000");

		cfgfile->AddValue("KILL TABLES","hilite_level2", "75");
		cfgfile->AddValue("KILL TABLES","hilite_color2", "#d07000");

		cfgfile->AddValue("KILL TABLES","hilite_level3", "50");
		cfgfile->AddValue("KILL TABLES","hilite_color3", "#804020");
	}
	hilite_levels=atoi(temp.c_str());
	if(hilite_levels>=KILLTABLE_MAXHILITE){
		hilite_levels=KILLTABLE_MAXHILITE-1;
		itoa(hilite_levels,numbuf,10);
		cfgfile->AddValue("KILL TABLES","hilite_levels", numbuf);
	}
	//loop to get colors/percents
	for(i=0;i<hilite_levels;i++){
		itoa(i,numbuf,10);
		temp1="hilite_level";
		temp1+=numbuf;
		FindSetting(&temp, "KILL TABLES",temp1.c_str(), "101");
		hilite_percents[i]=atoi(temp.c_str());
		temp1="hilite_color";
		temp1+=numbuf;
		FindSetting(&hilite_colors[i], "KILL TABLES",temp1.c_str(), "white");
	}
	FindSetting(&temp, "KILL TABLES","hilite_minhilite", "2");
	hilite_minhilite=atoi(temp.c_str());
	//columncolors
	FindSetting(&killtable_column[0], "KILL TABLES","column0", "#202020");
	FindSetting(&killtable_column[1], "KILL TABLES","column1", "#404040");
	FindSetting(&killtable_column[2], "KILL TABLES","column2", "#606060");


	//MAIN settings
	FindSetting(&temp, "MAIN","limit_overall_killtables", "1");
	main_overall_killtable=atoi(temp.c_str());
	FindSetting(&temp, "MAIN","players_min_time", "900");
	players_min_time=atoi(temp.c_str());
	FindSetting(&temp, "MAIN","players_min_games", "2");
	players_min_games=atoi(temp.c_str());
	FindSetting(&temp, "MAIN","players_min_kills", "20");
	players_min_kills=atoi(temp.c_str());
	FindSetting(&time_format, "MAIN","time_format", "%X %x");
	FindSetting(&temp, "MAIN","use_cust_rank", "0");
	use_cust_rank=atoi(temp.c_str());
	FindSetting(&temp, "MAIN","cust_rank_start", "10000");
	cust_rank_start=atoi(temp.c_str());
	FindSetting(&cust_rank_template, "MAIN","cust_rank_template", "custrank.htmlt");
	FindSetting(&srcpath, "MAIN","src_path", ".\\templates");
	FindSetting(&destpath, "MAIN","dest_path", ".\\output");
	FindSetting(&dbpath, "MAIN","db_path", ".\\database");
	FindSetting(&temp, "MAIN","use_db", "1");
	use_db=atoi(temp.c_str());
	


	//GAME settings
	FindSetting(&temp, "GAME","min_players", "2");
	game_minplayers=atoi(temp.c_str());
	FindSetting(&temp, "GAME","min_time", "21");
	game_mintime=atoi(temp.c_str());
	FindSetting(&temp, "GAME","min_kills", "10");
	game_minkills=atoi(temp.c_str());
	FindSetting(&temp, "GAME","game_complete", "2");
	game_complete=atoi(temp.c_str());
	FindSetting(&temp, "GAME","game_last_placed", "3");
	game_placed=atoi(temp.c_str());


	//LOGS settings
	FindSetting(&temp, "LOGS","number_of_logs", "-1");
	logs_number=atoi(temp.c_str());
	if(logs_number<=0 || logs_number>1000){				//yes, I know 1000 is arbitrary, but will anyone ever need more that that?
		cfgfile->AddValue("LOGS","number_of_logs", "1");
		cfgfile->AddValue("LOGS","logpath0", "games.log");
		cfgfile->AddValue("LOGS","logtype0", "0");
		logs_number=1;
	}
	logs_paths = new string[logs_number];
	logs_type = new int[logs_number];
	for(i=0;i<logs_number;i++){
		itoa(i,numbuf,10);
		temp1="logpath";
		temp1+=numbuf;
		FindSetting(&logs_paths[i], "LOGS",temp1.c_str(), "games.log");
		temp1="logtype";
		temp1+=numbuf;
		FindSetting(&temp, "LOGS",temp1.c_str(), "0");
		logs_type[i]=atoi(temp.c_str());
	}

	//TEMPLATES settings
	FindSetting(&temp, "TEMPLATES","main_templates_number", "-1");
	main_number=atoi(temp.c_str());
	if(main_number<0 || main_number>1000){				//yes, I know 1000 is arbitrary, but will anyone ever need more that that?
		cfgfile->AddValue("TEMPLATES","main_templates_number", "1");
		cfgfile->AddValue("TEMPLATES","main_template0", "stats");
		main_number=1;
	}
	main_templates = new string[main_number];
	for(i=0;i<main_number;i++){
		itoa(i,numbuf,10);
		temp1="main_template";
		temp1+=numbuf;
		FindSetting(&main_templates[i], "TEMPLATES",temp1.c_str(), "stats");
	}
	FindSetting(&temp, "TEMPLATES","game_templates_number", "-1");
	game_number=atoi(temp.c_str());
	if(game_number<0 || game_number>1000){				//yes, I know 1000 is arbitrary, but will anyone ever need more that that?
		cfgfile->AddValue("TEMPLATES","game_templates_number", "1");
		cfgfile->AddValue("TEMPLATES","game_template0", "game");
		game_number=1;
	}
	game_templates = new string[game_number];
	for(i=0;i<game_number;i++){
		itoa(i,numbuf,10);
		temp1="game_template";
		temp1+=numbuf;
		FindSetting(&game_templates[i], "TEMPLATES",temp1.c_str(), "game");
	}
	FindSetting(&lastgame_template, "TEMPLATES","lastgame_template", "lastgame");
	FindSetting(&gamelist_template, "TEMPLATES","gamelist_template", "gamelist");
	FindSetting(&gamelistloop_template, "TEMPLATES","gamelist_loop_template", "gamelistloop");
	FindSetting(&player_template, "TEMPLATES","player_template", "player");
	FindSetting(&temp, "TEMPLATES","map_templates_number", "-1");
	map_number=atoi(temp.c_str());
	if(map_number<0 || map_number>1000){				//yes, I know 1000 is arbitrary, but will anyone ever need more that that?
		cfgfile->AddValue("TEMPLATES","map_templates_number", "1");
		cfgfile->AddValue("TEMPLATES","map_template0", "map");
		map_number=1;
	}
	map_templates = new string[map_number];
	for(i=0;i<map_number;i++){
		itoa(i,numbuf,10);
		temp1="map_template";
		temp1+=numbuf;
		FindSetting(&map_templates[i], "TEMPLATES",temp1.c_str(), "map");
	}

	return true;
}


bool Settings::FindSetting(string *value, const char* section,const char* name, const char* default_){
	string temp;
	value->erase();
	//first look in cmdline params
	temp=initChar;
	temp+=section;
	temp+="/";
	temp+=name;

	int i;
	for(i=0;i<argc;i++){
		if(!strcmp(argv[i],temp.c_str())){
		    printf ("-> matched argv[%d] = %s == %s \n", i, //argv[i], temp.c_str()); // ss5
			//found match - if next param doesn't start with \, return it  else return blank
			if(i+1 < argc && argv[i+1][0]!=initChar){
				(*value)=argv[i+1];
			}
			return true;
		}
	}

	// didn't find it in cmdline - look in config file
	if(cfgfile->GetValue(section, name, value)!=2){
		//didn't find - add default to config file
		cfgfile->AddValue(section, name, default_);
		(*value)=default_;
		return false;
	}
	return true;

}

	

const char Settings::initChar = '\\';

