
#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace lms::scanner
{
    struct PlayList
    {
        std::string name;
        std::vector<std::filesystem::path> files;
    };
} // namespace lms::scanner
