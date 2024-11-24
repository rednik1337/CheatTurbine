#include "imguiImpl.h"
#include "../vulkan/vulkanImpl.h"
#include <cstdlib>
#include <imgui_impl_glfw.h>
#include <cstdio>
#include <fontconfig/fontconfig.h>
#include <string>
#include <iostream>

namespace ImGuiImpl {
    GLFWwindow* window;
    ImGui_ImplVulkanH_Window* wd;
    ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}



std::string findSystemFont(const std::string& fontName) {
    const char* appdir = std::getenv("APPDIR");
    if (appdir) {
        std::string bundledFontPath = std::string(appdir) + "/usr/share/fonts/TTF/" + fontName + ".ttf";
        if (FILE* file = fopen(bundledFontPath.c_str(), "r")) {
            fclose(file);
            return bundledFontPath;
        }
    }


    FcInit();
    FcPattern* pattern = FcNameParse((const FcChar8*)fontName.c_str());
    FcConfigSubstitute(nullptr, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);

    FcResult result;
    FcPattern* matched = FcFontMatch(nullptr, pattern, &result);
    if (matched) {
        FcChar8* file = nullptr;
        if (FcPatternGetString(matched, FC_FILE, 0, &file) == FcResultMatch) {
            std::string fontPath((char*)file);
            FcPatternDestroy(matched);
            FcPatternDestroy(pattern);
            return fontPath;
        }
        FcPatternDestroy(matched);
    }
    FcPatternDestroy(pattern);
    return "";
}


void ImGuiImpl::init() {
    glfwSetErrorCallback(VulkanImpl::glfw_error_callback);
    if (!glfwInit())
        exit(0);

    // Create Window with Vulkan context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(1280, 720, mainWindowName, nullptr, nullptr);
    if (!glfwVulkanSupported()) {
        printf("GLFW: Vulkan Not Supported\n");
        exit(0);
    }

    ImVector<const char*> extensions;
    uint32_t extensions_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
    for (uint32_t i = 0; i < extensions_count; i++)
        extensions.push_back(glfw_extensions[i]);
    VulkanImpl::SetupVulkan(extensions);

    // Create Window Surface
    VkSurfaceKHR surface;
    VkResult err = glfwCreateWindowSurface(VulkanImpl::g_Instance, window, VulkanImpl::g_Allocator, &surface);
    VulkanImpl::check_vk_result(err);

    // Create Framebuffers
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    wd = &VulkanImpl::g_MainWindowData;
    VulkanImpl::SetupVulkanWindow(wd, surface, w, h);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();


    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = VulkanImpl::g_Instance;
    init_info.PhysicalDevice = VulkanImpl::g_PhysicalDevice;
    init_info.Device = VulkanImpl::g_Device;
    init_info.QueueFamily = VulkanImpl::g_QueueFamily;
    init_info.Queue = VulkanImpl::g_Queue;
    init_info.PipelineCache = VulkanImpl::g_PipelineCache;
    init_info.DescriptorPool = VulkanImpl::g_DescriptorPool;
    init_info.RenderPass = wd->RenderPass;
    init_info.Subpass = 0;
    init_info.MinImageCount = VulkanImpl::g_MinImageCount;
    init_info.ImageCount = wd->ImageCount;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = VulkanImpl::g_Allocator;
    init_info.CheckVkResultFn = VulkanImpl::check_vk_result;
    ImGui_ImplVulkan_Init(&init_info);

    // hidpi support
    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);
    io.Fonts->AddFontDefault();

    ImFontAtlas* atlas = ImGui::GetIO().Fonts;
    atlas->Clear();
    std::cout << findSystemFont("JetBrainsMonoNerdFont-Regular") << std::endl;

    io.FontDefault = io.Fonts->AddFontFromFileTTF(findSystemFont("JetBrainsMonoNerdFont-Regular").c_str(), 17.0f * xscale);
    atlas->Build();
    style = ImGui::GetStyle();
    style.ScaleAllSizes(xscale);

    {
        VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
        VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

        err = vkResetCommandPool(VulkanImpl::g_Device, command_pool, 0);
        VulkanImpl::check_vk_result(err);
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(command_buffer, &begin_info);
        VulkanImpl::check_vk_result(err);

        ImGui_ImplVulkan_CreateFontsTexture();

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &command_buffer;
        err = vkEndCommandBuffer(command_buffer);
        VulkanImpl::check_vk_result(err);
        err = vkQueueSubmit(VulkanImpl::g_Queue, 1, &end_info, VK_NULL_HANDLE);
        VulkanImpl::check_vk_result(err);

        err = vkDeviceWaitIdle(VulkanImpl::g_Device);
        VulkanImpl::check_vk_result(err);
    }


    style.FrameRounding = 1.0f;
    style.WindowRounding = 3.0f;
    style.ChildRounding = 3.0f;
    style.PopupRounding = 2.0f;
    style.GrabRounding  = 1.0f;
    style.WindowBorderSize = 0.0f;
    style.Colors[ImGuiCol_DockingEmptyBg] = ImColor(0, 0, 0, 0);

    ImVec4 textColor = ImColor(234, 235, 234);
    ImVec4 backgroundColor = ImColor(25, 25, 25);
    ImVec4 primaryColor = ImColor(110, 104, 109);
    ImVec4 secondaryColor = ImColor(22, 24, 23);
    ImVec4 accentColor = ImColor(129, 121, 127);

    clearColor = backgroundColor;
    style.Colors[ImGuiCol_PlotHistogram] = ImColor(66, 150, 250, 100);
