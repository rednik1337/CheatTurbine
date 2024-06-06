#ifndef CHEAT_TURBINE_SELECTEDPROCESS_H
#define CHEAT_TURBINE_SELECTEDPROCESS_H


#include <csignal>
#include <vector>
#include <functional>

enum Pid {
    detached = -1,
};


namespace SelectedProcess {
    extern pid_t pid;

    void sendSignal(int sig);
    void attach(pid_t p);
    void detach();
    unsigned long long getBaseAddress();
    void suspend();
    void resume();
    void terminate();
    void kill();
    bool isSuspended();

    extern std::vector<std::function<void()>> attachObservers;
    extern std::vector<std::function<void()>> detachObservers;

    unsigned subscribeToAttach(const std::function<void()> &);
    unsigned subscribeToDetach(const std::function<void()> &);
    void unsubscribeFromAttach(unsigned subID);
    void unsubscribeFromDetach(unsigned subID);
}


#endif //CHEAT_TURBINE_SELECTEDPROCESS_H
