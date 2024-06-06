#include "dockSpaceWindow.h"


void DockSpaceWindow::draw() {
    const float barsSizeY = ImGui::CalcTextSize("A").y * 1.333;
    auto io = ImGui::GetIO();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::SetNextWindowPos({0, barsSizeY+1});
    ImGui::SetNextWindowSize({io.DisplaySize.x, io.DisplaySize.y - barsSizeY*2});
    ImGui::Begin("##DockSpace", nullptr, ImGuiWindowFlags_NoMouseInputs |
                                         ImGuiWindowFlags_NoInputs |
                                         ImGuiWindowFlags_NoTitleBar |
                                         ImGuiWindowFlags_NoDocking |
                                         ImGuiWindowFlags_NoScrollbar |
                                         ImGuiWindowFlags_NoScrollWithMouse |
                                         ImGuiWindowFlags_NoCollapse |
                                         ImGuiWindowFlags_NoDecoration |
                                         ImGuiWindowFlags_NoFocusOnAppearing |
                                         ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_NoNav |
                                         ImGuiWindowFlags_NoNavInputs |
                                         ImGuiWindowFlags_NoNavFocus |
                                         ImGuiWindowFlags_NoResize |
                                         ImGuiWindowFlags_NoSavedSettings |
                                         ImGuiWindowFlags_NoBringToFrontOnFocus |
                                         ImGuiWindowFlags_NoBackground);


    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        const ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
    }

    ImGui::End();
    ImGui::PopStyleVar();
}
