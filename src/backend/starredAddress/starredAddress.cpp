#include "starredAddress.h"
#include "../virtualMemory/virtualMemory.h"


StarredAddress::StarredAddress(std::string name, const CTvalue valueType, void* address) {
    this->name = std::move(name);
    this->valueType = valueType;
    this->address = address;
    valueBytes.resize(16);  //TODO
}


void StarredAddress::update() {
    if (valueType.flags & CTValueFlags::pchain)
        address = pchain.getTail();

    if (valueType.type == string) {
        valueBytes.resize(256);
        valueType.stringLength = 256;
    }

    if (isFrozen) {
        VirtualMemory::write(valueBytes.data(), address, valueType.type == string ? strlen((char*)valueBytes.data()) : valueType.getSize());
    } else {
        VirtualMemory::read(address, valueBytes.data(), valueType.getSize());
    }
}
