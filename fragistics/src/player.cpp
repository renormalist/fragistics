/* player.cpp
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

#include <stdio.h>
#include "player.h"
#include "settings.h"
#include "stats.h"

extern Settings settings;

extern char kill_strings[26][32];

// item strings_nice - must be the same order as item defines
char item_strings_nice[][30] = {	"Unknown Item",
							"Gauntlet",
							"Machinegun",
							"Shotgun",
							"Grenade Launcher",
							"Rocket Launcher",
							"Lightning Gun",
							"Railgun",
							"Plasma Gun",
							"BFG",
							"Armor Shard",
							"Yellow Armor",
							"Red Armor",
							"+5 Health",
							"25 Health",
							"50 Health",
							"Battle Suit",
							"Flight",
							"Haste",
							"Invisibility",
							"Mega Health",
							"Quad",
							"Regeneration",
							"Medkit",
							"Teleporter",
							"Red Flag",
							"Blue Flag",
							"Bullets",
							"Shells",
							"Grenades",
							"Rockets",
							"Lightning",
							"Slugs",
							"Cells",
							"BFG Ammo",
							"\0\0\0" };





/*
void SimplePlayer::parsename(){
	char *ptr1;
	ptr1=name;

	htmlname="<font color=white>";
	vertname="<font color=white>";
	
	while(*ptr1!='\0'){
		if(*ptr1=='^'){
			ptr1++;
			if(*ptr1>='0'&&*ptr1<='9'){
				switch(*ptr1){
				case '1':
					htmlname+="</font><font color=red>";
					vertname+="</font><font color=red>";
					break;
				case '2':
					htmlname+="</font><font color=\"#00FF00\">";
					vertname+="</font><font color=\"#00FF00\">";
					break;
				case '3':
					htmlname+="</font><font color=yellow>";
					vertname+="</font><font color=yellow>";
					break;
				case '4':
					htmlname+="</font><font color=blue>";
					vertname+="</font><font color=blue>";
					break;
				case '5':
					htmlname+="</font><font color=cyan>";
					vertname+="</font><font color=cyan>";
					break;
				case '6':
					htmlname+="</font><font color=\"#ff00ff\">";
					vertname+="</font><font color=\"#ff00ff\">";
					break;
				default:
					htmlname+="</font><font color=white>";
					vertname+="</font><font color=white>";
					break;
				}
			}else if(*ptr1=='^'){
				htmlname+="^";
				vertname+="^<BR>";
				plainname+="^";
			}else{
				htmlname+="^";
				vertname+="^<BR>";
				plainname+="^";
				htmlname+=*ptr1;
				vertname+=*ptr1;
				vertname+="<BR>";
				plainname+=*ptr1;
			}
		}else{
			htmlname+=*ptr1;
			vertname+=*ptr1;
			vertname+="<BR>";
			plainname+=*ptr1;
		}
		ptr1++;
	}
	htmlname+="</font>";
	vertname+="</font>";
}
*/

void SimplePlayer::parsename(){
	char *ptr1;
	string tmp;
	ptr1=name;

	settings.GetNameColor(7,&tmp); // default color = white
	htmlname="<font color=";
	vertname="<font color=";
	htmlname+=tmp;
	vertname+=tmp;
	htmlname+=">";
	vertname+=">";
	
	while (*ptr1!='\0') {
	  // simple colors
	  if (*ptr1=='^') {
	    ptr1++;
	    if (*ptr1>='0'&&*ptr1<='9') {
	      htmlname+="</font><font color=";
	      vertname+="</font><font color=";
	      settings.GetNameColor((*ptr1)-'0',&tmp);
	      htmlname+=tmp;
	      vertname+=tmp;
	      htmlname+=">";
	      vertname+=">";
	    }
	    // special char ^
	    else if (*ptr1=='^') {
	      htmlname+="^";
	      vertname+="^<BR>";
	      plainname+="^";
	    }
	    // explicit rgb code
	    else if (*ptr1=='X' || *ptr1=='x') {
	      string rgbCode = "#";
	      for (int c=0; c<6; c++) { // read 6 more characters
		ptr1++;
		rgbCode+=(*ptr1);
	      }
	      tmp = rgbCode;
	      htmlname+="</font><font color=";
	      vertname+="</font><font color=";
	      htmlname+=tmp;
	      vertname+=tmp;
	      htmlname+=">";
	      vertname+=">";
	      //printf ("DBG: color %s\n", rgbCode.c_str());
	    }
	    // ignore special feature ^b: blinking --> ignore
	    else if (*ptr1=='B' || *ptr1=='b') {
	      // do nothing
	    }
	    // special feature ^f: alternating text --> ignore
	    else if (*ptr1=='F' || *ptr1=='f') {
	      // do nothing
	    }
	    // special feature: reset --> map to white color
	    else if (*ptr1=='N' || *ptr1=='n') {
	      tmp = "white";
	      htmlname+="</font><font color=";
	      vertname+="</font><font color=";
	      htmlname+=tmp;
	      vertname+=tmp;
	      htmlname+=">";
	      vertname+=">";
	    }
	    // default
	    else {
	      htmlname+="^";
	      vertname+="^<BR>";
	      plainname+="^";
	      htmlname+=*ptr1;
	      vertname+=*ptr1;
	      vertname+="<BR>";
	      plainname+=*ptr1;
	    }
	  }
	  else {
	    htmlname+=*ptr1;
	    vertname+=*ptr1;
	    vertname+="<BR>";
	    plainname+=*ptr1;
	  }
	  ptr1++;
	}
	htmlname+="</font>";
	vertname+="</font>";
}

void SimplePlayer::Write(string *str,int vert){
	if(vert == 1){
		(*str)+=vertname;
	}else if(vert==2){
		(*str)+=plainname;
	}else{
		(*str)+=htmlname;
	}
}


void PList::AddPlayer(Player *play,char* name_){
	bool done=false;
	PlayerListNode *temp = new PlayerListNode(play,name_);
		
	//insert in sorted order
	if(head==NULL)
		head=temp;
	else{
		if(strcmp(name_,head->GetName().c_str())<0){
			temp->SetNext(head);
			head=temp;
		}else{
			PlayerListNode *ptr1=head,*ptr2=head->GetNext();
			while(ptr2!=NULL){
				if(strcmp(name_,ptr2->GetName().c_str())<0){
					temp->SetNext(ptr2);
					ptr1->SetNext(temp);
					done=true;
					break;
				}else{
					ptr1=ptr2;
					ptr2=ptr2->GetNext();
				}
			}
			if(!done){
				ptr1->SetNext(temp);
			}
		}
	}
	count++;
}



