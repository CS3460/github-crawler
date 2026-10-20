#pragma once

#include "repository.hpp"

#include <string>
#include <vector>

// Named comparison functions for sorting
bool compare_by_stars(const Repository &a, const Repository &b);
bool compare_by_forks(const Repository &a, const Repository &b);
bool compare_by_updated(const Repository &a, const Repository &b);

// Filter helpers
bool matches_filters(const Repository &r, const AnalysisOptions &options);
std::vector<Repository> filter_repositories(const std::vector<Repository> &input,
                                            const AnalysisOptions &options);

// Sorter helper
void sort_repositories(std::vector<Repository> &repositories,
                       const std::string &sort_key);

// Complete analysis pipeline: Filter -> Sort -> Top N
std::vector<Repository> analyze_repositories(const std::vector<Repository> &input,
                                             const AnalysisOptions &options);
