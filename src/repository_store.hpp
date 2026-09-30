#pragma once

#include "repository.hpp"

#include <string>
#include <vector>

// Print a collection of repositories to sout.
void print_repositories(const std::vector<Repository> &repositories);

// Save a collection of repositories to a JSON file at the specified path.
bool save_repositories(const std::vector<Repository> &repositories, const std::string &path);
