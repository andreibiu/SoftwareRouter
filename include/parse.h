#ifndef PARSE_H
#define PARSE_H

#include <fstream>
#include <string>

#include "skel.h"
#include "RoutingTable.hpp"

uint32 parseIPOrMaskHost(char* buffer);
RoutingEntry readParseRoutingEntry(const std::ifstream& file);
void readParseRoutingTable(const char* filename);

#endif // PARSE_H
