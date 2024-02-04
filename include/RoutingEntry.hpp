#ifndef ROUTING_ENTRY_H
#define ROUTING_ENTRY_H

#include "typedefs.h"

struct RoutingEntry
{
public:
    // Fields
    uint32 prefixIP;
    uint32 nextHopIP;
    uint32 mask;
    uint32 interface;

    // Constructors
    RoutingEntry();
    RoutingEntry(uint32 prefixIP);

    // Operators
    bool operator < (const RoutingEntry& entry) const;
    bool operator > (const RoutingEntry& entry) const;
    bool operator == (const RoutingEntry& entry) const;
};

#endif // ROUTING_ENTRY_H