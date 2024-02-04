#include "../include/ARPTable.hpp"

/* Static Instance - Singleton */

ARPTable ARPTable::instance = ARPTable();

/* Public Methods */

void ARPTable::add(const ARPEntry& entry)
{
    entries.add(entry);
}

void ARPTable::remove(const ARPEntry& entry)
{
    entries.remove(entry);
}

ARPEntry* ARPTable::find(uint32 destinationIPNet)
{
    return entries.find(ARPEntry(ntohl(destinationIPNet)));
}