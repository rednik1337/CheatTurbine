#include "pointerScan.h"
#include "../regions/regions.h"
#include "../selectedProcess/selectedProcess.h"

#include <queue>
#include <algorithm>
#include <set>


void PointerScan::scan(std::vector<u_int64_t> targets) {
    std::vector<std::unordered_set<void*>::iterator> iterators(targets.size());
    std::vector<std::unordered_map<void*, std::unordered_set<void*> >*> maps;
    std::vector<Regions*> regions;
    for (const auto i: pmaps) {
        maps.emplace_back(&i->map);
        regions.emplace_back(&i->regions);
    }

    std::set<std::vector<u_int64_t>> visited;
    std::queue<std::pair<std::vector<int>, std::vector<u_int64_t> > > q;
    q.push({{}, targets});

    std::vector<u_int64_t> nextTargets(targets.size());
    std::vector<int> offsets;
    std::vector<int> resOffsets;
    offsets.reserve(maxDepth + 1);
    resOffsets.reserve(maxDepth + 1);

    while (!q.empty()) {
        if (shouldStop)
            return;
        offsets = q.front().first;
        targets = q.front().second;
        q.pop();
        offsets.push_back(0);
        currentDepth = offsets.size();

        for (int offset = -maxOffsetPositive; offset < maxOffsetNegative; offset += fastScanOffset) {
            for (int i = 0; i < targets.size(); ++i) {
                if (!maps[i]->contains((void*)(targets[i] + offset))) {
                    goto DONE;
                }
                iterators[i] = maps[i]->at((void*)(targets[i] + offset)).begin();
                nextTargets[i] = (u_int64_t)*iterators[i];
            }

            offsets.back() = -offset;

            while (true) {
                if (!visited.contains(nextTargets)) {
                    // bool areAllTargetsStatic = true;
                    // bool areTargetsDifferent = false;
                    // bool areSomeTargetsStatic = false;
                    // u_int64_t prevDiff = nextTargets[0] - (u_int64_t)regions[0]->get((void*)nextTargets[0]).value().start;
                    // for (int i = 0; i < targets.size(); ++i) {
                    //     if (!regions[i]->isStaticAddress((void*)nextTargets[i]))
                    //         areAllTargetsStatic = false;
                    //     else
                    //         areSomeTargetsStatic = true;
                    //     if (prevDiff != nextTargets[i] - (u_int64_t)regions[i]->get((void*)nextTargets[i]).value().start)
                    //         areTargetsDifferent = true;
                    // }
                    if (regions[0]->isStaticAddress((void*)nextTargets[0])) {
                        // if (!areTargetsDifferent) {
                            resOffsets = offsets;
                            int regionOffset = nextTargets[0] - (u_int64_t)regions[0]->get((void*)nextTargets[0]).value().start;
                            resOffsets.push_back(regionOffset);
                            std::ranges::reverse(resOffsets);
                            auto region = regions[0]->get((void*)nextTargets[0]).value();
                            pChains.emplace_back(region.path, region.start, resOffsets, region.offset);
                        // }
                    } else if (currentDepth < maxDepth - 1) {
                        // if (!areSomeTargetsStatic)
                            q.push({offsets, nextTargets});
                    }
                    visited.insert(nextTargets);
                }

                ++iterators[0];
                int i = 0;
                while (iterators[i] == maps[i]->at((void*)(targets[i] + offset)).end()) {
                    if (i == iterators.size() - 1)
                        goto DONE;
                    iterators[i] = maps[i]->at((void*)(targets[i] + offset)).begin();
                    nextTargets[i] = (u_int64_t)*iterators[i];
                    ++i;
                    ++iterators[i];
                }
                nextTargets[i] = (u_int64_t)*iterators[i];
            }
        DONE:
        }
    }
}


void PointerScan::newScan(const std::vector<u_int64_t>& targets, const std::vector<PointerMap*> pmaps) {
    maxOffsetPositive -= maxOffsetPositive % fastScanOffset;
    maxOffsetNegative -= maxOffsetNegative % fastScanOffset;

    this->pmaps = pmaps;

    scanInProgress = true;

    scan(targets);

    shouldStop = false;
    scanInProgress = false;
}

void PointerScan::nextScan(const void* target) {
    scanInProgress = true;

    Regions regions;
    regions.parse();

    std::vector<PointerChain> newPchains;
    for (auto& pChain: pChains) {
        if (shouldStop)
            break;
        if (pChain.getTail() == target)
            newPchains.emplace_back(pChain);
    }
    pChains = newPchains;

    shouldStop = false;
    scanInProgress = false;
}

PointerScan::PointerScan() {
    SPsubscriptionID = SelectedProcess::subscribeToAttach([this] {
        Regions regions;
        regions.parse();
        for (auto& i: pChains)
            i.updateHead(regions);
    });
}

PointerScan::~PointerScan() {
    SelectedProcess::unsubscribeFromAttach(SPsubscriptionID);
}
