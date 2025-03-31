// Cradle Engine example master server - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

#include "core/platform.h"
#include "libtomcrypt/mycrypt.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct ms; // Master Server wrap structure
extern ms M;

// Useful string constants...
static const char *MasterNoSuchHost = "No Such Host";
static const char *MasterRequestTimedOut = "Timed Out";

extern void readConfigFile();
#include "core/mutex.h"
#include "core/assert.h"
#include "core/base_type_declarations.h"
#include "core/base_type_traits.h"
#include "core/construct.h"
#include "core/memory_functions.h"
#include "math/mathConstants.h"

struct ms {
    #include "core/core.h"
    #include "notify_sockets/notify_sockets.h"
    #include "replicator/replicator.h"
    #include "master_interface.h"
    
    context _context;
    type_database _type_database;
    indexed_string_table &_string_table;

    array<char *> MOTDTypeVec;
    array<char *> MOTDStringVec;
    enum {
        DefaultMasterPort = 29005,
    };

    uint32 master_port;

    safe_ptr<net_interface> _net_interface;
    
    ms() : _type_database(&_context), _string_table(_context.get_string_table())
    {
        _context.set_current_on_thread();
    }

    class MissionGameType : public ref_object {
      public:
        MissionGameType(const indexed_string &name) { mName = name; }
        indexed_string mName;
    };

    class master_server_connection;

    class GameConnectRequest {
      public:
        safe_ptr<master_server_connection> initiator;
        safe_ptr<master_server_connection> host;

        uint32 initiatorQueryId;
        uint32 hostQueryId;
        time requestTime;
    };
    array<safe_ptr<MissionGameType> > gMissionTypeList;
    array<safe_ptr<MissionGameType> > gGameTypeList;
    array<GameConnectRequest *> gConnectList;

    class master_server_connection : public master_server_interface {
    public:
        declare_dynamic_class()

      private:
        typedef master_server_interface Parent;

      protected:
        master_server_connection *mNext;
        master_server_connection *mPrev;

        /// @name Connection Info
        ///
        /// General information about this connection.
        ///
        /// @{

        ///
        bool mIsGameServer;    ///< True if this is a game server.
        uint32 mStrikeCount;      ///< Number of "strikes" this connection has... 3
                               ///< strikes and you're out!
        uint32 mLastQueryId;      ///< The last query id for info from this master.
        time mLastActivityTime; ///< The last time we got a request or an update
                               ///< from this host.

        /// A list of connection requests we're working on fulfilling for this connection.
        array<GameConnectRequest *> mConnectList;

        /// @}

        /// @name Server Info
        ///
        /// This info is filled in if this connection maps to a
        /// game server.
        ///
        /// @{

        uint32 mRegionCode; ///< The region code in which this server operates.
        indexed_string
            mGameString;  ///< The unique game string for this server or client.
        uint32 mCPUSpeed;    ///< The CPU speed of this server.
        uint32 mInfoFlags;   ///< Info flags describing this server.
        uint32 mPlayerCount; ///< Current number of players on this server.
        uint32 mMaxPlayers;  ///< Maximum number of players on this server.
        uint32 mNumBots;     ///< Current number of bots on this server.
        ref_ptr<MissionGameType> mCurrentGameType;
        ref_ptr<MissionGameType> mCurrentMissionType;

        void setGameType(const indexed_string &gameType) {
            for (uint32 i = 0; i < M.gGameTypeList.size(); i++) {
                if (M.gGameTypeList[i].is_valid() &&
                    M.gGameTypeList[i]->mName == gameType) {
                    mCurrentGameType = M.gGameTypeList[i];
                    return;
                }
            }
            mCurrentGameType = new MissionGameType(gameType);
            M.gGameTypeList.push_back((MissionGameType *)mCurrentGameType);
        }

        void setMissionType(const indexed_string &missionType) {
            for (uint32 i = 0; i < M.gMissionTypeList.size(); i++) {
                if (M.gMissionTypeList[i].is_valid() &&
                    M.gMissionTypeList[i]->mName == missionType) {
                    mCurrentMissionType = M.gMissionTypeList[i];
                    return;
                }
            }
            mCurrentMissionType = new MissionGameType(missionType);
            M.gMissionTypeList.push_back((MissionGameType *)mCurrentMissionType);
        }

