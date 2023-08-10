xt
     					########__########_########_
					##_____##_##_______##_____##
					##_____##_##_______##_____##
					########__#######__########
					##___##_________##_##___##
					##____##__##____##_##____##
					##_____##__######__##_____##
__________________________________________________________________________________________________________________________________
___8_8888_________,o888888o._________,o888888o.________,o888888o.____8_8888888888___8_888888888o.___
___8_8888______._8888_____`88.______8888_____`88._____8888_____`88.__8_8888_________8_8888____`88.__
___8_8888_____,8_8888_______`8b__,8_8888_______`8._,8_8888_______`8._8_8888_________8_8888_____`88__
__8_8888_____88_8888________`8b_88_8888___________88_8888___________8_8888_________8_8888_____,88__
__8_8888_____88_8888_________88_88_8888___________88_8888___________8_888888888888_8_8888.___,88'__
__8_8888_____88_8888_________88_88_8888___________88_8888___________8_8888_________8_888888888P'_
__8_8888_____88_8888________,8P_88_8888___8888888_88_8888___8888888_8_8888_________8_8888`8b_______
___8_8888_____`8_8888_______,8P__`8_8888_______.8'_`8_8888_______.8'_8_8888_________8_8888_`8b._____
____8_8888______`_8888_____,88'______8888_____,88'_____8888_____,88'__8_8888_________8_8888___`8b.___
____8_888888888888_`8888888P'_________`8888888P'________`8888888P'____8_888888888888_8_8888_____`88._
	
					#########################
					#	R5R LOGGER	#
					#########################
					#	Version 1.0.3	#
					#	By R5mkos	#
					#	https://r5r.dev	#	
					#	BETA RELEASE	#
					#	For: 2.2.7 R5R	#
					#########################
					#	<<<<<<<<<<	#
#########################################	<<<<<<<<<<	#
#	REQUIREMENTS				<<<<<<<<<<	#
#################################################################


Version 1.3 only works with 
version rc2.2.7 of the r5reloaded sdk.


You can download this version here
https://github.com/Mauler125/r5sdk/releases/tag/v2.2_rc7_tournaments


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
 Your log name is saved as the MATCH ID.





###########################################################################################

  ADD PLAYLIST VAR BOOLS TO PLAYLISTS FILE (GLOBAL SETTINGS WILL DO)

###########################################################################################


	logging_enabled 								1
	logging_encryption								1
	logging_shipstats 								1



###########################################################################################



The logs can be found in:
{r5reloaded}/platform/eventlogs


The logger uses customizable commands within the gamemodes .nut files.

    LogEvent(
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

Currently the logger and parser is only set up for 

SOLO BR
1V1
FS_DM
TDM




#########################
#	PARSER		#
#########################

Upload your log to 

https://r5r.dev
to see the results!



#########################
#	PARSER		#
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

