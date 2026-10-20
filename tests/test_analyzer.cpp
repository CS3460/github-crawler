#include "analyzer.hpp"

#include <cstdlib>
#include <format>
#include <iostream>
#include <string>
#include <vector>

#define TEST_ASSERT(cond, msg)                                                                                             \
    do                                                                                                                     \
    {                                                                                                                      \
        if (!(cond))                                                                                                       \
        {                                                                                                                  \
            std::cerr << "Assertion failed: " << (msg) << " (" << #cond << ") at " << __FILE__ << ":" << __LINE__ << '\n'; \
            std::exit(1);                                                                                                  \
        }                                                                                                                  \
    } while (false)

Repository make_repo(std::string owner, std::string name, std::uint64_t stars,
                     std::uint64_t forks, std::string language, std::string updated_at)
{
    Repository r;
    r.owner = std::move(owner);
    r.name = std::move(name);
    r.stars = stars;
    r.forks = forks;
    r.language = std::move(language);
    r.updated_at = std::move(updated_at);
    return r;
}

void test_filter_min_stars()
{
    std::vector repos = {
        make_repo("a", "r1", 50, 10, "C++", "2026-01-01T00:00:00Z"),
        make_repo("b", "r2", 150, 20, "C++", "2026-01-02T00:00:00Z"),
        make_repo("c", "r3", 300, 30, "Python", "2026-01-03T00:00:00Z"),
    };

    AnalysisOptions options;
    options.min_stars = 100;

    auto filtered = filter_repositories(repos, options);
    TEST_ASSERT(filtered.size() == 2, "Expected 2 repositories with stars >= 100");
    TEST_ASSERT(filtered[0].stars == 150, "First match should have 150 stars");
    TEST_ASSERT(filtered[1].stars == 300, "Second match should have 300 stars");
    std::cout << "PASS: test_filter_min_stars\n";
}

void test_filter_language()
{
    std::vector repos = {
        make_repo("a", "r1", 100, 10, "C++", "2026-01-01T00:00:00Z"),
        make_repo("b", "r2", 200, 20, "Python", "2026-01-02T00:00:00Z"),
        make_repo("c", "r3", 300, 30, "Rust", "2026-01-03T00:00:00Z"),
    };

    // Case-insensitive match: "c++" matches "C++"
    AnalysisOptions options1;
    options1.language = "c++";
    auto filtered1 = filter_repositories(repos, options1);
    TEST_ASSERT(filtered1.size() == 1, "Expected 1 C++ repository");
    TEST_ASSERT(filtered1[0].name == "r1", "Expected r1");

    // "Python" match
    AnalysisOptions options2;
    options2.language = "PYTHON";
    auto filtered2 = filter_repositories(repos, options2);
    TEST_ASSERT(filtered2.size() == 1, "Expected 1 Python repository");
    TEST_ASSERT(filtered2[0].name == "r2", "Expected r2");

    // Empty language matches all
    AnalysisOptions options3;
    options3.language = "";
    auto filtered3 = filter_repositories(repos, options3);
    TEST_ASSERT(filtered3.size() == 3, "Empty language should match all repositories");

    std::cout << "PASS: test_filter_language\n";
}

void test_sort_stars_with_tie_breaker()
{
    std::vector repos = {
        make_repo("zeta", "b", 100, 10, "C++", "2026-01-01T00:00:00Z"),
        make_repo("alpha", "b", 100, 10, "C++", "2026-01-01T00:00:00Z"),
        make_repo("alpha", "a", 100, 10, "C++", "2026-01-01T00:00:00Z"),
        make_repo("beta", "top", 500, 10, "C++", "2026-01-01T00:00:00Z"),
    };

    sort_repositories(repos, "stars");

    // Highest stars first
    TEST_ASSERT(repos[0].name == "top" && repos[0].stars == 500, "Highest stars must be first");

    // Among equal stars (100): owner ASC -> "alpha", then name ASC -> "a", then "b"
    TEST_ASSERT(repos[1].owner == "alpha" && repos[1].name == "a", "Tie break 1 failed");
    TEST_ASSERT(repos[2].owner == "alpha" && repos[2].name == "b", "Tie break 2 failed");
    TEST_ASSERT(repos[3].owner == "zeta" && repos[3].name == "b", "Tie break 3 failed");

    std::cout << "PASS: test_sort_stars_with_tie_breaker\n";
}

