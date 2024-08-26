#include "logsWindow.h"

#include "../../gui.h"

#include <fstream>

void LogsWindow::draw() {
    if (!ImGui::Begin(name.c_str(), &pOpen, ImGuiWindowFlags_MenuBar))
        return;

    for (auto [logStr, multiplier]: Gui::logs) {
        if (multiplier)
            logStr += " x" + std::to_string(multiplier);
        ImGui::TextUnformatted(logStr.c_str());
    }


    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Export")) {
            if (ImGui::MenuItem("To file")) {

                char filename[1024];
                FILE *f = popen("zenity --file-selection --save --filename=\"CTlog.txt\"", "r");
                fgets(filename, 1024, f);
                filename[strcspn(filename, "\n")] = 0;

                std::ofstream logOutput(filename);
                for (auto [logStr, multiplier]: Gui::logs) {
                    if (multiplier)
                        logStr += " x" + std::to_string(multiplier);
                    logOutput << logStr << '\n';
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();
}
