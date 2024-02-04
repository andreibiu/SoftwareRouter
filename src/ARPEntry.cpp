#include "../include/ARPEntry.hpp"

/* Constructors */

ARPEntry::ARPEntry()
{
    ip = 0;
    memset(mac, 0, MAC_SIZE);
}

ARPEntry::ARPEntry(uint32 ip)
{
    this->ip = ip;
    memset(mac, 0, MAC_SIZE);
}

ARPEntry::ARPEntry(uint32 ip, uint8 mac[6])
{
    this->ip = ip;
    memcpy(this->mac, mac, MAC_SIZE);
}

/* Operators */

bool ARPEntry::operator < (const ARPEntry& entry) const
{
    return ip < entry.ip;
}

bool ARPEntry::operator > (const ARPEntry& entry) const
{
    return ip > entry.ip;
}

bool ARPEntry::operator == (const ARPEntry& entry) const
{
    return ip == entry.ip;
}