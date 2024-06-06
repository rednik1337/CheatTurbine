#include "pointerMapManagerWindow.h"

#include "../../gui.h"
#include "../../windows/windows.h"

#include <format>
#include <imgui_stdlib.h>
#include <thread>


void PointerMapManagerWindow::draw() {
    if (!ImGui::Begin(name.c_str(), &pOpen))
        return;


    static std::string newPtrmapName = "New ptrmap";
    ImGui::InputText("Name", &newPtrmapName);

    const std::list<StarredAddressesWindow*> starredAddressesWindows = Gui::getWindows<StarredAddressesWindow>();

    static std::string previewValue;
    static StarredAddressesWindow* selectedWindow = nullptr;
    if (selectedWindow != nullptr and !selectedWindow->pOpen)
        selectedWindow = nullptr;
    if (!selectedWindow) {
        previewValue = "";
        if (!starredAddressesWindows.empty())
            selectedWindow = starredAddressesWindows.front();
    } else {
        previewValue = selectedWindow->name;
    }
    if (ImGui::BeginCombo("Match", previewValue.c_str(), 0)) {
        for (const auto& window: starredAddressesWindows) {
            const bool isSelected = selectedWindow == window;
            if (ImGui::Selectable(window->name.c_str(), isSelected))
                selectedWindow = window;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (!selectedWindow)
        ImGui::BeginDisabled();

    if (ImGui::Button("Generate")) {
        const std::string ptrmapName = newPtrmapName;
        addPmap(selectedWindow->addresses, ptrmapName);
    }

    if (!selectedWindow)
        ImGui::EndDisabled();

    auto y = ImGui::GetCursorPosY();
    if (ImGui::TreeNode("Settings")) {
        if (ImGui::InputInt("Threads", &threadsPerPmap)) {
            for (auto& i: ptrMaps)
                i.maxThreads = threadsPerPmap;
        }
        ImGui::TreePop();
    }

    auto yNext = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(y);
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Pointer scan").x - ImGui::GetStyle().FramePadding.x * 2 - ImGui::GetStyle().WindowPadding.x);
    if (ImGui::SmallButton("Pointer scan")) {
        auto* window = new PointerScanWindow();

        bool primaryPmapChosen = false;
        for (auto& i: ptrMaps) {
            if (i.fromCurrentProcess) {
                if (!i.savedAddresses.empty()) {
                    window->selectedPmaps[0] = &i;
                    window->selectedAddresses[0] = {i.savedAddresses[0].name, i.savedAddresses[0].address};
                    window->selectedValueType = i.savedAddresses[0].valueType;
                    primaryPmapChosen = true;
                }
            }
        }

        if (primaryPmapChosen) {
            for (auto& i: ptrMaps) {
                if (!i.fromCurrentProcess) {
                    if (!i.savedAddresses.empty()) {
                        window->selectedPmaps.emplace_back(&i);
                        window->selectedAddresses.emplace_back(i.savedAddresses[0].name, i.savedAddresses[0].address);
                    }
                }
            }
        }

        Gui::addWindow(window);
    }
    ImGui::SetCursorPosY(yNext);


    if (ImGui::BeginTable("Pointermaps", 4, ImGuiTableFlags_RowBg)) {
        static const float actionsColumnSize = ImGui::CalcTextSize("UpdateDelete").x + ImGui::GetStyle().FramePadding.x * 4 + ImGui::GetStyle().CellPadding.x * 3;

        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Status");
        ImGui::TableSetupColumn("Size");
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, actionsColumnSize);
        ImGui::TableHeadersRow();

        auto pmap = ptrMaps.begin();
        while (pmap != ptrMaps.end()) {
            ImGui::PushID(*(int*)&pmap);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##name", &pmap->name);

            ImGui::TableNextColumn();
            if (pmap->generationInProgress) {
                char buf[32];
                sprintf(buf, "%llu/%llu", pmap->addressesScanned, pmap->addressesToScan);
                const float progress = (float)pmap->addressesScanned / (float)pmap->addressesToScan;
                ImGui::ProgressBar(progress, ImVec2(-1, 0), buf);
            } else {
                ImGui::TextUnformatted("Generated");
            }


            ImGui::TableNextColumn();
            ImGui::Text("%d", pmap->map.size());

            bool wasGreyedOut = false;
            if (!pmap->fromCurrentProcess or pmap->generationInProgress) {
                wasGreyedOut = true;
                ImGui::BeginDisabled();
            }
            ImGui::TableNextColumn();
            if (ImGui::SmallButton("Update"))
                std::thread(&PointerMap::update, pmap).detach();
            ImGui::SameLine();

            if (wasGreyedOut)
                ImGui::EndDisabled();

            if (pmap->generationInProgress) {
                bool wasDisabled = false;
                if (pmap->shouldStop) {
                    ImGui::BeginDisabled();
                    wasDisabled = true;
                }
                if (ImGui::SmallButton(" Stop "))
                    pmap->shouldStop = true;
                ++pmap;
                if (wasDisabled)
                    ImGui::EndDisabled();
            } else {
                if (ImGui::SmallButton("Delete"))
                    pmap = ptrMaps.erase(pmap);
                else
                    ++pmap;
            }

            ImGui::PopID();
        }


        ImGui::EndTable();
    }


    ImGui::End();
}

void PointerMapManagerWindow::addPmap(const std::vector<StarredAddress>& addrs, const std::string& name) {
    ptrMaps.emplace_back(addrs, name, threadsPerPmap, fastScanOffset);
    std::thread(&PointerMap::generate, &ptrMaps.back()).detach();
}

PointerMapManagerWindow::PointerMapManagerWindow() {
    name = "Pointermap manager";

    SPsubID = SelectedProcess::subscribeToDetach([this] {
        for (auto& i: ptrMaps)
            i.fromCurrentProcess = false;
    });
}

PointerMapManagerWindow::~PointerMapManagerWindow() {
    SelectedProcess::unsubscribeFromDetach(SPsubID);
}
