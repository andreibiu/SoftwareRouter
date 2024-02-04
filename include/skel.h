#pragma once

#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>     // the L2 protocols
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/select.h>       // according to POSIX.1-2001, POSIX.1-2008
#include <net/ethernet.h>     // ethheader
#include <arpa/inet.h>        // ether_header
#include <netinet/ip_icmp.h>  // icmphdr
#include <netinet/if_ether.h>
#include <net/if_arp.h>       // arphdr
#include <asm/byteorder.h>

#include "namespace.h"
#include "typedefs.h"

#define MAX_LEN 1600  // Note that "buffer" should be at least the MTU size of the interface, e.g. 1500 bytes 
#define ROUTER_NUM_INTERFACES 4

#define DIE(condition, message) \
	do { \
		if ((condition)) { \
			fprintf(stderr, "[%d]: %s\n", __LINE__, (message)); \
			perror(""); \
			exit(1); \
		} \
	} while (0)

BEGIN_SKEL_NAMESPACE

struct Packet
{
	int interface;
	int len;
	uint8 payload[MAX_LEN];
};

void init();
int sendPacket(int interface, Packet& packet);
int getPacket(Packet& packet);
int getInterfaceMac(int interface, uint8* mac);
uint16 checksum(void* vdata, szint length);

/**
* Converts ASCII string to MAC address (colon-delimited format)
* @param txt MAC address as a string (e.g., "00:11:22:33:44:55")
* @param addr Buffer for the MAC address (ETH_ALEN = 6 bytes)
* Returns 0 on success, -1 on failure (e.g., string not a MAC address)
*/
int getMacAddress(const char* txt, uint8* addr);

extern int interfaces[ROUTER_NUM_INTERFACES];

END_SKEL_NAMESPACE