        /// @}

      public:
        /// Constructor initializes the linked list info  with "safe" values so we don't explode if we destruct right away.
        master_server_connection() : master_server_interface(false) {
            mStrikeCount = 0;
            mLastActivityTime = 0;
            mNext = this;
            mPrev = this;
            //setIsAdaptive();
        }

        /// Destructor removes the connection from the doubly linked list of server connections.
        ~master_server_connection() {
            // unlink it if it's in the list
            mPrev->mNext = mNext;
            mNext->mPrev = mPrev;
            string remote_addr =get_remote_address().to_string();
            logf(master, ("%s disconnected", remote_addr.c_str()));
        }

        /// Adds this connection to the doubly linked list of servers.
        void linkToServerList() {
            mNext = M.gServerList.mNext;
            mPrev = M.gServerList.mNext->mPrev;
            mNext->mPrev = this;
            mPrev->mNext = this;
        }

        /// RPC's a list of mission and game types to the requesting client. This function also cleans up any game types from the global lists that are no longer referenced.
        void g2mQueryGameTypes(uint32 queryId)
        {
            logf(master, ("Received Game Types query %d from %d", queryId, get_notify_connection()));
            max_sized_array<indexed_string, GameMissionTypesPerPacket> gameTypes;
            gameTypes.reserve(GameMissionTypesPerPacket);
            max_sized_array<indexed_string, GameMissionTypesPerPacket> missionTypes;
            missionTypes.reserve(GameMissionTypesPerPacket);
            uint32 listSize = 0;

            // Iterate through game types list, culling out any null entries. Add all non-null entries to the gameTypes array.
            for (uint32 i = 0; i < M.gGameTypeList.size();) {
                if (M.gGameTypeList[i].is_null()) {
                    M.gGameTypeList.erase_fast(i);
                    continue;
                }

                gameTypes.push_back(M.gGameTypeList[i]->mName);
                i++;

                listSize++;
                if (listSize >= GameMissionTypesPerPacket) {
                    call( &master_server_interface::m2gQueryGameTypesResponse, queryId, gameTypes, missionTypes);
                    listSize = 0;
                    gameTypes.clear();
                }
            }

            // Iterate through mission types list, culling out any null entries. Add all non-null entries to the missionTypes array.
            for (uint32 i = 0; i < M.gMissionTypeList.size();) {
                if (M.gMissionTypeList[i].is_null()) {
                    M.gMissionTypeList.erase_fast(i);
                    continue;
                }
                missionTypes.push_back(M.gMissionTypeList[i]->mName);
                i++;
                listSize++;
                if (listSize >= GameMissionTypesPerPacket) {
                    call(&master_server_interface::m2gQueryGameTypesResponse, queryId, gameTypes, missionTypes);
                    listSize = 0;
                    gameTypes.clear();
                    missionTypes.clear();
                }
            }

            // Send the last lists to the client.
            call( &master_server_interface::m2gQueryGameTypesResponse, queryId, gameTypes, missionTypes);

            // Send a pair of empty lists to the client to signify that the query is done.
            if (gameTypes.size() || missionTypes.size()) {
                gameTypes.clear();
                missionTypes.clear();
                call( &master_server_interface::m2gQueryGameTypesResponse, queryId, gameTypes, missionTypes);
            }
        }

