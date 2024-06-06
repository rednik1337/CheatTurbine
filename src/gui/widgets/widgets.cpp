#include "widgets.h"

#include <cmath>
#include <array>
#include <string>
#include <imgui_internal.h>


bool Widgets::valueInputTrueOnEditing(const ValueType& valueType, void* to) {
    ImGui::PushID(int((unsigned long long)to << 1));
    const auto res = ImGui::InputScalar("##ValueInputE", ValueUtils::CTValueTypeToImGui(valueType), to);
    ImGui::PopID();
    return res;
}


bool Widgets::valueInputTrueOnDeactivation(const ValueType& valueType, void* to, bool hex) {
    ImGui::PushID(int((unsigned long long)to << 1));
    ImGui::InputScalar("##ValueInputD", ValueUtils::CTValueTypeToImGui(valueType), to, 0, 0, hex ? "%x" : 0, hex ? ImGuiInputTextFlags_CharsHexadecimal : 0);
    ImGui::PopID();
    return ImGui::IsItemDeactivatedAfterEdit();
}

void Widgets::valueText(const ValueType& valueType, void* value) {
    if (valueType & isSigned) {
        if (valueType & i64)
            ImGui::Text("%lld", *(long long*)value);
        else if (valueType & i32)
            ImGui::Text("%d", *(int*)value);
        else if (valueType & i16)
            ImGui::Text("%hd", *(short*)value);
        else if (valueType & i8)
            ImGui::Text("%hhd", *(char*)value);
    } else {
        if (valueType & i64)
            ImGui::Text("%llu", *(unsigned long long*)value);
        else if (valueType & i32)
            ImGui::Text("%u", *(unsigned int*)value);
        else if (valueType & i16)
            ImGui::Text("%hu", *(unsigned short*)value);
        else if (valueType & i8)
            ImGui::Text("%hhu", *(unsigned char*)value);
    }
    if (valueType & f64)
        ImGui::Text("%f", *(double*)value);
    else if (valueType & f32)
        ImGui::Text("%f", *(float*)value);
}

void Widgets::valueTypeSelector(ValueType& valueType, bool enablePchain) {
    const std::array<std::string, 8> items{"signed?", "int64", "int32", "int16", "int8", "float64", "float32", "pchain"};
    unsigned currentItem = std::log2(valueType & ~isSigned & ~pchain);
    std::string previewString = (valueType & isSigned ? "" : "u") + items[currentItem];
    if (valueType & pchain)
        previewString = "pchain -> " + previewString;
    if (ImGui::BeginCombo("##ValueTypeCombo", previewString.c_str())) {
        for (unsigned n = 0; n < items.size() - !enablePchain; n++) {
            if (n == 0) {
                if (ImGui::Selectable(items[n].c_str(), valueType & isSigned))
                    valueType = ValueType(valueType ^ isSigned);
            } else if (n == 7) {
                if (ImGui::Selectable(items[n].c_str(), valueType & pchain))
                    valueType = ValueType(valueType ^ pchain);
            } else {
                if (ImGui::Selectable(items[n].c_str(), currentItem == n))
                    currentItem = n;
            }
        }
        ImGui::EndCombo();
    }
    if (valueType & (f32 | f64))
        valueType = static_cast<ValueType>(valueType | isSigned);
    valueType = ValueType(1 << currentItem | valueType & isSigned | valueType & pchain);
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