Player* PList::FindPlayer(char* name){

	PlayerListNode *ptr1=head;
	while(ptr1!=NULL){
		if(!strcmp(ptr1->GetName().c_str(),name)){
			return ptr1->GetPlayer();
		}
		ptr1=ptr1->GetNext();
	}

	return NULL;
}

bool PList::ContainsPlayer(Player *p){

	PlayerListNode *ptr1=head;
	while(ptr1!=NULL){
		if(ptr1->GetPlayer()==p){
			return true;
		}
		ptr1=ptr1->GetNext();
	}

	return false;
}

int PList::GetPos(char *name){
	int i=0;

	PlayerListNode *ptr1=head;
	while(ptr1!=NULL){
		if(!strcmp(ptr1->GetName().c_str(),name)){
			return i;
		}
		ptr1=ptr1->GetNext();
		i++;
	}

	return -1;
}

int PList::GetPos(Player* Play){
	int i=0;

	PlayerListNode *ptr1=head;
	while(ptr1!=NULL){
		if(ptr1->GetPlayer()==Play){
			return i;
		}
		ptr1=ptr1->GetNext();
		i++;
	}

	return -1;
}


bool PList::DupeList(PList *list){
	if(head!=NULL){
		return false;
	}
	if(list->head==NULL){
		return false;
	}

	PlayerListNode *ptr=list->head;
	Player *p;
	while(ptr!=NULL){
		p = new Player((char*)ptr->GetName().c_str(),(char*)ptr->GetPlayer()->GetModel().c_str());
		AddPlayer(p,p->GetName());
		p->CopyStats(ptr->GetPlayer());
		ptr = ptr->GetNext();
	}
	return true;

}

// ss5:
int Player::GetEffInt(int killtype)
{
  if(GetAggregateKills(kills, killtype)<=0)
    return 0;
  else
    return
      (int)( 
      ((float)GetAggregateKills(kills, killtype)*10000.0) / 
      ((float)GetAggregateKills(kills, killtype) + GetAggregateKills(deaths, killtype) + GetAggregateKills(suicides, killtype)));
}

// ss5:
int Player::GetWepSkillInt(int killtype)
{
  return (int) (
    ((float)GetAggregateKills(kills, killtype)*100)/((float)(GetAggregateKills(deaths, killtype)+GetAggregateKills(suicides, killtype)+1)));
}

void Player::SetInfo(char* name_, char* model_, int team_, int hc_, int skill_,int w1v1, int l1v1, int timesec, Stats *st){
		if(name)
			delete name;
		name= new SimplePlayer(name_);
		model=model_;
		if(team!=TEAM_SPECTATOR &&team_==TEAM_SPECTATOR){
			//going into spectator mode - stop counting time
			time_sec+=timesec;
		}
		if(team==TEAM_SPECTATOR &&team_!=TEAM_SPECTATOR){
			//leaving spectator mode - start counting time
			time_sec-=timesec;
		}
		team=team_;
		hc=hc_;
		skill=skill_;
		wins1v1=w1v1;
		loss1v1=l1v1;

		if(startrank=-1){
			rank=startrank=st->GetPlayerRank(GetName());
		}
		if(custstartrank=-1){
			custrank=custstartrank=st->GetCustRank(GetName());
		}

	}


double Player::GetSortVal(int type){
	double div=1.0;
	if(type>(PLAYERLIST_SORTTYPE_PERGAME-1)){
		if(type>(PLAYERLIST_SORTTYPE_PERHOUR-1)){
			div=(double)((double)time_sec/((double)3600.0));
			type-=PLAYERLIST_SORTTYPE_PERHOUR;
		}else{
			div=(double)games;
			type-=PLAYERLIST_SORTTYPE_PERGAME;
		}
	}
	switch(type){
	case PLAYERLIST_SORT_GAMES:			return games/div;
	case PLAYERLIST_SORT_TIME:			return time_sec/div;
	case PLAYERLIST_SORT_KILLS:			return kills[0]/div;
	case PLAYERLIST_SORT_DEATHS:		return deaths[0]/div;
	case PLAYERLIST_SORT_SUICIDES:		return suicides[0]/div;
	case PLAYERLIST_SORT_EFF:			return GetEffInt(0);
	case PLAYERLIST_SORT_ITEMS:			return items_total/div;
	case PLAYERLIST_SORT_SCORE:			return score/div;
	case PLAYERLIST_SORT_FFAWINS:		return winsFFA/div;
	case PLAYERLIST_SORT_1V1WINS:		return wins1v1/div;
	case PLAYERLIST_SORT_TDMWINS:		return winsTDM/div;
	case PLAYERLIST_SORT_CTFWINS:		return winsCTF/div;
	case PLAYERLIST_SORT_FFAPLACE:		return placedFFA/div;
	case PLAYERLIST_SORT_FFAWINPLACE:	return (winsFFA+placedFFA)/div;
	case PLAYERLIST_SORT_SKILL:			return GetWepSkillInt(0);
	case PLAYERLIST_SORT_AVEHC:			return GetAveHc();
	default: return GetFrags(0)/div;  // 5 or other
	}
}


void Player::GetFileName(string *buf){
	char n[256],*s,*d;
	char badchars[]="~`!@#$%^&*\'\"\\/?.,<>|;: ";

	buf->erase();
	name->Write(buf,2);
	strcpy(n,buf->c_str());
	s=d=n;
	while(*s!='\0'){
	  if ((strchr(badchars,*s)==NULL) && (*s>' ') && (*s<128)){ // ss5
	    *d=*s;
	    d++;	
	  }
	  s++;
	}
	*d='\0';
	(*buf)="player_";
	(*buf)+=n;
}



void Player::GetTimeStr(char *buf){
	string str;
	char numbuf[50];
	int temptime=time_sec;
	if(temptime>(60*60*24)){
		str+=itoa((temptime/(60*60*24)),numbuf,10);
		str+="D, ";
		temptime%=(60*60*24);
	}
	if(temptime>(60*60)){
		str+=itoa((temptime/(60*60)),numbuf,10);
		str+="H, ";
		temptime%=(60*60);
	}
	str+=itoa((temptime/(60)),numbuf,10);
	str+="M, ";
	temptime%=(60);
	str+=itoa((temptime),numbuf,10);
	str+="S";

	strcpy(buf,str.c_str());

}




