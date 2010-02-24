/* crosstable.cpp
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


#include "crosstable.h"

//use SimplePlayerListNode instrad of player list


void SimplePList::AddPlayer(Player *play,char* name_){
	bool done=false;
	SimplePlayerListNode *temp = new SimplePlayerListNode(play,name_);
		
	//insert in sorted order
	if(head==NULL)
		head=temp;
	else{
		if(strcmp(name_,head->GetName().c_str())<0){
			temp->SetNext(head);
			head=temp;
		}else{
			SimplePlayerListNode *ptr1=head,*ptr2=head->GetNext();
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



Player* SimplePList::FindPlayer(char* name){

	SimplePlayerListNode *ptr1=head;
	while(ptr1!=NULL){
		if(!strcmp(ptr1->GetName().c_str(),name)){
			return ptr1->GetPlayer();
		}
		ptr1=ptr1->GetNext();
	}

	return NULL;
}

bool SimplePList::ContainsPlayer(Player *p){

	SimplePlayerListNode *ptr1=head;
	while(ptr1!=NULL){
		if(ptr1->GetPlayer()==p){
			return true;
		}
		ptr1=ptr1->GetNext();
	}

	return false;
}

int SimplePList::GetPos(char *name){
	int i=0;

	SimplePlayerListNode *ptr1=head;
	while(ptr1!=NULL){
		if(!strcmp(ptr1->GetName().c_str(),name)){
			return i;
		}
		ptr1=ptr1->GetNext();
		i++;
	}

	return -1;
}

int SimplePList::GetPos(Player* Play){
	int i=0;

	SimplePlayerListNode *ptr1=head;
	while(ptr1!=NULL){
		if(ptr1->GetPlayer()==Play){
			return i;
		}
		ptr1=ptr1->GetNext();
		i++;
	}

	return -1;
}


bool SimplePList::DupeList(SimplePList *list){
	if(head!=NULL){
		return false;
	}
	if(list->head==NULL){
		return false;
	}

	SimplePlayerListNode *ptr=list->head;
	Player *p;
	while(ptr!=NULL){
		p=ptr->GetPlayer();
		AddPlayer(p,p->GetName());
		ptr = ptr->GetNext();
	}
	return true;

}




bool CrossTable::AddPlayer(Player* p){
	//Player *play = new Player(p->GetName());
	players.AddPlayer(p,p->GetName());
	return true;
}


bool CrossTable::AddValue(Player* killer,Player* killee){
	kl.AddKill(killer,killee);
	return true;
}


void CrossTable::ColorScore(int score, string *str){
	char scorebuf[50];
	int i,val=hilitelevels,relscore;
	
	if(score>=minhilite){
		relscore=score*100/topscore;
		for(i=hilitelevels-1;i>-1;i--){
			if(relscore>=hilitevalues[i])
				val=i;
			else
				break;
		}
	}else{
		if(topscore>0 && score==topscore){
			val=0;
		}
	}
	if(val!=hilitelevels){
		(*str)+="<font color=\"";
		(*str)+=hilitecolors[val];
		(*str)+="\">";
		(*str)+=itoa(score,scorebuf,10);
		(*str)+="</font>";
	}else{
		(*str)+=itoa(score,scorebuf,10);
	}
}

void CrossTable::Write(string* str){
	int *array;//,*array1;
	int max=players.GetCount(),i;
	Player *p;
	int totalsize=(max+1)*(max+1);
	int realsize=max*max;

	//create array
	array = new int[totalsize];
	for(i=0;i<totalsize;i++)
			array[i]=0;

	//fill array
	SimpleKillNode *kln;
	kln=kl.GetFirst();
	if(kln!=NULL){
		do{
			int ker,kee;
			ker=players.GetPos(kln->GetKiller());
			kee=players.GetPos(kln->GetKillee());
			if(kee>-1 && kee<max && ker>-1 && ker<max){
				array[ker*max+kee]++;
			}else{
				printf("Bad kill in cross table\n");
			}
		}while((kln=kl.GetNext())!=NULL);
	}

		//find topscore and toponeper
/*	int topscore, toponeper,temp,pos;
	array1 = new int[totalsize];
	for(i=0;i<totalsize;i++)
			array1[i]=array[i];
*/	/*
	bool change=true;
	while(change){			//bubble sort  -  inefficent
		change=false;
		for(i=1;i<totalsize;i++){
			if(array1[i]>array1[i-1]){
				temp=array1[i];
				array1[i]=array1[i-1];
				array1[i-1]=temp;
				change=true;
			}
		}
	}*/
	/* qsort still takes too long for 533 players!!
	qsort(array1,totalsize,sizeof(int),intcomp);
	*/