void test_sort_forks_with_tie_breaker()
{
    std::vector repos = {
        make_repo("b", "r1", 10, 50, "C++", "2026-01-01T00:00:00Z"),
        make_repo("a", "r1", 10, 50, "C++", "2026-01-01T00:00:00Z"),
        make_repo("c", "top", 10, 200, "C++", "2026-01-01T00:00:00Z"),
    };

    sort_repositories(repos, "forks");

    TEST_ASSERT(repos[0].name == "top" && repos[0].forks == 200, "Highest forks must be first");
    TEST_ASSERT(repos[1].owner == "a", "Tie-breaker by owner ASC failed");
    TEST_ASSERT(repos[2].owner == "b", "Tie-breaker by owner ASC failed");

    std::cout << "PASS: test_sort_forks_with_tie_breaker\n";
}

void test_sort_updated_with_tie_breaker()
{
    std::vector repos = {
        make_repo("a", "older", 10, 10, "C++", "2026-01-01T00:00:00Z"),
        make_repo("a", "newer", 10, 10, "C++", "2026-09-30T12:00:00Z"),
        make_repo("b", "same_time", 10, 10, "C++", "2026-09-30T12:00:00Z"),
    };

    sort_repositories(repos, "updated");

    // Later timestamp first, tie break on owner ASC
    TEST_ASSERT(repos[0].name == "newer" && repos[0].owner == "a", "Newer timestamp first");
    TEST_ASSERT(repos[1].name == "same_time" && repos[1].owner == "b", "Tie break on owner ASC");
    TEST_ASSERT(repos[2].name == "older", "Older timestamp last");

    std::cout << "PASS: test_sort_updated_with_tie_breaker\n";
}

void test_top_n_truncation()
{
    std::vector<Repository> repos;
    for (std::size_t i = 1; i <= 20; ++i)
    {
        repos.push_back(make_repo("owner", std::format("repo{}", i), i * 10, i, "C++", "2026-01-01T00:00:00Z"));
    }

    AnalysisOptions options;
    options.top_n = 5;

    auto result = analyze_repositories(repos, options);
    TEST_ASSERT(result.size() == 5, "Expected truncation to exactly 5 items");
    TEST_ASSERT(result[0].stars == 200, "First item should be top stars (200)");
    std::cout << "PASS: test_top_n_truncation\n";
}

void test_top_n_fewer_than_n()
{
    std::vector repos = {
        make_repo("a", "r1", 10, 10, "C++", "2026-01-01T00:00:00Z"),
        make_repo("b", "r2", 20, 20, "C++", "2026-01-02T00:00:00Z"),
    };

    AnalysisOptions options;
    options.top_n = 10; // More than available

    auto result = analyze_repositories(repos, options);
    TEST_ASSERT(result.size() == 2, "Expected all 2 items returned when fewer than top_n exist");
    std::cout << "PASS: test_top_n_fewer_than_n\n";
}

void test_invalid_sort_key()
{
    std::vector<Repository> repos;
    bool threw = false;
    try
    {
        sort_repositories(repos, "invalid_key");
    }
    catch (const std::invalid_argument &)
    {
        threw = true;
    }
    TEST_ASSERT(threw, "sort_repositories should throw on invalid key");
    std::cout << "PASS: test_invalid_sort_key\n";
}

int main()
{
    std::cout << "=== Running RepositoryAnalyzer Tests ===\n";
    test_filter_min_stars();
    test_filter_language();
    test_sort_stars_with_tie_breaker();
    test_sort_forks_with_tie_breaker();
    test_sort_updated_with_tie_breaker();
    test_top_n_truncation();
    test_top_n_fewer_than_n();
    test_invalid_sort_key();
    std::cout << "All RepositoryAnalyzer tests passed successfully!\n";
    return 0;
}
