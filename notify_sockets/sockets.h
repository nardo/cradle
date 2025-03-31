// The notify_sockets library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

typedef unsigned notify_connection_id;
static const notify_connection_id invalid_notify_connection = 0;
    
enum net_sockets_constants {
    invalid_notify_connection_id = 0,
    first_valid_notify_connection_id = 1,
    net_sockets_max_datagram_size = 1480,
    net_sockets_max_status_datagram_size = 511,
    net_sockets_max_public_key_size = 512,
    net_sockets_packet_window_size = 31,
    net_sockets_info_packet_first_byte_min = 32,
    net_sockets_info_packet_first_byte_max = 127,
};
enum net_socket_event_type
{
    notify_connection_challenge_response_event_type = 1,
    notify_connection_requested_event_type,
    notify_connection_accepted_event_type,
    notify_connection_timed_out_event_type,
    notify_connection_disconnected_event_type,
    notify_connection_established_event_type,
    notify_connection_packet_event_type,
    notify_connection_packet_notify_event_type,
    net_socket_packet_event_type,
};

enum bind_result
{
    bind_success,
    address_in_use, ///< A process on this computer is already bound to this address.
    address_invalid, ///< The specified address is not valid for this computer
    initialization_failure,
    socket_allocation_failure,
    generic_failure,
};

struct net_socket_event
{
    unsigned event_type;
    notify_connection_id connection;
    notify_connection_id arranger_connection;
    unsigned client_identity;
    unsigned connection_token;
    unsigned key_size;
    unsigned char *key;
    unsigned data_size;
    unsigned char *data;
    unsigned packet_sequence;
    int delivered;
    address source_address;
};

static bool sockets_init()
{
	static bool success = true;
#if defined PLATFORM_WIN32
	static bool initialized = false;

	if(!initialized)
	{
		initialized = true;
		WSADATA startup_wsa_data;
		success = !WSAStartup(0x0202, &startup_wsa_data);
	}
#endif
	return success;
}

static void sockets_unit_test()
{
	address_unit_test();
	udp_socket_unit_test();
}
