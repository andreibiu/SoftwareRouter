#include "../include/skel.h"

BEGIN_SKEL_NAMESPACE

int getSocket(const char* ifName)
{
	int res;
	int s = socket(AF_PACKET, SOCK_RAW, 768);
	DIE(s == -1, "socket");

	struct ifreq intf;
	strcpy(intf.ifr_name, ifName);
	res = ioctl(s, SIOCGIFINDEX, &intf);
	DIE(res, "ioctl SIOCGIFINDEX");

	struct sockaddr_ll addr;
	memset(&addr, 0x00, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = intf.ifr_ifindex;

	res = bind(s , (struct sockaddr*) &addr , sizeof(addr));
	DIE(res == -1, "bind");
	return s;
}

Packet& receiveMessage(int sockfd, Packet& packet)
{        
	/* 
	 * Note that "buffer" should be at least the MTU size of the 
	 * interface, eg 1500 bytes 
	 * */
	packet.len = read(sockfd, packet.payload, MAX_LEN);
	DIE(packet.len == -1, "read");
	return packet;
}

int sendPacket(int sockfd, Packet& packet)
{        
	/* 
	 * Note that "buffer" should be at least the MTU size of the 
	 * interface, eg 1500 bytes 
	 * */
	int ret;
	ret = write(interfaces[sockfd], packet.payload, packet.len);
	DIE(ret == -1, "write");
	return ret;
}

int getPacket(Packet& packet)
{
	int res;
	fd_set set;

	FD_ZERO(&set);
	while (true)
	{
		for (int i = 0; i < ROUTER_NUM_INTERFACES; i++) 
			FD_SET(interfaces[i], &set);

		res = select(interfaces[ROUTER_NUM_INTERFACES - 1] + 1, &set, NULL, NULL, NULL);
		DIE(res == -1, "select");

		for (int i = 0; i < ROUTER_NUM_INTERFACES; i++)
		{
			if (FD_ISSET(interfaces[i], &set))
			{
				receiveMessage(interfaces[i], packet);
				packet.interface = i;
				return 0;
			}
		}
	}
	return -1;
}

int getInterfaceMac(int interface, uint8* mac)
{
	struct ifreq ifr;
	sprintf(ifr.ifr_name, "r-%u", interface);
	ioctl(interfaces[interface], SIOCGIFHWADDR, &ifr);
	memcpy(mac, ifr.ifr_addr.sa_data, 6);
	return 1;
}

void init()
{
	int s0 = getSocket("r-0");
	int s1 = getSocket("r-1");
	int s2 = getSocket("r-2");
	int s3 = getSocket("r-3");
	interfaces[0] = s0;
	interfaces[1] = s1;
	interfaces[2] = s2;
	interfaces[3] = s3;
}

static int convertHexToNum(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

int convertHexToByte(const char* hex)
{
	int a, b;
	a = convertHexToNum(*hex++);
	if (a < 0)
		return -1;
	b = convertHexToNum(*hex++);
	if (b < 0)
		return -1;
	return (a << 4) | b;
}

int getMacAddress(const char* txt, uint8* addr)
{
	int i;
	for (i = 0; i < 6; i++) {
		int a, b;
		a = convertHexToNum(*txt++);
		if (a < 0)
			return -1;
		b = convertHexToNum(*txt++);
		if (b < 0)
			return -1;
		*addr++ = (a << 4) | b;
		if (i < 5 && *txt++ != ':')
			return -1;
	}
	return 0;
}

uint16 checksum(void* vdata, szint length)
{
	// Cast the data pointer to one that can be indexed.
	char* data = (char*) vdata;

	// Initialise the accumulator.
	uint64 acc = 0xffff;

	// Handle any partial block at the start of the data.
	unsigned int offset = ((szint) data) & 3;
	if (offset)
	{
		szint count = 4 - offset;
		if (count>length)
			count = length;

		uint32 word = 0;
		memcpy(offset+(char*) &word, data, count);
		acc += ntohl(word);
		data += count;
		length -= count;
	}

	// Handle any complete 32-bit blocks.
	char* data_end = data + (length & ~3);
	while (data != data_end)
	{
		uint32 word;
		memcpy(&word, data, 4);
		acc += ntohl(word);
		data += 4;
	}
	length &= 3;

	// Handle any partial block at the end of the data.
	if (length)
	{
		uint32 word=0;
		memcpy(&word, data, length);
		acc += ntohl(word);
	}

	// Handle deferred carries.
	acc = (acc & 0xffffffff) + (acc >> 32);
	while (acc >> 16)
		acc = (acc & 0xffff) + (acc >> 16);

	// If the data began at an odd byte address
	// then reverse the byte order to compensate.
	if (offset & 1)
	{
		acc = ((acc & 0xff00) >> 8) | ((acc & 0x00ff) << 8);
	}

	// Return the checksum in network byte order.
	return htons(~acc);
}

int interfaces[ROUTER_NUM_INTERFACES];

END_SKEL_NAMESPACE