        /// The query server method builds a piecewise list of servers that match the client's particular filter criteria and sends it to the client, followed by a QueryServersDone RPC.
        void g2mQueryServers (uint32 queryId, uint32 regionMask, uint32 minPlayers, uint32 maxPlayers, uint32 infoFlags, uint32 maxBots, uint32 minCPUSpeed, indexed_string &gameType, indexed_string &missionType)
        {
            logf(master, ("Received Server query %d from %d", queryId, get_notify_connection()));

            max_sized_array<address, IPMessageAddressCount> address_vector;
            address_vector.reserve(IPMessageAddressCount);
            max_sized_array<notify_connection_id, IPMessageAddressCount> id_vector;
            id_vector.reserve(IPMessageAddressCount);

            for (master_server_connection *walk = M.gServerList.mNext;
                 walk != &M.gServerList; walk = walk->mNext) {
                // Skip to the next if we don't match on any particular...
                if (walk->mGameString != mGameString)
                    continue;
                if (!(walk->mRegionCode & regionMask))
                    continue;
                if (walk->mPlayerCount > maxPlayers ||
                    walk->mPlayerCount < minPlayers)
                    continue;
                if (infoFlags & ~walk->mInfoFlags)
                    continue;
                if (maxBots < walk->mNumBots)
                    continue;
                if (minCPUSpeed > walk->mCPUSpeed)
                    continue;
                if (gameType.valid() &&
                    (gameType != walk->mCurrentGameType->mName))
                    continue;
                if (missionType.valid() &&
                    (missionType != walk->mCurrentMissionType->mName))
                    continue;

                // Somehow we matched! Add us to the results list.
                address_vector.push_back(walk->get_remote_address());
                id_vector.push_back(walk->get_notify_connection());

                // If we get a packet's worth, send it to the client and empty our buffer...
                if (address_vector.size() == IPMessageAddressCount) {
                    call( &master_server_interface::m2gQueryServersResponse, queryId, id_vector, address_vector);
                    address_vector.clear();
                    id_vector.clear();
                }
            }
            call( &master_server_interface::m2gQueryServersResponse, queryId, id_vector, address_vector);
            // If we sent any with the previous message, send another list with no servers.
            if (address_vector.size()) {
                address_vector.clear();
                id_vector.clear();
                call( &master_server_interface::m2gQueryServersResponse, queryId, id_vector, address_vector);
            }
        }

        /// checkActivityTime validates that this particular connection is not issuing too many requests at once in an attempt to DOS by flooding either the master server or any other server connected to it.  A client whose last activity time falls within the specified delta gets a strike... 3 strikes and you're out!  Strikes go away after being good for a while.
        void checkActivityTime(uint32 timeDeltaMinimum) {
            time currentTime = time::get_current();
            if (currentTime - mLastActivityTime < timeDeltaMinimum) {
                mStrikeCount++;
                if (mStrikeCount == 3)
                {
                    byte_buffer_ptr reason = new byte_buffer("You're out!");
                    disconnect(reason);
                }
            } else if (mStrikeCount > 0)
                mStrikeCount--;
        }

        void removeConnectRequest(GameConnectRequest *gcr) {
            for (uint32 j = 0; j < mConnectList.size(); j++) {
                if (gcr == mConnectList[j]) {
                    mConnectList.erase_fast(j);
                    break;
                }
            }
        }

        GameConnectRequest *findAndRemoveRequest(uint32 requestId) {
            GameConnectRequest *req = NULL;
            for (uint32 j = 0; j < mConnectList.size(); j++) {
                if (mConnectList[j]->hostQueryId == requestId) {
                    req = mConnectList[j];
                    mConnectList.erase_fast(j);
                    break;
                }
            }
            if (!req)
                return NULL;

            if (req->initiator.is_valid())
                req->initiator->removeConnectRequest(req);
            for (uint32 j = 0; j < M.gConnectList.size(); j++) {
                if (M.gConnectList[j] == req) {
                    M.gConnectList.erase_fast(j);
                    break;
                }
            }
            return req;
        }

        // This is called when a client wishes to arrange a connection with a server.
        void g2mRequestArrangedConnection(uint32 requestId, notify_connection_id remote_host, byte_buffer_ptr &connectionParameters)
        {
            // First, make sure that we're connected with the server that
            // they're requesting a connection with.
            master_server_connection *conn =
                (master_server_connection *) M._net_interface->find_connection(remote_host);
            if (!conn) {
                byte_buffer_ptr ptr = new byte_buffer(MasterNoSuchHost);
                call( &master_server_interface::g2mRejectArrangedConnection, requestId, ptr);
                return;
            }

            // Record the request...
            GameConnectRequest *req = new GameConnectRequest;
            req->initiator = this;
            req->host = conn;
            req->initiatorQueryId = requestId;
            req->hostQueryId = mLastQueryId++;
            req->requestTime = time::get_current();

            string my_addr = get_remote_address().to_string();
            string remote_addr = conn->get_remote_address().to_string();
            logf(master, ("Client: %s requested connection to %s", my_addr.c_str(), remote_addr.c_str()));

            // Add the request to the relevant lists (the global list, this connection's list, and the other connection's list).
            mConnectList.push_back(req);
            conn->mConnectList.push_back(req);
            M.gConnectList.push_back(req);

            // Do some DOS checking...
            checkActivityTime(2000);
            
            max_sized_array<address, MaxNetIFAddresses> possible_addresses;
            possible_addresses.push_back(get_remote_address());
            // And inform the other party of the request.
            conn->call( &master_server_interface::m2gClientRequestedArrangedConnection,
                req->hostQueryId, get_notify_connection(), possible_addresses, connectionParameters);
        }

