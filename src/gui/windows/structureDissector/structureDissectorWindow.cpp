#include "structureDissectorWindow.h"

#include "../../../backend/virtualMemory/virtualMemory.h"
#include "../../widgets/widgets.h"
#include "../../../backend/regions/regions.h"

#include <vector>
#include <imgui_stdlib.h>
#include <array>
#include <cmath>
#include <format>
#include <set>


void StructureDissectorWindow::guessTypes(std::vector<std::vector<StructureField> >& fields) {
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
        } else if (currChar == 0 and strLen >= 2) {
            ++strLen;
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
        void* pv = (void*)*(u_int64_t*)((char*)buf + i);

        int floatScore = std::fpclassify(f32v) == FP_NORMAL and f32v >= -1000000 and f32v <= 1000000;
        int doubleScore = std::fpclassify(f64v) == FP_NORMAL and f64v >= -1000000 and f64v <= 1000000;
        int pointerScore = regions.isValidAddress(pv);

        if (pointerScore > 0) {
            fields[i].emplace_back(ValueType(pchain | i32), "");
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

void StructureDissectorWindow::drawFields(std::vector<std::vector<StructureField> >& fields) {
    for (int offset = 0; offset < fields.size(); ++offset) {
        if (fields[offset].empty())
            continue;

        for (auto& [type, description, stringSize, pointerFields]: fields[offset]) {
            ImGui::PushID(&type);
            ImGui::AlignTextToFramePadding();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (type & pchain) {
                const bool open = ImGui::TreeNodeEx(std::format("{}", offset).c_str());

                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-1);
                Widgets::valueTypeSelector(type);

                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-1);
                Widgets::valueInputTrueOnDeactivation(i64, (char*)buf + offset, false, stringSize, true);

                if (open) {
                    void* pointsTo = (void*)*(u_int64_t*)((char*)buf + offset);
                    if (!VirtualMemory::read(pointsTo, buf, structureSize)) {
                        ImGui::TextUnformatted("read error");
                        ImGui::TreePop();
                        ImGui::PopID();
                        continue;
                    }

                    if (pointerFields.empty()) {
                        pointerFields.resize(256);
                        guessTypes(pointerFields);
                    }

                    drawFields(pointerFields);

                    VirtualMemory::read(address, buf, structureSize);
                    ImGui::TreePop();
                }

                ImGui::PopID();
                continue;
            }

            ImGui::SetNextItemWidth(-1);
            ImGui::Text("%x", offset);

            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1);
            Widgets::valueTypeSelector(type, false);

            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1);

            Widgets::valueInputTrueOnDeactivation(type, (char*)buf + offset, false, stringSize);

            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##desc", &description);

            ImGui::PopID();
        }
    }
    // ImGui::TableNextRow();
    // ImGui::TableNextColumn();
    // if (ImGui::Button("Add fields")) {
    //     structureSize *= 2;
    //     buf = realloc(buf, structureSize + 56);
    // }
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

        drawFields(mainFields);

        ImGui::PopStyleColor();

        ImGui::EndTable();
    }


    ImGui::End();
}


StructureDissectorWindow::StructureDissectorWindow() {
    name = "Structure dissector";
}
