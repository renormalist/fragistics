+--------+
|Q3Logger|
+--------+
BETA 3  version 1.3.0.0  by TroZ

About
--------------------------------------------------------------------------------
Q3Logger is a a Open Source program that will take the games.log file generated 
by a Quake 3:Arena server and generate several webpages containing statistics 
about the games played on that server. There will be one Overall Stats page, one 
Player Stats page, and one page of stats for each game played on the server. 

The main features are Game by Game and 'Lifetime' stats as well as Doom like 
Killer - Killee tables for all kills and also broken up by specific weapons. 

More info is available at http://members.xoom.com\troz_\q3logger\index.html


Installation
--------------------------------------------------------------------------------
Create an empty directory and unzip Q3Logger there.
You can then either copy your games.log file into that directory,
or modify the q3logger.cfg file and change the line that says 'logpath0=' 
to contain the pathto your Quake 3 games.log file.

Then, just double click on Q3logger and in a few seconds the statistics webpages 
will be created in the output directory.  This output is based on templates in
your q3logger's 'templates' directory.  For information on modifying the tempaltes
see the web site.


Q3Logger uses a config file to store several settings.  You can modify these
to change the way Q3Logger works.  Most of the settings are commented in the
config file or their use is clear.  If the config file is deleted, a new one
with default settings will be regenerated, however, all comments will be lost.


Q3Logger can show a image of each level played and the models used by the 
players. Because of the Quake 3 License, I cannot distribute these images. 
However, it isn't too hard to setup yourself. Here's how: 

Inside your Q3Logger directory's output directory, there is an 'images' directory.
Inside the 'images' there are two directories called
'levelshots' and 'models'

You can either take your own screenshots of the levels and models or use images
from inside the Quake 3 pak0.pk3 file.  The instructions for extracting the 
imgaes from the pak file follow.
 
To get the level shots, open the pak0.pk3 file in you quake3/baseq3 directory 
with winzip. Then extract the images in the levelshots directory in the zipfile
to the levelshots directory you created. 

To get the images of the models is a little more difficult. Inside the pak file
are a series of \models\players\* directorys. There is one for each player. In 
each of these directories is 3 or more 'icon_*.tga' files. You first must 
extract these files and convert them to jpg files. I used Paint Shop Pro 
because it has a batch conversion routine to convert them all at once. The 
files also must be renamed. You must replace the word 'icon' in the filename 
with the players name. For example, the icon images in the sarge directory 
should be renamed to sarge_blue.jpg , sarge_default.jpg , sarge_krusade.jpg , 
sarge_red.jpg and sarge_roderic.jpg . Place all the converted, renamed files in 
the models directory. 

For more help with the images, see the example at the webpage. 
http://members.xoom.com/TroZ_/q3logger/index.html


Config File
--------------------------------------------------------------------------------
Q3Logger uses a config file to store several settings.  You can modify these
to change the way Q3Logger works.  Most of the settings are commented in the
config file or their use is clear.  If the config file is deleted, a new one
with default settings will be regenerated, however, all comments will be lost.

 

History
--------------------------------------------------------------------------------
Version 1.3.0.0 - Feb 14 2000
	Ablilty to sort players by any displayable stat (including forumlas!)
	Bar charts on player stat pages.
	Player per minute stats
	#CREDIT# has been updated to the new homepage
	

Version 1.2.0.0 - Jan 30 2000
	Better 1v1 Support - Consecutive match on same map treated as same game.
	Per Map Statistics - Sums up stats for all games on that map.
	Ability to mkae your own stats! - You can now specify formulas on the 
		templates which will be evalulated
	Better handling of player playing time - time spent spectating no longer
		counted as playing


Version 1.1.0.0 - Jan 20 2000
	99% configureable output without modifying the code - template based
		This is a HUGE feature, allowing anyone to custimize the output to
		show just the stats they want!
	All time players killed and players who killed you in player stats
	List of games played for each player in player stats
	MORE!

Version 1.0.2.0 - Jan 8 2000
	Config file (color, hilite, limit, and path settings)
	Support for as many as 1000 log files
	Able to totally remove or only display All Kills Kill Table for Overall Stats
	Win and placed tracking
	Limited support for 1v1, TDM and CTF

Version 1.0.1.0 - Dec 27 1999
	Fixed all known crashes
	Fixed handling of kills for kill tables - you can now have two of the same 
		bot play in the same game and kill table will be correct
	Kill table top scores now hilited
	Overall Kill tables now generated with less html code
	Better handing of game and player time - Game Exit reason listed if known

Version 1.0.0.2  Dec 18 1999
	Fixed the crash in disconnect before connect bug
	Fixed some mishandling of player time in game (still some potential 
		problems though)
	Now Handling Mod_Suicide and Mod_Telefrag kill types - still missing
		type 21

Version 1.0.0.1 origional release  Dec 15 1999
	support for colored names
	overall and per game stats
	killer / killee tables
	level and model images inserted in to html


Thanks
--------------------------------------------------------------------------------
I would like to thank everyone who emailed me for their help in debugging by 
sending in logs that would break Q3Logger or for giving me idea or for just
sending an email of encouragement.  THANK YOU ALL!
	

--------------------------------------------------------------------------------
For more info see: http://members.xoom.com/TroZ_/q3logger/index.html
You can email me at: troz_@xoommail.com