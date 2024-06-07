#include "menuBar.h"
#include "../../gui.h"
#include "../../../backend/selectedProcess/selectedProcess.h"
#include "../windows.h"

#include <imgui_stdlib.h>


void MenuBarWindow::draw() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Project")) {
            if (ImGui::MenuItem("Open")) {

            }
            if (ImGui::MenuItem("Save")) {

            }
            ImGui::EndMenu();
        }

        // if (ImGui::BeginMenu("Edit")) {
        //     if (ImGui::MenuItem("Settings"))
        //         Gui::addWindow(new SettingsWindow());
        //     ImGui::EndMenu();
        // }

        if (ImGui::BeginMenu("Windows")) {
            if (ImGui::BeginMenu("Opened")) {
                for (const auto& window : Gui::windows) {
                    if (window->name.substr(0, 14) != "Unnamed window") {
                        if (ImGui::BeginMenu(window->name.c_str())) {
                            if (ImGui::BeginMenu("Rename")) {
                                std::string newName = window->name;
                                ImGui::SetNextItemWidth(ImGui::GetStyle().FramePadding.x * 2 + ImGui::CalcTextSize(newName.c_str()).x);
                                if (ImGui::InputText("##windowname", &newName, ImGuiInputTextFlags_EnterReturnsTrue))
                                    window->name = newName;
                                ImGui::EndMenu();
                            }
                            if (ImGui::MenuItem("Focus"))
                                window->shouldBringToFront = true;
                            if (ImGui::MenuItem("Close"))
                                window->pOpen = false;

                            ImGui::EndMenu();
                        }
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("New")) {
                if (ImGui::MenuItem("Scanner"))
                    Gui::addWindow(new ScannerWindow());
                if (ImGui::MenuItem("Memory editor"))
                    Gui::addWindow(new MemoryEditorWindow());
                // if (ImGui::MenuItem("Settings"))
                //     Gui::addWindow(new SettingsWindow());
                if (ImGui::MenuItem("Starred addresses"))
                    Gui::addWindow(new StarredAddressesWindow());
                if (ImGui::MenuItem("Logs"))
                    Gui::addWindow(new LogsWindow());
                if (ImGui::MenuItem("ImGui demo"))
                    Gui::addWindow(new ImguiDemoWindow());
                if (ImGui::MenuItem("Pointer scan"))
                    Gui::addWindow(new PointerScanWindow());
                if (ImGui::MenuItem("Pointermap manager"))
                    Gui::addWindow(new PointerMapManagerWindow());
                if (ImGui::MenuItem("Structure dissector"))
                    Gui::addWindow(new StructureDissectorWindow());
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Process")) {
            if (ImGui::MenuItem("Select another"))
                SelectedProcess::detach();

            if (ImGui::MenuItem("Terminate"))
                SelectedProcess::terminate();

            if (ImGui::MenuItem("Kill"))
                SelectedProcess::kill();

            if (SelectedProcess::isSuspended()) {
                if (ImGui::MenuItem("Resume"))
                    SelectedProcess::resume();
            } else {
                if (ImGui::MenuItem("Suspend"))
                    SelectedProcess::suspend();
            }

            if (ImGui::MenuItem("Memory Editor"))
                Gui::addWindow(new MemoryEditorWindow());


            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
