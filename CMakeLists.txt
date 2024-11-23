cmake_minimum_required(VERSION 3.28)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(CheatTurbine VERSION 0.2.0)


set(ARCH_LEVEL "native" CACHE STRING "Target microarchitecture level")

set(TARGET_ARCH_FLAGS "")

if(ARCH_LEVEL STREQUAL "x86-64")
    set(TARGET_ARCH_FLAGS "-march=x86-64")
elseif(ARCH_LEVEL STREQUAL "x86-64-v2")
    set(TARGET_ARCH_FLAGS "-march=x86-64-v2")
elseif(ARCH_LEVEL STREQUAL "x86-64-v3")
    set(TARGET_ARCH_FLAGS "-march=x86-64-v3")
elseif(ARCH_LEVEL STREQUAL "x86-64-v4")
    set(TARGET_ARCH_FLAGS "-march=x86-64-v4")
elseif(ARCH_LEVEL STREQUAL "native")
    set(TARGET_ARCH_FLAGS "-march=native -mtune=native")
else()
    message(FATAL_ERROR "Invalid ARCH_LEVEL: ${ARCH_LEVEL}")
endif()

message("Compiling with ${TARGET_ARCH_FLAGS}")


cmake_policy(SET CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)


set(COMMON_FLAGS "-Wall -Wextra -Winline -pipe -static-libgcc -fno-stack-protector -fcf-protection=none -fno-pie -no-pie")
set(DEBUG_FLAGS "-g -ggdb3")
set(RELEASE_FLAGS "-DNDEBUG -DNS_BLOCK_ASSERTIONS -O3 -flto")

set(CMAKE_C_FLAGS "${COMMON_FLAGS} ${TARGET_ARCH_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} ${DEBUG_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} ${RELEASE_FLAGS}")
set(CMAKE_C_STANDARD 23)

set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -static-libstdc++ ${TARGET_ARCH_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${RELEASE_FLAGS}")
set(CMAKE_CXX_STANDARD 26)

set(CMAKE_SKIP_RPATH TRUE)

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_FIND_LIBRARY_PREFER_STATIC TRUE)

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
        src/gui/windows/pointerScan/pointerScanWindow.cpp
        src/backend/regions/regions.cpp
        src/gui/widgets/widgets.cpp
        src/backend/pointerMap/pointerMap.cpp
        src/backend/pointerScan/pointerScan.cpp
        src/gui/windows/pointerMapManager/pointerMapManagerWindow.cpp
        src/backend/pointerChain/pointerChain.cpp
        src/gui/windows/pchainEditor/pchainEditorWindow.cpp
        src/gui/windows/structureDissector/structureDissectorWindow.cpp
        src/backend/CTvalue/CTvalue.cpp
)

target_link_libraries(CheatTurbine PRIVATE Vulkan::Vulkan imgui::imgui glfw)

configure_file(src/config/config.h.in config.h)
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
