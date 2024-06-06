#include "selectedProcess.h"
#include "../../gui/gui.h"

#include <sys/wait.h>
#include <fstream>
#include <sstream>


namespace SelectedProcess {
    pid_t pid = detached;

    std::vector<std::function<void()>> attachObservers;
    std::vector<std::function<void()>> detachObservers;
}


void SelectedProcess::attach(const pid_t p) {
    pid = p;

    for (const auto &i: attachObservers)
        i();
}


void SelectedProcess::detach() {
    for (const auto &i: detachObservers)
        i();

    pid = detached;
}


unsigned SelectedProcess::subscribeToAttach(const std::function<void()> &f) {
    attachObservers.emplace_back(f);
    return attachObservers.size()-1;
}


unsigned SelectedProcess::subscribeToDetach(const std::function<void()> &f) {
    detachObservers.emplace_back(f);
    return detachObservers.size()-1;
}

void SelectedProcess::unsubscribeFromAttach(const unsigned subID) {
    attachObservers.erase(attachObservers.begin() + subID);
}

void SelectedProcess::unsubscribeFromDetach(const unsigned subID) {
    detachObservers.erase(detachObservers.begin() + subID);
}


void SelectedProcess::sendSignal(const int sig) {
    ::kill(pid, sig);
}

void SelectedProcess::suspend() {
    sendSignal(SIGSTOP);
}

void SelectedProcess::resume() {
    sendSignal(SIGCONT);
}

void SelectedProcess::terminate() {
    sendSignal(SIGTERM);
}

void SelectedProcess::kill() {
    sendSignal(SIGKILL);
}

bool SelectedProcess::isSuspended() {
    std::string statusFilePath = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream statusFile(statusFilePath);

    if (!statusFile) {
        return false;
    }

    std::string line;
    while (std::getline(statusFile, line)) {
        if (line.substr(0, 6) == "State:") {
            std::istringstream iss(line);
            std::string state;
            iss >> state >> state;

            if (state == "T")
                return true;
            return false;

        }
    }

    return false;
}

unsigned long long SelectedProcess::getBaseAddress() {
    if (std::ifstream mapsFile("/proc/" + std::to_string(pid) + "/maps"); mapsFile.is_open()) {
        std::string line;
        while (std::getline(mapsFile, line)) {
            std::istringstream iss(line);
            std::string addressRange;
            std::getline(iss, addressRange, '-');
            unsigned long long startAddress = std::stoull(addressRange, nullptr, 16);

            mapsFile.close();
            return startAddress;
        }
        mapsFile.close();
    }

    return 0;
}