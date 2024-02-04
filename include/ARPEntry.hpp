#ifndef ARP_ENTRY_HPP
#define ARP_ENTRY_HPP

#include <cstring>

#include "typedefs.h"

#define MAC_SIZE 6
#define MAC_START_OFFSET 4

struct ARPEntry
{
public:
    // Fields
    uint32 ip;
    uint8 mac[MAC_SIZE];

    // Constructors
    ARPEntry();
    ARPEntry(uint32 ip);
    ARPEntry(uint32 ip, uint8 mac[MAC_SIZE]);

    // Operators
    bool operator < (const ARPEntry& entry) const;
    bool operator > (const ARPEntry& entry) const;
    bool operator == (const ARPEntry& entry) const;
};

#endif // ARP_ENTRY_HPP
