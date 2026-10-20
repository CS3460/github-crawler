#include "data_loader.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

std::vector<Repository> DataLoader::load_repositories(const std::filesystem::path &path)
{
    std::ifstream input(path);
    if (!input.is_open())
    {
        throw std::runtime_error("Cannot open repository dataset: " + path.string());
    }

    nlohmann::json root;
    try
    {
        input >> root;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        throw std::runtime_error("Failed to parse JSON dataset: " + std::string(e.what()));
    }

    if (!root.is_array())
    {
        throw std::runtime_error("Repository dataset must be a JSON array");
    }

    std::vector<Repository> repositories;
    repositories.reserve(root.size());

    for (const auto &item : root)
    {
        if (!item.is_object())
        {
            continue;
        }

        Repository r;
        r.owner = item.value("owner", "");
        r.name = item.value("name", "");
        r.description = item.value("description", "");
        r.stars = item.value("stars", std::uint64_t{0});
        r.forks = item.value("forks", std::uint64_t{0});
        r.language = item.value("language", "");
        r.size = item.value("size", std::uint64_t{0});
        r.updated_at = item.value("updated_at", "");
        r.url = item.value("url", "");

        repositories.push_back(std::move(r));
    }

    return repositories;
}
