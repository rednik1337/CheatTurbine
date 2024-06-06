#ifndef CHEAT_TURBINE_STARREDADDRESSESWINDOW_H
#define CHEAT_TURBINE_STARREDADDRESSESWINDOW_H

#include "../generic/window.h"
#include "../../../backend/starredAddress/starredAddress.h"
#include <vector>


class StarredAddressesWindow final : public Window {
    void addAddressPopup(bool& pOpen);
public:
    std::vector<StarredAddress> addresses;

    void draw() override;

    void addAddress(const std::string& name, void* address, ValueType valueType);
    void addAddress(const std::string& name, void* address, ValueType valueType, PointerChain pointerChain);
    void getAddresses();

    StarredAddressesWindow() { name = "Starred"; }
};


#endif //CHEAT_TURBINE_STARREDADDRESSESWINDOW_H
