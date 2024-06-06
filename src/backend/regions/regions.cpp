#include "regions.h"

#include "../selectedProcess/selectedProcess.h"
#include "../../gui/gui.h"

#include <fstream>
#include <iostream>
#include <sstream>


void Regions::parse() {
    regions.clear();
    Gui::log("Parsing regions..");
    std::ifstream maps("/proc/" + std::to_string(SelectedProcess::pid) + "/maps");

    std::string line;
    unsigned regionsParsed = 0;

    while (std::getline(maps, line)) {
        std::istringstream iss(line);
        std::string data;
        Region region;

        std::getline(iss, data, '-');

        region.start = (void*)std::stoul(data, nullptr, 16);

        std::getline(iss, data, ' ');
        region.end = (void*)std::stoul(data, nullptr, 16);

        std::getline(iss, data, ' ');
        region.mode = RegionPerms((data[0] != '-' ? r : none) | (data[1] != '-' ? w : none) | (data[2] != '-' ? x : none) | (data[0] == 'p' ? p : none));

        std::getline(iss, data, ' ');
        region.offset = std::stoul(data, nullptr, 16);

        std::getline(iss, data, ' ');
        region.device = data;

        std::getline(iss, data, ' ');
        region.inodeID = atoi(data.c_str());

        std::getline(iss, data, '\n');
        region.path = data;
        region.path.erase(region.path.find_last_not_of(' ') + 1);
        region.path.erase(0, region.path.find_first_not_of(' '));

        largestRegionSize = std::max(largestRegionSize, (unsigned long long)((char*)region.end - (char*)region.start));

        regionsParsed++;
        if ((region.mode & mustNotHavePerms) != 0)
            continue;
        if ((region.mode & mustHavePerms) != mustHavePerms)
            continue;
        regions.emplace_back(region);
    }

    Gui::log("Parsed {} regions, {} match", regionsParsed, regions.size());
}

// TODO: do something about gcc not inlining every regions func
std::optional<Region> Regions::get(const void* address) {
    int left = 0, right = regions.size() - 1;
    while (left <= right) {
        const int mid = left + (right - left) / 2;
        if (regions[mid].start <= address and address <= regions[mid].end)
            return regions[mid];
        if (regions[mid].start > address)
            right = mid - 1;
        else
            left = mid + 1;
    }
    return std::nullopt;
}

std::optional<Region> Regions::get(const std::string& path, u_int64_t offset) {
    for (const auto& i: regions)
        if (i.path == path and i.offset == offset)
            return i;
    return std::nullopt;
}


bool Regions::isValidAddress(const void* address) {
    return Regions::get(address).has_value();
}


bool Regions::isStaticAddress(const void* address) {
    const auto regionOptional = Regions::get(address);
    if (!regionOptional.has_value())
        return false;
    const auto& region = regionOptional.value();
    if (region.inodeID)
        if (region.path.rfind("/dev/") != 0)
            return true;
    return false;
}
