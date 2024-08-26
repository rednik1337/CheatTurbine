#include "pointerScanWindow.h"

#include "../../gui.h"
#include "../../../backend/pointerMap/pointerMap.h"
#include "../pointerMapManager/pointerMapManagerWindow.h"

#include <format>

#include <thread>

#include "../../../backend/selectedProcess/selectedProcess.h"
#include "../../../backend/virtualMemory/virtualMemory.h"
#include "../starredAddresses/starredAddressesWindow.h"


void PointerScanWindow::scanResults() {
    if (ImGui::BeginTable("Results", pointerScan.maxDepth, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupScrollFreeze(0, 1);
        for (int i = 0; i < pointerScan.maxDepth; ++i)
            ImGui::TableSetupColumn(std::format("{}", i + 1).c_str());
        ImGui::TableHeadersRow();

        ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
        ImGuiListClipper clipper;

        clipper.Begin(pointerScan.pChains.size());
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                ImGui::TableNextRow();
                ImGui::PushID(row);
                ImGui::TableNextColumn();
                auto& pchain = pointerScan.pChains[row];

                ImGui::Text("%s (%x) + %x", pchain.path.c_str(), pchain.regionOffset, pchain.offsets[0]);
                for (int i = 1; i < pchain.offsets.size() - 1; ++i) {
                    ImGui::TableNextColumn();
                    ImGui::Text("%x", pchain.offsets[i]);
                }
                ImGui::TableNextColumn();
                void* addr = pchain.getTail();
                uint64_t value;
                VirtualMemory::read(addr, &value, 4);
                ImGui::Text("%x = %p (%s)", pchain.offsets.back(), addr, selectedValueType.format(&value, false).c_str());

                ImGui::AlignTextToFramePadding();
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, 0);
                ImGui::Selectable("##Selectable", false, ImGuiSelectableFlags_SpanAllColumns);
                ImGui::PopStyleColor();
                if (ImGui::BeginPopupContextItem("##Popup")) {
                    if (ImGui::BeginMenu("Add to starred")) {
                        for (const auto starredAddressesWindow: Gui::getWindows<StarredAddressesWindow>()) {
                            if (ImGui::MenuItem(starredAddressesWindow->name.c_str()))
                                starredAddressesWindow->addAddress("New pchain", nullptr, {selectedValueType.type, selectedValueType.flags | CTValueFlags::pchain}, pchain); // TODO: add window highlighting
                        }

                        ImGui::EndMenu();
                    }
                    ImGui::EndPopup();
                }

                if (ImGui::IsItemHovered() and ImGui::IsMouseDoubleClicked(0)) {
                    for (const auto starredAddressesWindow: Gui::getWindows<StarredAddressesWindow>()) {
                        starredAddressesWindow->addAddress("New pchain", nullptr, {selectedValueType.type, selectedValueType.flags | CTValueFlags::pchain}, pchain); // TODO: add window highlighting
                        break;
                    }
                }

                ImGui::PopID();
            }
        }
        ImGui::PopStyleColor();
        ImGui::EndTable();
    }
}