void PList::ReSortList(string *type, Stats *stats){
	//creates a temp list then repeatedly scans the the temp list \
	//taking the largest off the temp list and adding it at the bottom of the real list
	PlayerListNode *tmphead=head,*max,*premax,*ptr,*preptr,*last;
	last=head=NULL;

	string maxval,thisval,tmptype;
	double maxnum,thisnum;
	char maxchar='\0',thischar='\0';
	bool str=false;
	bool eval=false;
	os.TrimStr(type);
	if(type->find_first_of(' ')!=-1){
		eval=true;
	}

	while(tmphead!=NULL){
		premax=NULL;
		max=tmphead;
		preptr=NULL;
		ptr=tmphead;
		//setup max
		stats->SetCurrentPlayer(max->GetPlayer());
		if(!eval){
			maxval.erase();
			stats->GetVariableValue(type,&maxval);
			//printf ("type = %s, maxval = %s\n", type->c_str(),maxval.c_str()); // ss5
			if(!str){
				if(maxval.length()>0){
					maxchar=(char)maxval.at(0);
				}
				if(maxchar>='0' && maxchar<='9' || maxchar=='-' || maxchar=='.'){
					//looks like a double
					maxnum=atof(maxval.c_str());
				}else{
					str=true;
					//value is a string, don't treat as double
				}
			}
		}else{
			tmptype=(*type);
			maxnum=stats->DoMath(&tmptype);
		}
		
		
		//find max
		while(ptr!=NULL){
			//compare current to max

			//get current player val/num
			stats->SetCurrentPlayer(ptr->GetPlayer());
			if(!eval){
				thisval.erase();
				stats->GetVariableValue(type,&thisval);
				if(!str){
					if(thisval.length()>0){
						thischar=(char)thisval.at(0);
					}
					if(thischar>='0' && thischar<='9' || thischar=='-' || thischar=='.'){
						//looks like a double
						thisnum=atof(thisval.c_str());
					}else{
						str=true;
						//value is a string, don't treat as double
					}
				}
			}else{
				tmptype=(*type);
				thisnum=stats->DoMath(&tmptype);
			}

			//now compare this to max
			if(eval || !str){
				//compare double
				if( thisnum > maxnum ){
					max=ptr;
					premax=preptr;

					//re-setup max
					stats->SetCurrentPlayer(max->GetPlayer());
					if(!eval){
						maxval.erase();
						stats->GetVariableValue(type,&maxval);
						if(!str){
							if(maxval.length()>0){
								maxchar=(char)maxval.at(0);
							}
							if(maxchar>='0' && maxchar<='9' || maxchar=='-' || maxchar=='.'){
								//looks like a double
								maxnum=atof(maxval.c_str());
							}else{
								str=true;
								//value is a string, don't treat as double
							}
						}
					}else{
						tmptype=(*type);
						maxnum=stats->DoMath(&tmptype);
					}
				}
			}else{
				//string compare
				if(strcmp(maxval.c_str(),thisval.c_str())==1){
					max=ptr;
					premax=preptr;

					//re-setup max
					stats->SetCurrentPlayer(max->GetPlayer());
					if(!eval){
						maxval.erase();
						stats->GetVariableValue(type,&maxval);
						if(!str){
							if(maxval.length()>0){
								maxchar=(char)maxval.at(0);
							}
							if(maxchar>='0' && maxchar<='9' || maxchar=='-' || maxchar=='.'){
								//looks like a double
								maxnum=atof(maxval.c_str());
							}else{
								str=true;
								//value is a string, don't treat as double
							}
						}
					}else{
						tmptype=(*type);
						maxnum=stats->DoMath(&tmptype);
					}
				}
			}



/* old compare
			double thisval = ptr->GetPlayer()->GetSortVal(type);
			double maxval  = max->GetPlayer()->GetSortVal(type);
			if( thisval > maxval ){
				max=ptr;
				premax=preptr;
			}
*/

			preptr=ptr;
			ptr=ptr->GetNext();
		}
		//clean up a little
		if(premax==NULL){
			tmphead=max->GetNext();//if max is first, adjust tmphead to be max->next
		}else{
			premax->SetNext(max->GetNext());//else properly remove max from list
		}
		//add max to end of real list
		max->SetNext(NULL);
		if(head==NULL){
			last=head=max;
		}else{
			last->SetNext(max);
			last=max;
		}
	}
	stats->SetCurrentPlayer(NULL);	//cleanup - reset currentplayer to null;
}
	
void PList::TrimPlayers(){
	PlayerListNode *ptr1,*ptr2,*ptr3;

	//first find the first good node to be the head
	ptr3=NULL;
	ptr2=ptr1=head;
	while(ptr1!=NULL && ptr1->GetPlayer()->isGood()==false){
		ptr3=ptr1;
		ptr1=ptr1->GetNext();
	}
	if(ptr3){
		ptr3->SetNext(NULL);
		head=ptr1;
		delete ptr2;
	}
	ptr2=head;
	if(head!=NULL)
		ptr1=head->GetNext();
	//now go through the list removing bad player
	while(ptr1!=NULL){
		if(ptr1->GetPlayer()->isGood()==false){
			ptr3=ptr1->GetNext();
			ptr2->SetNext(ptr3);
			ptr1->SetNext(NULL);
			delete ptr1;
			ptr1=ptr3;
		}else{
			ptr2=ptr1;
			ptr1=ptr1->GetNext();
		}
	}

	//redo count
	count=0;
	ptr2=head;
	while(ptr2!=NULL){
		count++;
		ptr2=ptr2->GetNext();
	}

}




