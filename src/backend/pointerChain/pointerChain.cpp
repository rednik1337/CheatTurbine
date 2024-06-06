#include "pointerChain.h"

#include <iostream>

#include "../../gui/gui.h"
#include "../virtualMemory/virtualMemory.h"


void* PointerChain::getTail() {
    u_int64_t addr = (u_int64_t)head;
    bool invalid = false;
    for (int i = 0; i < offsets.size() - 1; ++i) {
        addr += offsets[i];
        if (!VirtualMemory::read((void*)addr, &addr, 8))
            invalid = true;
    }
    addr += offsets.back();
    isValid = !invalid;
    return (void*)addr;
}

void PointerChain::updateHead(Regions& parsedRegions) {
    auto region = parsedRegions.get(path, regionOffset);
    if (region.has_value()) {
        head = region.value().start;
    }
}

PointerChain::PointerChain(const std::string& path, void* head, const std::vector<int>& offsets, const u_int64_t regionOffset) {
    this->path = path;
    this->head = head;
    this->offsets = offsets;
    this->regionOffset = regionOffset;
    isValid = true;
}
