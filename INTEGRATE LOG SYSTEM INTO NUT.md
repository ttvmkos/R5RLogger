



Current to-do list:

        Add more complex parsing and logging conditions for ctf/duo/trip/movement gym

        https://r5r.dev














                        INTEGRATE LOG SYSTEM INTO .NUT FILES 





###########################################################################################

    STEP 1: ADD PLAYLIST VAR BOOLS TO PLAYLISTS FILE (GLOBAL SETTINGS WILL DO)

###########################################################################################


	logging_enabled 								1
	logging_encryption								1
	logging_shipstats 								1





###########################################################################################

    STEP 2: SET UP CONFIG SYSTEM FOR PLAYLIST AT GLOBAL SCOPE 

###########################################################################################



//DEPRECATED
	// The way data is passed to sqvm for integers is weird, so let's convert to an int in squirrel logic:
	/*int function parseStr(string hex) {
		int dec = 0;
		int len = hex.len();
		for (int i = 0; i < len; i++) {
			string char = hex.slice(len - 1 - i, len - i);
			int digit = "0123456789abcdef".find(char);
			if (digit < 0) {
				digit = "0123456789ABCDEF".find(char);
			}
			if (digit < 0) {
				continue;
			}
			dec = (dec + digit * (1 << (4 * i))) % (1 << 28);  // Modulo operation
		}
		return dec;
	}*/


	//startup logger settings
	string function MatchID(){

			return SQMatchID().tostring();
	}

	bool logging_check = false;
	bool enc = false; 
	bool ship = false; 

	void function logsettings() 
	{
		logging_check = false;
		enc = false;
		ship = false;

		if (GetCurrentPlaylistVarBool("logging_enabled", false)) 
		{
			logging_check = true;

			if (GetCurrentPlaylistVarBool("logging_encryption", false)) {
				enc = true; 
			}
			if (GetCurrentPlaylistVarBool("logging_shipstats", false)) {
				ship = true;
			}
		} else {
			logging_check = false;
		}
	}
	// end log setup






###########################################################################################

    STEP 3, INITIATE LOGGING AT START OF MATCH/ROUND

 ##########################################################################################




//start logging for solo br
	logsettings();
	if (logging_check)
	{
		
		SetMatchID();
		// Announce via server Everyone's match id
		sqprint(format(":::::::::::::::: Match started with MatchID: [ %s ] ::::::::::::::::\n`", MatchID()));
		LogEvent(
		format("|| New match started at: %d\n|#MatchID:%s\n", GetUnixTimestamp(), MatchID()), 
		true, // Start of match param 2 must be true to start logging thread
		enc
		);
	} else {
		sqprint("::: Logging disabled -- to enable set in playlists file --");
		}






###########################################################################################

    STEP 4, ADD GAME FINISHED AND CONFIGURE FOR GAMETYPE CLOSING,

    !!! IMPORTANT !!!

    MUST ADD STOPLOGGING FUNCTION AFTER GAME IS COMPLETE, TO SHUTDOWN LOG THREAD 
    AND OPTIONALLY SEND DATA TO STATS SERVER 

    #######################################################################################
    
    THE EXAMPLE PROVIDED IS SOLO BR

    THIS IMPLEMENTATION WILL NEED CHANGED, AND THE FORMAT WILL NEED TO INCLUDE A "TEAM" LINE
    FOR PARSING SYSTEM TO PLACE PLAYERS ACCORDING TO TEAMS, AS WELL AS AN ADDITIONAL 
    INDICATION SUCH AS 1 FOR SOLOS, 2 FOR DUOS, 3 FOR TRIOS.

    &&,playername,placement,timestamp
    && = DEATH

    will be

    &&,playername,placement,timestamp,survival_variant_1-3,team

    for solo br this will change to

    &&,playername,1,timestamp,1,getplayerteam

 ##########################################################################################



		logsettings();
		if (logging_check)
		{
			//mkos: log winner placement - && denotes placement entry for parser
			if( GetPlayerArray_Alive().len() > 0 )
			{			
				LogEvent(
				format("&&,%s,1,,%d\n|| Game ended at %d - Player %s won! \n", player.GetPlayerName(),GetUnixTimestamp(),GetUnixTimestamp(),player.GetPlayerName()),
				false,
				enc
				);
			}
		} //end r5r.dev logging


