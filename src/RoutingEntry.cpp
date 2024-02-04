#include "../include/RoutingEntry.hpp"

/* Constructors */

RoutingEntry::RoutingEntry()
{
    prefixIP = nextHopIP = mask = interface = 0;
}

RoutingEntry::RoutingEntry(uint32 prefixIP)
{
    this->prefixIP = prefixIP;
    nextHopIP = mask = interface = 0;
}

/* Operators */

bool RoutingEntry::operator < (const RoutingEntry& entry) const
{
    return prefixIP < entry.prefixIP;
}

bool RoutingEntry::operator > (const RoutingEntry& entry) const
{
    return prefixIP > entry.prefixIP;
}

bool RoutingEntry::operator == (const RoutingEntry& entry) const
{
    return prefixIP == entry.prefixIP;
}
