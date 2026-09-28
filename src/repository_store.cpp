#include <nlohmann/json.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


// This stuipid strucuture is duplicated in multiple files, we could refactor it into a header file but the project doens't list it so -\(_-_-)_/-
struct Repository {
    std::string owner;
    std::string name;
    std::string description;
    std::uint64_t stars{};
    std::uint64_t forks{};
    std::string language;
    std::uint64_t size{};
    std::string updated_at;
    std::string url;
};


// Print a vector of Repository structures to the console in a nice format.
void print_repositories(const std::vector<Repository>& repositories)
{
    for (std::size_t index = 0; index < repositories.size(); ++index) {
        const auto& repository = repositories[index];
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


// Save a vector of Repository structures to a JSON file at the specified path. So we don't have to parse again.
bool save_repositories(const std::vector<Repository>& repositories, const std::string& path)
{
    nlohmann::json output = nlohmann::json::array();
    for (const auto& repository : repositories) {
        output.push_back({
            {"owner", repository.owner},
            {"name", repository.name},
            {"description", repository.description},
            {"stars", repository.stars},
            {"forks", repository.forks},
            {"language", repository.language},
            {"size", repository.size},
            {"updated_at", repository.updated_at},
            {"url", repository.url}
        });
    }

    std::ofstream file(path);
    if (!file) {
        return false;
    }
    file << output.dump(2) << '\n';
    return file.good();
}
