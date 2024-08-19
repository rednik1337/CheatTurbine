#ifndef CHEAT_TURBINE_STARREDADDRESS_H
#define CHEAT_TURBINE_STARREDADDRESS_H

#include "../CTvalue/CTvalue.h"

#include <string>
#include <vector>

#include "../pointerChain/pointerChain.h"


enum DisplayType {
    dec = 0,
    hex = 1,
};


class StarredAddress {
public:
    std::string name;
    CTvalue valueType;
    void* address;
    bool isFrozen = false;
    std::vector<unsigned char> valueBytes;
    unsigned char displayType = dec;
    PointerChain pchain;

    void update();
    StarredAddress(std::string name, CTvalue valueType, void* address);
};


#endif //CHEAT_TURBINE_STARREDADDRESS_H
