#ifndef CHEAT_TURBINE_VALUETYPE_H
#define CHEAT_TURBINE_VALUETYPE_H

enum ValueType : unsigned {
    isSigned =  1 << 0,
    i64 =       1 << 1,
    i32 =       1 << 2,
    i16 =       1 << 3,
    i8 =        1 << 4,
    f64 =       1 << 5,
    f32 =       1 << 6,
    pchain =    1 << 7,
    sig =       1 << 8,
};

#endif //CHEAT_TURBINE_VALUETYPE_H