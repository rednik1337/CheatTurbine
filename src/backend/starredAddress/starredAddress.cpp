#include "starredAddress.h"

#include "../CTvalue/valueUtils.h"
#include "../virtualMemory/virtualMemory.h"


StarredAddress::StarredAddress(std::string name, const ValueType valueType, void* address, int stringSize) {
    this->name = std::move(name);
    this->valueType = valueType;
    this->address = address;
    this->stringSize = stringSize;
    valueBytes.resize(16);
}


void StarredAddress::update() {
    if (valueType & ValueType::pchain)
        address = pchain.getTail();
    if (isFrozen) {
        VirtualMemory::write(valueBytes.data(), address, stringSize ? stringSize : ValueUtils::sizeofValueType(ValueType(valueType & ~ValueType::pchain)));
    } else {
        VirtualMemory::read(address, valueBytes.data(), stringSize ? stringSize : ValueUtils::sizeofValueType(ValueType(valueType & ~ValueType::pchain)));
    }
}
