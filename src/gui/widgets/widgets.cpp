#include "widgets.h"

#include <cmath>
#include <array>
#include <string>
#include <imgui_internal.h>
#include <utility>


bool Widgets::valueInputTrueOnEditing(const CTvalue& valueType, void* to, const int bufSize) {
    bool res;
    ImGui::PushID(int((uint64_t)to << 1));
    if (valueType.type == string)
        res = ImGui::InputText("##ValueInputE", (char*)to, bufSize);
    else
        res = ImGui::InputScalar("##ValueInputE", valueType.getImGuiDataType(), to);
    ImGui::PopID();
    return res;
}


bool Widgets::valueInputTrueOnDeactivation(const CTvalue& valueType, void* to, const bool hex, const int bufSize, const bool pointer) {
    ImGui::PushID((void*)((uint64_t)to << 1));
    if (valueType.type == string)
        ImGui::InputText("##ValueInputS", (char*)to, bufSize, hex ? ImGuiInputTextFlags_CharsHexadecimal : 0);
    else
        ImGui::InputScalar("##ValueInputD", valueType.getImGuiDataType(), to, nullptr, nullptr, hex ? "%x" : pointer ? "%p" : nullptr, hex ? ImGuiInputTextFlags_CharsHexadecimal : 0);
    ImGui::PopID();
    return ImGui::IsItemDeactivatedAfterEdit();
}

void Widgets::valueText(const CTvalue& valueType, void* value) {
    ImGui::TextUnformatted(valueType.format(value, false).c_str());
}

void Widgets::valueTypeSelector(CTvalue& valueType, const bool enablePchain) {
    const std::array<std::string, 9> items{"pchain", "signed?", "int64", "int32", "int16", "int8", "float64", "float32", "string"};
    unsigned currentItem = valueType.type + 2;
    std::string previewString = (valueType.flags & isSigned or currentItem > 5 ? "" : "u") + items[currentItem];
    if (valueType.flags & pchain)
        previewString = "pchain -> " + previewString;

    if (ImGui::BeginCombo("##ValueTypeCombo", previewString.c_str())) {
        for (unsigned n = 0; n < items.size(); ++n) {
            if (n == 1) {
                if (valueType.type == i64 or valueType.type == i32 or valueType.type == i16 or valueType.type == i8)
                    if (ImGui::Selectable(items[n].c_str(), valueType.flags & isSigned))
                        valueType.flags = valueType.flags ^ isSigned;
            } else if (n == 0) {
                if (enablePchain)
                    if (ImGui::Selectable(items[n].c_str(), valueType.flags & pchain))
                        valueType.flags = valueType.flags ^ pchain;
            } else {
                if (ImGui::Selectable(items[n].c_str(), currentItem == n))
                    currentItem = n;
            }
        }
        ImGui::EndCombo();
    }
    // if (valueType.type == f32 or valueType.type == f64 or valueType.type == string)
        // valueType.flags = CTValueFlags(valueType.flags | isSigned);

    valueType.type = CTValueType(currentItem - 2);
}


bool Widgets::tristateCheckbox(const char* label, int* state) {
    bool ret;
    if (*state == 0) {
        bool b = false;
        ret = ImGui::Checkbox(label, &b);
        if (ret)
            *state = 1;
    } else if (*state == 1) {
        ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, true);
        bool b = false;
        ret = ImGui::Checkbox(label, &b);
        if (ret)
            *state = 2;
        ImGui::PopItemFlag();
    } else if (*state == 2) {
        bool b = true;
        ret = ImGui::Checkbox(label, &b);
        if (ret)
            *state = 0;
    }
    return ret;
}