        // Called to indicate a connect request is being accepted.
        void g2mAcceptArrangedConnection(uint32 requestId, byte_buffer_ptr &connectionData)
        {
            GameConnectRequest *req = findAndRemoveRequest(requestId);
            if (!req)
                return;
            string server_addr = get_remote_address().to_string();
            string req_addr = req->initiator.is_valid() ? req->initiator->get_remote_address().to_string() : string("Unknown");
            
            logf(master, ("Server: %s accept connection request from %s", server_addr.c_str(), req_addr.c_str()));

            // If we still know about the requestor, pass along the good news...
            if (req->initiator.is_valid())
            {
                req->initiator->call( &master_server_interface::m2gArrangedConnectionAccepted, req->initiatorQueryId, connectionData);
                
                // and tell the net interface that it can broker a connection request from the initiator to the host
                M._net_interface->introduce(req->initiator, this);
            }
            delete req;
        }

        // Called to indicate a connect request is being rejected.
        void g2mRejectArrangedConnection (uint32 requestId, byte_buffer_ptr &rejectData)
        {
            GameConnectRequest *req = findAndRemoveRequest(requestId);
            if (!req)
                return;

            string server_addr = get_remote_address().to_string();
            string req_addr = req->initiator.is_valid() ? req->initiator->get_remote_address().to_string() : string("Unknown");

            logf(master, ("Server: %s reject connection request from %s",
                      server_addr.c_str(), req_addr.c_str()));

            if (req->initiator.is_valid())
                req->initiator->call( &master_server_interface::m2gArrangedConnectionRejected,
                    req->initiatorQueryId, rejectData);

            delete req;
        }

        // Called to update the status of a game server.
        void g2mUpdateServerStatus(indexed_string &gameType, indexed_string &missionType, uint32 botCount, uint32 playerCount, uint32 maxPlayers, uint32 infoFlags)
        {
            // If we didn't know we were a game server, don't accept updates.
            if (!mIsGameServer)
                return;

            setGameType(gameType);
            setMissionType(missionType);
            mNumBots = botCount;
            mPlayerCount = playerCount;
            mMaxPlayers = maxPlayers;
            mInfoFlags = infoFlags;

            checkActivityTime(15000);
            string server_addr = get_remote_address().to_string();
            logf(master, ("Server: %s updated server status (%s, %s, %d, %d, %d)",
                      server_addr.c_str(), gameType.c_str(),
                      missionType.c_str(), botCount, playerCount,
                      maxPlayers));
        }

