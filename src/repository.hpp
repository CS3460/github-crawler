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
