#ifndef CHEAT_TURBINE_PROCESSSELECTORWINDOW_H
#define CHEAT_TURBINE_PROCESSSELECTORWINDOW_H

#include "../generic/window.h"


class ProcessSelectorWindow final : public Window {
public:
    void draw() override;

    ProcessSelectorWindow() { name = "Process selector"; }
};


#endif //CHEAT_TURBINE_PROCESSSELECTORWINDOW_H
