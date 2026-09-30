#include "repository_store.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Print a vector of Repository structures to the console in a nice format.
void print_repositories(const std::vector<Repository> &repositories)
{
    for (std::size_t index = 0; index < repositories.size(); ++index)
    {
        const auto &repository = repositories[index];
        std::cout << "\nRepository " << index + 1 << '/' << repositories.size() << '\n'
                  << "  Owner/name: " << repository.owner << '/' << repository.name << '\n'
                  << "  Description: " << repository.description << '\n'
                  << "  Stars: " << repository.stars << '\n'
                  << "  Forks: " << repository.forks << '\n'
                  << "  Language: " << repository.language << '\n'
                  << "  Size: " << repository.size << " KB\n"
                  << "  Updated: " << repository.updated_at << '\n'
                  << "  URL: " << repository.url << '\n';
    }
}

// Save a collection of repositories to a JSON file at the specified path.
bool save_repositories(const std::vector<Repository> &repositories, const std::string &path)
{
    nlohmann::json output = nlohmann::json::array();
    for (const auto &repository : repositories)
    {
        output.push_back({{"owner", repository.owner},
                          {"name", repository.name},
                          {"description", repository.description},
                          {"stars", repository.stars},
                          {"forks", repository.forks},
                          {"language", repository.language},
                          {"size", repository.size},
                          {"updated_at", repository.updated_at},
                          {"url", repository.url}});
    }

    std::ofstream file(path);
    if (!file)
    {
        return false;
    }
    file << output.dump(2) << '\n';
    return file.good();
}
