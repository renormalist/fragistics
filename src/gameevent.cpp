/* gameevent.cpp
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
   
*/

#include <stdio.h>
#include "defines.h"
#include "gameevent.h"
#include "aux.h"

GameEvent::GameEvent()
{
    event=EVENT_NONE;
    time_min=0;
    time_sec=0;
    player=-1;
    other=ITEM_NONE;
    name[0]='\0';
    model[0]='\0';
    msg.erase();
    team=TEAM_FFA;
    wins=0;
    losses=0;
}

GameEvent::~GameEvent()
{
    // NOP
};

void GameEvent::print ()
{
    printf ("GameEvent: \n");
    printf ("  event=%s\n", Aux::event2str (event));
    printf ("  how=%d\n", how);
    printf ("  time_min=%d\n", time_min);
    printf ("  time_sec=%d\n", time_sec);
    printf ("  player=%d\n", player);
    printf ("  other=%s\n", Aux::item2str (other));
    printf ("  name=%s\n", name);
    printf ("  model=%s\n", model);
    printf ("  msg=%s\n", msg.c_str());
    printf ("  team=%s\n", Aux::team2str (team));
    printf ("  wins=%d\n", wins);
    printf ("  losses=%d\n", losses);
}
