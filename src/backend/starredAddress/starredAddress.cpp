#include "starredAddress.h"

#include "../CTvalue/valueUtils.h"
#include "../virtualMemory/virtualMemory.h"


StarredAddress::StarredAddress(std::string name, const ValueType valueType, void* address) {
    this->name = std::move(name);
    this->valueType = valueType;
    this->address = address;
    valueBytes.resize(16);
}


void StarredAddress::update() {
    if (valueType & ValueType::pchain)
        address = pchain.getTail();
    if (isFrozen) {
        VirtualMemory::write(valueBytes.data(), address, ValueUtils::sizeofValueType(ValueType(valueType & ~ValueType::pchain)));
    } else {
        VirtualMemory::read(address, valueBytes.data(), ValueUtils::sizeofValueType(ValueType(valueType & ~ValueType::pchain)));
    }
}