/*	//modified insertion sort for only the largest realsize/100+1 elements   - variables reused from below
	temp=0;
	while( temp<((realsize/100)+1) ){
		//find largest score starting at temp
		topscore=0;
		pos=temp;
		for(i=temp;i<(realsize+1);i++){
			if(array1[i]>topscore){
				topscore=array1[i];
				pos=i;
			}
		}
		//move topscore to front (and score at front to pos)
		array1[pos]=array1[temp];
		array1[temp]=topscore;
		temp++;
	}

	//this code was common for the above 3 sort methods
	topscore=array1[0];
	toponeper=array1[realsize/100];
	if(realsize/100==0)
		toponeper=array1[1];
	if(topscore==0) topscore++;
	if(toponeper>0){
		i=realsize/100+1;
		if(toponeper==array1[i]){
			while(i>1&&toponeper==array1[i]){
				i--;
			}
			toponeper=array1[i];
		}
	}
	if(toponeper==0){
		toponeper=1;
	}
	delete [] array1;
*/

	//OK  now we are just finding the top score and coloring any score within cretain percentages of it by calling colorscore;
	topscore=0;
	for(i=0;i<totalsize;i++){
		if(array[i]>topscore){
			topscore=array[i];
		}
	}


	//add array to string
	//headings
	str->erase();
	(*str)+="<TABLE border=1>\n<TR><TD>";
	string temp;
	settings.GetTemplateSrcpath(&temp);
	if(temp.length()>0){
		temp+=os.PathSeparator;
	}
	temp+="killtablecorner.htmlt";
	File* fp=os.GetFile(temp.c_str());
	fp->GetFile(&temp);
	delete fp;
	(*str)+=temp;
	(*str)+="</TD>\n";
	p=players.GetFirst();
	if(p!=NULL){
		do{
			string temp;
			(*str)+="<TD align=center>";
			p->Write(&temp,1);
			(*str)+=temp;
			(*str)+="</TD>";
		}while((p=players.GetNext())!=NULL);
	}
	(*str)+="</TR>\n";

	//write data
	int pos=0;
	p=players.GetFirst();
	if(p!=NULL){
		do{
			string temp;
			(*str)+="<TR><TD>";
			p->Write(&temp);
			(*str)+=temp;
			(*str)+="</TD>";
			int pos2=0;
//			char numbuf[50];
			for(pos2=0;pos2<max;pos2++){
				(*str)+="<TD>";

				ColorScore(array[pos*max+pos2],str);

/*				if(array[pos*max+pos2]==topscore){
					(*str)+="<font color=\"#00ff00\">";
					(*str)+=itoa(array[pos*max+pos2],numbuf,10);
					(*str)+="</font>";
				}else if(array[pos*max+pos2]>=toponeper){
					(*str)+="<font color=\"#c0c000\">";
					(*str)+=itoa(array[pos*max+pos2],numbuf,10);
					(*str)+="</font>";
				}else{
					(*str)+=itoa(array[pos*max+pos2],numbuf,10);
				}*/

				(*str)+="</TD>";
			}
			pos++;
			(*str)+="</TR>\n";

		}while((p=players.GetNext())!=NULL);
	}
	(*str)+="</table>\n";

}

int
#ifdef WIN32
__cdecl
#endif
intcomp(const void* p1,const void* p2){
	long num1,num2;
	num1=(long)p1;
	num2=(long)p2;
	if(p1<p2){
		return 1;
	}else{
		if(p1>p2){
			return -1;
		}else{
			return 0;
		}
	}
}

