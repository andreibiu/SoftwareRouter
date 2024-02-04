#ifndef ROUTER_UTILS_H
#define ROUTER_UTILS_H

#include "skel.h"
#include "net_typedefs.h"

#include "ARPTable.hpp"
#include "RoutingTable.hpp"
#include "Queue.hpp"

#define TTL_DEFAULT 64
#define IP_HEADER_IHL 5

using Skel::Packet;

void receivePacket(Packet& packet);
void sendPacket(Packet& packet);
uint32 getRouterInterfaceIpNet(int interface);
bool isRouterIP(uint32 ipNet, int interface);
void getARPRequest(uint32 destIPNet, Packet& packet);
void getARPReply(Packet& packet);
void updateARPTable(ARPHeader* header);
bool checkIPPacket(IPHeader* header);
void getICMPReply(uint8 type, Packet& packet);
bool prepareIPForward(RoutingEntry* entry, Packet& packet);
void sendQueuedPackets(Queue<Packet>& queue, uint32 ipNet, uint8* macNet);

#endif // ROUTER_UTILS_H
