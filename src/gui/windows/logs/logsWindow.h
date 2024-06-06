#ifndef LOGS_H
#define LOGS_H
#include "../generic/window.h"


class LogsWindow final : public Window {
public:
    void draw() override;

    LogsWindow() { name = "Logs"; }
};


#endif //LOGS_H
