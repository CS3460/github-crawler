#pragma once

#include "repository.hpp"

#include <string>
#include <vector>

// Converts GitHub API JSON responses into structured Repository objects.
class RepositoryParser
{
public:
    static std::vector<Repository> parse(const std::string &body);
};
