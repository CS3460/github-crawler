#pragma once

#include "repository.hpp"

#include <cstddef>
#include <vector>

// Formats and displays repository analysis reports to the console
class Reporter
{
public:
    static void print_report(const std::vector<Repository> &selected,
                             std::size_t total_loaded,
                             const AnalysisOptions &options);
};
