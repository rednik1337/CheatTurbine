#include "pointerMap.h"

#include "../../gui/gui.h"
#include "../regions/regions.h"
#include "../virtualMemory/virtualMemory.h"

#include <fstream>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <ranges>



void PointerMap::scan(void* regionStart, void* buf, const u_int64_t size, const u_int64_t offset, std::unordered_map<void*, std::unordered_set<void*> >& pmap) {
    for (u_int64_t i = 0; i < size; i += fastScanOffset) {
        void* value = (void*)*(u_int64_t*)((char*)buf + i + offset);
        if (regions.isValidAddress(value) and !regions.isStaticAddress(value))
            pmap[value].insert((char*)regionStart + i + offset);
    }
}

PointerMap::PointerMap(const std::vector<StarredAddress>& addrs, const std::string& name, const int threads, const int fastScanOffset) {
    savedAddresses = addrs;
    this->name = name;
    this->maxThreads = threads;
    this->fastScanOffset = fastScanOffset;
}

void PointerMap::generate() {
    Gui::log("Generating pointermap");
    generationInProgress = true;
    shouldStop = false;

    regions.parse();

    for (const auto& i: regions.regions)
        addressesToScan += ((char*)i.end - (char*)i.start) / fastScanOffset;

    alignas(std::hardware_destructive_interference_size) std::unordered_map<void*, std::unordered_set<void*> > maps[maxThreads];
    std::vector<std::thread> threads;

    void* buf = malloc(regions.largestRegionSize);
    for (const auto& [start, end, mode, offset, device, inodeID, path]: regions.regions) {
        if (shouldStop)
            break;
        const u_int64_t regionSize = (char*)end - (char*)start;
        if (!VirtualMemory::read(start, buf, regionSize)) {
            addressesScanned += regionSize / fastScanOffset;
            continue;
        }
        Gui::log("Scanning region {:p}-{:p}", start, end);
        for (auto& i: maps)
            i.clear();
        threads.clear();

        u_int64_t sizePerThread = regionSize / maxThreads;
        sizePerThread -= sizePerThread % fastScanOffset;
        u_int64_t sizeCovered = sizePerThread * (maxThreads - 1);

        for (int i = 0; i < maxThreads - 1; ++i)
            threads.emplace_back(&PointerMap::scan, this, start, buf, sizePerThread, i * sizePerThread, std::ref(maps[i]));

        scan(start, buf, regionSize - sizeCovered, sizeCovered, maps[maxThreads-1]);

        for (int i = 0; i < maxThreads-1; ++i) {
            threads[i].join();
            for (auto& [key, val]: maps[i])
                map[key].merge(val);
        }
        addressesScanned += regionSize / fastScanOffset;
    }

    free(buf);

    Gui::log("Pointermap generated, size: {}", map.size());

    generationInProgress = false;
}

void PointerMap::update() {
    Gui::log("Updating pointermap");
    generationInProgress = true;
    addressesScanned = 0;
    shouldStop = false;
    addressesToScan = map.size();

    regions.parse();

    std::queue<void*> ptrsMarkedForDeletion;
    std::queue<void*> addrsMarkedForDeletion;
    for (auto& [addr, ptrs]: map) {
        addressesScanned++;

        if (shouldStop)
            break;
        if (!regions.isValidAddress(addr)) {
            addrsMarkedForDeletion.push(addr);
            continue;
        }

        for (auto ptr: ptrs) {
            if (!regions.isValidAddress(ptr)) {
                ptrsMarkedForDeletion.push(ptr);
                continue;
            }
            void* value;
            if (!VirtualMemory::read(ptr, &value, 8))
                ptrsMarkedForDeletion.push(ptr);
            else if (!regions.isValidAddress(value))
                ptrsMarkedForDeletion.push(ptr);
        }

        while (!ptrsMarkedForDeletion.empty()) {
            ptrs.erase(ptrsMarkedForDeletion.front());
            ptrsMarkedForDeletion.pop();
        }
        if (ptrs.empty())
            addrsMarkedForDeletion.push(addr);
    }

    while (!addrsMarkedForDeletion.empty()) {
        map.erase(addrsMarkedForDeletion.front());
        addrsMarkedForDeletion.pop();
    }

    Gui::log("Pointermap updated, new size: {}", map.size());

    generationInProgress = false;
}


void PointerMap::save(const std::string& path) {
    std::ofstream outfile(path, std::ios::binary | std::ios::out);;
    outfile << "CTptrmap";

    outfile << map.size();
    for (const auto& [addr, ptrs]: map) {
        outfile << addr << ptrs.size();
        for (const auto i: ptrs)
            outfile << i;
    }
}

std::string PointerMap::load(const std::string& path) {
    std::ifstream infile(path, std::ios::binary | std::ios::in);
    if (!infile)
        return std::format("Failed to open {}", path);

    char header[9];
    infile.read(header, 9);
    if (strcmp(header, "CTptrmap"))
        return "File is not a pointermap";

    u_int64_t mapSize;
    infile >> mapSize;
    while (mapSize--) {
        void* addr;
        infile >> addr;

        u_int64_t ptrsSize;
        infile >> ptrsSize;
        while (ptrsSize--) {
            void* ptr;
            infile >> ptr;
            map[addr].insert(ptr);
        }
    }

    return "";
}