void CrossTable::WriteRowCompress(string* str){
	int *array;//,*array1;
	int max=players.GetCount(),i;
	Player *p;
	int totalsize=(max+1)*(max+1)+1;
	int realsize = max*max;

	//create array
	array = new int[totalsize];
	for(i=0;i<totalsize;i++)
			array[i]=0;

	//fill array
	SimpleKillNode *kln;
	kln=kl.GetFirst();
	if(kln!=NULL){
		do{
			int ker,kee;
			ker=players.GetPos(kln->GetKiller());
			kee=players.GetPos(kln->GetKillee());
			if(kee>-1 && kee<max && ker>-1 && ker<max){
				array[ker*max+kee]++;
			}else{
//				printf("Bad kill in cross table\n");
				//may not be bad because of players left out by limits will cause this
			}
		}while((kln=kl.GetNext())!=NULL);
	}


	//find topscore and toponeper
/*	int topscore, toponeper,temp,pos;
	array1 = new int[totalsize];
	for(i=0;i<totalsize;i++)
			array1[i]=array[i];
*/	/*
	bool change=true;
	while(change){			//bubble sort  -  inefficent
		change=false;
		for(i=1;i<totalsize;i++){
			if(array1[i]>array1[i-1]){
				temp=array1[i];
				array1[i]=array1[i-1];
				array1[i-1]=temp;
				change=true;
			}
		}
	}*/
	/* qsort still takes too long for 533 players!!
	qsort(array1,totalsize,sizeof(int),intcomp);
	*/
/*	//modified insertion sort for only the largest realsize/100+1 elements   - variables reused from below
	temp=0;
	while( temp<((realsize/100)+1) ){
		//find largest score starting at temp
		topscore=0;
		pos=temp;
		for(i=temp;i<(realsize+1);i++){
			if(array1[i]>topscore){
				topscore=array1[i];
				pos=i;
			}
		}
		//move topscore to front (and score at front to pos)
		array1[pos]=array1[temp];
		array1[temp]=topscore;
		temp++;
	}

	//this code was common for the above 3 sort methods
	topscore=array1[0];
	toponeper=array1[realsize/100];
	if(realsize/100==0)
		toponeper=array1[1];
	if(topscore==0) topscore++;
	if(toponeper>0){
		i=realsize/100+1;
		if(toponeper==array1[i]){
			while(i>1&&toponeper==array1[i]){
				i--;
			}
			toponeper=array1[i];
		}
	}
	if(toponeper==0){
		toponeper=1;
	}
	delete [] array1;
*/

	//OK  now we are just finding the top score and coloring any score within cretain percentages of it by calling colorscore;
	topscore=0;
	for(i=0;i<totalsize;i++){
		if(array[i]>topscore){
			topscore=array[i];
		}
	}

	//add array to string
	//headings
	int pos=0;
	str->erase();
	(*str)+="<TABLE border=1>\n<TR><TD>";
	string temp;
	settings.GetTemplateSrcpath(&temp);
	if(temp.length()>0){
		temp+=os.PathSeparator;
	}
	temp+="killtablecorner.htmlt";
	File* fp=os.GetFile(temp.c_str());
	fp->GetFile(&temp);
	delete fp;
	(*str)+=temp;
	(*str)+="</TD>\n";
	p=players.GetFirst();
	if(p!=NULL){
		do{
			string temp;
			(*str)+="<TD align=center bgcolor=\"";
			switch(pos%3){
			case 1: (*str)+=settings.GetKilltableColumn(1); break;
			case 2: (*str)+=settings.GetKilltableColumn(2); break;
			default:(*str)+=settings.GetKilltableColumn(0); break;
			}
			(*str)+="\">";
			p->Write(&temp,1);
			(*str)+=temp;
			(*str)+="</TD>";
			pos++;
		}while((p=players.GetNext())!=NULL);
	}
	(*str)+="</TR>\n";

	//write data
	pos=0;
	p=players.GetFirst();
	if(p!=NULL){
		do{
			string temp;
			(*str)+="<TR><TD>";
			p->Write(&temp);
			(*str)+=temp;
			(*str)+="</TD>";
			int pos2=0;
			char numbuf[50];
			int count=1;
			for(pos2=0;pos2<max;pos2+=count){
				count=1;
				int val=array[pos*max+pos2];
				while( val==0 && pos2+count<max && array[pos*max+pos2+count]==val)
					count++;
				if(count>1){
					(*str)+="<TD colspan=\"";
					(*str)+=itoa(count,numbuf,10);
					(*str)+="\">&nbsp;</TD>";
				}else{
					if(val==0){
						(*str)+="<TD>&nbsp;</TD>";
					}else{
						(*str)+="<TD bgcolor=\"";
						switch(pos2%3){
						case 1: (*str)+=settings.GetKilltableColumn(1); break;
						case 2: (*str)+=settings.GetKilltableColumn(2); break;
						default:(*str)+=settings.GetKilltableColumn(0); break;
						}
						(*str)+="\">";
						
						ColorScore(array[pos*max+pos2],str);

/*						if(array[pos*max+pos2]==topscore){
							(*str)+="<font color=\"#00ff00\">";
							(*str)+=itoa(array[pos*max+pos2],numbuf,10);
							(*str)+="</font>";
						}else if(array[pos*max+pos2]>=toponeper){
							(*str)+="<font color=\"#c0c000\">";
							(*str)+=itoa(array[pos*max+pos2],numbuf,10);
							(*str)+="</font>";
						}else{
							(*str)+=itoa(array[pos*max+pos2],numbuf,10);
						}*/

						(*str)+="</TD>";
					}
				}
			}
			pos++;
			(*str)+="</TR>\n";

		}while((p=players.GetNext())!=NULL);
	}
	(*str)+="</table>\n";



}


