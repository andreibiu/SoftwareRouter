#include "../include/parse.h"

uint32 parseIPOrMaskHost(char* buffer)
{
   uint32 value;
   inet_aton(buffer, (in_addr*) &value);
   return ntohl(value);
}

RoutingEntry readParseRoutingEntry(std::ifstream& file)
{
    char* buffer = new char[16];
    RoutingEntry entry;

    file >> buffer;
    entry.prefixIP = parseIPOrMaskHost(buffer);
    file >> buffer; 
    entry.nextHopIP = parseIPOrMaskHost(buffer);
    file >> buffer;
    entry.mask = parseIPOrMaskHost(buffer);
    file >> entry.interface;

    delete[] buffer;
    return entry;
}

void readParseRoutingTable(const char* filename)
{
    std::ifstream file(filename);

    while (!file.eof())
        RoutingTable::instance.add(
            readParseRoutingEntry(file));

    file.close();
}

