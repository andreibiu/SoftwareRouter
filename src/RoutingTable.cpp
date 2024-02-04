#include "../include/RoutingTable.hpp"

/* Static Instance - Singleton */

RoutingTable RoutingTable::instance = RoutingTable();

/* Public Methods */

void RoutingTable::add(const RoutingEntry& entry)
{
    entries[__builtin_popcount(entry.mask)].add(entry);
}

void RoutingTable::remove(const RoutingEntry& entry)
{
    entries[__builtin_popcount(entry.mask)].remove(entry);
}

RoutingEntry* RoutingTable::find(uint32 destinationIPNet)
{
    uint8 index;
    uint32 ip = ntohl(destinationIPNet);
    uint32 mask = 0xFFFFFFFF;
    RoutingEntry* entry = nullptr;

    while (mask && !entry)
    {
        index = __builtin_popcount(mask) - 1;
        if (!entries[index].isEmpty())
            entry = entries[index].find(RoutingEntry(ip & mask));
        mask <<= 1;
    }

    return entry;
}
