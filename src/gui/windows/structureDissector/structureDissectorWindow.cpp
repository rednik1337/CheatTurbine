#include "structureDissectorWindow.h"

#include "../../../backend/virtualMemory/virtualMemory.h"
#include "../../widgets/widgets.h"
#include "../../../backend/regions/regions.h"
#include "../../gui.h"
#include "../../windows/windows.h"

#include <vector>
#include <imgui_stdlib.h>
#include <cmath>
#include <format>
#include <queue>
#include <set>


void StructureDissectorWindow::guessTypes(std::vector<std::list<StructureField> >& fields) {
    Regions regions;
    regions.parse();

    fields.clear();
    fields.resize(structureSize);

    std::set<int> coveredArea;
    int strLen = 0;
    for (int i = 0; i < structureSize; ++i) {
        auto currChar = *((char*)buf + i);
        if (currChar <= 126 and currChar >= 32 and (strLen > 0 or i % 4 == 0)) {
            ++strLen;
        } else if ((currChar == 0 and strLen >= 2) or strLen >= 32) {
            int strLenExtended = strLen;
            if (strLen % 4)
                strLenExtended += 4 - strLen % 4;

            fields[i - strLen].emplace_back(string, "", strLenExtended);
            for (int j = i - strLen; j < i + strLenExtended - strLen; j += 4)
                coveredArea.insert(j);
            strLen = 0;
        } else {
            strLen = 0;
        }
    }

    for (int i = 0; i < structureSize; i += 4) {
        if (coveredArea.contains(i))
            continue;

        float_t f32v = *(float_t*)((char*)buf + i);
        double_t f64v = *(double_t*)((char*)buf + i);
        void* pv = (void*)*(uint64_t*)((char*)buf + i);

        int floatScore = std::fpclassify(f32v) == FP_NORMAL and f32v >= -1000000 and f32v <= 1000000;
        int doubleScore = std::fpclassify(f64v) == FP_NORMAL and f64v >= -1000000 and f64v <= 1000000;
        int pointerScore = regions.isValidAddress(pv);

        if (pointerScore > 0) {
            fields[i].emplace_back(CTvalue{i32, pchain}, "");
            i += 4;
        } else if (floatScore > 0 or doubleScore > 0) {
            if (floatScore >= doubleScore)
                fields[i].emplace_back(f32, "");
            else {
                fields[i].emplace_back(f64, "");
                i += 4;
            }
        } else {
            fields[i].emplace_back(i32, "");
        }
    }
}

