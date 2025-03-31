// The notify_sockets library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

class udp_socket
{
public:
	enum
	{
		default_send_buffer_size = 32768,
		default_recv_buffer_size = 32768,
		max_datagram_size = 1536, ///< some routers have issues with packets larger than this
		recommended_datagram_size = 512,
	};
	
	udp_socket()
	{
		_socket = INVALID_SOCKET;
	}

	~udp_socket()
	{
		unbind();
	}

	bind_result bind(const address bind_address, bool non_blocking_io = true, time recv_timeout = 0, bool accepts_broadcast_packets = true, uint32 send_buffer_size = default_send_buffer_size, uint32 recv_buffer_size = default_recv_buffer_size)
	{
        bind_result result = bind_success;
        
        journal_read_block(bind,
                         journal_read(_socket);
                         journal_read(result);
                         return result;
        )
		if(!sockets_init())
			result = initialization_failure;
        
        if(result == bind_success)
		_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if(_socket == INVALID_SOCKET)
			result = socket_allocation_failure;

		if(result == bind_success)
        {
            SOCKADDR sockaddr;
            bind_address.to_sockaddr(&sockaddr);
            
            if(::bind(_socket, &sockaddr, sizeof(sockaddr)) == SOCKET_ERROR)
            {
                result = generic_failure;
                
#if defined(PLATFORM_WIN32)
                switch(WSAGetLastError())
                {
                    case WSAEADDRINUSE:
                        result = address_in_use;
                        break;
                    case WSAEADDRNOTAVAIL:
                        result = address_invalid;
                        break;
                    case WSAENOBUFS:
                        result = socket_allocation_failure;
                        break;
                }
#else
#endif
                unbind();
            }
        }
		if(result == bind_success && (setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (char *) &recv_buffer_size, sizeof(recv_buffer_size)) == SOCKET_ERROR || setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (char *) &send_buffer_size, sizeof(send_buffer_size)) == SOCKET_ERROR))
		{
			unbind();
			result = socket_allocation_failure;
		}
		int32 accepts_broadcast = accepts_broadcast_packets;
		uint32 not_block = non_blocking_io;

        if(result == bind_success)
        {
#if defined(PLATFORM_WIN32)
            DWORD nb = not_block;
            if(ioctlsocket(_socket, FIONBIO, &nb) ||
#else
            if(ioctl(_socket, FIONBIO, &not_block) ||
#endif
                  setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, (char *) &accepts_broadcast, sizeof(accepts_broadcast)) == SOCKET_ERROR)
            {
                unbind();
                result = generic_failure;
            }
        }
		if(result == bind_success && recv_timeout != 0)
		{
			logf(udp_socket, ("set recv timeout to %d", int(recv_timeout.get_milliseconds())));
			#if defined(PLATFORM_WIN32)
				DWORD ms_delay = recv_timeout.get_milliseconds();
				setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &ms_delay, sizeof(ms_delay));
			#else
				uint32 ms_delay = recv_timeout.get_milliseconds();
				struct timeval tv;
				tv.tv_sec = ms_delay / 1000;
				tv.tv_usec = (ms_delay % 1000) * 1000;
				setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
			#endif
		}
        journal_write_block(bind,
                         journal_write(_socket);
                         journal_write(result);
                         return result;
        )
		return result;
	}

	void unbind()
	{
        journal_read_block(unbind, return;)
        journal_write_block(unbind, ;)
        
		if(_socket != INVALID_SOCKET)
		{
			closesocket(_socket);
			_socket = INVALID_SOCKET;
		}
	}

	address get_bound_address()
	{
        address ret;
        journal_read_block(get_bound_address, journal_read(ret); return ret;);
		SOCKADDR sockaddr;
		socklen_t address_size = sizeof(sockaddr);
		getsockname(_socket, &sockaddr, &address_size);
		ret.from_sockaddr(sockaddr);
        journal_write_block(get_bound_address, journal_write(ret););
		return ret;
	}

	bool is_bound()
	{
		return _socket != INVALID_SOCKET;
	}

	enum send_to_result
	{
		send_to_success,
		send_to_failure,
	};
	send_to_result send_to(const address &the_address, const byte *buffer, uint32 buffer_size)
	{
        send_to_result result;
        journal_read_block(sendto, journal_read(result); return result;)
        
        logf(udp_socket, ("udp socket sending to %s: %d bytes - %s.", the_address.to_string().c_str(), buffer_size, string((const char *) buffer_encode_base_16(buffer, buffer_size)->get_buffer()).c_str()));

		SOCKADDR dest_address;
		the_address.to_sockaddr(&dest_address);
		if(sendto(_socket, (const char *) buffer, int(buffer_size), 0, &dest_address, sizeof(dest_address)) == SOCKET_ERROR)
			result = send_to_failure;
        else
            result = send_to_success;
        journal_write_block(sendto, journal_write(result);)
        return result;
	}

	enum recv_from_result
	{
		packet_received,
		would_block_or_timeout,
		invalid_socket,
		unknown_error,
	};

	recv_from_result recv_from(address *sender_address, byte *buffer, uint32 buffer_size, uint32 *incoming_packet_size)
	{
        recv_from_result result = packet_received;
        journal_read_block(recv_from,
            journal_read(result);
            if(result == packet_received)
            {
                if(sender_address)
                    journal_read(*sender_address);
                journal_read(*incoming_packet_size);
                journal_read_bytes(buffer, *incoming_packet_size);
            }
            return result;
        )
        
		SOCKADDR sender_sockaddr;
		socklen_t addr_len = sizeof(sender_sockaddr);
		int32 bytes_read = recvfrom(_socket, (char *) buffer, buffer_size, 0, &sender_sockaddr, &addr_len);
		//logf(udp_socket, ("recv_from result = %d", errno));
		if(bytes_read == SOCKET_ERROR)
		{
			switch(errno)
			{
				case EAGAIN:
					result = would_block_or_timeout;
				case EBADF:
                    result = invalid_socket;
				default:
                    result = unknown_error;
			}
		}
        if(result == packet_received)
        {
            *incoming_packet_size = uint32(bytes_read);
            
            if(sender_address)
                sender_address->from_sockaddr(sender_sockaddr);
            
            logf(udp_socket, ("udp socket received from %s: %d bytes - %s.", sender_address->to_string().c_str(), *incoming_packet_size, string((const char *) buffer_encode_base_16(buffer, *incoming_packet_size)->get_buffer()).c_str()));
        }
        journal_write_block(recv_from,
            journal_write(result);
            if(result == packet_received)
            {
            if(sender_address)
            {
                journal_write(*sender_address);
            }
            journal_write(*incoming_packet_size);
            journal_write_bytes(buffer, *incoming_packet_size);
            }
        )
		return result;
	}
