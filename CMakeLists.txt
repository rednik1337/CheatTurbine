cmake_minimum_required(VERSION 3.28)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(CheatTurbine VERSION 0.1.0)

cmake_policy(SET CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

message("Remove -march=native -mtune=native if distributing")

set(CMAKE_C_FLAGS "Wall -Wextra -Winline -march=native -mtune=native -pipe")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -ggdb3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -flto -fno-stack-protector -fcf-protection=none")
set(CMAKE_C_STANDARD 23)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Winline -march=native -mtune=native -pipe")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -ggdb3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -flto -fno-stack-protector -fcf-protection=none")
set(CMAKE_CXX_STANDARD 26)

set(CMAKE_THREAD_LIBS_INIT "-lpthread")

find_package(imgui CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_executable(CheatTurbine src/main.cpp
        src/gui/gui.cpp
        src/gui/impl/vulkan/vulkanImpl.cpp
        src/gui/impl/imgui/imguiImpl.cpp
        src/gui/windows/generic/window.cpp
        src/gui/windows/processSelector/processSelectorWindow.cpp
        src/gui/windows/processSelector/processSelectorWindow.cpp
        src/gui/windows/memoryEditor/memoryEditorWindow.cpp
        src/backend/selectedProcess/selectedProcess.cpp
        src/backend/CTvalue/valueUtils.cpp
        src/backend/virtualMemory/virtualMemory.cpp
        src/backend/scanner/scanner.cpp
        src/gui/windows/dockSpace/dockSpaceWindow.cpp
        src/gui/windows/logBar/logBarWindow.cpp
        src/gui/windows/settings/settingsWindow.cpp
        src/gui/windows/starredAddresses/starredAddressesWindow.cpp
        src/backend/starredAddress/starredAddress.cpp
        src/gui/windows/menuBar/menuBar.cpp
        src/backend/settings/settings.cpp
        src/gui/windows/scanner/scannerWindow.cpp
        src/gui/windows/logs/logsWindow.cpp
        src/gui/windows/imguiDemo/imguiDemo.cpp
        src/gui/windows/decompiler/decompilerWindow.cpp
        src/gui/windows/disassembler/disassemblerWindow.cpp
        src/gui/windows/pointerScan/pointerScanWindow.cpp
        src/backend/regions/regions.cpp
        src/gui/widgets/widgets.cpp
        src/backend/pointerMap/pointerMap.cpp
        src/backend/pointerScan/pointerScan.cpp
        src/gui/windows/pointerMapManager/pointerMapManagerWindow.cpp
        src/backend/pointerChain/pointerChain.cpp
        src/gui/windows/pchainEditor/pchainEditorWindow.cpp
)

target_link_libraries(CheatTurbine PRIVATE Vulkan::Vulkan imgui::imgui glfw)

configure_file(src/config/config.h.in config.h)
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
