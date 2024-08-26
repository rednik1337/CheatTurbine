#ifndef CTVALUE_H
#define CTVALUE_H

#include <cstdint>
#include <imgui.h>
#include <string>


enum CTValueType : uint8_t {
    i64,
    i32,
    i16,
    i8,
    f64,
    f32,
    string,
};


enum CTValueFlags : uint8_t {
    none                = 0,
    isSigned            = 1 << 0,
    isNullTerminated    = 1 << 1,
    pchain              = 1 << 2,
};


inline CTValueFlags operator|(const CTValueFlags a, const CTValueFlags b) {
    return CTValueFlags((uint8_t)a | (uint8_t)b);
}

inline CTValueFlags operator&(const CTValueFlags a, const CTValueFlags b) {
    return CTValueFlags((uint8_t)a & (uint8_t)b);
}

inline CTValueFlags operator^(const CTValueFlags a, const CTValueFlags b) {
    return CTValueFlags((uint8_t)a ^ (uint8_t)b);
}


class CTvalue {
public:
    CTValueType type : 8;
    CTValueFlags flags : 8;
    unsigned stringLength : 16;

    CTvalue(CTValueType type=i32, CTValueFlags flags=none, unsigned stringLength=0);
    unsigned getSize() const;
    ImGuiDataType getImGuiDataType() const;
    std::string format(void* mem, bool hex) const;
    std::string getFmtStr(bool hex) const;
};



#endif //CTVALUE_H
