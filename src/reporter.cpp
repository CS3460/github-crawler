#include "reporter.hpp"

#include <format>
#include <iostream>
#include <string>
#include <vector>

void Reporter::print_report(const std::vector<Repository> &selected,
                            std::size_t total_loaded,
                            const AnalysisOptions &options)
{
    std::cout << std::format("Dataset: {}\n", options.dataset_path)
              << std::format("Repositories loaded: {}\n", total_loaded)
              << std::format("Filter: stars >= {}\n", options.min_stars)
              << std::format("Language: {}\n", options.language.empty() ? "any" : options.language)
              << std::format("Sort: {} (descending)\n", options.sort_key)
              << std::format("Top N: {}\n\n", options.top_n);

    std::cout << std::format("{:<36}{:>8}   {:>8}   {:<12}{}\n",
                             "OWNER/NAME", "STARS", "FORKS", "LANGUAGE", "UPDATED");

    for (const auto &repo : selected)
    {
        std::string full_name = repo.owner + "/" + repo.name;
        if (full_name.size() > 34)
        {
            full_name = full_name.substr(0, 31) + "...";
        }

        const std::string language = repo.language.empty() ? "-" : repo.language;

        std::cout << std::format("{:<36}{:>8}   {:>8}   {:<12}{}\n",
                                 full_name, repo.stars, repo.forks, language, repo.updated_at);
    }

    std::cout << std::format("\nSelected repositories: {}\n", selected.size())
              << std::format("Exported to: {}\n", options.export_path);
}
