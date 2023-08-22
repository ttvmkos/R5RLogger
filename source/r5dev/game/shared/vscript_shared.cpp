//=============================================================================//
//
// Purpose: Expose native code to VScript API
// 
//-----------------------------------------------------------------------------
// 
// Create functions here under the target VM namespace. If the function has to
// be registered for 2 or more VM's, put them under the 'SHARED' namespace. 
// Ifdef them out for 'DEDICATED' / 'CLIENT_DLL' if the target VM's do not 
// include 'SERVER' / 'CLIENT'.
//
//=============================================================================//

#include "core/stdafx.h"
#include "vpc/keyvalues.h"
#ifndef CLIENT_DLL
#include "engine/server/server.h"
#endif // CLIENT_DLL
#include "engine/cmodel_bsp.h"
#include "engine/host_state.h"
#include "networksystem/pylon.h"
#ifndef CLIENT_DLL
#include "networksystem/bansystem.h"
#endif // !CLIENT_DLL
#ifndef DEDICATED
#include "networksystem/listmanager.h"
#endif // !DEDICATED
#include "vscript_shared.h"
#include "vscript/languages/squirrel_re/include/sqvm.h"
#include <random>
#include "game/server/logger.h"




namespace VScriptCode
{
    namespace SHARED
    {

        //-----------------------------------------------------------------------------
       // Generate / get usable matchID 
       //-----------------------------------------------------------------------------
    
        std::atomic<int64_t> g_MatchID{0};

        //not exposed to sqvm
        void setMatchID(int64_t newID) {
            g_MatchID = newID;
        }

        //not exposed to sqvm
        int64_t VScriptCode::SHARED::getMatchID() {
            return g_MatchID;
        }


        // exposed to sqvm - retrieves matchID
        SQRESULT SHARED::SQMatchID(HSQUIRRELVM v) {
            std::string matchIDStr = std::to_string(g_MatchID);
            sq_pushstring(v, matchIDStr.c_str(), -1);
            return SQ_OK;
        }

        // exposed to sqvm - Sets new matchid from scripts
        SQRESULT SHARED::SetMatchID(HSQUIRRELVM v) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int64_t> distr(0, INT64_MAX);

            int64_t randomNumber = distr(gen);
            auto now = std::chrono::system_clock::now();
            auto nowAsTimeT = std::chrono::system_clock::to_time_t(now);
            auto nowAsInt64 = static_cast<int64_t>(nowAsTimeT);

            int64_t newID = randomNumber + nowAsInt64;
            setMatchID(newID);

            return SQ_OK;
        }


        // Give control of logger to set match ID if none was set in scripts. 
        int64_t selfSetMatchID() { 
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int64_t> distr(0, INT64_MAX);

            int64_t randomNumber = distr(gen);
            auto now = std::chrono::system_clock::now();
            auto nowAsTimeT = std::chrono::system_clock::to_time_t(now);
            auto nowAsInt64 = static_cast<int64_t>(nowAsTimeT);

            int64_t newID = randomNumber + nowAsInt64;
            setMatchID(newID);

            return newID;
        }


        //-----------------------------------------------------------------------------
       // Purpose: mkos funni io logger
       //-----------------------------------------------------------------------------


        // Check of is currently running -- returns true if not logging, false if running
        SQRESULT isLogging(HSQUIRRELVM v) {

            bool isRunning = LOGGER::Logger::getInstance().isLogging();
            sq_pushbool(v, isRunning);
            return SQ_OK;
        }


        SQRESULT LogEvent(HSQUIRRELVM v)
        {

            if (getMatchID() == 0)
            {
               selfSetMatchID();
            }
            std::string matchIDstr = std::to_string(g_MatchID);

            std::string filename = matchIDstr;
            const SQChar* logString = sq_getstring(v, 1);
            SQBool checkDir = sq_getbool(v, 2);
            SQBool encrypt = sq_getbool(v, 3);


            if (!VALID_CHARSTAR(filename.c_str()) || !VALID_CHARSTAR(logString)) {
                return SQ_ERROR;
            }


            LOGGER::Logger& logger = LOGGER::Logger::getInstance();
            logger.LogEvent(filename.c_str(), logString, checkDir, encrypt);

            return SQ_OK;
        }


