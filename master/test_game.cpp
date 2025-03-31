// Cradle Engine example master server test game - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

// This program operates in one of two modes - as a game client or a game server.
// In game server mode, which you can access by running:
//
//      test_game <masterAddress> -server <port>
//
// the process creates a master_server_connection to the master server and periodically updates its server status with a random game type and mission string.  If the connection to the master server times out or is disconnected, the server will attempt to reconnect to the master.  If a client requests an arranged connection, the server will accept it 75% of the time, using a GameConnection instance for the arranged connection.  The GameConnection lasts between 15 and 30 seconds (random), at which point the server will force a disconnect with the client.
//
// The client behaves in much the same way.  It will log in to the master server, and upon successful connection will query the master for a list of game servers.  It will pick one of the servers out of the list to connect to randomly and will initiate an arranged connection with that host.  If the connection to the server is rejected, or if it connects succesfully and later disconnects, it will restart the process of requesting a server list from the master.
//
// This example client demonstrates everything you need to know to implement your own master server clients.

#include "core/platform.h"
#include "libtomcrypt/mycrypt.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct mtg;
extern mtg M;

#include "core/mutex.h"
#include "core/assert.h"
#include "core/base_type_declarations.h"
#include "core/base_type_traits.h"
#include "core/construct.h"
#include "core/memory_functions.h"
#include "math/mathConstants.h"


struct mtg { // master test game
    class master_server_connection;
    class GameConnection;

    #include "core/core.h"
    #include "notify_sockets/notify_sockets.h"
    #include "replicator/replicator.h"
    #include "master_interface.h"

    //------------------------------------------------------------------------

    ref_ptr<master_server_connection> gMasterServerConnection;
    ref_ptr<GameConnection> gClientConnection;
    random_generator _random;
    
    bool gIsServer = false;
    bool gQuit = false;

    context _context;
    type_database _type_database;
    indexed_string_table &_string_table;
    ref_ptr<net_interface> _net_interface;
    indexed_string gCurrentGameType;
    indexed_string gCurrentMissionType;

    mtg() : _type_database(&_context), _string_table(_context.get_string_table())
    {
        gCurrentGameType = indexed_string(_string_table, "SomeGameType");
        gCurrentMissionType = indexed_string(_string_table, "SomeMissionType");
        gIsServer = false;
        gQuit = false;
        _context.set_current_on_thread();
    }
    
    class GameConnection : public rpc_connection {
      public:
        declare_dynamic_class()
        enum {
            mtg_game_tok = 0xBEEF1212,
        };
        // The server maintains a linked list of clients...
        GameConnection *mNext;
        GameConnection *mPrev;

        // Time in milliseconds at which we were created.
        time mCreateTime;

        GameConnection(bool is_initiator = false) : rpc_connection(is_initiator) {
            mNext = mPrev = this;
            mCreateTime = time::get_current();
        }

        ~GameConnection() {
            // unlink ourselves if we're in the client list
            mPrev->mNext = mNext;
            mNext->mPrev = mPrev;

            // Tell the user...
            string address_str = get_address_string();
            logf(master, ("%s disconnected", address_str.c_str()));
        }
        string get_address_string() { return get_remote_address().to_string(); }

        /// Adds this connection to the doubly linked list of clients.
        void linkToClientList() {
            mNext = M.gClientList.mNext;
            mPrev = M.gClientList.mNext->mPrev;
            mNext->mPrev = this;
            mPrev->mNext = this;
        }

        void on_connection_established() {
            string my_address = get_address_string();
            logf(master, ("connection to %s - %s established.",
                      is_connection_initiator() ? "server" : "client", my_address.c_str()));

            // If we're a server (ie, being connected to by the client) add this
            // new connection to our list of clients.
            if (!is_connection_initiator())
                linkToClientList();
        }

        static void checkGameTimeouts() {
            // Look for people who have been connected longer than the threshold
            // and disconnect them.

            time currentTime = time::get_current();
            for (GameConnection *walk = M.gClientList.mNext;
                 walk != &M.gClientList; walk = walk->mNext) {
                if (currentTime - walk->mCreateTime > 15000) {
                    byte_buffer_ptr reason = new byte_buffer("You're done!");
                    walk->disconnect(reason);
                    break;
                }
            }
        }

        // Various things that should trigger us to try another server...
        void on_connection_rejected(bit_stream &reject_stream) {
            M.GameConnectionDone();
        }
        void on_connection_timed_out() { M.GameConnectionDone(); }

        void on_connection_disconnected(byte_buffer_ptr &disconnect_message) {
            logf(master, ("Connection to remote host terminated - %s",
                      disconnect_message->get_buffer()));
            M.GameConnectionDone();
        }
    };

    GameConnection gClientList;

    //------------------------------------------------------------------------

    class master_server_connection : public master_server_interface {
        typedef master_server_interface Parent;

        // ID of our current query.
        uint32 mCurrentQueryId;
        