private:
	SOCKET _socket;
};

#if defined ( PLATFORM_WIN32 )
static void net_query_interface_addresses(array<address>& address_vector)
{
   typedef struct {
      DWORD dwAddr;
      DWORD dwIndex;
      DWORD dwMask;
      DWORD dwBCastAddr;
      DWORD dwReasmSize;
      unsigned short unused1;
      unsigned short unused2;
   } MIB_IPADDRROW;
   typedef struct {
      DWORD dwNumEntries;
      MIB_IPADDRROW table[1];
   } MIB_IPADDRTABLE, *PMIB_IPADDRTABLE;
   typedef DWORD  (WINAPI *GetIPAddrTableFn_t)(PMIB_IPADDRTABLE,PULONG,BOOL);

   static GetIPAddrTableFn_t GetIpAddrTableFn = NULL;
  
   if(!GetIpAddrTableFn)
   {
      HMODULE module = LoadLibrary("iphlpapi.dll");
      if(!module)
         return;

      GetIpAddrTableFn = (GetIPAddrTableFn_t) GetProcAddress(module, "GetIpAddrTable");
   }

   // Under Win32, we use the IP helper library to query all
   // available network interfaces.
   PMIB_IPADDRTABLE pIPAddrTable;
   DWORD dwSize = 0;

   pIPAddrTable = (MIB_IPADDRTABLE*) mem_alloc( sizeof( MIB_IPADDRTABLE) );

   // Make an initial call to GetIpAddrTable to get the
   // necessary size into the dwSize variable
   if (GetIpAddrTableFn(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)
   {
      mem_free( pIPAddrTable );
      pIPAddrTable = (MIB_IPADDRTABLE *) mem_alloc ( dwSize );
   }

   // Make a second call to GetIpAddrTable to get the
   // actual data we want
   DWORD dwRetVal;
   if ( (dwRetVal = GetIpAddrTableFn( pIPAddrTable, &dwSize, 0 )) == NO_ERROR ) {
      for(uint32 i = 0; i < pIPAddrTable->dwNumEntries; i++)
      {
         // construct an Address for this interface
         address a;
         a._port = 0;
         a._host = htonl(pIPAddrTable->table[i].dwAddr);
         // don't add the loopback address or the
         if(a._host != INADDR_ANY && a._host != 0x7F000001)
            address_vector.push_back(a);
      }
   }
   mem_free(pIPAddrTable);
}

#elif defined (PLATFORM_MAC_OSX)
static void net_query_interface_addresses(array<address> &address_vector)
{
   struct ifaddrs *addrs;
   getifaddrs(&addrs);
   
   for(struct ifaddrs *walk = addrs; walk; walk = walk->ifa_next)
   {
       address the_address(*walk->ifa_addr);
      if(the_address._host != INADDR_ANY && the_address._host != 0x7F000001)
      {
         //logf(udp_socket, ("found IF address %s", theAddress.toString()));
          address_vector.push_back(the_address);
      }
   }
   
   freeifaddrs(addrs);
}

#elif defined (PLATFORM_LINUX)

static void net_query_interface_addresses(array<address> &addressVector)
{
   int sfd = socket(AF_INET, SOCK_STREAM, 0);
   if(sfd < 0)
     return;

   FILE *f = fopen("/proc/net/dev", "r");
   if(!f)
   {
      close(sfd);
      return;
   }
   char buf[1024];
   fgets(buf, 1024, f);
   fgets(buf, 1024, f);

   struct ifreq ifr;
   struct sockaddr_in *sin = (struct sockaddr_in *) &ifr.ifr_addr;
   memset(&ifr, 0, sizeof(ifr));
   

   while(fgets(buf, 1024, f))
   {
      char *s = buf;
      while(*s == ' ')
         s++;
      char *end = strchr(s, ':');
      if(!end)
         continue;
      *end = 0;

      strcpy(ifr.ifr_name, s);
      sin->sin_family = AF_INET;
      if(ioctl(sfd, SIOCGIFADDR, &ifr) == 0)
      {
         address theAddress;
	theAddress._host = sin->sin_addr.s_addr;
     if(theAddress._host != INADDR_ANY && theAddress._host != 0x7F000001)
     {
            addressVector.push_back(theAddress);
         }
      }
   }
   fclose(f);
   close(sfd);
}
#endif

static void udp_socket_unit_test()
{
	udp_socket s;
	address bind_address;

	s.bind(bind_address);
	bind_address = s.get_bound_address();

	string addr_string = bind_address.to_string();

	printf("Socket created, bound to address: %s\n", addr_string.c_str());
}
