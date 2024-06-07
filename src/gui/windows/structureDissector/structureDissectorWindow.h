#ifndef STRUCTUREDISSECTOR_H
#define STRUCTUREDISSECTOR_H

#include "../generic/window.h"
#include "../../../backend/CTvalue/valueType.h"

#include <vector>


struct StructureField {
    ValueType type;
    std::string description;
    int stringSize;
    std::vector<std::vector<StructureField>> fields;
};

class StructureDissectorWindow final : public Window {
    void* address;
    void* buf = malloc(512);
    u_int64_t structureSize = 500;
    std::vector<std::vector<StructureField>> mainFields;

    void guessTypes(std::vector<std::vector<StructureField>>& fields);

    void drawFields(std::vector<std::vector<StructureField>>& fields);
public:
    void draw() override;

    StructureDissectorWindow();
};

#endif //STRUCTUREDISSECTOR_H
