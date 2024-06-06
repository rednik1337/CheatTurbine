#include "virtualMemory.h"
#include "../selectedProcess/selectedProcess.h"

#include <cstdio>
#include <sys/uio.h>


bool VirtualMemory::read(void* from, void* to, const unsigned long long length) {
    const iovec local[1] = {{to, length}};
    const iovec remote[1] = {{from, length}};

    const ssize_t nread = process_vm_readv(SelectedProcess::pid, local, 1, remote, 1, 0);
    // if (nread <= 0)
    //     perror("process_vm_readv");
    return nread > 0;
}


bool VirtualMemory::write(void* from, void* to, const unsigned long long length) {
    const iovec local[1] = {{from, length}};
    const iovec remote[1] = {{to, length}};

    const ssize_t nwrote = process_vm_writev(SelectedProcess::pid, local, 1, remote, 1, 0);
    // if (nwrote <= 0)
    //     perror("process_vm_writev");
    return nwrote > 0;
}
