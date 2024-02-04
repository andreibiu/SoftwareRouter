#ifndef ARPTABLE_H
#define ARPTABLE_H

#include "skel.h"
#include "SortedList.hpp"
#include "ARPEntry.hpp"

class ARPTable
{
private:
    // Fields & Components
    SortedList<ARPEntry> entries;

public:
    // Static Instance - Singleton
    static ARPTable instance;

    // Public Methods
    void add(const ARPEntry& entry);
    void remove(const ARPEntry& entry);
    ARPEntry* find(uint32 destinationIPNet);
};

#endif // ARPTABLE_H