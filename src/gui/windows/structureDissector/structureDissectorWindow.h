#ifndef STRUCTUREDISSECTOR_H
#define STRUCTUREDISSECTOR_H

#include "../generic/window.h"
#include "../../../backend/CTvalue/valueType.h"

#include <list>
#include <vector>


struct StructureField {
    ValueType type;
    std::string description;
    int stringSize;
    std::vector<std::list<StructureField>> fields;
};


class StructureDissectorWindow final : public Window {
    void* address;
    void* buf = malloc(512);
    u_int64_t structureSize = 500;
    std::vector<std::list<StructureField>> mainFields;

    void guessTypes(std::vector<std::list<StructureField>>& fields);

    void drawFields(std::vector<std::list<StructureField>>& fields, u_int64_t currentAddress);

    void fieldPopup();
public:
    void draw() override;

    StructureDissectorWindow();
};

#endif //STRUCTUREDISSECTOR_H
