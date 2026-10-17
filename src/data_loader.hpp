#pragma once

#include "repository.hpp"

#include <filesystem>
#include <vector>

// Loads repository datasets from JSON files.
class DataLoader
{
public:
    // Loads and deserializes repository records from a JSON file.
    // Throws std::runtime_error if the file cannot be opened or is invalid JSON.
    static std::vector<Repository> load_repositories(const std::filesystem::path &path);
};
