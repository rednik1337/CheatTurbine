#ifndef POINTERMAPMANAGERWINDOW_H
#define POINTERMAPMANAGERWINDOW_H

#include "../../../backend/pointerMap/pointerMap.h"
#include "../generic/window.h"

#include <list>



class PointerMapManagerWindow final : public Window {
    int fastScanOffset = 4;
    int threadsPerPmap = 8;
    unsigned SPsubID;
public:
    std::list<PointerMap> ptrMaps;

    void draw() override;
    void addPmap(const std::vector<StarredAddress>& addrs, const std::string& name);

    PointerMapManagerWindow();
    ~PointerMapManagerWindow();
};


#endif //POINTERMAPMANAGERWINDOW_H
