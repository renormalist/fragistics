/* htmlfile.h
 Fragistics  -  Game Statistics program for Quake 3 Arena
 Copyright (C) 2000   Brian Risinger

see htmlfile.cpp or main.cpp for license info
*/

#ifndef HTMLFILE_H_INCLUDED
#define HTMLFILE_H_INCLUDED


#include <stdio.h>
#include <string>
#include "os.h"
#include "settings.h"
#include "stats.h"

extern Settings settings;

using namespace std;




bool GetNextWord(string *text,string *word);




class HTMLFile {
	File *fp;

	
public:
	HTMLFile( char* filename);
	~HTMLFile(){
		if(fp!=NULL){
			fp->Close();
			delete fp;
		}
	};

	bool Open();

	bool Write(string *str);
	bool Write(const char *str);

	bool WriteHeader(const char *title);

};



class HtmlTemplate {

/* 

Implements a class to parse html templates for reporting the stats
##variable_name##	is replaced by the value of the variable
$$file_name$$		is replaced by the parsed contents of the file 'file_name' in the path contained by the srcpath variable
**IF( condition )**  **ELSE**  **ENDIF**	implements if then else functionality where
					condition is made up of  value operator value
					value <- variable_name || "const" ||  ( condition )
					operator <- &&, ||, >, <, =, <=, >=, !=
@@LOOP( loop_name )@@  @@ENDLOOP@@			implements loop functionality where loop_name
					is the name of a special loop variable.  Inside the loop, you can access parameters of that iteration of the loop
!!format,expression!!		is replaces by the value of the evualted experssion  (math)
					expression consists of variables and operators  
					format specifies the output format such as %2.2f or %g
					format is optional and defaults to %f

*/
private:
	string srcpath;
	string destpath;

	File *srcfile;
	File *destfile;

	Stats *stats;
public:
	HtmlTemplate( Stats *stat){
		stats=stat;
		srcfile=NULL;
		destfile=NULL;
	};
	HtmlTemplate(const char *src, const char *dest, Stats *stat){
		string temp;
		stats=stat;
		srcfile=NULL;
		destfile=NULL;

		settings.GetTemplateDestpath(&destpath);
		settings.GetTemplateSrcpath(&srcpath);
		if(srcpath.length()>0){
			temp=srcpath;
			temp+=os.PathSeparator;
		}
		temp+=src;
		if(strstr(src,".htmlt")==NULL)
			temp+=".htmlt";
		srcfile=os.GetFile(temp.c_str());
		temp.erase();
		if(destpath.length()>0){
			temp=destpath;
			temp+=os.PathSeparator;
		}
		temp+=dest;
		if(strstr(dest,".html")==NULL)
			temp+=".html";
		destfile=os.GetFile(temp.c_str());
	};
	~HtmlTemplate(){
		if(srcfile)
			delete srcfile;
		if(destfile)
			delete destfile;
	};

	bool Write();
	bool ParseFile(string *buf);	//parses the source file, puts result in buffer
	
	void Parse(string *text, string *buf);		//parses the text, calls ParseToken to interpret the tokens in the text 

private:
	void ParseToken(string *token, string *buf);  //parses the first token in the string token and removes it from that string, returns the parsed token in buf

	bool Eval(string *condition);

	void DoLoop(string *looptype,string *text,string *result);
	void DoIf(string *condition,string *text,string *result);

	void Compute(string *expression, string *buf);  //parses the expression
	
};

#endif //HTMLFILE_H_INCLUDED