void Player::WriteToDb(ConfigFile *cf){
	string section;
	char temp[500],temp2[50],temp3[500];

	section=name->GetName();

	cf->AddValue(section.c_str(),"games",itoa(games,temp,10));
	cf->AddValue(section.c_str(),"time_sec",itoa(time_sec,temp,10));
	cf->AddValue(section.c_str(),"msgs",itoa(msgs,temp,10));
	cf->AddValue(section.c_str(),"msgs_team",itoa(msgs_team,temp,10));

	int i;
	for(i=0;i<ITEM_LAST;i++){
		sprintf(temp2,"item_%d",i);
		cf->AddValue(section.c_str(),temp2,itoa(items[i],temp,10));
	}
	for(i=0;i<MAX_KILLTYPE;i++){
		sprintf(temp2,"kill_%d",i);
		cf->AddValue(section.c_str(),temp2,itoa(kills[i],temp,10));
		sprintf(temp2,"death_%d",i);
		cf->AddValue(section.c_str(),temp2,itoa(suicides[i],temp,10));
		sprintf(temp2,"suicide_%d",i);
		cf->AddValue(section.c_str(),temp2,itoa(deaths[i],temp,10));
	}
	
	cf->AddValue(section.c_str(),"model",model.c_str());
	cf->AddValue(section.c_str(),"team",itoa(team,temp,10));
	cf->AddValue(section.c_str(),"items_total",itoa(items_total,temp,10));
	cf->AddValue(section.c_str(),"hc",itoa(hc,temp,10));
	cf->AddValue(section.c_str(),"ping",itoa(ping,temp,10));
	cf->AddValue(section.c_str(),"pinggames",itoa(pinggames,temp,10));
	cf->AddValue(section.c_str(),"skill",itoa(skill,temp,10));
	cf->AddValue(section.c_str(),"score",itoa(score,temp,10));
	cf->AddValue(section.c_str(),"FFAGames",itoa(FFAGames,temp,10));
	cf->AddValue(section.c_str(),"TDMGames",itoa(TDMGames,temp,10));
	cf->AddValue(section.c_str(),"OneV1Games",itoa(OneV1Games,temp,10));
	cf->AddValue(section.c_str(),"CTFGames",itoa(CTFGames,temp,10));
	cf->AddValue(section.c_str(),"winsFFA",itoa(winsFFA,temp,10));
	cf->AddValue(section.c_str(),"placedFFA",itoa(placedFFA,temp,10));
	cf->AddValue(section.c_str(),"winsTDM",itoa(winsTDM,temp,10));
	cf->AddValue(section.c_str(),"wins1v1",itoa(wins1v1,temp,10));
	cf->AddValue(section.c_str(),"winsCTF",itoa(winsCTF,temp,10));
	cf->AddValue(section.c_str(),"loss1v1",itoa(loss1v1,temp,10));
	cf->AddValue(section.c_str(),"rank",itoa(rank,temp,10));
	cf->AddValue(section.c_str(),"custrank",itoa(custrank,temp,10));

	//player kills
	i=0;
	PlayerKillList *pk;
	pk=pkill.GetFirst();
	while(pk!=NULL){
		sprintf(temp2,"pkill%d",i);
		temp[0]='\0';
		temp3[0]='\0';
		for(int j=1;j<MAX_KILLTYPE;j++){				//start at one, zero is just sum of others and is recalced on load
			sprintf(temp,"%s %d",temp3,pk->GetKill(j));
			strcpy(temp3,temp);
		}
		sprintf(temp,"%s %s",temp3,pk->GetName());
				
		cf->AddValue(section.c_str(),temp2,temp);

		i++;
		pk=pk->GetNext();
	}
	//player deathes
	i=0;
	pk=pdeath.GetFirst();
	while(pk!=NULL){
		sprintf(temp2,"pdeath%d",i);
		temp[0]='\0';
		temp3[0]='\0';
		for(int j=1;j<MAX_KILLTYPE;j++){				//start at one, zero is just sum of others and is recalced on load
			sprintf(temp,"%s %d",temp3,pk->GetKill(j));
			strcpy(temp3,temp);
		}
		sprintf(temp,"%s %s",temp3,pk->GetName());
				
		cf->AddValue(section.c_str(),temp2,temp);
		i++;
		pk=pk->GetNext();
	}
	
	//game list
	string buf;
	gamelist.Write(&buf);
	cf->AddValue(section.c_str(),"gamelist",buf.c_str());

}

