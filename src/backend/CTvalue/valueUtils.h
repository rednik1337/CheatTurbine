#ifndef CHEAT_TURBINE_VALUEUTILS_H
#define CHEAT_TURBINE_VALUEUTILS_H

#include "valueType.h"

#include <imgui.h>
#include <string>


namespace ValueUtils {
    unsigned sizeofValueType(ValueType valueType);
    unsigned valueIndex(ValueType valueType);
    ImGuiDataType CTValueTypeToImGui(ValueType valueType);
    std::string format(ValueType valueType, void* mem, bool hex=false);
}

#endif //CHEAT_TURBINE_VALUEUTILS_H