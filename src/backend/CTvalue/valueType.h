#ifndef CHEAT_TURBINE_VALUETYPE_H
#define CHEAT_TURBINE_VALUETYPE_H
#include <sys/types.h>

enum ValueType : u_int32_t {
    pchain =    1 << 0,
    isSigned =  1 << 1,
    // signature = 1 << 2,

    i64 =       1 << 2,
    i32 =       1 << 3,
    i16 =       1 << 4,
    i8 =        1 << 5,
    f64 =       1 << 6,
    f32 =       1 << 7,
    string =    1 << 8,
};

#endif //CHEAT_TURBINE_VALUETYPE_H