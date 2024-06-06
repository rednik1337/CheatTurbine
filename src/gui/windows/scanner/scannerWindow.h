#ifndef SCANNER_H
#define SCANNER_H

#include "../generic/window.h"
#include "../../../backend/scanner/scanner.h"


class ScannerWindow final : public Window {
    Scanner scanner{name.c_str()};

    void scanControls();

    void scanResults();

public:
    void draw() override;

    ScannerWindow() { name = "Scanner"; }
};


#endif //SCANNER_H
