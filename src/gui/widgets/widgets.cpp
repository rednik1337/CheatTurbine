#include "widgets.h"

#include <cmath>
#include <array>
#include <string>
#include <imgui_internal.h>


bool Widgets::valueInputTrueOnEditing(const ValueType& valueType, void* to, const int bufSize) {
    bool res;
    ImGui::PushID(int((unsigned long long)to << 1));
    if (valueType & string)
        res = ImGui::InputText("##ValueInputE", (char*)to, bufSize);
    else
        res = ImGui::InputScalar("##ValueInputE", ValueUtils::CTValueTypeToImGui(valueType), to);
    ImGui::PopID();
    return res;
}


bool Widgets::valueInputTrueOnDeactivation(const ValueType& valueType, void* to, bool hex, const int bufSize, bool pointer) {
    ImGui::PushID((void*)((u_int64_t)to << 1));
    if (valueType & string)
        ImGui::InputText("##ValueInputD", (char*)to, bufSize, hex ? ImGuiInputTextFlags_CharsHexadecimal : 0);
    else
        ImGui::InputScalar("##ValueInputD", ValueUtils::CTValueTypeToImGui(valueType), to, nullptr, nullptr, hex ? "%x" : pointer ? "%p" : nullptr, hex ? ImGuiInputTextFlags_CharsHexadecimal : 0);
    ImGui::PopID();
    return ImGui::IsItemDeactivatedAfterEdit();
}

void Widgets::valueText(const ValueType& valueType, void* value) {
    if (valueType & isSigned) {
        if (valueType & i64)
            ImGui::Text("%lld", *(int64_t*)value);
        else if (valueType & i32)
            ImGui::Text("%d", *(int32_t*)value);
        else if (valueType & i16)
            ImGui::Text("%hd", *(int16_t*)value);
        else if (valueType & i8)
            ImGui::Text("%hhd", *(int8_t*)value);
    } else {
        if (valueType & i64)
            ImGui::Text("%llu", *(u_int64_t*)value);
        else if (valueType & i32)
            ImGui::Text("%u", *(u_int32_t*)value);
        else if (valueType & i16)
            ImGui::Text("%hu", *(u_int16_t*)value);
        else if (valueType & i8)
            ImGui::Text("%hhu", *(u_int8_t*)value);
    }
    if (valueType & f64)
        ImGui::Text("%f", *(double_t*)value);
    else if (valueType & f32)
        ImGui::Text("%f", *(float_t*)value);
}

void Widgets::valueTypeSelector(ValueType& valueType, bool enablePchain) {
    const std::array<std::string, 9> items{"pchain", "signed?", "int64", "int32", "int16", "int8", "float64", "float32", "string"};
    unsigned currentItem = std::log2(valueType & ~isSigned & ~pchain);
    std::string previewString = (valueType & isSigned | (valueType & string) >> 7 ? "" : "u") + items[currentItem];
    if (valueType & pchain)
        previewString = "pchain -> " + previewString;
    if (ImGui::BeginCombo("##ValueTypeCombo", previewString.c_str())) {
        for (unsigned n = 0; n < items.size(); n++) {
            if (n == 1) {
                if (valueType & ~(f32 | f64 | string | isSigned))
                    if (ImGui::Selectable(items[n].c_str(), valueType & isSigned))
                        valueType = ValueType(valueType ^ isSigned);
            } else if (n == 0) {
                if (enablePchain)
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
    // else if (valueType & string)
    //     valueType = ValueType(valueType | ~isSigned);
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
