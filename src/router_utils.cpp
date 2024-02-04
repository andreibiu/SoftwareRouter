#include "../include/router_utils.h"

void receivePacket(Packet& packet)
{
    int resultCode = Skel::getPacket(packet);
    DIE(resultCode < 0, "get_message");
}

void sendPacket(Packet& packet)
{
    int resultCode = Skel::sendPacket(packet.interface, packet);
    DIE(resultCode < 0, "sendPacket");
}

uint32 getRouterInterfaceIpNet(int interface)
{
	struct ifreq ifr;
	sprintf(ifr.ifr_name, "r-%u", interface);
	ioctl(Skel::interfaces[interface], SIOCGIFADDR, &ifr);
	return *((uint32*) &((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr);
}

bool isRouterIP(uint32 ipNet, int interface)
{   
    for (interface = 0; interface < ROUTER_NUM_INTERFACES; interface++)
    {
        if (getRouterInterfaceIpNet(interface) == ipNet)
            return true;
    }

    return false;
}

void getARPRequest(uint32 destIPNet, Packet& packet)
{
    int interface, resultCode;
    uint32 srcIPNet;
    EthernetHeader* headerEthernet;
    ARPHeader* headerARP;
    
    interface = packet.interface;
    srcIPNet = getRouterInterfaceIpNet(interface);
    packet = Packet();
    headerEthernet = (EthernetHeader*) packet.payload;
    headerARP = (ARPHeader*) (packet.payload + sizeof(EthernetHeader));

    headerEthernet->ether_type = htons(ETHERTYPE_ARP);
    resultCode = Skel::getMacAddress("FF:FF:FF:FF:FF:FF",
        (uint8*) headerEthernet->ether_dhost);
    DIE(resultCode < 0, "getMacAddress");
    resultCode = Skel::getInterfaceMac(interface,
        (uint8*) headerEthernet->ether_shost);
    DIE(resultCode < 0, "getInterfaceMac");

    headerARP->ea_hdr.ar_hln = 6;
    headerARP->ea_hdr.ar_pln = 4;
    headerARP->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
    headerARP->ea_hdr.ar_pro = htons(ETHERTYPE_IP);
    headerARP->ea_hdr.ar_op = htons(ARPOP_REQUEST);
    memcpy(headerARP->arp_tpa, &destIPNet, sizeof(uint32));
    memcpy(headerARP->arp_spa, &srcIPNet, sizeof(uint32));
    memset(headerARP->arp_tha, 0, MAC_SIZE);
    memcpy(&headerARP->arp_sha, &headerEthernet->ether_shost, MAC_SIZE);

    packet.interface = interface;
    packet.len = sizeof(EthernetHeader) + sizeof(ARPHeader);
}

void getARPReply(Packet& packet)
{
    int resultCode;
    uint32 ip = getRouterInterfaceIpNet(packet.interface);
    EthernetHeader* headerEthernet = (EthernetHeader*) packet.payload;
    ARPHeader* headerARP = (ARPHeader*) (packet.payload + sizeof(EthernetHeader));

    memcpy(headerEthernet->ether_dhost,
        headerEthernet->ether_shost, MAC_SIZE);
    resultCode = Skel::getInterfaceMac(packet.interface,
        (uint8*) &headerEthernet->ether_shost);
    DIE(resultCode < 0, "getInterfaceMac");

    headerARP->ea_hdr.ar_op = htons(ARPOP_REPLY);
    memcpy(headerARP->arp_tha, headerARP->arp_sha, MAC_SIZE);
    memcpy(headerARP->arp_tpa, headerARP->arp_spa, sizeof(uint32));
    memcpy(headerARP->arp_spa, &ip, sizeof(uint32));
    memcpy(&headerARP->arp_sha, &headerEthernet->ether_shost, MAC_SIZE);
}

void updateARPTable(ARPHeader* header)
{
    uint32 ip = ntohl(*((uint32*) header->arp_spa));
    ARPEntry* entry = ARPTable::instance.find(ip);

    if (entry)
        memcpy(((uint8*) entry) + MAC_START_OFFSET,
            header->arp_sha, MAC_SIZE);
    else
        ARPTable::instance.add(ARPEntry(ip, header->arp_sha));
}

bool checkIPPacket(IPHeader* header)
{
    uint16 checkSum = header->check;

    header->check = 0;
    bool res = (checkSum == Skel::checksum(header, sizeof(IPHeader)));
    return res;
}

void getICMPReply(uint8 type, Packet& packet)
{
    int resultCode;
    uint16 size;
    EthernetHeader* headerEthernet = (EthernetHeader*)
        (packet.payload);
    IPHeader* headerIP = (IPHeader*)
        (packet.payload + sizeof(EthernetHeader));
    ICMPHeader* headerICMP = (ICMPHeader*)
        (packet.payload + sizeof(EthernetHeader) + sizeof(IPHeader));

    if (type != ICMP_ECHOREPLY)
    {
        size = sizeof(IPHeader) + sizeof(ICMPHeader);

        packet.len = sizeof(EthernetHeader) + size;

        headerICMP->type = type;
        headerICMP->code = 0;
        headerICMP->un.echo.id = htons(getpid());
        headerICMP->un.echo.sequence = 1;
        headerICMP->checksum = 0;
        headerICMP->checksum = Skel::checksum(headerICMP, sizeof(ICMPHeader));

        headerIP->ihl = IP_HEADER_IHL;
        headerIP->tot_len = htons(size);
        headerIP->protocol = IPPROTO_ICMP;
        headerIP->id = htons(getpid());
        headerIP->frag_off = 0;
        headerIP->ttl = TTL_DEFAULT;
        headerIP->daddr = headerIP->saddr;
        headerIP->saddr = getRouterInterfaceIpNet(packet.interface);
        headerIP->check = 0;
        headerIP->check = Skel::checksum(headerIP, sizeof(IPHeader));
    }
    else
    {
        size = packet.len - sizeof(EthernetHeader);
        
        headerICMP->type = type;
        headerICMP->code = 0;
        headerICMP->checksum = 0;
        headerICMP->checksum = Skel::checksum(headerICMP, size - sizeof(IPHeader));

        headerIP->ihl = IP_HEADER_IHL;
        headerIP->tot_len = htons(size);
        headerIP->protocol = IPPROTO_ICMP;
        headerIP->frag_off = 0;
        headerIP->ttl--;
        headerIP->daddr = headerIP->saddr;
        headerIP->saddr = getRouterInterfaceIpNet(packet.interface);
        headerIP->check = 0;
        headerIP->check = Skel::checksum(headerIP, sizeof(IPHeader));
    }

    memcpy(headerEthernet->ether_dhost,
        headerEthernet->ether_shost, MAC_SIZE);
    resultCode = Skel::getInterfaceMac(
        packet.interface, (uint8*) headerEthernet->ether_shost);
    DIE(resultCode < 0, "getInterfaceMac");
}

bool prepareIPForward(RoutingEntry* entry, Packet& packet)
{
    bool canBeForwaredNow;
    int resultCode;
    ARPEntry* entryARP;
    EthernetHeader* headerEthernet = (EthernetHeader*) packet.payload;
    IPHeader* headerIP = (IPHeader*) (packet.payload + sizeof(EthernetHeader));

    resultCode = Skel::getInterfaceMac(
        entry->interface, (uint8*) headerEthernet->ether_shost);
    DIE(resultCode < 0, "getInterfaceMac");

    entryARP = ARPTable::instance.find(htonl(entry->nextHopIP));
    if (entryARP != nullptr)
    {
        memcpy(headerEthernet->ether_dhost,
            ((uint8*) entryARP) + MAC_START_OFFSET, MAC_SIZE);
        canBeForwaredNow = true;
    }
    else
    {
        canBeForwaredNow = false;
    }

    headerIP->ttl--;
    headerIP->check = 0;
    headerIP->check = Skel::checksum(headerIP, sizeof(IPHeader));

    packet.interface = entry->interface;
    
    return canBeForwaredNow;
}


void sendQueuedPackets(Queue<Packet>& queue, uint32 ipNet, uint8* macNet)
{
    Packet packet;
    EthernetHeader* header;
    uint64 count = queue.getCount();

    while (count--)
    {
        packet = queue.dequeue();
        header = (EthernetHeader*) packet.payload;
        if (((IPHeader*)(packet.payload + sizeof(EthernetHeader)))->daddr == ipNet)
        {
            memcpy(header->ether_dhost, macNet, MAC_SIZE);
            sendPacket(packet);
        }
        else
        {
            queue.enqueue(packet);
        }
    }
}
