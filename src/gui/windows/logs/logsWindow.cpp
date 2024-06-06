#include "logsWindow.h"
#include "../../gui.h"

void LogsWindow::draw() {
    if (!ImGui::Begin(name.c_str(), &pOpen))
        return;

    for (const std::string& log_string: Gui::logs)
        ImGui::Text(log_string.c_str());

    ImGui::End();
}
