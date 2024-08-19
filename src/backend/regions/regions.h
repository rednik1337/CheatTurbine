#ifndef REGIONS_H
#define REGIONS_H

#include "memoryRegion.h"

#include <cstdint>
#include <optional>
#include <vector>



class Regions {
public:
    std::vector<Region> regions;
    unsigned long long largestRegionSize{};
    RegionPerms mustHavePerms = RegionPerms::no_perms, mustNotHavePerms = RegionPerms::no_perms;

    void parse();
    std::optional<Region> get(const void* address);
    std::optional<Region> get(const std::string& path, uint64_t offest);
    bool isValidAddress(const void* address);
    bool isStaticAddress(const void* address);
};


#endif //REGIONS_H