void StructureDissectorWindow::drawFields(std::vector<std::list<StructureField> >& fields, uint64_t currentAddress) {
    if (fields.empty()) {
        fields.resize(structureSize);
        guessTypes(fields);
    } else if (fields.size() < structureSize) {
        fields.resize(structureSize);
        for (uint64_t i = structureSize / 2; i < structureSize; i += 4)
            fields[i].emplace_back(i32);
    }


    for (int offset = 0; offset < fields.size(); ++offset, ++currentAddress) {
        if (fields[offset].empty())
            continue;


        for (auto it = fields[offset].begin(); it != fields[offset].end(); ++it) {
            auto& [type, description, stringSize, pointerFields] = *it;

            ImGui::PushID(&type);
            ImGui::AlignTextToFramePadding();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (type.flags & pchain) {
                ImGui::SetNextItemWidth(-1);
                const bool open = ImGui::TreeNodeEx(std::format("{}", offset).c_str(), ImGuiTreeNodeFlags_SpanFullWidth);

                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-1);
                Widgets::valueTypeSelector(type);

                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-1);
                Widgets::valueInputTrueOnDeactivation(i64, (char*)buf + offset, false, stringSize, true);

                if (open) {
                    void* pointsTo = (void*)*(uint64_t*)((char*)buf + offset);
                    if (!VirtualMemory::read(pointsTo, buf, structureSize)) {
                        ImGui::TextUnformatted("read error");
                        ImGui::TreePop();
                        ImGui::PopID();
                        continue;
                    }

                    drawFields(pointerFields, currentAddress);

                    VirtualMemory::read(address, buf, structureSize);
                    ImGui::TreePop();
                }

                goto END;
            }

            ImGui::SetNextItemWidth(-1);
            ImGui::Text("%x", offset);

            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1);
            Widgets::valueTypeSelector(type, false);

            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1);

            if (Widgets::valueInputTrueOnDeactivation(type, (char*)buf + offset, false, stringSize)) {
                VirtualMemory::write((char*)buf + offset, address + offset, type.getSize());
                Gui::log("Wrote {} to {:p}", type.format((char*)buf + offset, false), address + offset);
            }

            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##desc", &description);


        END:
            ImGui::AlignTextToFramePadding();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, 0);
            ImGui::Selectable("##Selectable", false, ImGuiSelectableFlags_SpanAllColumns);
            ImGui::PopStyleColor();

            static StructureField structureField{i32};
            static int newFieldOffset = -1235;
            static int popupOpened = 0;
            if (ImGui::BeginPopupContextItem("##Popup")) {
                popupOpened = offset;
                ImGui::PopStyleColor();
                if (ImGui::BeginMenu("Add new")) {
                    if (newFieldOffset == -1235)
                        newFieldOffset = offset;
                    ImGui::InputInt(std::format("Offset = {:p}", (void*)(currentAddress + newFieldOffset)).c_str(), &newFieldOffset, 1, 10, ImGuiInputTextFlags_CharsHexadecimal);
                    Widgets::valueTypeSelector(structureField.type, false);
                    ImGui::SameLine();
                    ImGui::TextUnformatted("Type");
                    ImGui::InputText("Description", &structureField.description);
                    ImGui::NewLine();
                    if (ImGui::Button("Add"))
                        fields[newFieldOffset].emplace_back(structureField);

                    ImGui::EndMenu();
                }

                if (ImGui::MenuItem("Remove"))
                    it = std::prev(fields[offset].erase(it));

                if (ImGui::BeginMenu("Add to starred")) {
                    for (const auto starredAddressesWindow: Gui::getWindows<StarredAddressesWindow>()) {
                        if (ImGui::MenuItem(starredAddressesWindow->name.c_str()))
                            starredAddressesWindow->addAddress("New address", (void*)currentAddress, type);
                    }

                    ImGui::EndMenu();
                }

                ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
                ImGui::EndPopup();
            } else {
                if (popupOpened == offset) {
                    newFieldOffset = -1235;
                    popupOpened = 0;
                }
            }

            if (ImGui::IsItemHovered() and ImGui::IsMouseDoubleClicked(0)) {
                for (const auto starredAddressesWindow: Gui::getWindows<StarredAddressesWindow>()) {
                    starredAddressesWindow->addAddress("New address", (void*)currentAddress, type);
                    break;
                }
            }
            ImGui::PopID();
        }
    }
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    if (ImGui::Button("Add fields")) {
        structureSize *= 2;
        buf = realloc(buf, structureSize + 56);
    }
}


void StructureDissectorWindow::draw() {
    if (!ImGui::Begin(name.c_str(), &pOpen))
        return;


    if (ImGui::InputScalar("Address", ImGuiDataType_U64, &address, nullptr, nullptr, "%p", ImGuiInputTextFlags_CharsHexadecimal)) {
        mainFields.resize(0);
    }

    if (!VirtualMemory::read(address, buf, structureSize)) {
        mainFields.resize(0);
        ImGui::End();
        return;
    }
    if (mainFields.empty()) {
        mainFields.resize(structureSize);
        guessTypes(mainFields);
    }

    if (ImGui::BeginTable("##fields", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Offset");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Value");
        ImGui::TableSetupColumn("Description");
        ImGui::TableHeadersRow();

        ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

        drawFields(mainFields, (uint64_t)address);

        ImGui::PopStyleColor();

        ImGui::EndTable();
    }


    ImGui::End();
}


StructureDissectorWindow::StructureDissectorWindow() {
    name = "Structure dissector";
}