Player::Player(Section *sec){
	string temp;
	char temp2[50];

	//init
	init();

	//ok, now read values from the database section object
	name=new SimplePlayer(sec->GetName().c_str());

	sec->GetValue("games",&temp);
	games=atoi(temp.c_str());
	
	sec->GetValue("time_sec",&temp);
	time_sec=atoi(temp.c_str());

	sec->GetValue("msgs",&temp);
	msgs=atoi(temp.c_str());

	sec->GetValue("msgs_team",&temp);
	msgs_team=atoi(temp.c_str());
	
	int i;
	for(i=0;i<ITEM_LAST;i++){
		sprintf(temp2,"item_%d",i);
		sec->GetValue(temp2,&temp);
		items[i]=atoi(temp.c_str());
	}
	
	for(i=0;i<MAX_KILLTYPE;i++){
		sprintf(temp2,"kill_%d",i);
		sec->GetValue(temp2,&temp);
		kills[i]=atoi(temp.c_str());
	
		sprintf(temp2,"death_%d",i);
		sec->GetValue(temp2,&temp);
		suicides[i]=atoi(temp.c_str());
	
		sprintf(temp2,"suicide_%d",i);
		sec->GetValue(temp2,&temp);
		deaths[i]=atoi(temp.c_str());
	}

	sec->GetValue("model",&model);

	sec->GetValue("team",&temp);
	team=atoi(temp.c_str());
	
	sec->GetValue("items_total",&temp);
	items_total=atoi(temp.c_str());
	
	sec->GetValue("hc",&temp);
	hc=atoi(temp.c_str());

	sec->GetValue("ping",&temp);
	ping=atoi(temp.c_str());
	
	sec->GetValue("pinggames",&temp);
	pinggames=atoi(temp.c_str());
	
	sec->GetValue("skill",&temp);
	skill=atoi(temp.c_str());
	
	sec->GetValue("score",&temp);
	score=atoi(temp.c_str());
	
	sec->GetValue("FFAGames",&temp);
	FFAGames=atoi(temp.c_str());

	sec->GetValue("TDMGames",&temp);
	TDMGames=atoi(temp.c_str());

	sec->GetValue("OneV1Games",&temp);
	OneV1Games=atoi(temp.c_str());

	sec->GetValue("CTFGames",&temp);
	CTFGames=atoi(temp.c_str());

	sec->GetValue("winsFFA",&temp);
	winsFFA=atoi(temp.c_str());

	sec->GetValue("placedFFA",&temp);
	placedFFA=atoi(temp.c_str());

	sec->GetValue("wins1v1",&temp);
	wins1v1=atoi(temp.c_str());

	sec->GetValue("winsCTF",&temp);
	winsCTF=atoi(temp.c_str());

	sec->GetValue("winsTDM",&temp);
	winsTDM=atoi(temp.c_str());

	sec->GetValue("loss1v1",&temp);
	loss1v1=atoi(temp.c_str());

	sec->GetValue("rank",&temp);
	rank=atoi(temp.c_str());

	sec->GetValue("custrank",&temp);
	custrank=atoi(temp.c_str());

	StringPair *sp;
	sp=sec->GetStrings();
	if(sp!=NULL){
		do{
			if(!strncmp(sp->GetName().c_str(),"pkill",5)){
				//this is a player kill list item
				char name[65];
				int w[MAX_KILLTYPE];
				sscanf(sp->GetValue().c_str(),"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s",
											/*  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 name*/
					&w[1],&w[2],&w[3],&w[4],&w[5],&w[6],&w[7],&w[8],&w[9],&w[10],&w[11],&w[12],&w[13],&w[14],&w[15],&w[16],&w[17],&w[18],&w[19],&w[20],&w[21],&w[22],&w[23],&name);
				for(i=1;i<MAX_KILLTYPE;i++){
					pkill.AddKill(name,i,w[i]);
				}
			}
		}while((sp=sp->GetNext())!=NULL);
	}

	sp=sec->GetStrings();
	if(sp!=NULL){
		do{
			if(!strncmp(sp->GetName().c_str(),"pdeath",6)){
				//this is a player kill list item
				char name[65];
				int w[MAX_KILLTYPE];
				sscanf(sp->GetValue().c_str(),"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s",
											/*  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 name*/
					&w[1],&w[2],&w[3],&w[4],&w[5],&w[6],&w[7],&w[8],&w[9],&w[10],&w[11],&w[12],&w[13],&w[14],&w[15],&w[16],&w[17],&w[18],&w[19],&w[20],&w[21],&w[22],&w[23],&name);
				for(i=1;i<MAX_KILLTYPE;i++){
					pdeath.AddKill(name,i,w[i]);
				}
			}
		}while((sp=sp->GetNext())!=NULL);
	}

	int pos;
	sec->GetValue("gamelist",&temp);
	os.TrimStr(&temp);
	while(temp.length()>0){
		gamelist.AddGame(atoi(temp.c_str()));
		pos=temp.find_first_of(' ');
		if(pos>-1){
			temp=temp.substr(pos+1);
		}else{
			break;
		}
	}

}


void Player::AddStats(Player *gameplay){
	modified=true;
	games+=gameplay->games;
	time_sec+=gameplay->time_sec;
	if(gameplay->time_sec<0 || time_sec<0){
		printf("bad enter or leave time for player\n");
	}

	msgs+=gameplay->msgs;
	msgs_team+=gameplay->msgs_team;

	int i;
	for(i=0;i<ITEM_LAST;i++)
		items[i]+=gameplay->items[i];
	for(i=0;i<MAX_KILLTYPE;i++){
		kills[i]+=gameplay->kills[i];
		suicides[i]+=gameplay->suicides[i];
		deaths[i]+=gameplay->deaths[i];
	}
	model=gameplay->model;
	team=0;
	items_total+=gameplay->items_total;

	hc+=gameplay->hc;
	skill+=gameplay->skill;
	if(gameplay->pinggames>0){
		ping+=gameplay->ping;
		pinggames+=gameplay->pinggames;
	}
	score+=gameplay->score;

	FFAGames+=gameplay->FFAGames;
	TDMGames+=gameplay->TDMGames;
	OneV1Games+=gameplay->OneV1Games;
	CTFGames+=gameplay->CTFGames;

	winsFFA+=gameplay->winsFFA;
	placedFFA+=gameplay->placedFFA;
	winsTDM+=gameplay->winsTDM;
	wins1v1+=gameplay->wins1v1;
	winsCTF+=gameplay->winsCTF;
	loss1v1+=gameplay->loss1v1;

	pkill.AddList(&gameplay->pkill);
	pdeath.AddList(&gameplay->pdeath);

	rank=gameplay->rank;
	custrank=gameplay->custrank;
}

void Player::CopyStats(Player *gameplay){
	games=gameplay->games;
	time_sec=gameplay->time_sec;

	msgs=gameplay->msgs;
	msgs_team=gameplay->msgs_team;

	int i;
	for(i=0;i<ITEM_LAST;i++)
		items[i]=gameplay->items[i];
	for(i=0;i<MAX_KILLTYPE;i++){
		kills[i]=gameplay->kills[i];
		suicides[i]=gameplay->suicides[i];
		deaths[i]=gameplay->deaths[i];
	}
	model=gameplay->model;
	team=gameplay->team;
	items_total=gameplay->items_total;


	hc=gameplay->hc;
	skill=gameplay->skill;
	ping=gameplay->ping;
	pinggames=gameplay->pinggames;
	score=gameplay->score;

	FFAGames=gameplay->FFAGames;
	TDMGames=gameplay->TDMGames;
	OneV1Games=gameplay->OneV1Games;
	CTFGames=gameplay->CTFGames;

	winsFFA=gameplay->winsFFA;
	placedFFA=gameplay->placedFFA;
	winsTDM=gameplay->winsTDM;
	wins1v1=gameplay->wins1v1;
	winsCTF=gameplay->winsCTF;
	loss1v1+=gameplay->loss1v1;

	gamelist.DupeList(&gameplay->gamelist);
	pkill.AddList(&gameplay->pkill);
	pdeath.AddList(&gameplay->pdeath);

	rank=gameplay->rank;
	startrank=gameplay->startrank;
	custrank=gameplay->custrank;
	custstartrank=gameplay->custstartrank;
	modified=gameplay->modified;
}


void Player::GetModelImgName(char *buf){
	char *ptr;
	strcpy(buf,model.c_str());
	ptr=buf;
	bool def=true;
	while(*ptr!='\0'){
		if(*ptr=='/'){
			*ptr='_';
			def=false;
		}
		ptr++;
	}
	if(def)
		strcat(buf,"_default");
	strcat(buf,".jpg");
}

