#include <charconv>
#include <cstdint>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

// A repository structure to hold the relevant information parsed from the GitHub API response.
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

// A structure to hold the search options parsed from the command line arguments.
struct SearchOptions {
    std::string query{"stars:>0"};
    std::size_t count{100};
};

std::string fetch_repository_json(const std::string& query, std::size_t count);
std::vector<Repository> parse_repositories(const std::string& body);
void print_repositories(const std::vector<Repository>& repositories);
bool save_repositories(const std::vector<Repository>& repositories, const std::string& path);

//All the cmd line stuff.
namespace {

enum class ParseResult {
    Success,
    HelpRequested,
    Error
};

// Print the usage message for the program to the console.
void print_usage(const char* program)
{
    std::cout << "Usage: " << program << " [OPTIONS] [SEARCH_QUERY]\n\n"
              << "Arguments:\n"
              << "  [SEARCH_QUERY]             GitHub search query (default: 'stars:>0')\n\n"
              << "Options:\n"
              << "  -q, --query <SEARCH_QUERY> GitHub search query\n"
              << "  -c, --count <1-100>        Number of repositories to collect (default: 100)\n"
              << "  -h, --help                 Show this help message and exit\n\n"
              << "Examples:\n"
              << "  " << program << " \"stars:>1000\"\n"
              << "  " << program << " -q \"stars:>1000\" --count 50\n"
              << "  " << program << " --query \"language:cpp stars:>500\" -c 100\n";
}

// Parse the command line arguments into a SearchOptions structure.
ParseResult parse_arguments(int argc, char* argv[], SearchOptions& options)
{
    bool query_supplied = false;
    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--help" || argument == "-h") {
            print_usage(argv[0]);
            return ParseResult::HelpRequested;
        }
        if ((argument == "--query" || argument == "-q") && index + 1 < argc) {
            if (query_supplied) {
                std::cerr << "Search query was specified multiple times.\n";
                print_usage(argv[0]);
                return ParseResult::Error;
            }
            options.query = argv[++index];
            query_supplied = true;
        } else if ((argument == "--count" || argument == "-c") && index + 1 < argc) {
            const std::string value = argv[++index];
            std::size_t count = 0;
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(), count);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                count == 0 || count > 100) {
                std::cerr << "Count must be an integer from 1 to 100.\n";
                return ParseResult::Error;
            }
            options.count = count;
        } else if (argument.starts_with("-")) {
            std::cerr << "Unknown or incomplete argument: " << argument << '\n';
            print_usage(argv[0]);
            return ParseResult::Error;
        } else {
            if (query_supplied) {
                std::cerr << "Unexpected extra positional argument: " << argument << '\n';
                print_usage(argv[0]);
                return ParseResult::Error;
            }
            options.query = argument;
            query_supplied = true;
        }
    }

    if (options.query.empty()) {
        std::cerr << "Search query cannot be empty.\n";
        return ParseResult::Error;
    }
    return ParseResult::Success;
}

}

int main(int argc, char* argv[])
{
    SearchOptions options;
    const ParseResult parse_result = parse_arguments(argc, argv, options);
    if (parse_result == ParseResult::HelpRequested) {
        return 0;
    }
    if (parse_result == ParseResult::Error) {
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