//UTIL functions

void AddKillToTables(CrossTable *killarray,int type, Player *killer,Player *killee){
	//must match GetAggregateKills in os.cpp

	//add normal kill types
	killarray[TOTAL_KILLS].AddValue(killer,killee);
	killarray[type].AddValue(killer,killee);

	//now add to aggregate kill types
	switch(type){
	case SHOTGUN_KILL:
		killarray[A_BULLET_KILL].AddValue(killer,killee);
		killarray[A_HITSCAN_KILL].AddValue(killer,killee);
		break;
	case GAUNTLET_KILL:
		killarray[A_OTHER_KILL].AddValue(killer,killee);
		killarray[A_HITSCAN_KILL].AddValue(killer,killee);
		break;
	case MACHINEGUN_KILL:
		killarray[A_BULLET_KILL].AddValue(killer,killee);
		killarray[A_HITSCAN_KILL].AddValue(killer,killee);
		break;
	case GRENADE_KILL_DIRECT:
		killarray[A_GRENADE_KILL].AddValue(killer,killee);
		killarray[A_EXPLOSIVE_KILL].AddValue(killer,killee);
		killarray[A_NONHITSCAN_KILL].AddValue(killer,killee);
		break;
	case GRENADE_KILL_SPLASH:
		killarray[A_GRENADE_KILL].AddValue(killer,killee);
		killarray[A_EXPLOSIVE_KILL].AddValue(killer,killee);
		killarray[A_NONHITSCAN_KILL].AddValue(killer,killee);
		break;
	case ROCKET_KILL_DIRECT:
		killarray[A_ROCKET_KILL].AddValue(killer,killee);
		killarray[A_EXPLOSIVE_KILL].AddValue(killer,killee);
		killarray[A_NONHITSCAN_KILL].AddValue(killer,killee);
		break;
	case ROCKET_KILL_SPLASH:
		killarray[A_ROCKET_KILL].AddValue(killer,killee);
		killarray[A_EXPLOSIVE_KILL].AddValue(killer,killee);
		killarray[A_NONHITSCAN_KILL].AddValue(killer,killee);
		break;
	case PLASMA_KILL_DIRECT:
		killarray[A_PLASMA_KILL].AddValue(killer,killee);
		killarray[A_ENERGY_KILL].AddValue(killer,killee);
		killarray[A_NONHITSCAN_KILL].AddValue(killer,killee);
		break;
	case PLASMA_KILL_SPLASH:
		killarray[A_PLASMA_KILL].AddValue(killer,killee);
		killarray[A_ENERGY_KILL].AddValue(killer,killee);
		killarray[A_NONHITSCAN_KILL].AddValue(killer,killee);
		break;
	case RAILGUN_KILL:
		killarray[A_OTHER_KILL].AddValue(killer,killee);
		killarray[A_HITSCAN_KILL].AddValue(killer,killee);
		break;
	case LIGHTNING_KILL:
		killarray[A_OTHER_KILL].AddValue(killer,killee);
		killarray[A_HITSCAN_KILL].AddValue(killer,killee);
		break;
	case BFG_KILL_DIRECT:
		killarray[A_BFG_KILL].AddValue(killer,killee);
		killarray[A_ENERGY_KILL].AddValue(killer,killee);
		killarray[A_NONHITSCAN_KILL].AddValue(killer,killee);
		break;
	case BFG_KILL_SPLASH:
		killarray[A_BFG_KILL].AddValue(killer,killee);
		killarray[A_ENERGY_KILL].AddValue(killer,killee);
		killarray[A_NONHITSCAN_KILL].AddValue(killer,killee);
		break;
	case SELF_WATER:
	case SELF_SLIME:
	case SELF_LAVA:
	case SELF_CRUSH:
	case SELF_FALLING:
	case SELF_SUICIDE:
	case SELF_TRIGGER_HURT:
		killarray[A_SUICIDE_KILL].AddValue(killer,killee);
		killarray[A_NONWEP_KILL].AddValue(killer,killee);
		break;
	case TELEFRAG_KILL:
		killarray[A_OTHER_KILL].AddValue(killer,killee);
		killarray[A_NONWEP_KILL].AddValue(killer,killee);
		break;
	default:
		printf("Found bad kill type!\n");
	}
}



