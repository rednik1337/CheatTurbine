#include <array>
#include <format>
#include <iostream>
#include <algorithm>

#include "memoryEditorWindow.h"
#include "../../../backend/CTvalue/valueUtils.h"
#include "../../../backend/virtualMemory/virtualMemory.h"
#include "../../gui.h"


void MemoryEditorWindow::updateChunk() const {
    memcpy(prevChunk, chunk, currentChunkSize);
    VirtualMemory::read((void*)startAddress, chunk, currentChunkSize);
}


void MemoryEditorWindow::draw() {
    static std::vector<float> cellHighlightTimeLeft(32768);

    static ImGuiIO& io = ImGui::GetIO();
    static ImGuiStyle& imGuiStyle = ImGui::GetStyle();

    static auto originalStyle = imGuiStyle;
    imGuiStyle.FramePadding.x = 0;
    imGuiStyle.CellPadding.x = 0;
    imGuiStyle.ItemSpacing.x = 0;
    imGuiStyle.WindowMinSize = {200, 100};
    ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);


    if (ImGui::Begin(name.c_str(), &pOpen, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar)) {
        static constexpr std::array<unsigned, 6> symbolsPerValueHex{16, 8, 4, 2, 16, 8};
        static constexpr std::array<unsigned, 6> symbolsPerValueDec{20, 11, 6, 4, 32, 32};
        const unsigned symbolsPerValue = displayAsHex ? symbolsPerValueHex[ValueUtils::valueIndex(cellValueType)] : symbolsPerValueDec[ValueUtils::valueIndex(cellValueType)];


        const ImGuiDataType ImGuiCellValueType = ValueUtils::CTValueTypeToImGui(cellValueType);

        const float spacingSize = ImGui::CalcTextSize(" ").x;
        const float extraSpacingSize = spacingSize * float(2 + bool(cellValueType & (i32 | i64 | f32 | f64)));
        constexpr unsigned extraSpacingAfterXValues = 4;

        const float addressColumnSize = ImGui::CalcTextSize(std::format(" {:p}: ", (void*)(startAddress + 100)).c_str()).x + extraSpacingSize;
        const float valueSize = ImGui::CalcTextSize(" ").x * symbolsPerValue;

        const float spaceLeftForValues = ImGui::GetWindowSize().x - addressColumnSize - imGuiStyle.WindowPadding.x * 2 + spacingSize;
        const unsigned valuesPerRow = spaceLeftForValues * extraSpacingAfterXValues / ((valueSize + spacingSize + int(innerBordersEnabled)) * extraSpacingAfterXValues + extraSpacingSize);
        const unsigned rowsAvailable = ImGui::GetWindowSize().y / ImGui::CalcTextSize(" ").y;

        updateChunk();
        const unsigned sizeofValue = ValueUtils::sizeofValueType(cellValueType);

        std::string fmtStr;
        if (displayAsHex) {
            cellValueType = ValueType(cellValueType & ~isSigned);
            fmtStr = "%0" + std::to_string(symbolsPerValueHex[ValueUtils::valueIndex(cellValueType)]);
            if (cellValueType & f64)
                fmtStr += "La";
            else if (cellValueType & f32)
                fmtStr += "a";
            else if (cellValueType & i64)
                fmtStr += "llx";
            else if (cellValueType & i32)
                fmtStr += "lx";
            else if (cellValueType & i16)
                fmtStr += "hx";
            else if (cellValueType & i8)
                fmtStr += "hhx";
        } else {
            if (cellValueType & f32)
                fmtStr = "%f";
            else if (cellValueType & f64)
                fmtStr = "%Lf";
            else {
                if (cellValueType & i64)
                    fmtStr = "%ll";
                else if (cellValueType & i32)
                    fmtStr = "%l";
                else if (cellValueType & i16)
                    fmtStr = "%h";
                else if (cellValueType & i8)
                    fmtStr = "%hh";
                fmtStr += cellValueType & isSigned ? "d" : "u";
            }
        }

        if (ImGui::BeginTable("Memory editor table", valuesPerRow + 1, ImGuiTableFlags_RowBg | (innerBordersEnabled ? ImGuiTableFlags_BordersInner : 0))) {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, addressColumnSize);

            for (unsigned i = 1; i <= valuesPerRow; ++i) {
                if (i % extraSpacingAfterXValues == 0)
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, valueSize + spacingSize + extraSpacingSize);
                else
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, valueSize + spacingSize);
            }

            for (unsigned row = 0; row < rowsAvailable; ++row) {
                ImGui::TableNextColumn();
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(189, 189, 189, 255));
                ImGui::AlignTextToFramePadding();
                ImGui::Text(" %p: ", startAddress + row * valuesPerRow * sizeofValue);
                ImGui::AlignTextToFramePadding();
                ImGui::PopStyleColor();


                for (unsigned i = 0; i < valuesPerRow; ++i) {
                    ImGui::TableNextColumn();
                    ImGui::PushID(row * 4096 + i);
                    ImGui::SetNextItemWidth(-1);

                    void* currentValue = (void*)((u_int64_t)chunk + (row * valuesPerRow + i) * sizeofValue);
                    const void* previousValue = (void*)((u_int64_t)prevChunk + (row * valuesPerRow + i) * sizeofValue);
                    u_int64_t currentAddress = startAddress + row * valuesPerRow * sizeofValue + i * sizeofValue;


                    cellHighlightTimeLeft[row * valuesPerRow + i] -= 255 * (1 / io.Framerate);
                    if (!scrolled and memcmp(currentValue, previousValue, ValueUtils::sizeofValueType(cellValueType)) != 0)
                        cellHighlightTimeLeft[row * valuesPerRow + i] = 255;
                    else if (cellHighlightTimeLeft[row * valuesPerRow + i] < 0)
                        cellHighlightTimeLeft[row * valuesPerRow + i] = 0;
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(15, 135, 250, cellHighlightTimeLeft[row * valuesPerRow + i]));
                    if (ImGui::InputScalar("", ImGuiCellValueType, currentValue, nullptr, nullptr, fmtStr.c_str(), displayAsHex ? ImGuiInputTextFlags_CharsHexadecimal : 0 | ImGuiInputTextFlags_EnterReturnsTrue)) {
                        VirtualMemory::write(currentValue, (void*)currentAddress, sizeofValue);
                        Gui::log("Wrote {} to {:p}",  ValueUtils::format(cellValueType, currentValue, displayAsHex), (void*)currentAddress);
                    }
                    ImGui::PopStyleColor();

                    ImGui::PopID();
                }
            }
            ImGui::EndTable();
        }

        scrolled -= (bool)scrolled;
        if (ImGui::IsWindowHovered() and io.MouseWheel != 0) {
            const int shift = io.MouseWheel * valuesPerRow * sizeofValue * 2;
            startAddress -= shift;
            startAddress = std::max(startAddress, baseAddress);
            if (shift > 0) {
                std::rotate(cellHighlightTimeLeft.rbegin(), cellHighlightTimeLeft.rbegin() + shift, cellHighlightTimeLeft.rend());
            } else if (shift < 0) {
                std::rotate(cellHighlightTimeLeft.begin(), cellHighlightTimeLeft.begin() - shift, cellHighlightTimeLeft.end());
            }
            if (shift > 0) {
                for (int i = 0; i < shift; ++i)
                    cellHighlightTimeLeft[i] = 0;
            } else if (shift < 0) {
                for (int i = cellHighlightTimeLeft.size() - shift - 1; i < cellHighlightTimeLeft.size(); ++i)
                    cellHighlightTimeLeft[i] = 0;
            }
            scrolled = 2;
        }

        ImGui::PopStyleColor();
        imGuiStyle = originalStyle;

        menuBar();
        ImGui::End();
    } else {
        ImGui::PopStyleColor();
        imGuiStyle = originalStyle;
    }
}

