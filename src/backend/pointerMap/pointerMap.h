#ifndef POINTERMAP_H
#define POINTERMAP_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../starredAddress/starredAddress.h"
#include "../regions/regions.h"



class PointerMap {
    void scan(void* regionStart, void* buf, uint64_t size, uint64_t offset, std::unordered_map<void*, std::unordered_set<void*> >& pmap);

public:
    Regions regions;

    uint64_t addressesToScan = 0;
    uint64_t addressesScanned = 0;
    unsigned fastScanOffset;
    unsigned maxThreads;
    bool generationInProgress = false;
    bool shouldStop = false;

    std::unordered_map<void*, std::unordered_set<void*> > map;
    std::vector<StarredAddress> savedAddresses;

    bool fromCurrentProcess = true;
    std::string name;

    PointerMap(const std::vector<StarredAddress>& addrs, const std::string& name, int threads, int fastScanOffset);


    void generate();
    void update();
    void save(const std::string& path);
    std::string load(const std::string& path);
};


#endif //POINTERMAP_H
