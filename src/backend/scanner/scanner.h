#ifndef CHEAT_TURBINE_SCANNER_H
#define CHEAT_TURBINE_SCANNER_H

#include "../regions/regions.h"
#include <vector>
#include <string>
#include <functional>

#include "../CTvalue/CTvalue.h"


enum ScanType : unsigned {
    equal,
    bigger,
    smaller,
    range,
    increased,
    increasedBy,
    decreased,
    decreasedBy,
    changed,
    unchanged,
    unknown
};


class Scanner {
    template<typename T>
    std::function<bool(void*)> getCommonComparator();

    std::function<bool(void*)> getStringComparator();

    std::function<bool(void*)> getTypeSpecificComparator();

    void newScan(std::function<bool(void*)> cmp);

    void nextScan(std::function<bool(void*)> cmp);

public:
    Regions regions;
    std::vector<uint8_t> valueBytes;
    std::vector<uint8_t> valueBytesSecond;

    std::vector<void*> addresses;
    void* latestValues;

    const char* name;
    ScanType scanType = equal;
    CTvalue valueType = i32;
    unsigned long long totalAddresses = 0;
    unsigned long long scannedAddresses = 0;
    unsigned long long latestValuesSize = 0;
    bool isReset = true;
    bool shouldSuspendWhileScanning = false;
    bool isAutonextEnabled = false;
    bool isScanRunning = false;
    unsigned fastScanOffset = 4;

    void newScan();

    void nextScan();

    void reset();

    explicit Scanner(const char* name);

    ~Scanner();
};


#endif //CHEAT_TURBINE_SCANNER_H
