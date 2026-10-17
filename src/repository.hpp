#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

// Represents a GitHub repository and its associated metadata.
struct Repository
{
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

// Represents command-line search options for querying the GitHub API.
struct SearchOptions
{
    std::string query{"stars:>0"};
    std::size_t count{100};
};

// Represents options for analyzing a repository dataset in Milestone 2.
struct AnalysisOptions
{
    std::string dataset_path{"repositories.json"};
    std::uint64_t min_stars{0};
    std::string language;          // empty indicates any language
    std::string sort_key{"stars"}; // "stars", "forks", or "updated"
    std::size_t top_n{10};
    std::string export_path{"selected.json"};
    std::string export_format{"json"}; // "json" or "csv"
};
