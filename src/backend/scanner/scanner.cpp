#include "scanner.h"

#include <cmath>

#include "../selectedProcess/selectedProcess.h"
#include "../virtualMemory/virtualMemory.h"
#include "../../gui/gui.h"

#include <fstream>
#include <thread>
#include <functional>
#include <utility>


Scanner::Scanner(const char* name) {
    this->name = name;
    latestValues = malloc(256);
    reset();
}


template<typename T>
std::function<bool(void*)> Scanner::getCommonComparator() {
    switch (scanType) {
        case equal:
            if constexpr (std::is_same_v<T, float> or std::is_same_v<T, double>)
                return [this](void* mem) {
                    return std::abs(*(T*)mem - *(T*)valueBytes.data()) < 0.001f;
                };
            return [this](void* mem) {
                return *(T*)mem == *(T*)valueBytes.data();
            };
        case bigger:
            return [this](void* mem) {
                return *(T*)mem > *(T*)valueBytes.data();
            };
        case smaller:
            return [this](void* mem) {
                return *(T*)mem < *(T*)valueBytes.data();
            };
        case range:
            return [this](void* mem) {
                return *(T*)mem > *(T*)valueBytes.data() and *(T*)mem < *(T*)valueBytesSecond.data();
            };
        case increased:
            return [this](void* mem) {
                return *(T*)mem > *(T*)((char*)latestValues + scannedAddresses * valueBytes.size());
            };
        case increasedBy:
            if constexpr (std::is_same_v<T, float> or std::is_same_v<T, double>)
                return [this](void* mem) {
                    return std::abs(*(T*)mem - *(T*)((char*)latestValues + scannedAddresses * valueBytes.size()) - *(T*)valueBytes.data()) < 0.001f;
                };
            return [this](void* mem) {
                return *(T*)mem - *(T*)((char*)latestValues + scannedAddresses * valueBytes.size()) == *(T*)valueBytes.data();
            };
        case decreased:
            return [this](void* mem) {
                return *(T*)mem < *(T*)((char*)latestValues + scannedAddresses * valueBytes.size());
            };
        case decreasedBy:
            if constexpr (std::is_same_v<T, float> or std::is_same_v<T, double>)
                return [this](void* mem) {
                    return std::abs(*(T*)((char*)latestValues + scannedAddresses * valueBytes.size()) - *(T*)mem - *(T*)valueBytes.data()) < 0.001f;
                };
            return [this](void* mem) {
                return *(T*)((char*)latestValues + scannedAddresses * valueBytes.size()) - *(T*)mem == *(T*)valueBytes.data();
            };
        case changed:
            return [this](void* mem) {
                return *(T*)mem != *(T*)((char*)latestValues + scannedAddresses * valueBytes.size());
            };
        case unchanged:
            return [this](void* mem) {
                return *(T*)mem == *(T*)((char*)latestValues + scannedAddresses * valueBytes.size());
            };
        case unknown:
            return [](void*) {
                return true;
            };
    }

    std::unreachable();
}


std::function<bool(void*)> Scanner::getTypeSpecificComparator() {
    if (valueType & isSigned) {
        if (valueType & i64)
            return getCommonComparator<int64_t>();
        if (valueType & i32)
            return getCommonComparator<int32_t>();
        if (valueType & i16)
            return getCommonComparator<int16_t>();
        if (valueType & i8)
            return getCommonComparator<int8_t>();
    } else {
        if (valueType & i64)
            return getCommonComparator<u_int64_t>();
        if (valueType & i32)
            return getCommonComparator<u_int32_t>();
        if (valueType & i16)
            return getCommonComparator<u_int16_t>();
        if (valueType & i8)
            return getCommonComparator<u_int8_t>();
    }
    if (valueType & f64)
        return getCommonComparator<double_t>();
    if (valueType & f32)
        return getCommonComparator<float_t>();

    std::unreachable();
}


void Scanner::newScan() {
    isScanRunning = true;
    scannedAddresses = 0;
    std::thread(static_cast<void (Scanner::*)(std::function<bool(void*)>)>(&Scanner::newScan), this, getTypeSpecificComparator()).detach();
}


void Scanner::nextScan() {
    isScanRunning = true;
    scannedAddresses = 0;
    std::thread(static_cast<void (Scanner::*)(std::function<bool(void*)>)>(&Scanner::nextScan), this, getTypeSpecificComparator()).detach();
}