void PointerScanWindow::pmapsSelection() {
    // ImGui::TextUnformatted("Pointermaps");

    std::list<PointerMap*> availablePrimaryPmaps;
    std::list<PointerMap*> availableSecondaryPmaps;

    if (selectedAddresses.back().second) {
        selectedPmaps.push_back(nullptr);
        selectedAddresses.emplace_back();
    }

    for (const auto pmapManagerWindow: Gui::getWindows<PointerMapManagerWindow>())
        for (auto& pmap: pmapManagerWindow->ptrMaps)
            availableSecondaryPmaps.push_back(&pmap);


    for (auto i: availableSecondaryPmaps)
        if (i->fromCurrentProcess)
            availablePrimaryPmaps.push_back(i);


    if (ImGui::BeginTable("Ptrmaps", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Pointermap");
        ImGui::TableSetupColumn("Address");
        // ImGui::TableHeadersRow();

        for (int i = 0; i < selectedPmaps.size(); ++i) {
            ImGui::PushID(i);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            std::string previewStr = selectedPmaps[i] ? selectedPmaps[i]->name : "";
            if (previewStr.empty())
                previewStr = i ? "Secondary ptrmap" : "Primary ptrmap";

            ImGui::SetNextItemWidth(-1);
            if (ImGui::BeginCombo("##ptrmapSelector", previewStr.c_str())) {
                for (const auto& pmap: i ? availableSecondaryPmaps : availablePrimaryPmaps) {
                    ImGui::PushID(pmap);
                    const bool isSelected = pmap == selectedPmaps[i];
                    if (ImGui::Selectable(pmap->name.c_str(), isSelected))
                        selectedPmaps[i] = pmap;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }
            ImGui::TableNextColumn();

            availableSecondaryPmaps.remove(selectedPmaps[i]);

            previewStr = selectedAddresses[i].second ? selectedAddresses[i].first : "Address";
            if (!selectedPmaps[i])
                ImGui::BeginDisabled();

            ImGui::SetNextItemWidth(-1);
            if (ImGui::BeginCombo("##addressSelector", previewStr.c_str())) {
                for (const auto& addr: selectedPmaps[i]->savedAddresses) {
                    ImGui::PushID(&addr);
                    const bool isSelected = selectedAddresses[i] == std::make_pair(addr.name, addr.address);
                    if (ImGui::Selectable(addr.name.c_str(), isSelected)) {
                        selectedAddresses[i] = std::make_pair(addr.name, addr.address);
                        selectedValueType = addr.valueType;
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }
            if (!selectedPmaps[i])
                ImGui::EndDisabled();

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}


void PointerScanWindow::draw() {
    if (!ImGui::Begin(name.c_str(), &pOpen))
        return;

    bool wasDisabled = false;
    if (pointerScan.scanInProgress) {
        wasDisabled = true;
        ImGui::BeginDisabled();
    }
    pmapsSelection();

    if (ImGui::TreeNode("Settings")) {
        ImGui::InputScalar("Max positivie offset", ImGuiDataType_U32, &pointerScan.maxOffsetPositive);
        ImGui::InputScalar("Max negative offset", ImGuiDataType_U32, &pointerScan.maxOffsetNegative);
        ImGui::InputScalar("Max depth", ImGuiDataType_U32, &pointerScan.maxDepth);

        ImGui::TreePop();
    }
    if (wasDisabled)
        ImGui::EndDisabled();

    if (pointerScan.scanInProgress) {
        if (ImGui::Button("Stop"))
            pointerScan.shouldStop = true;
    } else {
        if (!selectedAddresses[0].second)
            ImGui::BeginDisabled();
        if (ImGui::Button("Scan")) {
            std::vector<uint64_t> targets;
            for (const auto& [name, addr]: selectedAddresses)
                targets.push_back((uint64_t)addr);
            targets.pop_back();
            selectedPmaps.pop_back();
            std::thread(&PointerScan::newScan, &pointerScan, targets, selectedPmaps).detach();
            selectedPmaps.push_back(nullptr);
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Add all").x - ImGui::GetStyle().FramePadding.x * 2 - ImGui::GetStyle().WindowPadding.x);
        if (ImGui::Button("Add all")) {
            const auto window = Gui::getWindows<StarredAddressesWindow>().front();
            for (const auto& i: pointerScan.pChains)
                window->addAddress("New pchain", nullptr, {selectedValueType.type, selectedValueType.flags | CTValueFlags::pchain}, i);
        }
        if (!selectedAddresses[0].second)
            ImGui::EndDisabled();
    }


    if (pointerScan.scanInProgress)
        ImGui::Text("Scanning | Current depth %d | %d pointer chains found", pointerScan.currentDepth, pointerScan.pChains.size());
    else
        scanResults();


    ImGui::End();
}
