/* htmlfile.cpp
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

#include "htmlfile.h"
#include "settings.h"


extern OS os;
extern Settings settings;




bool GetNextWord(string *text,string *word){
	int pos;
	pos=text->find_first_of(' ');
	if(pos>-1){
		(*word)=text->substr(0,pos+1);
		(*text)=text->substr(pos+1);
		return true;
	}else{
		(*word)=(*text);
		text->erase();
		return false;
	}
}





HTMLFile::HTMLFile( char* filename){
	fp=os.GetFile(filename);
};

bool HTMLFile::Open(){
	if(fp->OpenWrite())
		return true;
	else{
		delete fp;
		fp=NULL;
		return false;
	}
}


bool HTMLFile::Write(string *str){
	if(!fp->canWrite()) return false;
	fp->WriteLine((char*)(*str).c_str());
	return true;
};

bool HTMLFile::Write(const char *str){
	if(!fp->canWrite()) return false;
	fp->WriteLine(str);
	return true;
};

bool HTMLFile::WriteHeader(const char *title){
	string temp,tmp;

	temp = "<html><head>\n<title>";
	temp+= title;
	temp+= "</title>\n</head>";
	temp+= "<body bgcolor=\"";
	settings.GetColorBg(&tmp);
	temp+= tmp;
	temp+= "\" text=\"";
	settings.GetColorText(&tmp);
	temp+= tmp;
	temp+= "\" link=\"";
	settings.GetColorLink(&tmp);
	temp+= tmp;
	temp+= "\" vlink=\"";
	settings.GetColorVlink(&tmp);
	temp+= tmp;
	temp+= "\" alink=\"";
	settings.GetColorAlink(&tmp);
	temp+= tmp;
	temp+= "\">";

	return Write(&temp);
};





bool HtmlTemplate::Write(){
	
	if(!srcfile->OpenRead()){
		printf("Error opening template %s for reading!\n",srcfile->GetName());
		return false;
	}
	if(!destfile->OpenWrite()){
		printf("Error opening %s for writing!\n",destfile->GetName());
		return false;
	}

	string source, desttext;
	source.erase();

	char tmp[1024];

	//read file into buffer
	while(srcfile->ReadLine(tmp,1023)!=NULL)
		source+=tmp;

	srcfile->Close();

	//now parse file - file contains plain-text with tokens inserted
	Parse(&source,&desttext);

	destfile->WriteStr(desttext.c_str());

	return true;
}


bool HtmlTemplate::ParseFile(string *buf){
	//very similar to Write above
	if(!srcfile->OpenRead()){
		printf("Error opening template %s for reading!\n",srcfile->GetName());
		return false;
	}
	
	string source;
	source.erase();
	buf->erase();

	char tmp[1024];

	//read file into buffer
	while(srcfile->ReadLine(tmp,1023)!=NULL)
		source+=tmp;

	srcfile->Close();

	//now parse file - file contains plain-text with tokens inserted
	Parse(&source,buf);

	return true;

}

void HtmlTemplate::Parse(string *text, string *buf){

	// write out the plain text  find and parse tokens, write out result
	int pos=0;
	char cur,last;
	string towrite;
	last='\0';
	cur='\0';
	buf->erase();
	while(pos<text->length()){
		cur=text->at(pos);
		if(last==cur && (cur=='#'||cur=='$'||cur=='*'||cur=='@'||cur=='!')){
			string temp;
			temp=text->substr(pos-1);
			(*text)=temp;
			(*buf)+=towrite;
			towrite.erase();

			ParseToken(text,&temp);
			(*buf)+=temp.c_str();

			last='\0';
			cur='\0';
			pos=-1;

		}
		if(last!='\0')
			towrite+=last;
		last=cur;
		pos++;
	}
	(*buf)+=towrite;
	if(cur!='\0')
		(*buf)+=cur;
}


void HtmlTemplate::ParseToken(string *token, string *buf){

	char type;
	type=token->at(0);

	string param;
	int pos=2;
	while(pos+1<token->length()&& !(token->at(pos)==type && token->at(pos+1)==type) ){
		param+=token->at(pos);
		pos++;
	}
	if(pos+2>token->length()) pos=token->length()-2;
	(*token)=token->substr(pos+2);


	switch(type){
	case '#':{
				stats->GetVariableValue(&param,buf);
			 }
		break;
	case '$':{
				HtmlTemplate *temp=new HtmlTemplate(param.c_str(),param.c_str(),stats);
				temp->ParseFile(buf);
				delete temp;
			 }
		break;
	case '*':{
				if(strncmp(param.c_str(),"IF(",3)){
					printf("%s while not in IF\n",param.c_str());
					return;
				}
				param=param.substr(3,param.length()-4);
				char paramt[256];
				strncpy(paramt,param.c_str(),254);
				paramt[255]='\0';
				param=os.TrimStr(paramt);
				DoIf(&param,token,buf);
			 }
		break;
	case '@':{
				if(strncmp(param.c_str(),"LOOP(",5)){
					printf("ENDLOOP while not in loop\n");
					return;
				}
				param=param.substr(5,param.length()-6);
				char paramt[256];
				strncpy(paramt,param.c_str(),254);
				paramt[255]='\0';
				param=os.TrimStr(paramt);
				DoLoop(&param,token,buf);

			 }
		break;
	case '!':{
				Compute(&param,buf);
			 }
		break;
	}
}

void HtmlTemplate::Compute(string *expression, string *buf){
	string format;
	string temp,tempformat;
	int pos;
	char numbuf[50];
	char formattype;
	double result;
	buf->erase();

	//parse format
	pos=expression->find_first_of(',');
	if(pos>-1){
		format="%";
		tempformat=expression->substr(0,pos);
		(*expression)=expression->substr(pos+1);
		formattype=tempformat.at(tempformat.length()-1);
		tempformat=tempformat.substr(0,tempformat.length()-1);
		if(formattype!='e' && formattype!='f' && formattype!='g'){
			formattype='f';
		}
		if(tempformat.at(0)=='%'){
			tempformat=tempformat.substr(1);
		}
		pos=tempformat.find_first_of('.');
		if(pos>-1){
			temp=tempformat.substr(0,pos);
			tempformat=tempformat.substr(pos+1);
			if(temp.length()>0){
				pos=atoi(temp.c_str());
				format+=itoa(pos,numbuf,10);
			}
			if(tempformat.length()>0){
				format+=".";
				pos=atoi(tempformat.c_str());
				format+=itoa(pos,numbuf,10);
			}
		}else{
			if(tempformat.length()>0){
				pos=atoi(temp.c_str());
				format+=itoa(pos,numbuf,10);
			}
		}
		format+=formattype;
	}else{
		format="%.2f";
	}
			
	result=stats->DoMath(expression);

	sprintf(numbuf,format.c_str(),result);

	(*buf)+=numbuf;
	
	
}



void HtmlTemplate::DoIf(string *condition,string *text,string *result){
	string ifthen,ifelse;
	string *current=&ifthen;
	
	string word;
	int depth=0;
	result->erase();
	bool res=true;

	while(depth>-1 && res){
		(*current)+=word;
		res=GetNextWord(text,&word);
		if( !strncmp(word.c_str(),"**IF(",5) ){
			do{
				(*current)+=word;
				GetNextWord(text,&word);
			}while(!strncmp(word.c_str(),")**",3) );
			depth++;
		}
		if(depth==0 && !strncmp(word.c_str(),"**ELSE**",8) ){
			current=&ifelse;
			word.erase();
		}
		if( !strncmp(word.c_str(),"**ENDIF**",9) ){
			depth--;
		}
	}
	if(res==false){
		printf("Unclosed IF found!\n");
	}

	if(Eval(condition)){
		Parse(&ifthen,result);
	}else{
		Parse(&ifelse,result);
	}

}

bool HtmlTemplate::Eval(string *condition){
	string var1,val1,var2,val2,oper;
	double v1,v2;
	char trim[256];

	//simple expressions for now   value operator value  only
	//TODO make this handle complex expressions

	GetNextWord(condition,&var1);
	strncpy(trim,var1.c_str(),254);
	trim[255]='\0';
	var1=os.TrimStr(trim);
	GetNextWord(condition,&oper);
	GetNextWord(condition,&var2);
	strncpy(trim,var2.c_str(),254);
	trim[255]='\0';
	var2=os.TrimStr(trim);

	if(var1.length()<2 || var2.length()<2){
		printf("Bad condition: %s %s %s\n",var1.c_str(),oper.c_str(),var2.c_str());
		return true;
	}

	if(var1.at(0)!='\"'){
		stats->GetVariableValue(&var1,&val1);
	}else{
		val1=var1.substr(1,var1.length()-2);
	}
	if(var2.at(0)!='\"'){
		stats->GetVariableValue(&var2,&val2);
	}else{
		val2=var2.substr(1,var2.length()-2);
	}

	v1=atof(val1.c_str());
	v2=atof(val2.c_str());

	if(!strncmp(oper.c_str(),"=",1)){
		if(!strcmp(val1.c_str(),val2.c_str())){
			return true;
		}else{
			return false;
		}
	}else if(!strncmp(oper.c_str(),">=",2)){
		if(v1>=v2){
			return true;
		}else{
			return false;
		}
	}else if(!strncmp(oper.c_str(),"<=",2)){
		if(v1 <= v2){
			return true;
		}else{
			return false;
		}
	}else if(!strncmp(oper.c_str(),">",1)){
		if(v1 > v2){
			return true;
		}else{
			return false;
		}
	}else if(!strncmp(oper.c_str(),"<",1)){
		if(v1 < v2){
			return true;
		}else{
			return false;
		}
	}else if(!strncmp(oper.c_str(),"%=",1)){
		if( (v2 != 0) && (((int)v1 % (int)v2) == 0) ){
			return true;
		}else{
			return false;
		}
	}else if(!strncmp(oper.c_str(),"!=",1)){
		if(!strcmp(val1.c_str(),val2.c_str())){
			return false;
		}else{
			return true;
		}
	}

	printf("UNKNOWN operator %s in if\n",oper.c_str());

	return true;
}


void HtmlTemplate::DoLoop(string *looptype,string *text,string *result){

	string body,sort;
	char cur,last;
	int pos=0,depth=0,intag=0;
	last='\0';
	result->erase();

	//find end of loop
	while(pos<text->length()){
		cur=text->at(pos);
		if(intag==1){
			//check first char after a @@
			intag++;
			if(cur=='L'){
				depth++;	//open loop
			}else{
				depth--;	//close loop
				if(depth==-1)
					intag=5;
			}
		}
		if(last==cur && cur=='@' ){
			intag++;
			if(intag==3){
				intag=0;
			}
			if(intag>5)
				break;
		}
		if(intag<4){
			if(last!='\0')
				body+=last;
			last=cur;
		}
		pos++;
	}
	if(intag<5){
		printf("LOOP with no ENDLOOP!\n");
	}
	if(pos+2>=text->length()){
		text->erase();
	}else{
		(*text)=text->substr(pos+2);
	}
	body=body.substr(0,body.length()-1);

	//find sort type
	pos=looptype->find_first_of('.');
	if(pos>-1){
		string temp=(*looptype);
		(*looptype)=temp.substr(0,pos);
		sort=temp.substr(pos+1);
		os.TrimStr(&sort);
	}


	//find type of loop
	int ltype=LOOP_NONE;
	if(!strcmp(looptype->c_str(),"LOOP_STATS_PLAYERS") ){
		ltype=LOOP_ALLPLAYER_FULL;
	}else if(!strcmp(looptype->c_str(),"LOOP_STATS_PLAYERS_T") ){
		ltype=LOOP_ALLPLAYER_TRIM;
	}else if(!strcmp(looptype->c_str(),"LOOP_GAME_PLAYERS") ){
		ltype=LOOP_GAMEPLAYER_FULL;
	}else if(!strcmp(looptype->c_str(),"LOOP_GAME_PLAYERS_T") ){
		ltype=LOOP_GAMEPLAYER_TRIM;
	}else if(!strcmp(looptype->c_str(),"LOOP_MAP_PLAYERS") ){
		ltype=LOOP_MAPPLAYER_FULL;
	}else if(!strcmp(looptype->c_str(),"LOOP_MAP_PLAYERS_T") ){
		ltype=LOOP_MAPPLAYER_TRIM;
	}else if(!strcmp(looptype->c_str(),"LOOP_MAPS") ){
		ltype=LOOP_MAPS;
	}else if(!strcmp(looptype->c_str(),"LOOP_PLAYER_GAMES") ){
		ltype=LOOP_PLAYERGAMES;
	}else if(!strcmp(looptype->c_str(),"LOOP_PLAYER_KILLS") ){
		ltype=LOOP_PLAYERKILLS;
	}else if(!strcmp(looptype->c_str(),"LOOP_PLAYER_DEATHS") ){
		ltype=LOOP_PLAYERDEATHS;
	}else if(!strcmp(looptype->c_str(),"LOOP_MAP_GAMES") ){
		ltype=LOOP_MAPGAMES;
	}else{
		printf("ERROR: Bad loop type: %s!\n",looptype->c_str());
		return;
	}

	//do the loop
	if(stats->SetupLoop(ltype,&sort)){
		do{
			string temp,buf;
			temp=body;
			Parse(&temp,&buf);
			(*result)+=buf;
//			(*result)+="\n";
		}while(stats->NextLoop(ltype));
	}
	

}




