#ifndef CHEAT_TURBINE_MEMORYREGION_H
#define CHEAT_TURBINE_MEMORYREGION_H

#include <string>


enum RegionPerms : unsigned char {
    no_perms = 0,
    r = 1 << 3,
    w = 1 << 2,
    x = 1 << 1,
    p = 1 << 0,
};


struct Region {
    void* start;
    void* end;
    RegionPerms mode;
    unsigned long long offset;
    std::string device;
    unsigned inodeID;
    std::string path;
};


#endif //CHEAT_TURBINE_MEMORYREGION_H