        SQRESULT stopLogging(HSQUIRRELVM v) {
            bool doSendToAPI = false;

            SQBool sendToAPI = sq_getbool(v, 1);
            if (sendToAPI) {
                doSendToAPI = sendToAPI != 0;
            }

            DevMsg(eDLL_T::SERVER, "Send to API bool set to: %s \n", doSendToAPI ? "true" : "false");
            LOGGER::Logger::getInstance().stopLogging(doSendToAPI);

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: mkos debug
        //-----------------------------------------------------------------------------

        SQRESULT sqprint(HSQUIRRELVM v) {

            SQChar* sqprintmsg = sq_getstring(v, 1);
            std::ostringstream oss;
            oss << sqprintmsg;

            std::string str = oss.str();
            DevMsg(eDLL_T::SERVER, ":: %s\n", str.c_str());


            return SQ_OK;
        }


        SQRESULT testbool(HSQUIRRELVM v) {
            SQBool sqbool = sq_getbool(v, 1);
            std::string returnvalue = sqbool ? "true" : "false";

            DevMsg(eDLL_T::SERVER, "Bool value: %s \n", returnvalue.c_str());

            sq_pushbool(v, sqbool);

            return SQ_OK;
        }


        //-----------------------------------------------------------------------------
        // Purpose: SDK test and example body
        //-----------------------------------------------------------------------------
        SQRESULT SDKNativeTest(HSQUIRRELVM v)
        {
            // Function code goes here.
            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: expose SDK version to the VScript API
        //-----------------------------------------------------------------------------
        SQRESULT GetSDKVersion(HSQUIRRELVM v)
        {
            sq_pushstring(v, SDK_VERSION, -1);
            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: return all available maps
        //-----------------------------------------------------------------------------
        SQRESULT GetAvailableMaps(HSQUIRRELVM v)
        {
            std::lock_guard<std::mutex> l(g_InstalledMapsMutex);

            if (g_InstalledMaps.empty())
                return SQ_OK;

            sq_newarray(v, 0);
            for (const string& it : g_InstalledMaps)
            {
                sq_pushstring(v, it.c_str(), -1);
                sq_arrayappend(v, -2);
            }

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: return all available playlists
        //-----------------------------------------------------------------------------
        SQRESULT GetAvailablePlaylists(HSQUIRRELVM v)
        {
            std::lock_guard<std::mutex> l(g_PlaylistsVecMutex);

            if (g_vAllPlaylists.empty())
                return SQ_OK;

            sq_newarray(v, 0);
            for (const string& it : g_vAllPlaylists)
            {
                sq_pushstring(v, it.c_str(), -1);
                sq_arrayappend(v, -2);
            }

            return SQ_OK;
        }
        //-----------------------------------------------------------------------------
        // Purpose: checks whether the server is active
        //-----------------------------------------------------------------------------
        SQRESULT IsServerActive(HSQUIRRELVM v)
        {
            bool isActive = false;
#ifndef CLIENT_DLL
            isActive = g_pServer->IsActive();
#endif // !CLIENT_DLL

            sq_pushbool(v, isActive);
            return SQ_OK;
        }
#ifndef CLIENT_DLL

        //-----------------------------------------------------------------------------
        // Purpose: kicks a player by given name
        //-----------------------------------------------------------------------------
        SQRESULT KickPlayerByName(HSQUIRRELVM v)
        {
            SQChar* playerName = sq_getstring(v, 1);
            SQChar* reason = sq_getstring(v, 2);

            // Discard empty strings, this will use the default message instead.
            if (!VALID_CHARSTAR(reason))
                reason = nullptr;

            g_pBanSystem->KickPlayerByName(playerName, reason);

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: kicks a player by given handle or id
        //-----------------------------------------------------------------------------
        SQRESULT KickPlayerById(HSQUIRRELVM v)
        {
            SQChar* playerHandle = sq_getstring(v, 1);
            SQChar* reason = sq_getstring(v, 2);

            // Discard empty strings, this will use the default message instead.
            if (!VALID_CHARSTAR(reason))
                reason = nullptr;

            g_pBanSystem->KickPlayerById(playerHandle, reason);

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: bans a player by given name
        //-----------------------------------------------------------------------------
        SQRESULT BanPlayerByName(HSQUIRRELVM v)
        {
            SQChar* playerName = sq_getstring(v, 1);
            SQChar* reason = sq_getstring(v, 2);

            // Discard empty strings, this will use the default message instead.
            if (!VALID_CHARSTAR(reason))
                reason = nullptr;

            g_pBanSystem->BanPlayerByName(playerName, reason);

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: bans a player by given handle or id
        //-----------------------------------------------------------------------------
        SQRESULT BanPlayerById(HSQUIRRELVM v)
        {
            SQChar* playerHandle = sq_getstring(v, 1);
            SQChar* reason = sq_getstring(v, 2);

            // Discard empty strings, this will use the default message instead.
            if (!VALID_CHARSTAR(reason))
                reason = nullptr;

            g_pBanSystem->BanPlayerById(playerHandle, reason);

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: unbans a player by given nucleus id or ip address
        //-----------------------------------------------------------------------------
        SQRESULT UnbanPlayer(HSQUIRRELVM v)
        {
            SQChar* szCriteria = sq_getstring(v, 1);
            g_pBanSystem->UnbanPlayer(szCriteria);

            return SQ_OK;
        }
#endif // !CLIENT_DLL
        //-----------------------------------------------------------------------------
        // Purpose: shutdown local game (host only)
        //-----------------------------------------------------------------------------
        SQRESULT ShutdownHostGame(HSQUIRRELVM v)
        {
            if (g_pHostState->m_bActiveGame)
                g_pHostState->m_iNextState = HostStates_t::HS_GAME_SHUTDOWN;

            return SQ_OK;
        }
#ifndef DEDICATED
        //-----------------------------------------------------------------------------
        // Purpose: checks whether this SDK build is a client dll
        //-----------------------------------------------------------------------------
        SQRESULT IsClientDLL(HSQUIRRELVM v)
        {
#ifdef CLIENT_DLL
            constexpr SQBool bClientOnly = true;
#else
            constexpr SQBool bClientOnly = false;
#endif
            sq_pushbool(v, bClientOnly);
            return SQ_OK;
        }
#endif // !DEDICATED
    }
#ifndef CLIENT_DLL
    namespace SERVER
    {
        //-----------------------------------------------------------------------------
        // Purpose: gets the number of real players on this server
        //-----------------------------------------------------------------------------
        SQRESULT GetNumHumanPlayers(HSQUIRRELVM v)
        {
            sq_pushinteger(v, g_pServer->GetNumHumanPlayers());
            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: gets the number of fake players on this server
        //-----------------------------------------------------------------------------
        SQRESULT GetNumFakeClients(HSQUIRRELVM v)
        {
            sq_pushinteger(v, g_pServer->GetNumFakeClients());
            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: checks whether this SDK build is a dedicated server
        //-----------------------------------------------------------------------------
        SQRESULT IsDedicated(HSQUIRRELVM v)
        {
            sq_pushbool(v, ::IsDedicated());
            return SQ_OK;
        }
    }
#endif // !CLIENT_DLL
#ifndef DEDICATED
    namespace CLIENT
    {
    }
    namespace UI
    {
        //-----------------------------------------------------------------------------
        // Purpose: refreshes the server list
        //-----------------------------------------------------------------------------
        SQRESULT RefreshServerCount(HSQUIRRELVM v)
        {
            string serverMessage; // Refresh list.
            size_t iCount = g_pServerListManager->RefreshServerList(serverMessage);

            sq_pushinteger(v, static_cast<SQInteger>(iCount));

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: get server's current name from serverlist index
        //-----------------------------------------------------------------------------
        SQRESULT GetServerName(HSQUIRRELVM v)
        {
            std::lock_guard<std::mutex> l(g_pServerListManager->m_Mutex);

            SQInteger iServer = sq_getinteger(v, 1);
            SQInteger iCount = static_cast<SQInteger>(g_pServerListManager->m_vServerList.size());

            if (iServer >= iCount)
            {
                v_SQVM_RaiseError(v, "Index must be less than %i.\n", iCount);
                return SQ_ERROR;
            }

            const string& serverName = g_pServerListManager->m_vServerList[iServer].m_svHostName;
            sq_pushstring(v, serverName.c_str(), -1);

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: get server's current description from serverlist index
        //-----------------------------------------------------------------------------
        SQRESULT GetServerDescription(HSQUIRRELVM v)
        {
            std::lock_guard<std::mutex> l(g_pServerListManager->m_Mutex);

            SQInteger iServer = sq_getinteger(v, 1);
            SQInteger iCount = static_cast<SQInteger>(g_pServerListManager->m_vServerList.size());

            if (iServer >= iCount)
            {
                v_SQVM_RaiseError(v, "Index must be less than %i.\n", iCount);
                return SQ_ERROR;
            }

            const string& serverDescription = g_pServerListManager->m_vServerList[iServer].m_svDescription;
            sq_pushstring(v, serverDescription.c_str(), -1);

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: get server's current map via serverlist index
        //-----------------------------------------------------------------------------
        SQRESULT GetServerMap(HSQUIRRELVM v)
        {
            std::lock_guard<std::mutex> l(g_pServerListManager->m_Mutex);

            SQInteger iServer = sq_getinteger(v, 1);
            SQInteger iCount = static_cast<SQInteger>(g_pServerListManager->m_vServerList.size());

            if (iServer >= iCount)
            {
                v_SQVM_RaiseError(v, "Index must be less than %i.\n", iCount);
                return SQ_ERROR;
            }

            const string& svServerMapName = g_pServerListManager->m_vServerList[iServer].m_svHostMap;
            sq_pushstring(v, svServerMapName.c_str(), -1);

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: get server's current playlist via serverlist index
        //-----------------------------------------------------------------------------
        SQRESULT GetServerPlaylist(HSQUIRRELVM v)
        {
            std::lock_guard<std::mutex> l(g_pServerListManager->m_Mutex);

            SQInteger iServer = sq_getinteger(v, 1);
            SQInteger iCount = static_cast<SQInteger>(g_pServerListManager->m_vServerList.size());

            if (iServer >= iCount)
            {
                v_SQVM_RaiseError(v, "Index must be less than %i.\n", iCount);
                return SQ_ERROR;
            }

            const string& serverPlaylist = g_pServerListManager->m_vServerList[iServer].m_svPlaylist;
            sq_pushstring(v, serverPlaylist.c_str(), -1);

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: get server's current player count via serverlist index
        //-----------------------------------------------------------------------------
        SQRESULT GetServerCurrentPlayers(HSQUIRRELVM v)
        {
            std::lock_guard<std::mutex> l(g_pServerListManager->m_Mutex);

            SQInteger iServer = sq_getinteger(v, 1);
            SQInteger iCount = static_cast<SQInteger>(g_pServerListManager->m_vServerList.size());

            if (iServer >= iCount)
            {
                v_SQVM_RaiseError(v, "Index must be less than %i.\n", iCount);
                return SQ_ERROR;
            }

            const string& playerCount = g_pServerListManager->m_vServerList[iServer].m_svPlayerCount.c_str();
            sq_pushinteger(v, strtol(playerCount.c_str(), NULL, NULL));

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: get server's current player count via serverlist index
        //-----------------------------------------------------------------------------
        SQRESULT GetServerMaxPlayers(HSQUIRRELVM v)
        {
            std::lock_guard<std::mutex> l(g_pServerListManager->m_Mutex);

            SQInteger iServer = sq_getinteger(v, 1);
            SQInteger iCount = static_cast<SQInteger>(g_pServerListManager->m_vServerList.size());

            if (iServer >= iCount)
            {
                v_SQVM_RaiseError(v, "Index must be less than %i.\n", iCount);
                return SQ_ERROR;
            }

            const string& maxPlayers = g_pServerListManager->m_vServerList[iServer].m_svMaxPlayers;
            sq_pushinteger(v, strtol(maxPlayers.c_str(), NULL, NULL));

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: get current server count from pylon
        //-----------------------------------------------------------------------------
        SQRESULT GetServerCount(HSQUIRRELVM v)
        {
            size_t iCount = g_pServerListManager->m_vServerList.size();
            sq_pushinteger(v, static_cast<SQInteger>(iCount));

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: get promo data for serverbrowser panels
        //-----------------------------------------------------------------------------
        SQRESULT GetPromoData(HSQUIRRELVM v)
        {
            enum class R5RPromoData : SQInteger
            {
                PromoLargeTitle,
                PromoLargeDesc,
                PromoLeftTitle,
                PromoLeftDesc,
                PromoRightTitle,
                PromoRightDesc
            };

            R5RPromoData ePromoIndex = static_cast<R5RPromoData>(sq_getinteger(v, 1));
            const char* pszPromoKey;

            switch (ePromoIndex)
            {
            case R5RPromoData::PromoLargeTitle:
            {
                pszPromoKey = "#PROMO_LARGE_TITLE";
                break;
            }
            case R5RPromoData::PromoLargeDesc:
            {
                pszPromoKey = "#PROMO_LARGE_DESCRIPTION";
                break;
            }
            case R5RPromoData::PromoLeftTitle:
            {
                pszPromoKey = "#PROMO_LEFT_TITLE";
                break;
            }
            case R5RPromoData::PromoLeftDesc:
            {
                pszPromoKey = "#PROMO_LEFT_DESCRIPTION";
                break;
            }
            case R5RPromoData::PromoRightTitle:
            {
                pszPromoKey = "#PROMO_RIGHT_TITLE";
                break;
            }
            case R5RPromoData::PromoRightDesc:
            {
                pszPromoKey = "#PROMO_RIGHT_DESCRIPTION";
                break;
            }
            default:
            {
                pszPromoKey = "#PROMO_SDK_ERROR";
                break;
            }
            }

            sq_pushstring(v, pszPromoKey, -1);
            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: create server via native serverbrowser entries
        // TODO: return a boolean on failure instead of raising an error, so we could
        // determine from scripts whether or not to spin a local server, or connect
        // to a dedicated server (for disconnecting and loading the lobby, for example)
        //-----------------------------------------------------------------------------
        SQRESULT CreateServer(HSQUIRRELVM v)
        {
#ifndef CLIENT_DLL
            SQChar* serverName = sq_getstring(v, 1);
            SQChar* serverDescription = sq_getstring(v, 2);
            SQChar* serverMapName = sq_getstring(v, 3);
            SQChar* serverPlaylist = sq_getstring(v, 4);
            EServerVisibility_t eServerVisibility = static_cast<EServerVisibility_t>(sq_getinteger(v, 5));

            if (!VALID_CHARSTAR(serverName) ||
                !VALID_CHARSTAR(serverMapName) ||
                !VALID_CHARSTAR(serverPlaylist))
            {
                return SQ_OK;
            }

            // Adjust browser settings.
            std::lock_guard<std::mutex> l(g_pServerListManager->m_Mutex);

            g_pServerListManager->m_Server.m_svHostName = serverName;
            g_pServerListManager->m_Server.m_svDescription = serverDescription;
            g_pServerListManager->m_Server.m_svHostMap = serverMapName;
            g_pServerListManager->m_Server.m_svPlaylist = serverPlaylist;
            g_pServerListManager->m_ServerVisibility = eServerVisibility;

            // Launch server.
            g_pServerListManager->LaunchServer();

            return SQ_OK;
#else
            v_SQVM_RaiseError(v, "\"%s\" is not supported for client builds.\n", "CreateServer");
            return SQ_ERROR;
#endif
        }

        //-----------------------------------------------------------------------------
        // Purpose: connect to server from native server browser entries
        //-----------------------------------------------------------------------------
        SQRESULT ConnectToServer(HSQUIRRELVM v)
        {
            SQChar* ipAddress = sq_getstring(v, 1);
            SQChar* cryptoKey = sq_getstring(v, 2);

            if (!VALID_CHARSTAR(ipAddress) || VALID_CHARSTAR(cryptoKey))
                return SQ_OK;

            DevMsg(eDLL_T::UI, "Connecting to server with ip address '%s' and encryption key '%s'\n", ipAddress, cryptoKey);
            g_pServerListManager->ConnectToServer(ipAddress, cryptoKey);

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: set netchannel encryption key and connect to server
        //-----------------------------------------------------------------------------
        SQRESULT ConnectToListedServer(HSQUIRRELVM v)
        {
            std::lock_guard<std::mutex> l(g_pServerListManager->m_Mutex);

            SQInteger iServer = sq_getinteger(v, 1);
            SQInteger iCount = static_cast<SQInteger>(g_pServerListManager->m_vServerList.size());

            if (iServer >= iCount)
            {
                v_SQVM_RaiseError(v, "Index must be less than %i.\n", iCount);
                return SQ_ERROR;
            }

            const NetGameServer_t& gameServer = g_pServerListManager->m_vServerList[iServer];

            g_pServerListManager->ConnectToServer(gameServer.m_svIpAddress, gameServer.m_svGamePort,
                gameServer.m_svEncryptionKey);

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: request token from pylon and join server with result.
        //-----------------------------------------------------------------------------
        SQRESULT ConnectToHiddenServer(HSQUIRRELVM v)
        {
            SQChar* privateToken = sq_getstring(v, 1);

            if (!VALID_CHARSTAR(privateToken))
                return SQ_OK;

            string hiddenServerRequestMessage;
            NetGameServer_t netListing;

            bool result = g_pMasterServer->GetServerByToken(netListing, hiddenServerRequestMessage, privateToken); // Send token connect request.
            if (result)
            {
                g_pServerListManager->ConnectToServer(netListing.m_svIpAddress, netListing.m_svGamePort, netListing.m_svEncryptionKey);
            }
            else
            {
                Warning(eDLL_T::UI, "Failed to connect to private server: %s\n", hiddenServerRequestMessage.c_str());
            }

            return SQ_OK;
        }

        //-----------------------------------------------------------------------------
        // Purpose: get response from private server request
        //-----------------------------------------------------------------------------
        SQRESULT GetHiddenServerName(HSQUIRRELVM v)
        {
            SQChar* privateToken = sq_getstring(v, 1);

            if (!VALID_CHARSTAR(privateToken))
                return SQ_OK;

            string hiddenServerRequestMessage;
            NetGameServer_t serverListing;

            bool result = g_pMasterServer->GetServerByToken(serverListing, hiddenServerRequestMessage, privateToken); // Send token connect request.
            if (!result)
            {
                if (hiddenServerRequestMessage.empty())
                {
                    sq_pushstring(v, "Request failed", -1);
                }
                else
                {
                    hiddenServerRequestMessage = Format("Request failed: %s", hiddenServerRequestMessage.c_str());
                    sq_pushstring(v, hiddenServerRequestMessage.c_str(), -1);
                }

                return SQ_OK;
            }

            if (serverListing.m_svHostName.empty())
            {
                if (hiddenServerRequestMessage.empty())
                {
                    hiddenServerRequestMessage = Format("Server listing empty");
                }
                else
                {
                    hiddenServerRequestMessage = Format("Server listing empty: %s", hiddenServerRequestMessage.c_str());
                }

                sq_pushstring(v, hiddenServerRequestMessage.c_str(), -1);
            }
            else
            {
                hiddenServerRequestMessage = Format("Found server: %s", serverListing.m_svHostName.c_str());
                sq_pushstring(v, hiddenServerRequestMessage.c_str(), -1);
            }

            return SQ_OK;
        }
    }
#endif // !DEDICATED
}