        // remote id of the server we are trying to intro to
        notify_connection_id mRemoteHostId;

        // List of game servers from our last query.
        array<address> mIPList;
        array<notify_connection_id> mIDList;

      public:
        declare_dynamic_class()

            master_server_connection()
            : master_server_interface(true) {
            mCurrentQueryId = 0;
        }

        void startGameTypesQuery() {
            // Kick off a game types query.
            mCurrentQueryId++;
            logf(master, ("initiating game types query - %d", mCurrentQueryId));
            call(&master_server_interface::g2mQueryGameTypes, mCurrentQueryId);
        }

        void m2gQueryGameTypesResponse(uint32 queryId, max_sized_array<indexed_string, GameMissionTypesPerPacket> &gameTypes, max_sized_array<indexed_string, GameMissionTypesPerPacket> &missionTypes) {
            // Ignore old queries...
            if (queryId != mCurrentQueryId)
                return;

            // Inform the user of the results...
            logf(master, (
                "Got game types response - %d game types, %d mission types",
                gameTypes.size(), missionTypes.size()));
            for (uint32 i = 0; i < gameTypes.size(); i++)
                logf(master, ("G(%d): %s", i, gameTypes[i].c_str()));
            for (uint32 i = 0; i < missionTypes.size(); i++)
                logf(master, ("M(%d): %s", i, missionTypes[i].c_str()));

            // when we receive the final list of mission and game types,
            // query for servers:
            if (gameTypes.size() == 0 && missionTypes.size() == 0) {
                // Invalidate old queries
                mCurrentQueryId++;
                mIPList.clear();
                mIDList.clear();

                // And automatically do a server query as well - you may not
                // want to do things in this order in your own clients.
                indexed_string null_str;
                call( &master_server_interface::g2mQueryServers, mCurrentQueryId, 0xFFFFFFFF, 0, 128, 0, 128, 0, null_str, null_str);
            }
        }

       void m2gQueryServersResponse(uint32 queryId, max_sized_array<notify_connection_id, IPMessageAddressCount> &idList, max_sized_array<address, IPMessageAddressCount> &ipList)
       {
           // Only process results from current query...
           if (queryId != mCurrentQueryId)
               return;

           assert(idList.size() == ipList.size());
           // Add the new results to our master result list...
           for (uint32 i = 0; i < ipList.size(); i++) {
               mIPList.push_back(ipList[i]);
               mIDList.push_back(idList[i]);
           }

           // if this was the last response, then echo out the list of servers,
           // and attempt to connect to one of them:
           if (ipList.size() == 0) {
               // Display the results...
               logf(master, ("got a list of servers from the master: %d servers",
                         mIPList.size()));
               for (uint32 i = 0; i < mIPList.size(); i++) {
                   string address_string = mIPList[i].to_string();
                   logf(master, ("  %d: %s", mIDList[i], address_string.c_str()));
               }

               // If we got anything...
               if (mIPList.size()) {
                   // pick a random server to connect to:
                   uint32 index = M._random.random_in_range(0, mIPList.size() - 1);

                   // Invalidate the query...
                   mCurrentQueryId++;
                   mRemoteHostId = mIDList[index];
                   byte_buffer_ptr request_data =
                       new byte_buffer("Hello, World!");
                   // And request an arranged connnection (notice gratuitous
                   // hardcoded payload)
                   call( &master_server_interface::g2mRequestArrangedConnection, mCurrentQueryId, mRemoteHostId, request_data);
                   string addr_string = mIPList[index].to_string();
                   logf(master, ("Requesting arranged connection with %d:%s",
                             mRemoteHostId, addr_string.c_str()));
               } else {
                   logf(master, ("No game servers available... exiting."));
                   M.gQuit = true;
               }
           }
       }

        void m2gClientRequestedArrangedConnection(uint32 requestId, notify_connection_id remote_initiator, max_sized_array<address, MaxNetIFAddresses> &possibleAddresses, byte_buffer_ptr &connectionParameters)
        {
           if (!M.gIsServer || M._random.random_unit_float() > 0.75) {
               // We reject connections about 75% of the time...
               string addr_string = possibleAddresses[0].to_string();
               logf(master, ("Rejecting arranged connection req %d from %s",
                         requestId, addr_string.c_str()));
               call( &master_server_interface::g2mRejectArrangedConnection, requestId, connectionParameters);
           } else {
               // Ok, let's do the arranged connection... just let the
               // net_interface know the intro is accepted. Normal connection
               // process should ensue.
               M._net_interface->accept_introduction(this, remote_initiator);
               logf(master, ("Accepting arranged connection from %s",
                         possibleAddresses[0].to_string().c_str()));
               call( &master_server_interface::g2mAcceptArrangedConnection, requestId, connectionParameters);
           }
       }

