#include "valueUtils.h"
#include <cmath>
#include <array>


unsigned ValueUtils::sizeofValueType(const ValueType valueType) {
    static constexpr std::array<unsigned, 8> sizes{8, 4, 2, 1, 8, 4};
    return sizes[valueIndex(valueType)];
}


unsigned ValueUtils::valueIndex(const ValueType valueType) {
    return std::log2(valueType >> 1);
}


ImGuiDataType ValueUtils::CTValueTypeToImGui(ValueType valueType) {
    static constexpr std::array<ImGuiDataType, 8> signedTypes{ImGuiDataType_S64, ImGuiDataType_S32, ImGuiDataType_S16, ImGuiDataType_S8, ImGuiDataType_Double, ImGuiDataType_Float};
    static constexpr std::array<ImGuiDataType, 8> unsignedTypes{ImGuiDataType_U64, ImGuiDataType_U32, ImGuiDataType_U16, ImGuiDataType_U8, ImGuiDataType_Double, ImGuiDataType_Float};
    const unsigned currentIndex = valueIndex(valueType);
    return valueType & isSigned ? signedTypes[currentIndex] : unsignedTypes[currentIndex];
}

std::string ValueUtils::format(const ValueType valueType, void* mem, bool hex) {
    std::string fmt = "%";
    if (valueType & f64)
        fmt += "L";
    else if (valueType & i64)
        fmt += "ll";
    else if (valueType & i32)
        fmt += "l";
    else if (valueType & i16)
        fmt += "h";
    else if (valueType & i8)
        fmt += "hh";
    if (hex)
        fmt += valueType & (f32 | f64) ? "a" : "x";
    else
        fmt += valueType & (f32 | f64) ? "f" : valueType & isSigned ? "d" : "u";

    static char buf[256];

    if (valueType & f32)
        sprintf(buf, fmt.c_str(), *(float_t*)mem);
    else if (valueType & f64)
        sprintf(buf, fmt.c_str(), *(double_t*)mem);
    else if (valueType & i32)
        sprintf(buf, fmt.c_str(), *(int32_t*)mem);
    else if (valueType & i16)
        sprintf(buf, fmt.c_str(), *(int16_t*)mem);
    else if (valueType & i8)
        sprintf(buf, fmt.c_str(), *(int8_t*)mem);

    return buf;
}
