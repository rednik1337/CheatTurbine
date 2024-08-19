#include "scannerWindow.h"
#include "../../widgets/widgets.h"
#include "../../../backend/virtualMemory/virtualMemory.h"
#include "../../gui.h"
#include "../memoryEditor/memoryEditorWindow.h"
#include "../starredAddresses/starredAddressesWindow.h"

#include <imgui.h>
#include <format>
#include <iostream>

void ScannerWindow::scanControls() {
    const bool isScanRunning = scanner.isScanRunning;
    const bool isScannerReset = scanner.isReset;

    if (isScanRunning)
        ImGui::BeginDisabled();

    ImGui::BeginGroup();
    const bool shouldDisable = scanner.scanType == unchanged or scanner.scanType == changed or scanner.scanType == unknown or scanner.scanType == increased or scanner.scanType == decreased;

    if (shouldDisable)
        ImGui::BeginDisabled(true);
    if (scanner.scanType == range) {
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 4 - ImGui::GetStyle().FramePadding.x);
        Widgets::valueInputTrueOnEditing(scanner.valueType, scanner.valueBytes.data());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 4 - ImGui::GetStyle().FramePadding.x);
        Widgets::valueInputTrueOnEditing(scanner.valueType, scanner.valueBytesSecond.data());
    } else {
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
        Widgets::valueInputTrueOnEditing(scanner.valueType, scanner.valueBytes.data(), 256);
    }
    if (shouldDisable)
        ImGui::EndDisabled();

    if (scanner.isReset) {
        if (ImGui::Button("New"))
            scanner.newScan();
    } else {
        if (ImGui::Button("Reset"))
            scanner.reset();

        ImGui::SameLine();
        if (ImGui::Button("Next"))
            scanner.nextScan();

        if (isScanRunning)
            ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::Checkbox("auto", &scanner.isAutonextEnabled);
        if (scanner.isAutonextEnabled and !scanner.isScanRunning)
            scanner.nextScan();

        if (isScanRunning)
            ImGui::BeginDisabled(true);
    }

    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();

    if (!isScannerReset)
        ImGui::BeginDisabled();

    ImGui::SetNextItemWidth(-1);
    Widgets::valueTypeSelector(scanner.valueType, false);
    scanner.valueType.stringLength = 256;
    scanner.valueBytes.resize(scanner.valueType.getSize());
    scanner.valueBytesSecond.resize(scanner.valueType.getSize());

    if (!isScannerReset)
        ImGui::EndDisabled();

    ImGui::SetNextItemWidth(-1);
    if (isScannerReset) {
        if (scanner.valueType.type == string) {
            constexpr std::array indexMapping{equal, unknown};
            const char* items[]{"Equal", "Unknown"};
            int currentItem;
            for (int i = 0; i < 2; ++i)
                if (indexMapping[i] == scanner.scanType)
                    currentItem = i;

            if ((unsigned)currentItem > 2) {
                currentItem = 0;
                scanner.scanType = equal;
            } else if (indexMapping[currentItem] != scanner.scanType) {
                scanner.scanType = equal;
            }
            if (ImGui::Combo("##My Combo", &currentItem, items, IM_ARRAYSIZE(items)))
                scanner.scanType = indexMapping[currentItem];
        } else {
            constexpr std::array indexMapping{equal, bigger, smaller, range, unknown};
            const char* items[]{"Equal", "Bigger than", "Smaller than", "Range", "Unknown"};
            int currentItem;
            for (int i = 0; i < 5; ++i)
                if (indexMapping[i] == scanner.scanType)
                    currentItem = i;
            if (ImGui::Combo("##My Combo", &currentItem, items, IM_ARRAYSIZE(items)))
                scanner.scanType = indexMapping[currentItem];
        }
    } else {
        if (scanner.scanType >= 10)
            scanner.scanType = equal;
        if (scanner.valueType.type == string)
            ImGui::Combo("##scanner_scan_type", (int*)&scanner.scanType, "Equal\0Changed\0Unchanged\0\0");
        else
            ImGui::Combo("##scanner_scan_type", (int*)&scanner.scanType, "Equal\0Bigger than\0Smaller than\0Range\0Increased\0Increased by\0Decreased\0Decreased by\0Changed\0Unchanged\0\0");
    }

    ImGui::EndGroup();

    static int regionPermR = 2, regionPermW = 2, regionPermX = 0, regionPermP = 1;
    if (ImGui::TreeNode("Settings")) {
        Widgets::tristateCheckbox("r", &regionPermR);
        ImGui::SameLine();
        Widgets::tristateCheckbox("w", &regionPermW);
        ImGui::SameLine();
        Widgets::tristateCheckbox("x", &regionPermX);
        ImGui::SameLine();
        Widgets::tristateCheckbox("p", &regionPermP);
        ImGui::SameLine();
        ImGui::TextUnformatted("Region permissions");

        ImGui::SetNextItemWidth(40);
        if (!isScannerReset)
            ImGui::BeginDisabled(true);
        ImGui::InputScalar("Fast scan offset", ImGuiDataType_U32, &scanner.fastScanOffset);
        if (!isScannerReset)
            ImGui::EndDisabled();
        ImGui::Checkbox("Suspend while scanning", &scanner.shouldSuspendWhileScanning);
        ImGui::TreePop();
    }
    scanner.regions.mustHavePerms = regionPermR == 2 ? RegionPerms(scanner.regions.mustHavePerms | r) : RegionPerms(scanner.regions.mustHavePerms & ~r);
    scanner.regions.mustHavePerms = regionPermW == 2 ? RegionPerms(scanner.regions.mustHavePerms | w) : RegionPerms(scanner.regions.mustHavePerms & ~w);
    scanner.regions.mustHavePerms = regionPermX == 2 ? RegionPerms(scanner.regions.mustHavePerms | x) : RegionPerms(scanner.regions.mustHavePerms & ~x);
    scanner.regions.mustHavePerms = regionPermP == 2 ? RegionPerms(scanner.regions.mustHavePerms | p) : RegionPerms(scanner.regions.mustHavePerms & ~p);

    scanner.regions.mustNotHavePerms = regionPermR == 0 ? RegionPerms(scanner.regions.mustNotHavePerms | r) : RegionPerms(scanner.regions.mustNotHavePerms & ~r);
    scanner.regions.mustNotHavePerms = regionPermW == 0 ? RegionPerms(scanner.regions.mustNotHavePerms | w) : RegionPerms(scanner.regions.mustNotHavePerms & ~w);
    scanner.regions.mustNotHavePerms = regionPermX == 0 ? RegionPerms(scanner.regions.mustNotHavePerms | x) : RegionPerms(scanner.regions.mustNotHavePerms & ~x);
    scanner.regions.mustNotHavePerms = regionPermP == 0 ? RegionPerms(scanner.regions.mustNotHavePerms | p) : RegionPerms(scanner.regions.mustNotHavePerms & ~p);


    if (isScanRunning)
        ImGui::EndDisabled();

    ImGui::NewLine();

    if (isScannerReset)
        ImGui::BeginDisabled(true);
    char buf[32];
    sprintf(buf, "%llu/%llu", scanner.scannedAddresses, scanner.totalAddresses);
    const float progress = scanner.scannedAddresses == 0 ? 0.0 : (float)scanner.scannedAddresses / scanner.totalAddresses;
    ImGui::ProgressBar(progress, ImVec2(-(ImGui::CalcTextSize("Add all").x + ImGui::GetStyle().FramePadding.x * 2 + ImGui::GetStyle().ItemSpacing.x), 0.0f), buf);

    ImGui::SameLine();
    if (ImGui::Button("Add all")) {
        const auto starredAddressesWindow = Gui::getWindows<StarredAddressesWindow>().front();
        for (const auto row: scanner.addresses)
            starredAddressesWindow->addAddress("New address", row, scanner.valueType);
    }
    if (isScannerReset)
        ImGui::EndDisabled();
}

