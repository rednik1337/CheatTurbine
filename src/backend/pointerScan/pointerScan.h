#ifndef POINTERSCAN_H
#define POINTERSCAN_H

#include "../pointerMap/pointerMap.h"
#include "../pointerChain/pointerChain.h"


class PointerScan {
    void scan(std::vector<u_int64_t> targets);
    unsigned SPsubscriptionID;

public:
    std::vector<PointerMap*> pmaps{nullptr};

    int fastScanOffset = 4;
    int maxOffsetPositive = 4096;
    int maxOffsetNegative = 0;
    unsigned maxDepth = 7;
    unsigned maxThreads = 25;
    unsigned currentDepth = 0;

    bool scanInProgress = false;
    bool shouldStop = false;

    std::vector<PointerChain> pChains;

    void newScan(const std::vector<u_int64_t>& targets, std::vector<PointerMap*> pmaps);
    void nextScan(const void* target);

    PointerScan();
    ~PointerScan();
};


#endif //POINTERSCAN_H
