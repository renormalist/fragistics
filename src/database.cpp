/* database.cpp
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

#include "os.h"
#include "database.h"
#include "settings.h"
#include <time.h>
#include <stdlib.h>

extern Settings settings;


int database::readplayers(){

	string path;
	string buf;

	settings.GetDatabasepath(&path);
	path+=os.PathSeparator;
	path+="players.fpdb";

	ConfigFile  cf(path.c_str());

	cf.GetValue("","lastline",&buf);
	startline=atoi(buf.c_str());

	cf.GetValue("","lastgame",&buf);
	st->SetGames(atoi(buf.c_str()));

	cf.GetValue("","nummaps",&buf);
	nummaps=atoi(buf.c_str());

	Player *p;
	Section *s;
	s=cf.GetSettings();
	if(s!=NULL){
		do{
			if(s->GetName().length()>0){
				printf(" reading player %s\t",s->GetName().c_str());
				p=new Player(s);
				st->AddPlayer(p); //will store player object
			}
		}while((s=s->GetNext())!=NULL);
	}

	printf("\n");
	return 0;
}

int database::writeplayers(){
	string path;
	string buf;
	char temp[50];
	int itemp;

	settings.GetDatabasepath(&path);
	path+=os.PathSeparator;
	path+="players.fpdb";

	ConfigFile  cf(path.c_str(),true);

	os.GetVersion(temp,49);
	cf.AddValue("","version",temp);

	settings.GetMainTimeFormat(&buf);
	time_t now;
	now=time(NULL);
	strftime(temp,49,buf.c_str(),localtime(&now));
	cf.AddValue("","timedate",temp);
	cf.AddValue("","time",itoa(now,temp,10));

	
	itemp=st->GetFileNameLine(&buf);
	cf.AddValue("","lastfile",buf.c_str());
	cf.AddValue("","lastline",itoa(itemp,temp,10));
	cf.AddValue("","lastgame",itoa(st->GetGames(),temp,10));

	itemp=0;
	MapList *ml;
	ml=st->GetMaps();

	if(ml!=NULL){
		if(ml->GetFirst()){
			do{
				itemp++;
			}while(ml->GetNext());
		}
	};

	cf.AddValue("","nummaps",itoa(itemp,temp,10));


	PList *pl;
	Player *p;
	pl=st->GetPlayers();
	if(pl!=NULL){
		p=pl->GetFirst();
		if(p!=NULL){
			do{
				buf.erase();
				p->Write(&buf,2);
				printf(" writing player %s\t",buf.c_str());
				p->WriteToDb(&cf);
			}while((p=pl->GetNext())!=NULL);
		}
	}
	
	cf.Write();
	printf("\n");

	return 0;
}

int database::readmaps(){
	string basepath,path;
	char temp[50];

	settings.GetDatabasepath(&basepath);

	basepath+=os.PathSeparator;

	int i;
	for(i=0;i<nummaps;i++){
		path=basepath;
		path+="map";
		path+=itoa(i,temp,10);
		path+=".fmdb";

		ConfigFile *cf;
		cf=new ConfigFile(path.c_str());
		Map *m;
		m=new Map(cf);
		st->AddMap(m);
		delete cf;

	}

	printf("\n");

	return 0;
}

int	database::writemaps(){
	string path;
	string buf;
	char temp[50];
	int count=0;
	MapList *ml;
	Map *m;

	ml=st->GetMaps();

	if(ml!=NULL){
		if(ml->GetFirst()){
			do{
				
				m=ml->GetMap();
				if(m!=NULL){

					m->GetName(&buf);
					printf(" writing map %s",buf.c_str());

					settings.GetDatabasepath(&path);
					path+=os.PathSeparator;
					path+="map";
					path+=itoa(count++,temp,10);
					path+=".fmdb";

					ConfigFile  cf(path.c_str(),true);

					
					m->WriteToDb(&cf);

				}

			}while(ml->GetNext());
		}
	};

	return 0;
}



int database::readgamelist(){
	File *f;
	string path,file;

	settings.GetDatabasepath(&path);

	path+=os.PathSeparator;
	path+="gamelist.fgdb";

	f=os.GetFile(path.c_str());
	if(f->GetFile(&file)==false)
		return 1;

	st->SetGamelist(&file);
	return 0;
}
	

int database::writegamelist(){

	File *f;
	string path,file;

	settings.GetDatabasepath(&path);

	path+=os.PathSeparator;
	path+="gamelist.fgdb";

	f=os.GetFile(path.c_str());

	st->GetGamelist(&file);

	if(f->OpenWrite()){
		f->WriteStr(file.c_str());
	}else{
		return 1;
	}
	return 0;

}





