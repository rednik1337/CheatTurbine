#ifndef POINTERSCANWINDOW_H
#define POINTERSCANWINDOW_H

#include "../generic/window.h"
#include "../../../backend/pointerScan/pointerScan.h"


class PointerScanWindow final : public Window {
    PointerScan pointerScan;
public:
    ValueType selectedValueType;
    std::vector<PointerMap*> selectedPmaps{nullptr};
    std::vector<std::pair<std::string, void*> > selectedAddresses{{}};

    void scanResults();
    void pmapsSelection();

    void draw() override;
    PointerScanWindow() { name = "Pointer scan"; }
};


#endif //POINTERSCANWINDOW_H
