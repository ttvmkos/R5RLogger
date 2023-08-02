# R5RLogger
 Server stats tracking for R5Reloaded

					########__########_########_
					##_____##_##_______##_____##
					##_____##_##_______##_____##
					########__#######__########
					##___##_________##_##___##
					##____##__##____##_##____##
					##_____##__######__##_____##
			_______##________#######___######____######___########_########__
			_______##_______##_____##_##____##__##____##__##_______##_____##_
			_______##_______##_____##_##________##________##_______##_____##_
			_______##_______##_____##_##___####_##___####_######___########__
			_______##_______##_____##_##____##__##____##__##_______##___##___
			_______##_______##_____##_##____##__##____##__##_______##____##__
			_______########__#######___######____######___########_##_____##_

	
	
	
	
					#########################
					#	R5R LOGGER	
					#########################
					#	Version 1.0.0	
					#	By R5mkos	#
					#	http://r5.mkos.pro	
					#	For: 2.2.7 R5R	
					#########################
					#	
#####################
#	REQUIREMENTS				
#####################


Version 1.1.0 only works with version 
rc2.2.7 of the r5reloaded sdk.


You can download the latest version of 
r5reloaded by visiting discord.gg/r5reloaded


1. Must have the core s3 apex legends game files.
2. Must be updated to latest r5reloaded SDK with flowstate


https://github.com/M1kep/R5ReloadedInstaller/releases/tag/v0.15.1


#########################
#	Installation	#
#########################


1. Download the latest version of the R5Logger from this repository.
2. Locate the root folder of your R5Reloaded game.
3. Copy ALL the files inside of INSTALL into the R5Reloaded game root folder.
4. Choose overwrite files.


That's it! 
The Logger is now installed.


#########################
#	Usage		#
#########################

The R5RLogger will automatically start logging game data when you start a match. 
DO NOT RENAME YOUR LOGS. Your log name is saved as your MATCH ID.

!!! ONLY WORKS AS HOST !!!


The logs can be found in:
{r5reloaded}/platform/eventlogs


The logger uses customizable commands within the gamemodes .nut files.

    LogEvent(
    "LOG NAME", 
    "LOG DATA", 
    "ENCRYPTION BOOL", 
    "DIRECTORY CHECK BOOL"
);


________________________
Log data is parsed in this way:


&& = placement entry
^^ = kill entry
|| = comment
|# = Match ID

________________________
Format:


&&,playername,players-remaining
^^,playername,kills,players-remaining



________________________
Adding more customization for
different types of logging soon.



#########################
#	GAMEMODES	#
#########################

Currently the logger and parser is only set up for SOLO Survival. (BattleRoyale)
This is version 1.0.0, primarily for testing.


By version 2.0

Expect every game mode supported, more types of data (killed by / dmg etcetc)
Expect flly customizable parser to extreme specifications.
Expect the option to automatically upload logs from your server to the stats tracker master server.
Expect being able to search matches by gamemode/playername/server/date/wins
Expect a highscores page for verified servers running the logger. 


#########################
#	PARSER		#
#########################

Upload your log to 

www.r5.mkos.pro 

to see the results!



#########################
#	LICENSE		#
#########################

License
This project is licensed under the MIT License. See the LICENSE file for details.

Support
If you encounter any problems or have any questions, please open an issue on GitHub.
Alternatively, you can open a ticket or post in #questions on the r5reloaded discord @
http://discord.gg/r5reloaded



Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the “Software”), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.

IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
IN THE SOFTWARE.

