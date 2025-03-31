// Cradle Engine example master server - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

/// The master_server_interface is the RPC interface to the Cradle Engine example master server. The master server tracks a list of public game servers and allows game clients to query for them based on different filter criteria, including maximum number of players, region codes, game or mission type, and others.
///
/// When a game client wants to initiate a connection with a game server listed by the master, it can ask the master server to arranage a connection.  The test_game example that ships with cradle demonstrates a client/server command line tool that uses the master server to arrange connections between a client and a server instance.
///
/// Client/Server programs using the master server for server listing should create a subclass of master_server_interface named "master_server_connection", and override all of the RPC methods that begin with m2g, as they signify master to game messages.

class master_server_interface : public rpc_connection
{
protected:
public:
    declare_dynamic_class()

    enum {
        master_server_interface_tok = 0xFEED4242,
        ConnectRequestTimeout = 30000,
        IPMessageAddressCount = 30,
        GameMissionTypesPerPacket = 20,
        MaxNetIFAddresses = 3,
    };
    // constructor registers all of the RPCs in the master interface protocol
    master_server_interface(bool is_initiator = false) : rpc_connection(is_initiator)
    {
        register_rpc(&master_server_interface::g2mQueryGameTypes, rpc_guaranteed_ordered, rpc_initiator_to_host);
        register_rpc(&master_server_interface::m2gQueryGameTypesResponse, rpc_guaranteed_ordered, rpc_host_to_initiator);
        register_rpc(&master_server_interface::g2mQueryServers, rpc_guaranteed_ordered, rpc_initiator_to_host);
        register_rpc(&master_server_interface::m2gQueryServersResponse, rpc_guaranteed_ordered, rpc_host_to_initiator);
        register_rpc(&master_server_interface::g2mRequestArrangedConnection, rpc_guaranteed_ordered, rpc_initiator_to_host);
        register_rpc(&master_server_interface::m2gClientRequestedArrangedConnection, rpc_guaranteed_ordered, rpc_host_to_initiator);
      
        register_rpc(&master_server_interface::g2mAcceptArrangedConnection, rpc_guaranteed_ordered, rpc_initiator_to_host);

        register_rpc(&master_server_interface::g2mRejectArrangedConnection, rpc_guaranteed_ordered, rpc_initiator_to_host);
        register_rpc(&master_server_interface::m2gArrangedConnectionAccepted, rpc_guaranteed_ordered, rpc_host_to_initiator);

        register_rpc(&master_server_interface::m2gArrangedConnectionRejected, rpc_guaranteed_ordered, rpc_host_to_initiator);
        register_rpc(&master_server_interface::g2mUpdateServerStatus, rpc_guaranteed_ordered, rpc_initiator_to_host);
        register_rpc(&master_server_interface::m2gSetMOTD, rpc_guaranteed_ordered, rpc_host_to_initiator);
    }
   /// g2mQueryGameTypes is sent from the client to the master to request a list of game and mission types that current game servers are reporting.  The queryId is specified by the client to identify the returning list from the master server.
    
    virtual void g2mQueryGameTypes (uint32 queryId) {}

   /// m2gQueryGameTypesResponse is sent by the master server in response to a c2mQueryGameTypes from a client.  The queryId will match the original queryId sent by the client.  Clients should override this method in their custom master_server_connection classes.  If there are more game or mission types than will fit in a single message, the master server will send multiple m2cQueryGameTypesResponse RPCs.  The master will always send a final m2cQueryGameTypesResponse with Vectors of size 0 to indicate that no more game or mission types are to be added.
    virtual void m2gQueryGameTypesResponse(uint32 queryId, max_sized_array<indexed_string, GameMissionTypesPerPacket> &gameTypes, max_sized_array<indexed_string, GameMissionTypesPerPacket> &missionTypes) { assert(1); }

   /// g2mQueryServers is sent by the client to the master server to request a list of servers that match the specified filter criteria.  A c2mQueryServers request will result in one or more m2cQueryServersResponse RPCs, with the final call having an empty array of servers.
   virtual void g2mQueryServers(uint32 queryId, uint32 regionMask, uint32 minPlayers, uint32 maxPlayers, uint32 infoFlags, uint32 maxBots, uint32 minCPUSpeed, indexed_string &gameType, indexed_string &missionType) { assert(1); }

   /// m2gQueryServersResponse is sent by the master server in response to a c2mQueryServers RPC, to return a partial list of the servers that matched the specified filter criteria.  Because packets are limited in size, the response server list is broken up into lists of at most IPMessageAddressCount IP addresses. The Master Server will always send a final, empty m2cQueryServersResponse to signify that the list is complete.
    virtual void m2gQueryServersResponse(uint32 queryId, max_sized_array<notify_connection_id, IPMessageAddressCount> &idList, max_sized_array<address, IPMessageAddressCount> &ipList) { assert(1); }

   /// g2mRequestArrangedConnection is an RPC sent from the client to the master to request an arranged connection with the specified server address.  The internalAddress should be the client's own self-reported IP address.  The connectionParameters buffer will be sent without modification to the specified server.
   virtual void g2mRequestArrangedConnection(uint32 requestId, notify_connection_id remote_host, byte_buffer_ptr &connectionParameters) { assert(1); }

   /// m2gClientRequestedArranged connection is sent from the master to a server to notify it that a client has requested a connection.  The possibleAddresses array is a list of possible IP addresses that the server should attempt to connect to for that client if it accepts the connection request.
    virtual void m2gClientRequestedArrangedConnection(uint32 requestId, notify_connection_id remote_initiator, max_sized_array<address, MaxNetIFAddresses> &possibleAddresses, byte_buffer_ptr &connectionParameters) {assert(1); }
 
   /// g2mAcceptArrangedConnection is sent by a server to notify the master that it will accept the connection request from a client.  The requestId parameter sent by the MasterServer in m2cClientRequestedArrangedConnection should be sent back as the requestId field.  The internalAddress is the server's self-determined IP address.
    virtual void g2mAcceptArrangedConnection(uint32 requestId, byte_buffer_ptr &connectionData) {assert(1); }

   /// g2mRejectArrangedConnection notifies the Master Server that the server is rejecting the arranged connection request specified by the requestId.  The rejectData will be passed along to the requesting client.
    virtual void g2mRejectArrangedConnection(uint32 requestId, byte_buffer_ptr &rejectData) {assert(1); }

   /// m2gArrangedConnectionAccepted is sent to a client that has previously requested a connection to a listed server via c2mRequestArrangedConnection if the server accepted the connection.  The possibleAddresses array is the list of IP addresses the client should attempt to connect to, and the connectionData buffer is the buffer sent by the server upon accepting the connection.
    virtual void m2gArrangedConnectionAccepted(uint32 requestId, byte_buffer_ptr &connectionData) {assert(1); }

   /// m2gArrangedConnectionRejected is sent to a client when an arranged connection request is rejected by the server, or when the request times out because the server never responded.
    virtual void m2gArrangedConnectionRejected (uint32 requestId, byte_buffer_ptr &rejectData) {assert(1); }

   /// g2mUpdateServerStatus updates the status of a server to the Master Server, specifying the current game and mission types, any player counts and the current info flags.
    virtual void g2mUpdateServerStatus(indexed_string gameType, indexed_string &missionType, uint32 botCount, uint32 playerCount, uint32 maxPlayers, uint32 infoFlags) {assert(1); }

   /// m2gSetMOTD is sent to a client when the connection is established.  The client's game string is used to pick which MOTD will be sent.
    virtual void m2gSetMOTD(string &motdString) {assert(1); }
    
};
