#include <charconv>
#include <cstdint>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

//
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

struct SearchOptions {
    std::string query{"stars:>0"};
    std::size_t count{100};
};

std::string fetch_repository_json(const std::string& query, std::size_t count);
std::vector<Repository> parse_repositories(const std::string& body);
void print_repositories(const std::vector<Repository>& repositories);
bool save_repositories(const std::vector<Repository>& repositories, const std::string& path);

namespace {

void print_usage(const char* program)
{
    std::cerr << "Usage: " << program << " [--query SEARCH_QUERY] [--count 1-100]\n"
              << "Example: " << program << " --query 'stars:>1000' --count 100\n";
}

bool parse_arguments(int argc, char* argv[], SearchOptions& options)
{
    bool query_supplied = false;
    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--help" || argument == "-h") {
            print_usage(argv[0]);
            return false;
        }
        if (argument == "--query" && index + 1 < argc) {
            options.query = argv[++index];
            query_supplied = true;
        } else if (argument == "--count" && index + 1 < argc) {
            const std::string value = argv[++index];
            std::size_t count = 0;
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(), count);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                count == 0 || count > 100) {
                std::cerr << "Count must be an integer from 1 to 100.\n";
                return false;
            }
            options.count = count;
        } else {
            std::cerr << "Unknown or incomplete argument: " << argument << '\n';
            print_usage(argv[0]);
            return false;
        }
    }

    if (options.query.empty()) {
        std::cerr << "Search query cannot be empty.\n";
        return false;
    }
    if (query_supplied && options.query.front() == '-') {
        std::cerr << "Search query must not be empty.\n";
        return false;
    }
    return true;
}

}

int main(int argc, char* argv[])
{
    SearchOptions options;
    if (!parse_arguments(argc, argv, options)) {
        return 2;
    }

    const std::string body = fetch_repository_json(options.query, options.count);
    if (body.empty()) {
        return 1;
    }

    std::vector<Repository> repositories;
    try {
        repositories = parse_repositories(body);
    } catch (const std::exception& error) {
        std::cerr << "Failed to parse GitHub response: " << error.what() << '\n';
        return 1;
    }

    print_repositories(repositories);
    if (!save_repositories(repositories, "repositories.json")) {
        std::cerr << "Could not write repositories.json.\n";
        return 1;
    }

    std::cout << "\nCollected " << repositories.size() << " repositories for query '"
              << options.query << "'. Saved to repositories.json.\n";
    return 0;
}