bool Player::isGood(){
	if(settings.GetMainPlayersMinGames()>games)
		return false;
	if(settings.GetMainPlayersMinKills()>kills[0])
		return false;
	if(settings.GetMainPlayersMinTime()>time_sec)
		return false;
	return true;
}


void Player::GetVariableValue(string *variable, string *value){
	string begining, end;
	int pos;
	char numbuf[50];

	value->erase();
	pos=variable->find_first_of('.');
	if(pos==-1){

		if(!strcmp(variable->c_str(),"NAME") && name!=NULL){
			name->Write(value);
		}else if(!strcmp(variable->c_str(),"AVEPING")){
			(*value)=itoa(GetAvePing(),numbuf,10);
		}else if(!strcmp(variable->c_str(),"AVEHC")){
			(*value)=itoa(GetAveHc(),numbuf,10);
		}else if(!strcmp(variable->c_str(),"STARTRANK")){
			(*value)=itoa(startrank,numbuf,10);
		}else if(!strcmp(variable->c_str(),"RANK")){
			(*value)=itoa(rank,numbuf,10);
		}else if(!strcmp(variable->c_str(),"CUSTSTARTRANK")){
			(*value)=itoa(custstartrank,numbuf,10);
		}else if(!strcmp(variable->c_str(),"CUSTRANK")){
			(*value)=itoa(custrank,numbuf,10);
		}else if(!strcmp(variable->c_str(),"AVEBOTSKILL")){
			sprintf(numbuf,"%.2f",(games>0)?(skill)/(float)games:0.0);
			(*value)=numbuf;
		}else if(!strcmp(variable->c_str(),"MODEL")){
			(*value)=model;
		}else if(!strcmp(variable->c_str(),"MODELIMG")){
			GetModelImgName(numbuf);
			(*value)=numbuf;
		}else if(!strcmp(variable->c_str(),"TEAMNUM")){
			(*value)=itoa(team,numbuf,10);
		}else if(!strcmp(variable->c_str(),"TEAM")){
			switch(team){
			case TEAM_RED:
				(*value)+="RED";
				break;
			case TEAM_BLUE:
				(*value)+="BLUE";
				break;
			case TEAM_SPECTATOR:
				(*value)+="Spectator";
				break;
			default: break;
			}
		}else if(!strncmp(variable->c_str(),"PERHOUR_",8)){
			(*variable)=variable->substr(8);
			if(time_sec>0)
				GetIntValue(variable,3600/(double)time_sec,value);
		}else if(!strncmp(variable->c_str(),"PERMIN_",7)){
			(*variable)=variable->substr(7);
			if(time_sec>0)
				GetIntValue(variable,60/(double)time_sec,value);
		}else if(!strncmp(variable->c_str(),"PERGAME_",8)){
			(*variable)=variable->substr(8);
			if(games>0)
				GetIntValue(variable,1/(double)games,value);
		}else {
			GetIntValue(variable,1.0,value);
		}
	}else{
		begining=variable->substr(0,pos);
		end=variable->substr(pos+1);
		if(!strcmp(begining.c_str(),"TOTTIME")){
			if(!strcmp(end.c_str(),"DAYS")){
				(*value)=itoa(time_sec/(60*60*24),numbuf,10);
			}else if(!strcmp(end.c_str(),"HOURS")){
				(*value)=itoa((time_sec/(60*60))%24,numbuf,10);
			}else if(!strcmp(end.c_str(),"MINUTES")){
				(*value)=itoa((time_sec/60)%60,numbuf,10);
			}else if(!strcmp(end.c_str(),"SECONDS")){
				(*value)=itoa(time_sec%60,numbuf,10);
			}else if(!strcmp(end.c_str(),"STR")){
				GetTimeStr(numbuf);
				(*value)=numbuf;
			}
		}else if(!strcmp(begining.c_str(),"EFF")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			GetEff(type,numbuf);
			(*value)=numbuf;
		}else if(!strcmp(begining.c_str(),"SKILL")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			GetWepSkill(type,numbuf);
			(*value)=numbuf;
		}else if(!strcmp(begining.c_str(),"NAME") && name!=NULL){
			if(!strcmp(end.c_str(),"HTML")){
				name->Write(value,0);
			}else if(!strcmp(end.c_str(),"VERT")){
				name->Write(value,1);
			}else if(!strcmp(end.c_str(),"PLAIN")){
				name->Write(value,2);
			}else if(!strcmp(end.c_str(),"FILE")){
				GetFileName(value);
			}
		}else if(!strncmp(variable->c_str(),"PERHOUR_",8)){
			(*variable)=variable->substr(8);
			if(time_sec>0)
				GetIntValue(variable,3600/(double)time_sec,value);
		}else if(!strncmp(variable->c_str(),"PERMIN_",7)){
			(*variable)=variable->substr(7);
			if(time_sec>0)
				GetIntValue(variable,60/(double)time_sec,value);
		}else if(!strncmp(variable->c_str(),"PERGAME_",8)){
			(*variable)=variable->substr(8);
			if(games>0)
				GetIntValue(variable,1/(double)games,value);
		}else{
			GetIntValue(variable,1.0,value);
		}
	}

}


