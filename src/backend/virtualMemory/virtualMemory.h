#ifndef CHEAT_TURBINE_VIRTUALMEMORY_H
#define CHEAT_TURBINE_VIRTUALMEMORY_H


namespace VirtualMemory {
    bool read(void* from, void* to, unsigned long long length);
    bool write(void* from, void* to, unsigned long long length);
}

#endif //CHEAT_TURBINE_VIRTUALMEMORY_H