void Scanner::newScan(const std::function<bool(void*)> cmp) {
    bool wasAttachedProgram = SelectedProcess::isSuspended();
    if (shouldSuspendWhileScanning and !SelectedProcess::isSuspended())
        SelectedProcess::suspend();

    unsigned long long matchingAddresses = 0;
    regions.parse();
    for (auto& i : regions.regions)
        totalAddresses += ((char*)i.end - (char*)i.start) / fastScanOffset;


    void* memory = malloc(regions.largestRegionSize);


    for (auto& [start, end, mode, offset, device, inodeID, fname]: regions.regions) {
        const unsigned long long regionSize = (char*)end - (char*)start;


        if (!VirtualMemory::read(start, memory, regionSize))
            continue;

        Gui::log("{}: Scanning region {} - {}", name, start, end);

        for (unsigned long long i = 0; i < regionSize; i += fastScanOffset) {
            if (cmp((char*)memory + i)) {
                if (matchingAddresses * valueBytes.size() >= latestValuesSize) {
                    latestValuesSize *= 2;
                    latestValues = realloc(latestValues, latestValuesSize * valueBytes.size());
                }

                addresses.emplace_back((char*)start + i);
                memcpy((char*)latestValues + matchingAddresses * valueBytes.size(), (char*)memory + i, valueBytes.size());
                matchingAddresses++;
            }
            scannedAddresses++;
        }
    }

    free(memory);

    Gui::log("{}: Scan completed, {} addresses scanned, {} match", name, scannedAddresses, matchingAddresses);

    totalAddresses = matchingAddresses;
    scannedAddresses = totalAddresses;

    addresses.resize(totalAddresses);
    addresses.shrink_to_fit();
    latestValues = realloc(latestValues, matchingAddresses * valueBytes.size());

    if (shouldSuspendWhileScanning and !wasAttachedProgram)
        SelectedProcess::resume();
    isScanRunning = false;
    isReset = false;
}


void Scanner::nextScan(const std::function<bool(void*)> cmp) {
    const bool wasSuspended = SelectedProcess::isSuspended();
    if (shouldSuspendWhileScanning and !SelectedProcess::pid)
        SelectedProcess::suspend();

    regions.parse();

    unsigned long long resAddrI = 0;
    unsigned long long matchingAddresses = 0;

    void* memory = malloc(regions.largestRegionSize);

    for (auto& [start, end, mode, offset, device, inodeID, fname]: regions.regions) {
        if (const unsigned long long regionSize = (char*)end - (char*)start; !VirtualMemory::read(start, memory, regionSize))
            continue;


        Gui::log("{}: Scanning region {} - {}", name, start, end);

        if (addresses[scannedAddresses] < start)
            continue;


        while (scannedAddresses < addresses.size() and addresses[scannedAddresses] <= (char*)end - valueBytes.size()) {
            if (cmp((char*)memory + ((char*)addresses[scannedAddresses] - (char*)start))) {
                addresses[resAddrI] = addresses[scannedAddresses];
                memcpy((char*)latestValues + resAddrI++ * valueBytes.size(), (char*)memory + ((char*)addresses[scannedAddresses] - (char*)start), valueBytes.size());
                matchingAddresses++;
            }
            scannedAddresses++;
        }
    }

    free(memory);

    Gui::log("{}: Scan completed, {} addresses scanned, {} match", name, scannedAddresses, matchingAddresses);

    totalAddresses = matchingAddresses;
    scannedAddresses = totalAddresses;

    addresses.resize(totalAddresses);
    addresses.shrink_to_fit();
    latestValues = realloc(latestValues, matchingAddresses * valueBytes.size());

    if (shouldSuspendWhileScanning and !wasSuspended)
        SelectedProcess::resume();
    isScanRunning = false;
}


void Scanner::reset() {
    valueBytes = std::vector<u_int8_t>(32);
    valueBytesSecond = std::vector<u_int8_t>(32);
    valueBytes.clear();
    valueBytesSecond.clear();
    latestValues = realloc(latestValues, 65535);
    latestValuesSize = 65535;
    addresses = std::vector<void*>();
    totalAddresses = 0;
    scannedAddresses = 0;
    isAutonextEnabled = false;
    isReset = true;
    scanType = equal;
    regions = Regions();
}


Scanner::~Scanner() {
    free(latestValues);
}