void Player::GetIntValue(string *variable,double ratio,string *value){
	string begining, end;
	int pos, intval=0;
	char numbuf[50];

	value->erase();
	pos=variable->find_first_of('.');
	if(pos==-1){

		if(!strcmp(variable->c_str(),"MSGS")){
			intval=msgs;
		}else if(!strcmp(variable->c_str(),"TEAMMSGS")){
			intval=msgs_team;
		}else if(!strcmp(variable->c_str(),"TIME")){
			intval=time_sec;
		}else if(!strcmp(variable->c_str(),"ITEMS")){
			intval=items_total;
		}else if(!strcmp(variable->c_str(),"SCORE")){
			intval=score;
		}else if(!strcmp(variable->c_str(),"GAMES")){
			intval=games;
		}else if(!strcmp(variable->c_str(),"FFAPLACE")){
			intval=placedFFA;
		}else if(!strcmp(variable->c_str(),"FFAWINPLACE")){
			intval=winsFFA+placedFFA;
		}else if(!strcmp(variable->c_str(),"TOTTIME")){
			intval=time_sec;
		}
	}else{
		begining=variable->substr(0,pos);
		end=variable->substr(pos+1);
		if(!strcmp(begining.c_str(),"KILLS")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			intval=GetAggregateKills(kills,type);
		}else if(!strcmp(begining.c_str(),"DEATHS")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			intval=GetAggregateKills(deaths,type);
		}else if(!strcmp(begining.c_str(),"SUICIDES")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			intval=GetAggregateKills(suicides,type);
		}else if(!strcmp(begining.c_str(),"FRAGS")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			intval=GetFrags(type);
		}else if(!strcmp(begining.c_str(),"KILLDIFF")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseKilltypeStr(&end);
			}
			intval=GetKillDiff(type);
		}else if(!strcmp(begining.c_str(),"GAMES")){
			int gtype=GAMETYPE_ALL;
			if(!strcmp(end.c_str(),"FFA")){
				gtype=GAMETYPE_FFA;
			}else if(!strcmp(end.c_str(),"1V1")){
				gtype=GAMETYPE_1V1;
			}else if(!strcmp(end.c_str(),"TDM")){
				gtype=GAMETYPE_TDM;
			}else if(!strcmp(end.c_str(),"CTF")){
				gtype=GAMETYPE_CTF;
			}
			intval=GetGamesOfType(gtype);
		}else if(!strcmp(begining.c_str(),"WINS")){
			int gtype=GAMETYPE_ALL;
			if(!strcmp(end.c_str(),"FFA")){
				gtype=GAMETYPE_FFA;
			}else if(!strcmp(end.c_str(),"1V1")){
				gtype=GAMETYPE_1V1;
			}else if(!strcmp(end.c_str(),"TDM")){
				gtype=GAMETYPE_TDM;
			}else if(!strcmp(end.c_str(),"CTF")){
				gtype=GAMETYPE_CTF;
			}
			intval=GetWins(gtype);
		}else if(!strcmp(begining.c_str(),"LOSSES")){
			int gtype=GAMETYPE_ALL;
			if(!strcmp(end.c_str(),"FFA")){
				gtype=GAMETYPE_FFA;
			}else if(!strcmp(end.c_str(),"1V1")){
				gtype=GAMETYPE_1V1;
			}else if(!strcmp(end.c_str(),"TDM")){
				gtype=GAMETYPE_TDM;
			}else if(!strcmp(end.c_str(),"CTF")){
				gtype=GAMETYPE_CTF;
			}
			intval=GetLosses(gtype);
		}else if(!strcmp(begining.c_str(),"MATCHES")){
			int gtype=GAMETYPE_ALL;
			if(!strcmp(end.c_str(),"FFA")){
				gtype=GAMETYPE_FFA;
			}else if(!strcmp(end.c_str(),"1V1")){
				gtype=GAMETYPE_1V1;
			}else if(!strcmp(end.c_str(),"TDM")){
				gtype=GAMETYPE_TDM;
			}else if(!strcmp(end.c_str(),"CTF")){
				gtype=GAMETYPE_CTF;
			}
			intval=GetMatches(gtype);
		}else if(!strcmp(begining.c_str(),"ITEM")){
			int type;
			if(end.at(0)>='0' && end.at(0)<='9'){
				type=atoi(end.c_str());
			}else{
				type=ParseItemStr(&end);
			}
			intval=items[type];
		}

		
	}

	if(ratio!=1.0){
		sprintf(numbuf,"%.2f",intval*ratio);
	}else{
		itoa(intval,numbuf,10);
	}
	(*value)=numbuf;

}



