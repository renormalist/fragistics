/* os.h
 Fragistics  -  Game Statistics program for Quake 3 Arena
 Copyright (C) 2000   Brian Risinger

see os.cpp or main.cpp for license info
*/





#ifndef OS_H_INCLUDED_
#define OS_H_INCLUDED_


#include <stdio.h>
#include <string.h>
#include <string>



using namespace std;

//util functions
int GetAggregateKills(int *killarray, int type);
int ParseKilltypeStr(string *str);
int ParseItemStr(string *str);
#ifndef WIN32
char * itoa(int num, char* buf, int base);
char getch();
#endif


//This file has functions for all operating system specific calls.
//if you find something that is operaing system specific, please move it
// into this file to make porting easier.  That way only os.h and os.cpp
// need to be ported to have Fragistics run on another operating system.

class File {
private:
	FILE *fp;
	char name[260];

	bool open;
	bool read;
	bool write;

public:
	File(const char *name_){
		fp=NULL;
		strncpy(name,name_,258);
		name[259]='\0';
		open=false;
		read=false;
		write=false;
	};
	~File(){
		if(open)Close();
	};

	char *GetName(){return name;};
	bool Exists();

	int OpenRead();
	int OpenWrite();
	//add other open types such as ReadWrite or Append

	int Close();

	char* ReadLine(char *buf,int bufsize);
	int WriteLine(const char *buf);
	int WriteStr(const char *buf);

	bool isOpen(){return open;};
	bool canRead(){return read;};
	bool canWrite(){return write;};

	bool GetFile(string *buf);

	//not implemented yet/not used yet
	int Rename();
	int Delete();
	int Copy(char *to);
	int Move(char *to);

	

};





class OS {
public:

	OS(){};
	~OS(){};

	int SetLowPriority();			//may be used in the future of background operation

	File* GetFile(const char *name){		//may want to keep a list of open files here and return same object if the is already and instance of it.
		return new File(name);
	};

	void Delay(int seconds);		//may be used in the future for background operation


	char* TrimStr(char *str){		//trims a string in the same buffer
		char *ptr1,*ptr2,*ptr3;
		ptr1=ptr2=ptr3=str;
		while(*ptr1>0 && *ptr1<=32) ptr1++;//skip bad chars
		while(*ptr1!='\0'){			//loop the good chars
			*ptr2=*ptr1;			//copying to begining of string
			if( !(*ptr2>0 && *ptr2<=32) )
				ptr3=ptr2+1;		//and record where to put the \0
			ptr2++;
			ptr1++;
		}
		*(ptr3)='\0';				//place the \0
		return str;
	}
	void TrimStr(string *str){
		char *buf=new char[str->length()+4];
		if(buf!=NULL){
			strcpy(buf,str->c_str());
			TrimStr(buf);
			(*str)=buf;
		}
	};

	int GetVersion(char *ver,int size);

	static const char PathSeparator;

};




#endif //OS_H_INCLUDED_
