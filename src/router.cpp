#include <iostream>

#include "../include/parse.h"
#include "../include/router_utils.h"

int main(int argumentsCount, char *arguments[])
{
	if (argumentsCount != 2)
	{
		std::cerr << "Expecting one argument - the path of the routing table\n";
		return 1;
	}
	
	uint32 ip;
	EthernetHeader* headerEthernet;
	Packet packet;
	Queue<Packet> queue;
	SortedList<uint32> arpRequestedIps;

	Skel::init();
	readParseRoutingTable(arguments[1]);

	while (true)
	{
		// Receive packet from network
		packet = Packet(); 
		receivePacket(packet);

		// Get Ethernet header
		headerEthernet = (EthernetHeader*) packet.payload;
	
		// Determine protocol type
		if (ntohs(headerEthernet->ether_type) == ETHERTYPE_ARP)
		{
			// Get ARP header
			ARPHeader* headerARP = (ARPHeader*) (packet.payload + sizeof(EthernetHeader));
			// Determine operation type
			switch (ntohs(headerARP->ea_hdr.ar_op))
			{
				// Process ARP request
				case ARPOP_REQUEST:
					ip = *((uint32*) headerARP->arp_tpa);
					if (isRouterIP(ip, packet.interface))
					{
						getARPReply(packet);
						sendPacket(packet);
					}
					break;
				// Process ARP reply
				case ARPOP_REPLY:
					ip = *((uint32*) headerARP->arp_tpa);
					if (isRouterIP(ip, packet.interface))
					{
						ip = *((uint32*) headerARP->arp_spa);
						arpRequestedIps.remove(ntohl(ip));
						updateARPTable(headerARP);
						sendQueuedPackets(queue, ip, headerARP->arp_sha);
					}
					break;
			}
		}
		else if (ntohs(headerEthernet->ether_type) == ETHERTYPE_IP)
		{
			// Get IP header
			IPHeader* headerIP = (IPHeader*) (packet.payload + sizeof(EthernetHeader));

			if (headerIP->ttl <= 1)
			{
				// Send timeout message
				getICMPReply(ICMP_TIME_EXCEEDED, packet);
				sendPacket(packet);
			}
			else if (!checkIPPacket(headerIP))
			{
				// Throw the packet if checksum is wrong
				continue;
			}
			else if (isRouterIP(headerIP->daddr, packet.interface) &&
				     headerIP->protocol == IPPROTO_ICMP)
			{
				ICMPHeader* headerICMP = (ICMPHeader*) (
					packet.payload + sizeof(EthernetHeader) + sizeof(IPHeader));

				// Realise the echo reply (ping) if needed
				if (headerICMP->type == ICMP_ECHO)
				{
					getICMPReply(ICMP_ECHOREPLY, packet);
					sendPacket(packet);
				}
			}
			else
			{
				// A packet that must be forwarded have been received
				RoutingEntry* entry = RoutingTable::instance.find(headerIP->daddr);

				if (entry == nullptr)
				{
					// No route for the packet - sending the corresponding response
					getICMPReply(ICMP_DEST_UNREACH, packet);
					sendPacket(packet);
				}
				else
				{
					// Prepare the packet for forwarding and check ARP table
					if (prepareIPForward(entry, packet))
					{
						sendPacket(packet);
					}
					else
					{
						queue.enqueue(packet);

						// To send ARP request once
						ip = ntohl(headerIP->daddr);
						if (!arpRequestedIps.find(ip))
						{
							arpRequestedIps.add(ip);
							getARPRequest(headerIP->daddr, packet);
							sendPacket(packet);
						}
					}
				}
			}
		}
	}
}
