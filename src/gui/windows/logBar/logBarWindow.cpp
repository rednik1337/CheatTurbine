#include "logBarWindow.h"
#include "../../gui.h"

void LogBarWindow::draw() {
    auto io = ImGui::GetIO();
    const float textSizeY = ImGui::CalcTextSize("A").y;
    float barSizeY = textSizeY * 1.333;
    float windowPaddingX = 10;

    ImGui::SetNextWindowPos({0, io.DisplaySize.y - barSizeY});
    ImGui::SetNextWindowSize({io.DisplaySize.x, barSizeY});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {windowPaddingX, (barSizeY - textSizeY) / 2});

    ImGui::Begin("##LogBarWindow", nullptr, ImGuiWindowFlags_NoMouseInputs |
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
                                            ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::TextUnformatted(Gui::logs.back().c_str()); ImGui::SameLine();
    const std::string framerateStr = std::format("{:.2f}", io.Framerate);

    ImGui::SetCursorPosX(io.DisplaySize.x - windowPaddingX - ImGui::CalcTextSize(framerateStr.c_str()).x);
    ImGui::TextUnformatted(framerateStr.c_str());
    ImGui::End();

    ImGui::PopStyleVar();
}
