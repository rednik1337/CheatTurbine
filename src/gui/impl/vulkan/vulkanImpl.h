// Dear ImGui: standalone example application for Glfw + Vulkan

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp


#ifndef CHEAT_TURBINE_GUIIMPL_H
#define CHEAT_TURBINE_GUIIMPL_H

#define GLFW_INCLUDE_VULKAN

#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan_beta.h>


// #define IMGUI_UNLIMITED_FRAME_RATE

namespace VulkanImpl {
    extern VkAllocationCallbacks *g_Allocator;
    extern VkInstance g_Instance;
    extern VkPhysicalDevice g_PhysicalDevice;
    extern VkDevice g_Device;
    extern uint32_t g_QueueFamily;
    extern VkQueue g_Queue;
    extern VkDebugReportCallbackEXT g_DebugReport;
    extern VkPipelineCache g_PipelineCache;
    extern VkDescriptorPool g_DescriptorPool;

    extern ImGui_ImplVulkanH_Window g_MainWindowData;
    extern int g_MinImageCount;
    extern bool g_SwapChainRebuild;


    void glfw_error_callback(int error, const char *description);
    void check_vk_result(VkResult err);

    bool IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties, const char *extension);
    VkPhysicalDevice SetupVulkan_SelectPhysicalDevice();
    void SetupVulkan(ImVector<const char *> instance_extensions);
    void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height);
    void CleanupVulkan();
    void CleanupVulkanWindow();
    void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data);
    void FramePresent(ImGui_ImplVulkanH_Window *wd);
}

#endif //CHEAT_TURBINE_GUIIMPL_H
