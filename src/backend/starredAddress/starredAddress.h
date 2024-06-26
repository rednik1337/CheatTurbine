#ifndef CHEAT_TURBINE_STARREDADDRESS_H
#define CHEAT_TURBINE_STARREDADDRESS_H

#include "../CTvalue/valueType.h"

#include <string>
#include <vector>

#include "../pointerChain/pointerChain.h"


enum DisplayType {
    dec = 0,
    hex = 1,
};


class StarredAddress {
public:
    int stringSize;
    std::string name;
    ValueType valueType;
    void* address;
    bool isFrozen = false;
    std::vector<unsigned char> valueBytes;
    unsigned char displayType = dec;
    PointerChain pchain;

    void update();

    StarredAddress(std::string name, ValueType valueType, void* address, int stringSize=0);
};


#endif //CHEAT_TURBINE_STARREDADDRESS_H
