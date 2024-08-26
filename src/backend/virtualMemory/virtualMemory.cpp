#include "virtualMemory.h"
#include "../selectedProcess/selectedProcess.h"

#include <cstdio>
#include <sys/uio.h>

#include "../../gui/gui.h"


bool VirtualMemory::read(void* from, void* to, const unsigned long long length) {
    const iovec local[1] = {{to, length}};
    const iovec remote[1] = {{from, length}};

    const ssize_t nread = process_vm_readv(SelectedProcess::pid, local, 1, remote, 1, 0);
    if (nread <= 0)
        Gui::log("process_vm_readv({:p}) failed: {} ({})", from, errno, strerror(errno));
    return nread > 0;
}


bool VirtualMemory::write(void* from, void* to, const unsigned long long length) {
    const iovec local[1] = {{from, length}};
    const iovec remote[1] = {{to, length}};

    const ssize_t nwrote = process_vm_writev(SelectedProcess::pid, local, 1, remote, 1, 0);
    if (nwrote <= 0)
        Gui::log("process_vm_writev({:p}) failed: {} ({})", to, errno, strerror(errno));
    return nwrote > 0;
}