/*
void Player::WriteStats(string *str, bool game){

	string temp,temp2;
	char numbuf[50];

	(*str).erase();
	(*str)+="<table border=1>\n<TR><TD>";
	name->Write(&temp);
	(*str)+=temp;
	(*str)+="<BR>";
	switch(team){
	case TEAM_RED:
		(*str)+="Team <font color=red>RED</font>";
		break;
	case TEAM_BLUE:
		(*str)+="Team <font color=blue>BLUE</font>";
		break;
	case TEAM_SPECTATOR:
		(*str)+="<font color=white>spectator</font>";
		break;
	default: break;
	}
	(*str)+="<p>";

	//add model image
	(*str)+="<img src=\"images/models/";
	GetModelImgName(numbuf);
	(*str)+=numbuf;
	(*str)+="\"><BR>";
	
	(*str)+="<p>";
	if(!game){
		(*str)+=itoa(games,numbuf,10);
		(*str)+="&nbsp;Games<BR>";

		(*str)+=itoa(FFAGames,numbuf,10);
		(*str)+="&nbsp;FFA,&nbsp;";
		(*str)+=itoa(winsFFA,numbuf,10);
		(*str)+="&nbsp;wins, ";
		(*str)+=itoa(placedFFA,numbuf,10);
		(*str)+="&nbsp;place<BR>";

		(*str)+=itoa(OneV1Games,numbuf,10);
		(*str)+="&nbsp;1v1,&nbsp;";
		(*str)+=itoa(wins1v1,numbuf,10);
		(*str)+="&nbsp;wins<BR>";

		(*str)+=itoa(TDMGames,numbuf,10);
		(*str)+="&nbsp;TDM,&nbsp;";
		(*str)+=itoa(winsTDM,numbuf,10);
		(*str)+="&nbsp;wins<BR>";

		(*str)+=itoa(CTFGames,numbuf,10);
		(*str)+="&nbsp;CTF,&nbsp;";
		(*str)+=itoa(winsCTF,numbuf,10);
		(*str)+="&nbsp;wins<P>";
	}
	int temptime=time_sec;
	if(temptime>(60*60*24)){
		(*str)+=itoa((temptime/(60*60*24)),numbuf,10);
		(*str)+="D, ";
		temptime%=(60*60*24);
	}
	if(temptime>(60*60)){
		(*str)+=itoa((temptime/(60*60)),numbuf,10);
		(*str)+="H, ";
		temptime%=(60*60);
	}
	(*str)+=itoa((temptime/(60)),numbuf,10);
	(*str)+="M, ";
	temptime%=(60);
	(*str)+=itoa((temptime),numbuf,10);
	(*str)+="S";

	(*str)+="<p>";
	(*str)+=itoa((msgs),numbuf,10);
	(*str)+="&nbsp;Msgs<BR>";
	(*str)+=itoa((msgs_team),numbuf,10);
	(*str)+="&nbsp;Team Msgs<BR>";

	(*str)+="<p>";
	(*str)+="Score: ";
	(*str)+=itoa(GetScore(),numbuf,10);
	(*str)+="<BR>";
	(*str)+=itoa((items_total),numbuf,10);
	(*str)+="&nbsp;Items<BR>";
	(*str)+="<p>";
	(*str)+="Ave Ping ";
	(*str)+=itoa(GetAvePing(),numbuf,10);
	(*str)+="<BR>Ave HC ";
	(*str)+=itoa(GetAveHc(),numbuf,10);
	if(GetAveSkill()>=0){
		(*str)+="<BR>Ave Bot Skill ";
		sprintf(numbuf,"%.2f",((float)GetAveSkill())/((float)100));
		(*str)+=numbuf;
	}

	(*str)+="</TD><TD>\n";

	int i;
	//weapons table
	(*str)+="Kill Stats:<BR>\n<table border=1>\n<tr><TD>&nbsp;</TD>";
	//heading
	for(i=0;i<MAX_KILLTYPE;i++){
		(*str)+="<TD>";
		(*str)+=kill_strings[i];
		(*str)+="</TD>";
	}
	(*str)+="</TR>\n<TR><TD>Kills</td>";
	//kills
	for(i=0;i<MAX_KILLTYPE;i++){
		(*str)+="<TD>";
		(*str)+=itoa(kills[i],numbuf,10);
		(*str)+="</TD>";
	}
	(*str)+="</TR>\n<TR><TD>Deaths</td>";
	//deaths
	for(i=0;i<MAX_KILLTYPE;i++){
		(*str)+="<TD>";
		(*str)+=itoa(deaths[i],numbuf,10);
		(*str)+="</TD>";
	}
	(*str)+="</TR>\n<TR><TD>Suicides</td>";
	//suicides
	for(i=0;i<MAX_KILLTYPE;i++){
		(*str)+="<TD>";
		(*str)+=itoa(suicides[i],numbuf,10);
		(*str)+="</TD>";
	}
	(*str)+="</TR>\n<TR><TD>Frags</td>";
	//frags
	for(i=0;i<MAX_KILLTYPE;i++){
		(*str)+="<TD>";
		(*str)+=itoa(GetFrags(i),numbuf,10);
		(*str)+="</TD>";
	}
	(*str)+="</TR>\n<TR><TD>Kill Dif.</td>";
	//kill differential
	for(i=0;i<MAX_KILLTYPE;i++){
		(*str)+="<TD>";
		(*str)+=itoa(GetKillDiff(i),numbuf,10);
		(*str)+="</TD>";
	}
	(*str)+="</TR>\n<TR><TD>Eff %</td>";
	//Eff
	for(i=0;i<MAX_KILLTYPE;i++){
		(*str)+="<TD>";
		GetEff(i,numbuf);
		(*str)+=numbuf;
		(*str)+="</TD>";
	}
	(*str)+="</TR>\n<TR><TD>Skill</td>";
	//Skill
	for(i=0;i<MAX_KILLTYPE;i++){
		(*str)+="<TD>";
		GetWepSkill(i,numbuf);
		(*str)+=numbuf;
		(*str)+="</TD>";
	}
	(*str)+="</TR>\n</table>\n<P>\nItems Picked Up:<BR>\n<table border=1>\n";
	for(i=1;i<ITEM_LAST;i++){
		(*str)+="<TD>";
		(*str)+=item_strings_nice[i];
		(*str)+="</TD>";
	}
	(*str)+="</TR>\n<TR>";
	for(i=1;i<ITEM_LAST;i++){
		(*str)+="<TD>";
		(*str)+=itoa(items[i],numbuf,10);
		(*str)+="</TD>";
	}
	(*str)+="</TR>\n</Table><P>\n";
	
	(*str)+="Rates:\n<table border=1>\n<tr><TD>&nbsp;</TD><TD>Kills</TD><TD>Deaths</TD><TD>Suicides</TD><TD>Frags</TD><TD>Items</TD><TD>Messages</TD><TD>Team Messages</TD></TR>";
	float timeper;
	timeper=((float)(60*60))/(float)time_sec;
	if(timeper>0.0){
		(*str)+="<TR><TD>Per Hour</TD>";
		sprintf(numbuf,"<td>%.2f</TD>",kills[0]*timeper);
		(*str)+=numbuf;
		sprintf(numbuf,"<td>%.2f</TD>",deaths[0]*timeper);
		(*str)+=numbuf;
		sprintf(numbuf,"<td>%.2f</TD>",suicides[0]*timeper);
		(*str)+=numbuf;
		sprintf(numbuf,"<td>%.2f</TD>",GetFrags(0)*timeper);
		(*str)+=numbuf;
		sprintf(numbuf,"<td>%.2f</TD>",items_total*timeper);
		(*str)+=numbuf;
		sprintf(numbuf,"<td>%.2f</TD>",msgs*timeper);
		(*str)+=numbuf;
		sprintf(numbuf,"<td>%.2f</TD>",msgs_team*timeper);
		(*str)+=numbuf;
		(*str)+="</TR>";
	}
	if(!game && games!=0){
		(*str)+="<TR><TD>Per Game</TD>";
		sprintf(numbuf,"<td>%.2f</TD>",kills[0]/(float)games);
		(*str)+=numbuf;
		sprintf(numbuf,"<td>%.2f</TD>",deaths[0]/(float)games);
		(*str)+=numbuf;
		sprintf(numbuf,"<td>%.2f</TD>",suicides[0]/(float)games);
		(*str)+=numbuf;
		sprintf(numbuf,"<td>%.2f</TD>",GetFrags(0)/(float)games);
		(*str)+=numbuf;
		sprintf(numbuf,"<td>%.2f</TD>",items_total/(float)games);
		(*str)+=numbuf;
		sprintf(numbuf,"<td>%.2f</TD>",msgs/(float)games);
		(*str)+=numbuf;
		sprintf(numbuf,"<td>%.2f</TD>",msgs_team/(float)games);
		(*str)+=numbuf;
		(*str)+="</TR>";
	}
	(*str)+="</table>\n";
	
	(*str)+="</table>\n<P>\n\n";

}
*/

