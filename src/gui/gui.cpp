#include "gui.h"
#include "impl/imgui/imguiImpl.h"
#include "windows/windows.h"
#include <map>


namespace Gui {
    std::list<std::unique_ptr<Window>> windows;
    std::list<std::string> logs;
}


void Gui::mainLoop() {
    ImGuiImpl::init();

    addWindow(new MenuBarWindow());
    addWindow(new DockSpaceWindow());
    addWindow(new LogBarWindow());
    addWindow(new ProcessSelectorWindow());
    addWindow(new StarredAddressesWindow());
    addWindow(new ScannerWindow());

    SelectedProcess::subscribeToAttach([] { log("Attached to {}", SelectedProcess::pid); });
    SelectedProcess::subscribeToDetach([] { addWindow(new ProcessSelectorWindow()); });
    SelectedProcess::subscribeToDetach([] { log("Detached from {}", SelectedProcess::pid); });

    while (!ImGuiImpl::shouldClose()) {
        ImGuiImpl::newFrame();
        for (auto it = windows.begin(); it != windows.end();) {
            if (const auto window = it->get(); !window->pOpen) {
                it = windows.erase(it);
            } else {
                window->operator()();
                ++it;
            }
        }

        ImGuiImpl::render();
    }

    ImGuiImpl::destroy();
}

void Gui::addWindow(Window* window) {
    static std::map<std::string, int> totalWindows;
    if (totalWindows[window->name] != 0)
        window->name = std::format("{} {}", window->name, totalWindows[window->name]);
    totalWindows[window->name]++;
    // std::cout << "Opened " << window->name << std::endl;
    windows.emplace_back(window);
}