//    imGuiStyle.Colors[ImGuiCol_Text] = textColor;
//    imGuiStyle.Colors[ImGuiCol_TextDisabled] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_WindowBg] = backgroundColor;
//    imGuiStyle.Colors[ImGuiCol_ChildBg] = backgroundColor;
//    imGuiStyle.Colors[ImGuiCol_PopupBg] = backgroundColor;
//    imGuiStyle.Colors[ImGuiCol_Border] = secondaryColor;
//    imGuiStyle.Colors[ImGuiCol_BorderShadow] = secondaryColor;
//    imGuiStyle.Colors[ImGuiCol_FrameBg] = secondaryColor;
//    imGuiStyle.Colors[ImGuiCol_FrameBgHovered] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_FrameBgActive] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_TitleBg] = backgroundColor;
//    imGuiStyle.Colors[ImGuiCol_TitleBgActive] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_TitleBgCollapsed] = backgroundColor;
//    imGuiStyle.Colors[ImGuiCol_MenuBarBg] = backgroundColor;
//    imGuiStyle.Colors[ImGuiCol_ScrollbarBg] = backgroundColor;
//    imGuiStyle.Colors[ImGuiCol_ScrollbarGrab] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_ScrollbarGrabHovered] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_ScrollbarGrabActive] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_CheckMark] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_SliderGrab] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_SliderGrabActive] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_Button] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_ButtonHovered] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_ButtonActive] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_Header] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_HeaderHovered] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_HeaderActive] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_Separator] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_SeparatorHovered] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_SeparatorActive] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_ResizeGrip] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_ResizeGripHovered] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_ResizeGripActive] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_Tab] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_TabHovered] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_TabActive] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_TabUnfocused] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_TabUnfocusedActive] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_PlotLines] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_PlotLinesHovered] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_PlotHistogram] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_PlotHistogramHovered] = accentColor;
//    imGuiStyle.Colors[ImGuiCol_TableHeaderBg] = backgroundColor;
//    imGuiStyle.Colors[ImGuiCol_TableBorderStrong] = secondaryColor;
//    imGuiStyle.Colors[ImGuiCol_TableBorderLight] = secondaryColor;
//    imGuiStyle.Colors[ImGuiCol_TableRowBg] = backgroundColor;
//    imGuiStyle.Colors[ImGuiCol_TableRowBgAlt] = backgroundColor;
//    imGuiStyle.Colors[ImGuiCol_TextSelectedBg] = backgroundColor;
//    imGuiStyle.Colors[ImGuiCol_DragDropTarget] = primaryColor;
//    imGuiStyle.Colors[ImGuiCol_NavHighlight] = primaryColor;
}

void ImGuiImpl::destroy() {
    VkResult err;
    err = vkDeviceWaitIdle(VulkanImpl::g_Device);
    VulkanImpl::check_vk_result(err);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    VulkanImpl::CleanupVulkanWindow();
    VulkanImpl::CleanupVulkan();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void ImGuiImpl::newFrame() {
    glfwPollEvents();

    // Resize swap chain?
    if (VulkanImpl::g_SwapChainRebuild) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (width > 0 && height > 0) {
            ImGui_ImplVulkan_SetMinImageCount(VulkanImpl::g_MinImageCount);
            ImGui_ImplVulkanH_CreateOrResizeWindow(VulkanImpl::g_Instance, VulkanImpl::g_PhysicalDevice, VulkanImpl::g_Device, &VulkanImpl::g_MainWindowData, VulkanImpl::g_QueueFamily, VulkanImpl::g_Allocator, width, height, VulkanImpl::g_MinImageCount);
            VulkanImpl::g_MainWindowData.FrameIndex = 0;
            VulkanImpl::g_SwapChainRebuild = false;
        }
    }

    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiImpl::render() {
    ImGui::Render();
    ImDrawData* main_draw_data = ImGui::GetDrawData();
    const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
    wd->ClearValue.color.float32[0] = clearColor.x * clearColor.w;
    wd->ClearValue.color.float32[1] = clearColor.y * clearColor.w;
    wd->ClearValue.color.float32[2] = clearColor.z * clearColor.w;
    wd->ClearValue.color.float32[3] = clearColor.w;
    if (!main_is_minimized)
        VulkanImpl::FrameRender(wd, main_draw_data);


    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    if (!main_is_minimized)
        VulkanImpl::FramePresent(wd);
}

bool ImGuiImpl::shouldClose() {
    return glfwWindowShouldClose(window);
}
