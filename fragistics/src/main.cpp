/* main.cpp
   Fragistics  -  Game Statistics program for Quake 3 Arena
   Copyright (C)  2000   Brian Risinger
   Some Revisions 2002   Steffen Schwigon
      
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

   Steffen Schwigon (maintainer since 2002) contact info:

   schwigon@sax.de
   http://fragistics.sourceforge.net
   
   98 Stone Lane
   Levittown, NY  11756
   USA
   
*/


#include <string>
#ifdef WIN32
#include <conio.h>
#else
#include <stdio.h>
#endif

#include "os.h"
#include "stats.h"
#include "gameslogparser.h"
#include "htmlfile.h"
#include "settings.h"
#include "database.h"


extern char kill_strings[26][32];


OS os;
Settings settings;

int main(int argc ,char** argv){
    
    bool error=false;
    bool pause=false;
    
    char ver[50];
    os.GetVersion(ver,50);
    
    printf("Fragistics version %s, Copyright (C) 2000  Brian Risinger\n",ver);
    printf("Revised 2002 by Steffen Schwigon\n");
    printf("Fragistics comes with ABSOLUTELY NO WARRANTY.\n");
    printf("This is free software, and you are welcome\n");
    printf("to redistribute it under certain conditions;\n");
    printf("For details, see the readme.txt that should have came with\n");
    printf("this program or see http://fragistics.sourceforge.net\n");
    //	os.Delay(2);


    /*  real code  */
	
    settings.ParseSettings("fragistics.conf",argc,argv);
    
    int i=0;
    Stats stats;
    string filename;
    database db(&stats);
    
    int dbstatus=DB_DONTADD;
    int logtype;
    int logdb;
    int startline=0;
    int temp;
    bool juststore=false;
    bool parse=true;
    
    //see if any file are being added to database
    for(i=0;i<settings.GetLogsNumber();i++){
	temp=settings.GetLogsType(i)/1000;
	if(dbstatus<temp) dbstatus=temp;
    }
    if(dbstatus=DB_ACTIVEFILE) dbstatus=DB_FILESTORED;
    if(dbstatus>=DB_STOREFILE && settings.GetUseDB()){
	juststore=true;
    }
    
    if(settings.GetUseDB() && !juststore){
	printf("\nReading Database\n");
	db.read();
	
    }
    
    
    
    //parse all the log files
    for(i=0;i<settings.GetLogsNumber();i++){
	
	settings.GetLogsPath(i,&filename);
	FileParser *fparse=NULL;
	
	logtype=settings.GetLogsType(i);
	logdb=logtype/1000;
	logtype%=1000;
	startline=0;
	
	//figure out what to do with db
	if(settings.GetUseDB()){
	    if(logdb>=dbstatus){
		//posible problem - cant skip a file 
		if(dbstatus==DB_FILESTORED && logdb==DB_STOREFILE && juststore){
		    logdb=DB_FILESTORED;
		}else if(dbstatus==DB_FILESTORED && logdb==DB_STOREFILE && !juststore){
		    logdb=DB_DONTADD;
		}else if(dbstatus==DB_ACTIVEFILE && logdb>=DB_ACTIVEFILE){
		    logdb=DB_DONTADD;
		}else if(dbstatus==DB_DONTADD && logdb>DB_DONTADD){
		    logdb=DB_DONTADD;
		}
		
		settings.SetLogsType(i,logdb*1000+logtype);
	    }
	    
	    if( logdb==DB_ACTIVEFILE ){
		startline=db.GetStartLine();
	    }
	    
	    //is it time to save database
	    if(juststore && dbstatus<DB_FILESTORED){
		parse=false;
		db.write();
		printf("\nWriting Database\n");
	    }
	    if(dbstatus>DB_DONTADD && logdb==DB_DONTADD && !juststore){
		printf("\nWriting Database\n");
		db.write();
	    }
	    
	}
	
	if(parse){
	    printf("\nParsing file %s\n",filename.c_str());
	    
	    //get correct parser for file
	    switch(logtype){
	    case LOGTYPE_GAMESLOG:
		fparse=new GameslogParser(&filename,&stats);
		break;
	    default:	fparse=NULL; break;
	    }
	    
	    //parse file
	    if(fparse!=NULL){
		stats.SetCurrentFile(filename.c_str(),startline);
		
		if(fparse->Parse(startline)==true){
		    printf("Done Parsing file %s\n\n",filename.c_str());
		}else{
		    error=true;
		    printf("ERROR Parsing file %s!\n",filename.c_str());
		    break;
		}
		delete fparse;
	    }else{
		pause=true;
		printf("ERROR - Unknown file type: %d for %s!\n",logtype,filename.c_str());
	    }
	    fparse=NULL;
	}
	
	dbstatus=logdb;
	
    }
    //is it time to save database
    if(settings.GetUseDB() && dbstatus>DB_DONTADD && !juststore){
	printf("\nWriting Database\n");
	db.write();
    }
    
    if(error==false && !juststore){
	//old way with hard coded pages
	//		bool result;
	//		result=stats.WriteMainStats();
	//		error=(!result);
	error=stats.WriteMainStats();
	if(error)
	    printf("Error writing main stats!\n");
    }
    
    if(juststore){
	printf("\nFile have been stored to DB.\nRerun to generate stats pages.\n");
    }
    
#ifdef _DEBUG
    pause=true;
#endif
    
    if(pause || error){
	printf("\n\nPress a key");
	char c=getch();
    }
    
    return 0;
}

/* *** Emacs-Variables: *** */
/* *** font-lock-mode: true *** */
/* *** Emacs-Variables: End *** */

