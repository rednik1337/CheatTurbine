#ifndef STRUCTUREDISSECTOR_H
#define STRUCTUREDISSECTOR_H

#include "../generic/window.h"
#include "../../../backend/CTvalue/CTvalue.h"

#include <list>
#include <vector>


struct StructureField {
    CTvalue type;
    std::string description;
    int stringSize;
    std::vector<std::list<StructureField>> fields;
};


class StructureDissectorWindow final : public Window {
    void* address;
    void* buf = malloc(512);
    uint64_t structureSize = 500;
    std::vector<std::list<StructureField>> mainFields;

    void guessTypes(std::vector<std::list<StructureField>>& fields);

    void drawFields(std::vector<std::list<StructureField>>& fields, uint64_t currentAddress);

    void fieldPopup();
public:
    void draw() override;

    StructureDissectorWindow(void* address=nullptr);
};

#endif //STRUCTUREDISSECTOR_H
