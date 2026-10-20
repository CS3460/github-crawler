#include "analyzer.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace
{

    // Case-insensitive comparison helper for language filtering
    bool equals_case_insensitive(std::string_view lhs, std::string_view rhs)
    {
        if (lhs.size() != rhs.size())
        {
            return false;
        }
        for (std::size_t i = 0; i < lhs.size(); ++i)
        {
            if (std::tolower(static_cast<unsigned char>(lhs[i])) !=
                std::tolower(static_cast<unsigned char>(rhs[i])))
            {
                return false;
            }
        }
        return true;
    }

}

// Sort descending by stars, tie breaking on owner ASC, name ASC
bool compare_by_stars(const Repository &a, const Repository &b)
{
    if (a.stars != b.stars)
    {
        return a.stars > b.stars;
    }
    if (a.owner != b.owner)
    {
        return a.owner < b.owner;
    }
    return a.name < b.name;
}

// Sort descending by forks, tie breaking on owner ASC, name ASC
bool compare_by_forks(const Repository &a, const Repository &b)
{
    if (a.forks != b.forks)
    {
        return a.forks > b.forks;
    }
    if (a.owner != b.owner)
    {
        return a.owner < b.owner;
    }
    return a.name < b.name;
}

// Sort descending by updated timestamp, tie breaking on owner ASC, name ASC
bool compare_by_updated(const Repository &a, const Repository &b)
{
    if (a.updated_at != b.updated_at)
    {
        return a.updated_at > b.updated_at;
    }
    if (a.owner != b.owner)
    {
        return a.owner < b.owner;
    }
    return a.name < b.name;
}

// Check whether a repository matches all active filters
bool matches_filters(const Repository &r, const AnalysisOptions &options)
{
    if (r.stars < options.min_stars)
    {
        return false;
    }
    if (!options.language.empty() && !equals_case_insensitive(r.language, options.language))
    {
        return false;
    }
    return true;
}

// Filter a collection of repositories using matches_filters
std::vector<Repository> filter_repositories(const std::vector<Repository> &input,
                                            const AnalysisOptions &options)
{
    std::vector<Repository> result;
    for (const auto &r : input)
    {
        if (matches_filters(r, options))
        {
            result.push_back(r);
        }
    }
    return result;
}

// Sort repositories using named comparison functions
void sort_repositories(std::vector<Repository> &repositories,
                       const std::string &sort_key)
{
    if (sort_key == "stars")
    {
        std::ranges::sort(repositories, compare_by_stars);
    }
    else if (sort_key == "forks")
    {
        std::ranges::sort(repositories, compare_by_forks);
    }
    else if (sort_key == "updated")
    {
        std::ranges::sort(repositories, compare_by_updated);
    }
    else
    {
        throw std::invalid_argument("Unknown sort key: " + sort_key);
    }
}

// Execute complete analysis: Filter -> Sort -> Top N
std::vector<Repository> analyze_repositories(const std::vector<Repository> &input,
                                             const AnalysisOptions &options)
{
    // Step 1: Filter
    std::vector<Repository> selected = filter_repositories(input, options);

    // Step 2: Sort
    sort_repositories(selected, options.sort_key);

    // Step 3: Top N
    if (selected.size() > options.top_n)
    {
        selected.resize(options.top_n);
    }

    return selected;
}
