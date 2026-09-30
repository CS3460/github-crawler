#include "repository_parser.hpp"

#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

// Parse a JSON string of repositories from the GitHub API response into a vector of Repository structures.
std::vector<Repository> RepositoryParser::parse(const std::string &body)
{
    using json = nlohmann::json;
    const json root = json::parse(body);
    if (!root.contains("items") || !root.at("items").is_array())
    {
        throw std::runtime_error("GitHub response does not contain a repository items array.");
    }

    std::vector<Repository> repositories;
    repositories.reserve(root.at("items").size());
    for (const auto &item : root.at("items"))
    {
        Repository repository;
        repository.owner = item.at("owner").at("login").get<std::string>();
        repository.name = item.at("name").get<std::string>();
        repository.description = item.at("description").is_null()
                                     ? ""
                                     : item.at("description").get<std::string>();
        repository.stars = item.at("stargazers_count").get<std::uint64_t>();
        repository.forks = item.at("forks_count").get<std::uint64_t>();
        repository.language = item.at("language").is_null()
                                  ? ""
                                  : item.at("language").get<std::string>();
        repository.size = item.at("size").get<std::uint64_t>();
        repository.updated_at = item.at("updated_at").get<std::string>();
        repository.url = item.at("html_url").get<std::string>();
        repositories.push_back(std::move(repository));
    }

    return repositories;
}