       void m2gArrangedConnectionAccepted(uint32 requestId, byte_buffer_ptr &connectionData)
        {
           if (!M.gIsServer && requestId == mCurrentQueryId) {
               logf(master, ("Remote host accepted arranged connection."));
               logf(master, ("  Connect data: %s",
                         connectionData->get_buffer()));
               ref_ptr<GameConnection> conn = new GameConnection(true);
               M._net_interface->connect_introduced(this, mRemoteHostId, conn);
            }
       }

       void m2gArrangedConnectionRejected(uint32 requestId,
                                 byte_buffer_ptr &rejectData) {
           if (!M.gIsServer && requestId == mCurrentQueryId) {
               logf(master, ("Remote host rejected arranged connection..."));
               logf(master, ("Requesting new game types list."));
               startGameTypesQuery();
           }
       }
        
       void m2gSetMOTD(string &the_motd)
        {
           logf(master, ("Received MOTD from master: \"%s\"", the_motd.c_str()));
       }
       void write_connect_request(bit_stream &stream) {
           Parent::write_connect_request(stream);

           stream.write_c_str("MasterServerTestGame", 255); // Game Name
           if (stream.write_bool(M.gIsServer)) {
               write(stream, (uint32)1000);       // CPU speed
               write(stream, (uint32)0xFFFFFFFF); // region code
               write(stream, (uint32)5);          // num bots
               write(stream, (uint32)10);         // num players
               write(stream, (uint32)32);         // max players
               write(stream, (uint32)0);          // info flags

               stream.write_c_str(M.gCurrentGameType.c_str(), 255); // Game type
               stream.write_c_str(M.gCurrentMissionType.c_str(), 255); // Mission type
           }
       }

       void on_connection_established() {
           logf(master, ("Connection to master server established."));
           if (!M.gIsServer)
           {
               startGameTypesQuery();
           }
       }
    };

    void GameConnectionDone() {
        // If we're still talking to the master server...
        if (gMasterServerConnection.is_valid() &&
            gMasterServerConnection->get_connection_state() ==
            net_connection::state_established) {
            // Query again!
            gMasterServerConnection->startGameTypesQuery();
        } else {
            logf(master, ("GameConnectionDone: No connection to master server available - terminating!"));
            gQuit = true;
        }
    }

    int main(int argc, const char **argv)
    {
        //CradleLogEnable(LogNetInterface, true);
        //CradleLogEnable(LogPlatform, true);
        //CradleLogEnable(LogNetBase, true);
        log_enable(master, true);

        // Parse command line arguments...
        if (argc < 2) {
            logf(master, (
                "Usage: masterclient <masterAddress> [-client|-server] [port]"));
            return 0;
        }

        address masterAddress(argv[1]);
        if (argc > 2)
            gIsServer = !strcasecmp(argv[2], "-server");
        uint32 port = 0;
        if (argc > 3)
            port = atoi(argv[3]);

        // Announce master status.
        logf(master, ("%s started - master is at %s.", gIsServer ? "Server" : "Client", masterAddress.to_string().c_str()));
        _net_interface = new net_interface();
        _net_interface->add_connection_type<master_server_connection>(
            master_server_interface::master_server_interface_tok);
        _net_interface->add_connection_type<GameConnection>(GameConnection::mtg_game_tok);
        
        SOCKADDR if_bindaddr;
        address a;
        a.set_port(port);
        a.to_sockaddr(&if_bindaddr);
        _net_interface->bind(if_bindaddr);
        
        if(gIsServer)
            _net_interface->set_allows_connections(true);

        // Initialize the random number generator.
        time value = time::get_current();
        
        _random.add_entropy((const uint8 *) &value, sizeof(value));
        _random.add_entropy((const uint8 *) &value, sizeof(value));
        _random.add_entropy((const uint8 *) &value, sizeof(value));
        _random.add_entropy((const uint8 *) &value, sizeof(value));

        // And start processing.
        while (!gQuit) {
            // If we don't have a connection to the master server, then
            // create one and connect to it.
            if (!gMasterServerConnection.is_valid() ||
                (gMasterServerConnection->get_connection_state() ==
                     net_connection::state_timed_out ||
                 gMasterServerConnection->get_connection_state() ==
                     net_connection::state_disconnected)) {
                logf(master, ("Connecting to master server at %s",
                          masterAddress.to_string().c_str()));
                gMasterServerConnection = new master_server_connection;
                _net_interface->connect(masterAddress, gMasterServerConnection);
            }

            // Make sure to process network traffic and connected clients...
            _net_interface->process_socket();
            _net_interface->check_for_packet_sends();
            GameConnection::checkGameTimeouts();

            // Sleep a bit so we don't saturate the system.
            os_sleep(1);
        }

        // All done!
        return 0;

    }
} M;

class stdout_log_consumer : public mtg::log_consumer
{
public:
    void log_c_str(const char *the_str)
    {
        printf("%s\n", the_str);
    }
} g_stdout_log_consumer;

int main(int argc, const char **argv) {
    return M.main(argc, argv);
}