        bool read_connect_request(address &source_address, bit_stream &bstream, bit_stream &response_stream) {
            if (!Parent::read_connect_request(source_address, bstream, response_stream))
                return false;

            char gameString[256];
            bstream.read_c_str(gameString, sizeof(gameString) - 1);
            mGameString = indexed_string(M._string_table, gameString);
            string addr_string = get_remote_address().to_string();

            // If it's a game server, read status info...
            if ((mIsGameServer = bstream.read_bool()) == true) {
                char infoString[256];
                read(bstream, mCPUSpeed);
                read(bstream, mRegionCode);
                read(bstream, mNumBots);
                read(bstream, mPlayerCount);
                read(bstream, mMaxPlayers);
                read(bstream, mInfoFlags);
                bstream.read_c_str(infoString, sizeof(infoString) - 1);

                setGameType(indexed_string(M._string_table, infoString));
                bstream.read_c_str(infoString, sizeof(infoString) - 1);
                setMissionType(indexed_string(M._string_table, infoString));

                linkToServerList();
                logf(master, ("%s - Server online: %d %d %d %d %d %d %s %s", addr_string.c_str(), mCPUSpeed, mRegionCode, mNumBots, mPlayerCount, mMaxPlayers, mInfoFlags, mCurrentGameType->mName.c_str(), mCurrentMissionType->mName.c_str()));
            }
            else
            {
                    logf(master, ("%s - Client online.", addr_string.c_str()));
            }
            uint32 matchLen = 0;
            const char *motdString = "Welcome to the Cradle Master Server.  Have a nice day.";
            for (uint32 i = 0; i < M.MOTDTypeVec.size(); i++) {
                uint32 len;
                const char *type = M.MOTDTypeVec[i];
                for (len = 0; type[len] == gameString[len] && type[len] != 0;
                     len++)
                    ;
                if (len > matchLen) {
                    matchLen = len;
                    motdString = M.MOTDStringVec[i];
                }
            }
            string the_motd(motdString);
            call( &master_server_interface::m2gSetMOTD, the_motd);

            return true;
        }

        static void checkConnectTimeouts() {
            time currentTime = time::get_current();

            // Expire any connect requests that have grown old...
            for (uint32 i = 0; i < M.gConnectList.size();) {
                GameConnectRequest *gcr = M.gConnectList[i];
                if (currentTime - gcr->requestTime > ConnectRequestTimeout) {
                    // It's old!

                    // So remove it from the initiator's list...
                    if (gcr->initiator.is_valid()) {
                        gcr->initiator->removeConnectRequest(gcr);
                        byte_buffer_ptr reqTimeoutBuffer =
                            new byte_buffer(MasterRequestTimedOut);
                        gcr->initiator->m2gArrangedConnectionRejected (
                            gcr->initiatorQueryId, reqTimeoutBuffer);
                    }

                    // And the host's lists..
                    if (gcr->host.is_valid())
                        gcr->host->removeConnectRequest(gcr);

                    // Delete it...
                    delete gcr;

                    // And remove it from our list, too.
                    M.gConnectList.erase_fast(i);
                    continue;
                }

                i++;
            }
        }
    };
    master_server_connection gServerList;
    
    int main(int argc, const char **argv) {
        master_port = DefaultMasterPort;
        
        // Parse command line parameters...
        readConfigFile();
        log_enable(notify_socket, true);
        //log_enable(udp_socket, true);
        //log_enable(net_connection, true);
        //log_enable(notify_connection, true);
        //log_enable(core, true);
        //log_enable(journal, true);
        //log_enable(cradle, true);
        //log_enable(replicant_connection, true);
        //log_enable(rpc_connection, true);
        log_enable(master, true);
        // Initialize our net interface so we can accept connections...
        _net_interface = new net_interface();
        _net_interface->add_connection_type<master_server_connection>(master_server_interface::master_server_interface_tok);
        SOCKADDR if_bindaddr;
        address a;
        a.set_port(master_port);
        a.to_sockaddr(&if_bindaddr);
        _net_interface->bind(if_bindaddr);

        // for the master server alone, we don't need a key exchange - that would be
        // a waste gNetInterface->setRequiresKeyExchange(true);
        // gNetInterface->setPrivateKey(new AsymmetricKey(20));

        logf(master, ("Master Server created - listening on port %d", master_port));

        // And until infinity, process whatever comes our way.
        time lastConfigReadTime = time::get_current();

        for (;;) {
            time currentTime = time::get_current();
            _net_interface->process_socket();
            _net_interface->check_for_packet_sends();
            if (currentTime - lastConfigReadTime > 5000) {
                lastConfigReadTime = currentTime;
                readConfigFile();
            }
            os_sleep(1000);
        }
        return 0;
    }

} M;

#include "master_config.h"

class stdout_log_consumer : public ms::log_consumer
{
public:
    void log_c_str(const char *the_str)
    {
        printf("%s\n", the_str);
    }
} g_stdout_log_consumer;

int main(int argc, const char **argv)
{
    return M.main(argc, argv);
}

