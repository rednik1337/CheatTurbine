#ifndef CHEAT_TURBINE_V1_IMGUIIMPL_H
#define CHEAT_TURBINE_V1_IMGUIIMPL_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <config.h>

namespace ImGuiImpl {
    constexpr char mainWindowName[] = "Cheat Turbine " CheatTurbineVersion;
    extern GLFWwindow* window;
    extern ImGui_ImplVulkanH_Window* wd;
    extern ImVec4 clearColor;

    void init();
    void destroy();
    void newFrame();
    void render();

    bool shouldClose();
}


#endif //CHEAT_TURBINE_V1_IMGUIIMPL_H
