#ifndef PCHAINEDITORWINDOW_H
#define PCHAINEDITORWINDOW_H

#include "../../../backend/starredAddress/starredAddress.h"
#include "../generic/window.h"


class PchainEditorWindow final : public Window {
public:
    StarredAddress* address;
    void draw() override;

    PchainEditorWindow() { name = "Edit pchain"; }
};



#endif //PCHAINEDITORWINDOW_H
