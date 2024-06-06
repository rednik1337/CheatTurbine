#ifndef CHEAT_TURBINE_WINDOW_H
#define CHEAT_TURBINE_WINDOW_H

#include <string>
#include <imgui.h>


class Window {
public:
    bool pOpen = true;
    bool shouldBringToFront = false;

    const long long UID{(long long)this}; // allows for multiple windows with the same name
    std::string name = "Unnamed window";


    virtual void draw();
    void operator()();

    Window() = default;
    virtual ~Window() = default;
};


#endif //CHEAT_TURBINE_WINDOW_H
