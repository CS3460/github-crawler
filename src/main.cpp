#include "github_client.hpp"
#include "repository.hpp"
#include "repository_parser.hpp"
#include "repository_store.hpp"

#include <charconv>
#include <cstdint>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

// Command-line argument parsing.
namespace
{

    enum class ProgramMode
    {
        Analyze,
        Crawl
    };

    struct CliOptions
    {
        ProgramMode mode{ProgramMode::Analyze};
        SearchOptions crawl_options;
        AnalysisOptions analysis_options;
    };

    enum class ParseResult
    {
        Success,
        HelpRequested,
        Error
    };

    // Print the usage message for the program to the console.
    void print_usage(const char *program)
    {
        std::cout << "Usage: " << program << " [OPTIONS]\n\n"
                  << "Milestone 2 Analysis Options (Default Mode):\n"
                  << "  -d, --dataset <PATH>       Path to JSON dataset (default: 'repositories.json')\n"
                  << "  -s, --min-stars <COUNT>    Filter repositories with minimum stars (default: 0)\n"
                  << "  -l, --language <LANG>      Filter repositories by programming language\n"
                  << "  -k, --sort <KEY>           Sort key: stars, forks, updated (default: 'stars')\n"
                  << "  -n, --top <N>              Number of top repositories to select (default: 10)\n"
                  << "  -o, --export <PATH>        Export selected repositories to file (default: 'selected.json')\n"
                  << "  -f, --format <json|csv>    Export format: json or csv (default: auto from extension)\n\n"
                  << "Milestone 1 Crawler Options:\n"
                  << "  --crawl                    Fetch repositories from GitHub API\n"
                  << "  -q, --query <QUERY>        GitHub search query (default: 'stars:>0')\n"
                  << "  -c, --count <1-100>        Number of repositories to fetch (default: 100)\n\n"
                  << "General Options:\n"
                  << "  -h, --help                 Show this help message and exit\n";
    }

