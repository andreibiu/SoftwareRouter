#ifndef ROUTING_TABLE_HPP
#define ROUTING_TABLE_HPP

#include "skel.h"

#include "RoutingEntry.hpp"
#include "SortedList.hpp"

class RoutingTable
{
private:
    // Constants
    static constexpr uint8 MaskCount = 32;

    // Fields & Components
    SortedList<RoutingEntry> entries[MaskCount];

public:
    // Static Instance - Singleton
    static RoutingTable instance;

    // Public Methods
    void add(const RoutingEntry& entry);
    void remove(const RoutingEntry& entry);
    RoutingEntry* find(uint32 destinationIPNet);
};

#endif // ROUTING_TABLE_HPP