### 
SOMEWHERE NEAR END GAME
###

	if (logging_check)
	{
		if(!ship){
			sqprint("Shipping to stats server DISABLED -- check playlists file to enable --");
		}
		
		stopLogging(ship); //IMPORTANT 
		// Set to true to send data to api, false to omit api connection.
	}



 ##########################################################################################

    STEP 4, INITIATE LOGGING FOR EVERY KILL

    LIKEWISE, FOR KILLS, WE WILL NEED TO ASSIGN A SURVIVAL VARIANT AND TEAM TO ACCOUNT FOR
    DUOS AND TRIOS

 ##########################################################################################




 	//mkos-log kill - check players real - ^^ denoted a kill for parser
	logsettings();
	if (logging_check)
	{
		if ( IsValid( victim ) && IsValid( attacker ) && victim.IsPlayer() && attacker.IsPlayer() && victim != attacker ) 
		{
			LogEvent(
			format("^^,%s,1,%s,%d\n", attacker.GetPlayerName(),GetNumTeamsRemaining().tostring(),GetUnixTimestamp()),
			false,
			true
			);
		}
	}




##########################################################################################

    STEP 5, INITIATE LOGGING FOR EVERY DEATH

    LIKEWISE, FOR DEATHS, WE WILL NEED TO ASSIGN A SURVIVAL VARIANT AND TEAM TO ACCOUNT FOR
    DUOS AND TRIOS

 ##########################################################################################




 		//mkos-add death placement log - && denotes placement in parser - line 861
		logsettings();
		if (logging_check)
		{
			int placeM = (GetNumTeamsRemaining() == 0) ? 0 : GetNumTeamsRemaining();
			LogEvent(
			format("&&,%s,%d,%s,%d\n", victim.GetPlayerName(), placeM,victim.GetPlayerName(),GetUnixTimestamp()),
			false,
			true
			);
		}//end add placement SOLO BR






##########################################################################################

    STEP 6, REPEAT THE PROCESS FOR EVERY GAMETYPE

    DM/TDM IS MUCH EASIER, WE ONLY NEED TO LOG THE START AND ENDING AND KILLS

##########################################################################################


FOR KILLS & DEATHS AddCallback_OnPlayerKilled
###


		logsettings();
		if (logging_check)
		{
			if ( IsValid( victim ) && IsValid( attacker ) && victim.IsPlayer() && attacker.IsPlayer() && victim != attacker ) 
			{
				LogEvent(
				format("^^,%s,1,%s,%d\n", attacker.GetPlayerName(),GetNumTeamsRemaining().tostring(),GetUnixTimestamp()),
				false,
				true
				);
			}

			//mkos-add death placement log - && denotes placement in parser - line 861
			int placeM = (GetNumTeamsRemaining() == 0) ? 0 : GetNumTeamsRemaining();
			LogEvent(
			format("&&,%s,%d,%s,%d\n", victim.GetPlayerName(), placeM,victim.GetPlayerName(),GetUnixTimestamp()),
			false,
			true
			);
		}



###
FOR START tdm
###



	logsettings();
	if (logging_check)
	{
		//mkos -start-log
		setMatchID();
		// Announce via server Everyone's match id
		sqprint(format(":::::::::::::::: Match started with MatchID: [ %d ] ::::::::::::::::", MatchID()))
		//Start of FIRST log should ALWAYS be passed with a 3rd parameter of true (mkdir check)
		//4th Parameter = Encryption ; Must be enabled to qualify for event/tournament servers

		LogEvent(
		format("|| New match started at: %d\n|#MatchID:%d\n", GetUnixTimestamp(), MatchID()), 
		true,
		true
		);
	} else {
		sqprint("::: Logging disabled -- to enable set in playlists file --");
		}




###
FOR END TDM END GAME NEEDS PLACED TWICE FOR BOTH TIMER END OR SCORE LIMIT END
###



						logsettings();
						if (logging_check)
						{
							//start r5r.dev log finish
								//mkos: log winner placement - && denotes placement entry for parser							
								LogEvent(
								format("|| Game ended at %d\n", GetUnixTimestamp()),
								false,
								true
								);
									if (logging_check)
                                    {
                                        if(!ship){
                                            sqprint("Shipping to stats server DISABLED -- check playlists file to enable --");
                                        }
                                        
                                        stopLogging(ship); //IMPORTANT 
                                        // Set to true to send data to api, false to omit api connection.
                                    }
						}

