#pragma once

#include "repository.hpp"

#include <filesystem>
#include <string_view>
#include <vector>

// Exports selected repository records to JSON or CSV formats
class Exporter
{
public:
    static bool export_json(const std::vector<Repository> &repositories,
                            const std::filesystem::path &path);

    static bool export_csv(const std::vector<Repository> &repositories,
                           const std::filesystem::path &path);

    static bool export_repositories(const std::vector<Repository> &repositories,
                                    const std::filesystem::path &path,
                                    std::string_view format = "");
};
