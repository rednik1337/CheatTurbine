#ifndef CHEAT_TURBINE_MEMORYEDITORWINDOW_H
#define CHEAT_TURBINE_MEMORYEDITORWINDOW_H

#include "../generic/window.h"
#include "../../../backend/CTvalue/CTvalue.h"
#include "../../../backend/selectedProcess/selectedProcess.h"


class MemoryEditorWindow final : public Window {
    uint64_t startAddress;
    uint64_t baseAddress;
    CTvalue cellValueType = i8;
    bool asciiViewEnabled = true;
    bool displayAsHex = true;
    bool innerBordersEnabled = false;

    void* chunk = malloc(32768);
    void* prevChunk = malloc(32768);

    unsigned currentChunkSize = 32760;
    int scrolled = 2;

    void menuBar();

    void updateChunk() const;

public:
    void draw() override;

    explicit MemoryEditorWindow(unsigned long long start = SelectedProcess::getBaseAddress());

    ~MemoryEditorWindow() override;
};


#endif //CHEAT_TURBINE_MEMORYEDITORWINDOW_H
