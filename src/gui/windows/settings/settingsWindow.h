#ifndef CHEAT_TURBINE_SETTINGSWINDOW_H
#define CHEAT_TURBINE_SETTINGSWINDOW_H

#include "../generic/window.h"


class SettingsWindow final : public Window {
public:
    void draw() override;

    SettingsWindow() { name = "Settings"; }
};


#endif //CHEAT_TURBINE_SETTINGSWINDOW_H
