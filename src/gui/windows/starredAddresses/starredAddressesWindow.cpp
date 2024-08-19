#include "starredAddressesWindow.h"

#include <cmath>

#include "../../widgets/widgets.h"
#include "../../gui.h"
#include "../../windows/windows.h"
#include "../../../backend/virtualMemory/virtualMemory.h"

#include <imgui_stdlib.h>
#include <string>
#include <format>
#include <iostream>
#include <thread>
#include <utility>


void StarredAddressesWindow::addAddressPopup(bool& pOpen) {
    if (ImGui::Begin("Add address", &pOpen)) {
        static std::string name;
        static void* address{};
        static CTvalue valueType = i32;
        static uint64_t mem;

        ImGui::InputText("Name", &name);
        Widgets::valueTypeSelector(valueType);
        ImGui::SameLine();
        ImGui::TextUnformatted("Type");

        static PointerChain pchain;
        if (valueType.flags & CTValueFlags::pchain) {
            static int selectedI = -1;
            static Region selectedRegion;
            std::string preview;
            std::string comboName = "Base address";
            if (selectedI != -1) {
                preview = std::format("{} ({:x})", selectedRegion.path, selectedRegion.offset);
                comboName = std::format("-> {:p}", selectedRegion.start);
            }
            if (ImGui::BeginCombo(comboName.c_str(), preview.c_str())) {
                static Regions regions;
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
                                pchain = PointerChain(region.path, region.start, {}, region.offset);
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
                    if (!VirtualMemory::read((char*)prevAddr + i, &nextAddr, 8)) {
                        ImGui::InputInt(std::format("{:p} + {:x} = ???", prevAddr, i).c_str(), &i, 1, 10, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
                    } else {
                        void* resAddr = (char*)prevAddr + i;
                        const auto fmt = std::format("{:p} + {:x} = {:p} ({})", prevAddr, i, resAddr, valueType.format(&nextAddr, false));
                        ImGui::InputInt(fmt.c_str(), &i, 1, 10, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
                    }
                } else {
                    if (!VirtualMemory::read((char*)prevAddr + i, &nextAddr, 8))
                        ImGui::InputInt(std::format("[{:p} + {:x}] -> ???", prevAddr, i).c_str(), &i, 1, 10, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
                    else
                        ImGui::InputInt(std::format("[{:p} + {:x}] -> {:p}", prevAddr, i, nextAddr).c_str(), &i, 1, 10, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
                }
                prevAddr = nextAddr;
                ImGui::PopID();
            }


            if (ImGui::SmallButton("Add offset"))
                pchain.offsets.emplace_back(0);
            ImGui::SameLine();
            if (ImGui::SmallButton("Remove offset"))
                pchain.offsets.pop_back();
        } else {
            ImGui::InputScalar("Address", ImGuiDataType_S64, &address, nullptr, nullptr, "%p", ImGuiInputTextFlags_CharsHexadecimal);

            if (!VirtualMemory::read(address, &mem, valueType.getSize())) {
                ImGui::TextUnformatted("= ?");
            } else {
                std::string text = "= " + valueType.format(&mem, false);
                ImGui::TextUnformatted(text.c_str());
            }
        }
        ImGui::NewLine();
        if (ImGui::Button("Add")) {
            addresses.emplace_back(name, valueType, address);
            if (valueType.flags & CTValueFlags::pchain)
                addresses.back().pchain = pchain;
        }

        ImGui::End();
    }
}


void StarredAddressesWindow::draw() {
    ImGui::Begin(name.c_str(), &pOpen);
    static bool showAddAddressPopup = false;
    if (ImGui::SmallButton("+"))
        showAddAddressPopup = !showAddAddressPopup;
    if (showAddAddressPopup)
        addAddressPopup(showAddAddressPopup);


    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("ClearGenerate pointermap").x - ImGui::GetStyle().FramePadding.x * 4 - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().CellPadding.x);
    if (ImGui::SmallButton("Generate pointermap")) {
        if (Gui::getWindows<PointerMapManagerWindow>().empty())
            Gui::addWindow(new PointerMapManagerWindow());
        const auto ptrmapManager = Gui::getWindows<PointerMapManagerWindow>().front();
        ptrmapManager->addPmap(addresses, std::format("{} ptrmap", name));
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Clear").x - ImGui::GetStyle().FramePadding.x * 2 - ImGui::GetStyle().WindowPadding.x);
    if (ImGui::SmallButton("Clear"))
        addresses.clear();


    static std::vector<unsigned> addressesMarkedForDeletion;
    if (ImGui::BeginTable("StarredAddrsTable", 5, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Freeze", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 0.0f);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();


        ImGuiListClipper clipper;
        clipper.Begin(addresses.size());
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                ImGui::TableNextRow();

                ImGui::PushID(row);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

                ImGui::TableNextColumn();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 13);
                ImGui::Checkbox("##Freeze", &addresses[row].isFrozen);

                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText("##Name", &addresses[row].name);

                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-1);
                if (addresses[row].valueType.flags & pchain)
                    ImGui::Text(" P->%p", addresses[row].address);
                else
                    ImGui::InputScalar("##Address", ImGuiDataType_U64, &addresses[row].address, nullptr, nullptr, "%p", ImGuiInputTextFlags_CharsHexadecimal);

                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-1);
                if (addresses[row].valueType.flags & pchain)
                    Widgets::valueTypeSelector(addresses[row].valueType);
                else
                    Widgets::valueTypeSelector(addresses[row].valueType, false);

                addresses[row].valueBytes.resize(addresses[row].valueType.getSize());

                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-1);
                if (addresses[row].valueType.flags & pchain and !addresses[row].pchain.isValid) {
                    ImGui::TextUnformatted("???");
                } else {
                    if (Widgets::valueInputTrueOnDeactivation(addresses[row].valueType, addresses[row].valueBytes.data(), addresses[row].displayType == hex)) {
                        VirtualMemory::write(addresses[row].valueBytes.data(), addresses[row].address, addresses[row].valueBytes.size());
                        Gui::log("Wrote {} to {:p}", addresses[row].valueType.format(addresses[row].valueBytes.data(), false), addresses[row].address);
                    }
                }


                ImGui::PopStyleColor();
                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, 0);
                ImGui::Selectable("##Selectable", false, ImGuiSelectableFlags_SpanAllColumns);
                ImGui::PopStyleColor();
                if (ImGui::BeginPopupContextItem("##Popup")) {
                    if (addresses[row].valueType.type != string) {
                        if (ImGui::BeginMenu("Display type")) {
                            const auto decimalRepresentation = addresses[row].valueType.format(addresses[row].valueBytes.data(), false);
                            const auto hexRepresentation = addresses[row].valueType.format(addresses[row].valueBytes.data(), true);
                            if (ImGui::MenuItem(std::format("dec -> {}", decimalRepresentation).c_str()))
                                addresses[row].displayType = dec;

                            if (ImGui::MenuItem(std::format("hex -> {}", hexRepresentation).c_str()))
                                addresses[row].displayType = hex;

                            ImGui::EndMenu();
                        }
                    }
                    if (ImGui::Selectable("Open in memory editor")) {
                        Gui::windows.emplace_back(new MemoryEditorWindow((uint64_t)addresses[row].address));
                    }
                    if (ImGui::Selectable("Remove from starred")) {
                        addressesMarkedForDeletion.push_back(row);
                    }

                    if (addresses[row].valueType.flags & CTValueFlags::pchain)
                        if (ImGui::Selectable("Edit")) {
                            auto* window = new PchainEditorWindow();
                            window->address = &addresses[row];
                            Gui::addWindow(window);
                        }

                    ImGui::EndPopup();
                }

                ImGui::PopID();
                addresses[row].update();
            }
        }
        for (const unsigned i: addressesMarkedForDeletion)
            addresses.erase(addresses.begin() + i);
        addressesMarkedForDeletion.clear();
        ImGui::EndTable();
    }
    ImGui::End();
}

void StarredAddressesWindow::addAddress(const std::string &name, void *address, CTvalue valueType) {
    addresses.emplace_back(name, valueType, address);
}


void StarredAddressesWindow::addAddress(const std::string& name, void* address, CTvalue valueType, PointerChain pointerChain) {
    addresses.emplace_back(name, valueType, address);
    addresses.back().pchain = std::move(pointerChain);
}
