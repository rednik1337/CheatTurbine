#include "pchainEditorWindow.h"
#include "../../widgets/widgets.h"
#include "../../../backend/virtualMemory/virtualMemory.h"

#include <format>
#include <imgui_stdlib.h>


void PchainEditorWindow::draw() {
    if (!ImGui::Begin(name.c_str(), &pOpen))
        return;

    static uint64_t mem;
    ImGui::InputText("Name", &address->name);
    Widgets::valueTypeSelector(address->valueType);
    ImGui::SameLine();
    ImGui::TextUnformatted("Type");


    PointerChain& pchain = address->pchain;
    if (address->valueType.flags & CTValueFlags::pchain) {
        static int selectedI = -1;
        static Region selectedRegion;
        std::string preview;
        std::string comboName = "Base address";
        static Regions regions;

        if (selectedI != -1) {
            preview = std::format("{} ({:x})", selectedRegion.path, selectedRegion.offset);
            comboName = std::format("-> {:p}", selectedRegion.start);
        } else {
            int i = 0;
            regions.parse();
            for (const auto& region: regions.regions) {
                if (region.inodeID) {
                    if (region.offset == address->pchain.regionOffset and region.path == address->pchain.path) {
                        selectedRegion = region;
                        selectedI = i;
                    }
                }
                ++i;
            }
        }
        if (ImGui::BeginCombo(comboName.c_str(), preview.c_str())) {
            static int framesPassed = 1000;
            framesPassed += selectedI == -1;
            if (framesPassed > 60) {
                regions.parse();
                framesPassed = 0;
            }
            int i = 0;

            for (auto region: regions.regions) {
                if (region.inodeID) {
                    if (region.path[0] != '[') {
                        if (ImGui::Selectable(std::format("{} ({:x})", region.path, region.offset).c_str(), i == selectedI)) {
                            selectedI = i;
                            selectedRegion = region;
                            pchain.head = region.start;
                            pchain.path = region.path;
                            pchain.regionOffset = region.offset;
                        }
                    }
                }
                ++i;
            }
            ImGui::EndCombo();
        }

        void* prevAddr = pchain.head;
        void* nextAddr;
        int j = 0;
        for (auto& i: pchain.offsets) {
            ImGui::PushID(&i);
            ImGui::SetNextItemWidth(ImGui::CalcTextSize("0xffffffff+-").x + ImGui::GetStyle().FramePadding.x * 6 + ImGui::GetStyle().CellPadding.x * 3);

            if (j++ == pchain.offsets.size() - 1) {
                if (!VirtualMemory::read((char*)prevAddr + i, &nextAddr, 8))
                    ImGui::InputInt(std::format("{:p} + {:x} = ???", prevAddr, i).c_str(), &i, 1, 100, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
                else
                    ImGui::InputInt(std::format("{:p} + {:x} = {:p} ({})", prevAddr, i, (void*)((char*)prevAddr + i), address->valueType.format(&nextAddr, false)).c_str(), &i, 1, 100, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
            } else {
                if (!VirtualMemory::read((char*)prevAddr + i, &nextAddr, 8))
                    ImGui::InputInt(std::format("[{:p} + {:x}] -> ???", prevAddr, i).c_str(), &i, 1, 100, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
                else
                    ImGui::InputInt(std::format("[{:p} + {:x}] -> {:p}", prevAddr, i, nextAddr).c_str(), &i, 1, 100, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
            }
            ImGui::PopID();
            prevAddr = nextAddr;
        }


        if (ImGui::SmallButton("Add offset"))
            pchain.offsets.emplace_back(0);
        ImGui::SameLine();
        if (ImGui::SmallButton("Remove offset"))
            pchain.offsets.pop_back();
    } else {
        ImGui::InputScalar("Address", ImGuiDataType_S64, &address, nullptr, nullptr, "%p", ImGuiInputTextFlags_CharsHexadecimal);

        if (!VirtualMemory::read(address, &mem, address->valueType.getSize())) {
            ImGui::TextUnformatted("= ?");
        } else {
            const std::string text = "= " + address->valueType.format(&mem, false);
            ImGui::TextUnformatted(text.c_str());
        }
    }
    ImGui::NewLine();


    ImGui::End();
}
