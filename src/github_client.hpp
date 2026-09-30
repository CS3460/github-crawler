#pragma once

#include "repository.hpp"

#include <cstddef>
#include <string>

// Client for querying the GitHub REST API.
class GitHubClient
{
public:
    explicit GitHubClient(std::string base_url = "https://api.github.com");

    std::string fetch_repositories_json(const SearchOptions &options) const;
    std::string fetch_repositories_json(const std::string &query, std::size_t count) const;

private:
    std::string base_url_;
};
