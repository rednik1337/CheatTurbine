#ifndef REGIONS_H
#define REGIONS_H

#include <optional>

#include "memoryRegion.h"
#include <vector>


class Regions {
public:
    std::vector<Region> regions;
    unsigned long long largestRegionSize{};
    RegionPerms mustHavePerms = none, mustNotHavePerms = none;

    void parse();
    std::optional<Region> get(const void* address);
    std::optional<Region> get(const std::string& path, u_int64_t offest);
    bool isValidAddress(const void* address);
    bool isStaticAddress(const void* address);
};


#endif //REGIONS_H
