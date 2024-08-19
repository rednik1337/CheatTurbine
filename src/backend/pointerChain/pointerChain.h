#ifndef POINTERCHAIN_H
#define POINTERCHAIN_H

#include "../regions/regions.h"


class PointerChain {
public:
    bool isValid;
    std::string path;
    uint64_t regionOffset;
    void* head;
    std::vector<int> offsets;

    void* getTail();
    void updateHead(Regions& parsedRegions);
    PointerChain(const std::string& path, void* head, const std::vector<int>& offsets, uint64_t regionOffset);
    PointerChain() = default;
};



#endif //POINTERCHAIN_H