    // Parse command line arguments into CliOptions.
    ParseResult parse_arguments(int argc, char *argv[], CliOptions &options)
    {
        for (int index = 1; index < argc; ++index)
        {
            const std::string argument = argv[index];
            if (argument == "--help" || argument == "-h")
            {
                print_usage(argv[0]);
                return ParseResult::HelpRequested;
            }

            // Crawler options
            if (argument == "--crawl")
            {
                options.mode = ProgramMode::Crawl;
            }
            else if ((argument == "--query" || argument == "-q") && index + 1 < argc)
            {
                options.mode = ProgramMode::Crawl;
                options.crawl_options.query = argv[++index];
            }
            else if ((argument == "--count" || argument == "-c") && index + 1 < argc)
            {
                options.mode = ProgramMode::Crawl;
                const std::string value = argv[++index];
                std::size_t count = 0;
                if (const auto parsed = std::from_chars(value.data(), value.data() + value.size(), count); parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                                                                                                           count == 0 || count > 100)
                {
                    std::cerr << "Count must be an integer from 1 to 100.\n";
                    return ParseResult::Error;
                }
                options.crawl_options.count = count;
            }
            // Analysis options
            else if ((argument == "--dataset" || argument == "-d") && index + 1 < argc)
            {
                options.analysis_options.dataset_path = argv[++index];
            }
            else if ((argument == "--min-stars" || argument == "-s") && index + 1 < argc)
            {
                const std::string value = argv[++index];
                std::uint64_t min_stars = 0;
                if (const auto parsed = std::from_chars(value.data(), value.data() + value.size(), min_stars); parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size())
                {
                    std::cerr << "min-stars must be a non-negative integer.\n";
                    return ParseResult::Error;
                }
                options.analysis_options.min_stars = min_stars;
            }
            else if ((argument == "--language" || argument == "-l") && index + 1 < argc)
            {
                options.analysis_options.language = argv[++index];
            }
            else if ((argument == "--sort" || argument == "-k") && index + 1 < argc)
            {
                const std::string key = argv[++index];
                if (key != "stars" && key != "forks" && key != "updated")
                {
                    std::cerr << "Sort key must be one of: 'stars', 'forks', 'updated'.\n";
                    return ParseResult::Error;
                }
                options.analysis_options.sort_key = key;
            }
            else if ((argument == "--top" || argument == "-n") && index + 1 < argc)
            {
                const std::string value = argv[++index];
                std::size_t top_n = 0;
                if (const auto parsed = std::from_chars(value.data(), value.data() + value.size(), top_n); parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size())
                {
                    std::cerr << "top must be a non-negative integer.\n";
                    return ParseResult::Error;
                }
                options.analysis_options.top_n = top_n;
            }
            else if ((argument == "--export" || argument == "-o") && index + 1 < argc)
            {
                options.analysis_options.export_path = argv[++index];
                if (options.analysis_options.export_path.ends_with(".csv"))
                {
                    options.analysis_options.export_format = "csv";
                }
                else if (options.analysis_options.export_path.ends_with(".json"))
                {
                    options.analysis_options.export_format = "json";
                }
            }
            else if ((argument == "--format" || argument == "-f") && index + 1 < argc)
            {
                const std::string format = argv[++index];
                if (format != "json" && format != "csv")
                {
                    std::cerr << "Export format must be either 'json' or 'csv'.\n";
                    return ParseResult::Error;
                }
                options.analysis_options.export_format = format;
            }
            else if (argument.starts_with("-"))
            {
                std::cerr << "Unknown or incomplete argument: " << argument << '\n';
                print_usage(argv[0]);
                return ParseResult::Error;
            }
            else
            {
                if (options.mode == ProgramMode::Crawl)
                {
                    options.crawl_options.query = argument;
                }
                else
                {
                    options.analysis_options.dataset_path = argument;
                }
            }
        }

        if (options.mode == ProgramMode::Crawl && options.crawl_options.query.empty())
        {
            std::cerr << "Search query cannot be empty.\n";
            return ParseResult::Error;
        }

        return ParseResult::Success;
    }

    int run_crawl_mode(const SearchOptions &options)
    {
        GitHubClient client;
        const std::string body = client.fetch_repositories_json(options);
        if (body.empty())
        {
            return 1;
        }

        std::vector<Repository> repositories;
        try
        {
            repositories = RepositoryParser::parse(body);
        }
        catch (const std::exception &error)
        {
            std::cerr << "Failed to parse GitHub response: " << error.what() << '\n';
            return 1;
        }

        print_repositories(repositories);
        if (!save_repositories(repositories, "repositories.json"))
        {
            std::cerr << "Could not write repositories.json.\n";
            return 1;
        }

        std::cout << "\nCollected " << repositories.size() << " repositories for query '"
                  << options.query << "'. Saved to repositories.json.\n";
        return 0;
    }

} // namespace

int main(int argc, char *argv[])
{
    CliOptions options;
    const ParseResult parse_result = parse_arguments(argc, argv, options);
    if (parse_result == ParseResult::HelpRequested)
    {
        return 0;
    }
    if (parse_result == ParseResult::Error)
    {
        return 2;
    }

    if (options.mode == ProgramMode::Crawl)
    {
        return run_crawl_mode(options.crawl_options);
    }

    std::cout << "Analysis Configuration:\n"
              << "  Dataset: " << options.analysis_options.dataset_path << '\n'
              << "  Min stars: " << options.analysis_options.min_stars << '\n'
              << "  Language: " << (options.analysis_options.language.empty() ? "any" : options.analysis_options.language) << '\n'
              << "  Sort key: " << options.analysis_options.sort_key << '\n'
              << "  Top N: " << options.analysis_options.top_n << '\n'
              << "  Export path: " << options.analysis_options.export_path << '\n'
              << "  Export format: " << options.analysis_options.export_format << '\n';

    return 0;
}
