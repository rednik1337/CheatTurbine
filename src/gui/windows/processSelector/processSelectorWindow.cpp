#include "processSelectorWindow.h"
#include "../../../backend/selectedProcess/selectedProcess.h"

#include <string>
#include <vector>
#include <dirent.h>
#include <fstream>


void ProcessSelectorWindow::draw() {
    auto io = ImGui::GetIO();
    if (ImGui::BeginPopupModal("Process selector", nullptr)) {
        static ImGuiTextFilter filter;
        filter.Draw();

        static float timeFromLastProcessListUpdate = 1;
        static std::vector<std::string> processList;
        static DIR* dir;
        static dirent* ent;
        timeFromLastProcessListUpdate += 1.0f / io.Framerate;

        if (timeFromLastProcessListUpdate >= 1) {
            processList.clear();
            if ((dir = opendir("/proc")) != nullptr) {
                while ((ent = readdir(dir)) != nullptr) {
                    if (std::isdigit(ent->d_name[0])) {
                        std::string pid = ent->d_name;

                        std::ifstream status_file("/proc/" + pid + "/status");
                        std::string line;
                        int ppid = -1;
                        while (std::getline(status_file, line)) {
                            if (line.substr(0, 4) == "PPid") {
                                ppid = std::stoi(line.substr(5));
                                break;
                            }
                        }

                        if (ppid != 0 and ppid != 2) {
                            std::ifstream comm_file("/proc/" + pid + "/comm");
                            std::string name;
                            std::getline(comm_file, name);
                            pid += " " + name;
                            processList.emplace_back(pid);
                        }
                    }
                }
                closedir(dir);
                timeFromLastProcessListUpdate = 0;
            }
        }

        ImGui::BeginChild("Processesamogus");
        for (const std::string&process: processList) {
            if (filter.PassFilter(process.c_str()))
                if (ImGui::MenuItem(process.c_str())) {
                    SelectedProcess::attach(std::stoi(process.substr(0, process.find(' '))));
                    pOpen = false;
                }
        }
        ImGui::EndChild();

        ImGui::EndPopup();
    }
    if (!ImGui::IsPopupOpen("Process selector"))
        ImGui::OpenPopup("Process selector");
}