void ScannerWindow::scanResults() {
    if (scanner.isScanRunning)
        return;
    if (ImGui::BeginTable("CurrentAddresses", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("Value");
        ImGui::TableSetupColumn("Previous");
        ImGui::TableHeadersRow();

        static std::vector<char> currentRowAddressValueBytes;
        currentRowAddressValueBytes.resize(scanner.valueBytes.size());

        ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
        ImGuiListClipper clipper;
        clipper.Begin(scanner.totalAddresses);
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                ImGui::TableNextRow();
                ImGui::PushID(row);
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                if (scanner.regions.isStaticAddress(scanner.addresses[row])) {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%p", scanner.addresses[row]);
                } else {
                    ImGui::Text("%p", scanner.addresses[row]);
                }


                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-1);


                VirtualMemory::read(scanner.addresses[row], currentRowAddressValueBytes.data(), currentRowAddressValueBytes.size());

                if (Widgets::valueInputTrueOnDeactivation(scanner.valueType, currentRowAddressValueBytes.data())) {
                    VirtualMemory::write(currentRowAddressValueBytes.data(), scanner.addresses[row], currentRowAddressValueBytes.size());
                    Gui::log("Wrote {} to {:p}", scanner.valueType.format(currentRowAddressValueBytes.data(), false), scanner.addresses[row]);
                }


                ImGui::TableNextColumn();
                Widgets::valueText(scanner.valueType, (char*)scanner.latestValues + row * scanner.valueBytes.size());


                ImGui::AlignTextToFramePadding();
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, 0);
                ImGui::Selectable("##Selectable", false, ImGuiSelectableFlags_SpanAllColumns);
                ImGui::PopStyleColor();
                if (ImGui::BeginPopupContextItem("##Popup")) {
                    if (ImGui::BeginMenu("Add to starred")) {
                        for (const auto starredAddressesWindow: Gui::getWindows<StarredAddressesWindow>()) {
                            if (ImGui::MenuItem(starredAddressesWindow->name.c_str()))
                                starredAddressesWindow->addAddress("New address", scanner.addresses[row], scanner.valueType); // TODO: add window highlighting
                        }

                        ImGui::EndMenu();
                    }
                    ImGui::EndPopup();
                }

                if (ImGui::IsItemHovered() and ImGui::IsMouseDoubleClicked(0)) {
                    for (const auto starredAddressesWindow: Gui::getWindows<StarredAddressesWindow>()) {
                        starredAddressesWindow->addAddress("New address", scanner.addresses[row], scanner.valueType);
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


void ScannerWindow::draw() {
    if (!ImGui::Begin(name.c_str(), &pOpen))
        return;

    scanControls();
    scanResults();

    ImGui::End();
}
