#include "CTvalue.h"
#include <array>
#include <cmath>


CTvalue::CTvalue(const CTValueType type, const CTValueFlags flags, const unsigned stringLength) {
    this->type = type;
    this->flags = flags;
    this->stringLength = stringLength;
}

unsigned CTvalue::getSize() const {
    static constexpr std::array<int, 6> sizes{8, 4, 2, 1, 8, 4};
    if (type == string)
        return stringLength;
    return sizes[type];
}


ImGuiDataType CTvalue::getImGuiDataType() const {
    static constexpr std::array<ImGuiDataType, 8> signedTypes {
        ImGuiDataType_S64, ImGuiDataType_S32, ImGuiDataType_S16, ImGuiDataType_S8, ImGuiDataType_Double,
        ImGuiDataType_Float
    };
    static constexpr std::array<ImGuiDataType, 8> unsignedTypes {
        ImGuiDataType_U64, ImGuiDataType_U32, ImGuiDataType_U16, ImGuiDataType_U8, ImGuiDataType_Double,
        ImGuiDataType_Float
    };
    const unsigned currentIndex = type;
    return flags & isSigned ? signedTypes[currentIndex] : unsignedTypes[currentIndex];
}


std::string CTvalue::format(void* mem, const bool hex) const {
    static char buf[128];
    const char* fmt = getFmtStr(hex).c_str();

    switch (type) {
        case i64:
            snprintf(buf, 128, fmt, *(uint64_t*)mem);
            break;
        case i32:
            snprintf(buf, 128, fmt, *(uint32_t*)mem);
            break;
        case i16:
            snprintf(buf, 128, fmt, *(uint16_t*)mem);
            break;
        case i8:
            snprintf(buf, 128, fmt, *(uint8_t*)mem);
            break;
        case f64:
            snprintf(buf, 128, fmt, *(double_t*)mem);
            break;
        case f32:
            snprintf(buf, 128, fmt, *(float_t*)mem);
            break;
        case string:
            snprintf(buf, 128, fmt, (char*)mem);
            break;
    }

    return buf;
}


std::string CTvalue::getFmtStr(const bool hex) const {
    char fmt[8] = "%";
    static constexpr std::array<char[3], 8> fmts{"ll", "l", "h", "hh", "", "", "s"};

    strcpy(&fmt[1], fmts[type]);
    if (type != string) {
        if (hex)
            strcat(fmt, (type == f32 or type == f64) ? "a" : "x");
        else
            strcat(fmt, (type == f32 or type == f64) ? "f" : (flags & isSigned ? "d" : "u"));
    }

    return fmt;
}
