#include "data_loader.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

#define TEST_ASSERT(cond, msg) \
    do \
    { \
        if (!(cond)) \
        { \
            std::cerr << "Assertion failed: " << (msg) << " (" << #cond << ") at " << __FILE__ << ":" << __LINE__ << '\n'; \
            std::exit(1); \
        } \
    } while (false)

void test_load_nonexistent_file()
{
    bool threw = false;
    try
    {
        DataLoader::load_repositories("nonexistent_file_12345.json");
    }
    catch (const std::runtime_error &)
    {
        threw = true;
    }
    TEST_ASSERT(threw, "DataLoader should throw when file does not exist");
    std::cout << "PASS: test_load_nonexistent_file\n";
}

void test_load_invalid_json()
{
    const std::filesystem::path temp_file = "test_invalid.json";
    {
        std::ofstream out(temp_file);
        out << "{ not valid json ";
    }

    bool threw = false;
    try
    {
        DataLoader::load_repositories(temp_file);
    }
    catch (const std::runtime_error &)
    {
        threw = true;
    }
    std::filesystem::remove(temp_file);
    TEST_ASSERT(threw, "DataLoader should throw when JSON is malformed");
    std::cout << "PASS: test_load_invalid_json\n";
}

void test_load_non_array_json()
{
    const std::filesystem::path temp_file = "test_non_array.json";
    {
        std::ofstream out(temp_file);
        out << "{\"error\": \"not an array\"}";
    }

    bool threw = false;
    try
    {
        DataLoader::load_repositories(temp_file);
    }
    catch (const std::runtime_error &)
    {
        threw = true;
    }
    std::filesystem::remove(temp_file);
    TEST_ASSERT(threw, "DataLoader should throw when root JSON is not an array");
    std::cout << "PASS: test_load_non_array_json\n";
}

void test_load_valid_sample()
{
    const std::filesystem::path temp_file = "test_sample.json";
    {
        std::ofstream out(temp_file);
        out << R"([
            {
                "owner": "test-owner",
                "name": "test-repo",
                "description": "A sample description",
                "stars": 42,
                "forks": 10,
                "language": "C++",
                "size": 1024,
                "updated_at": "2026-09-30T10:00:00Z",
                "url": "https://github.com/test-owner/test-repo"
            }
        ])";
    }

    const auto repos = DataLoader::load_repositories(temp_file);
    std::filesystem::remove(temp_file);

    TEST_ASSERT(repos.size() == 1, "Expected exactly 1 repository loaded");
    TEST_ASSERT(repos[0].owner == "test-owner", "Owner must match");
    TEST_ASSERT(repos[0].name == "test-repo", "Name must match");
    TEST_ASSERT(repos[0].stars == 42, "Stars must match");
    TEST_ASSERT(repos[0].forks == 10, "Forks must match");
    TEST_ASSERT(repos[0].language == "C++", "Language must match");
    TEST_ASSERT(repos[0].size == 1024, "Size must match");
    TEST_ASSERT(repos[0].updated_at == "2026-09-30T10:00:00Z", "Updated at must match");
    TEST_ASSERT(repos[0].url == "https://github.com/test-owner/test-repo", "URL must match");
    std::cout << "PASS: test_load_valid_sample\n";
}

void test_load_m1_dataset()
{
    if (!std::filesystem::exists("repositories.json"))
    {
        std::cout << "SKIP: repositories.json not found in working directory\n";
        return;
    }

    const auto repos = DataLoader::load_repositories("repositories.json");
    TEST_ASSERT(repos.size() == 100, "Expected 100 repositories in repositories.json");
    TEST_ASSERT(!repos[0].owner.empty(), "Owner must not be empty");
    TEST_ASSERT(!repos[0].name.empty(), "Name must not be empty");
    TEST_ASSERT(repos[0].stars > 0, "Stars should be greater than 0");
    std::cout << "PASS: test_load_m1_dataset (loaded " << repos.size() << " repositories)\n";
}

int main()
{
    std::cout << "=== Running DataLoader Tests ===\n";
    test_load_nonexistent_file();
    test_load_invalid_json();
    test_load_non_array_json();
    test_load_valid_sample();
    test_load_m1_dataset();
    std::cout << "All DataLoader tests passed successfully!\n";
    return 0;
}
