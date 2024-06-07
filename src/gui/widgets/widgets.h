#ifndef CHEAT_TURBINE_WIDGETS_H
#define CHEAT_TURBINE_WIDGETS_H

#include "../../backend/CTvalue/valueUtils.h"


namespace Widgets {
    bool valueInputTrueOnEditing(const ValueType& valueType, void* to, int bufSize=256);
    bool valueInputTrueOnDeactivation(const ValueType& valueType, void* to, bool hex=false, int bufSize=256, bool pointer=false);
    void valueText(const ValueType &valueType, void* value);
    void valueTypeSelector(ValueType& valueType, bool enablePchain=true);
    bool tristateCheckbox(const char* label, int* state);
}

#endif //CHEAT_TURBINE_WIDGETS_H