void MemoryEditorWindow::menuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("View")) {
            if (ImGui::BeginMenu("Value type")) {
                const std::array valueTypeSelection{bool(cellValueType & i8), bool(cellValueType & i16), bool(cellValueType & i32), bool(cellValueType & i64), bool(cellValueType & f32), bool(cellValueType & f64)};
                if (ImGui::MenuItem("signed", nullptr, cellValueType & isSigned))
                    cellValueType = ValueType(cellValueType ^ isSigned);
                if (ImGui::MenuItem("int8", nullptr, valueTypeSelection[0]))
                    cellValueType = i8;
                if (ImGui::MenuItem("int16", nullptr, valueTypeSelection[1]))
                    cellValueType = i16;
                if (ImGui::MenuItem("int32", nullptr, valueTypeSelection[2]))
                    cellValueType = i32;
                if (ImGui::MenuItem("int64", nullptr, valueTypeSelection[3]))
                    cellValueType = i64;
                if (ImGui::MenuItem("float32", nullptr, valueTypeSelection[4]))
                    cellValueType = f32;
                if (ImGui::MenuItem("float64", nullptr, valueTypeSelection[5]))
                    cellValueType = f64;

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Display type")) {
                bool displayAsDec = !displayAsHex;
                ImGui::MenuItem("hex", nullptr, &displayAsHex);
                if (ImGui::MenuItem("dec", nullptr, &displayAsDec))
                    displayAsHex = !displayAsHex;
                ImGui::EndMenu();
            }
            ImGui::MenuItem("Inner borders", nullptr, &innerBordersEnabled);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::BeginMenu("Current address")) {
                auto newStartAddress = startAddress;
                ImGui::InputScalar("##currentAddress", ImGuiDataType_U64, &newStartAddress, 0, 0, "%p", ImGuiInputTextFlags_CharsHexadecimal);
                if (ImGui::IsItemDeactivatedAfterEdit() and newStartAddress != startAddress) {
                    startAddress = newStartAddress;
                    scrolled = 2;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}


MemoryEditorWindow::MemoryEditorWindow(const unsigned long long start) {
    name = "Memory editor";
    startAddress = start;
    baseAddress = SelectedProcess::getBaseAddress();
}


MemoryEditorWindow::~MemoryEditorWindow() {
    free(chunk);
}
