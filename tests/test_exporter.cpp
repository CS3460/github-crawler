#include "exporter.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
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

Repository make_test_repo(std::string owner, std::string name, std::string desc,
                          std::uint64_t stars, std::uint64_t forks, std::string lang,
                          std::uint64_t size, std::string updated, std::string url)
{
    Repository r;
    r.owner = std::move(owner);
    r.name = std::move(name);
    r.description = std::move(desc);
    r.stars = stars;
    r.forks = forks;
    r.language = std::move(lang);
    r.size = size;
    r.updated_at = std::move(updated);
    r.url = std::move(url);
    return r;
}

void test_export_json_validity()
{
    const std::filesystem::path temp_file = "test_export.json";
    std::vector repos = {
        make_test_repo("owner1", "repo1", "First repo", 100, 10, "C++", 500, "2026-01-01T00:00:00Z", "https://github.com/owner1/repo1"),
        make_test_repo("owner2", "repo2", "Second repo", 200, 20, "Python", 800, "2026-02-01T00:00:00Z", "https://github.com/owner2/repo2"),
    };

    const bool ok = Exporter::export_json(repos, temp_file);
    TEST_ASSERT(ok, "export_json should return true");

    std::ifstream in(temp_file);
    nlohmann::json root;
    in >> root;
    in.close();
    std::filesystem::remove(temp_file);

    TEST_ASSERT(root.is_array(), "Exported JSON must be an array");
    TEST_ASSERT(root.size() == 2, "Exported JSON should have 2 elements");
    TEST_ASSERT(root[0]["owner"] == "owner1", "First owner mismatch");
    TEST_ASSERT(root[0]["stars"] == 100, "First stars mismatch");
    TEST_ASSERT(root[1]["owner"] == "owner2", "Second owner mismatch");
    TEST_ASSERT(root[1]["stars"] == 200, "Second stars mismatch");
    std::cout << "PASS: test_export_json_validity\n";
}

void test_export_csv_validity()
{
    const std::filesystem::path temp_file = "test_export.csv";
    std::vector repos = {
        make_test_repo("owner1", "repo1", "A \"cool\", simple project", 50, 5, "C++", 100, "2026-01-01T00:00:00Z", "https://github.com/owner1/repo1"),
    };

    const bool ok = Exporter::export_csv(repos, temp_file);
    TEST_ASSERT(ok, "export_csv should return true");

    std::ifstream in(temp_file);
    std::string header;
    std::getline(in, header);
    TEST_ASSERT(header == "owner,name,description,stars,forks,language,size,updated_at,url", "CSV header mismatch");

    std::string row;
    std::getline(in, row);
    in.close();
    std::filesystem::remove(temp_file);

    // Verify escaped double quotes inside description: "A ""cool"", simple project"
    TEST_ASSERT(row.find("\"A \"\"cool\"\", simple project\"") != std::string::npos, "CSV escaping of quotes and commas failed");
    std::cout << "PASS: test_export_csv_validity\n";
}

void test_export_empty_collection()
{
    const std::vector<Repository> empty;
    const std::filesystem::path temp_json = "test_empty.json";
    const std::filesystem::path temp_csv = "test_empty.csv";

    TEST_ASSERT(Exporter::export_json(empty, temp_json), "Empty export_json failed");
    std::ifstream in_json(temp_json);
    nlohmann::json root;
    in_json >> root;
    in_json.close();
    std::filesystem::remove(temp_json);
    TEST_ASSERT(root.is_array() && root.empty(), "Empty JSON array expected");

    TEST_ASSERT(Exporter::export_csv(empty, temp_csv), "Empty export_csv failed");
    std::ifstream in_csv(temp_csv);
    std::string line;
    std::getline(in_csv, line);
    TEST_ASSERT(line == "owner,name,description,stars,forks,language,size,updated_at,url", "CSV header expected");
    TEST_ASSERT(!std::getline(in_csv, line), "No data rows expected for empty collection");
    in_csv.close();
    std::filesystem::remove(temp_csv);

    std::cout << "PASS: test_export_empty_collection\n";
}

void test_export_dispatcher_extension_detection()
{
    std::vector repos = {
        make_test_repo("owner1", "repo1", "desc", 10, 1, "C++", 10, "2026-01-01T00:00:00Z", "https://url"),
    };

    const std::filesystem::path temp_csv = "test_dispatch.csv";
    const std::filesystem::path temp_json = "test_dispatch.json";

    TEST_ASSERT(Exporter::export_repositories(repos, temp_csv), "Dispatch to CSV failed");
    std::ifstream in_csv(temp_csv);
    std::string header;
    std::getline(in_csv, header);
    in_csv.close();
    std::filesystem::remove(temp_csv);
    TEST_ASSERT(header.starts_with("owner,name"), "Expected CSV format when extension is .csv");

    TEST_ASSERT(Exporter::export_repositories(repos, temp_json), "Dispatch to JSON failed");
    std::ifstream in_json(temp_json);
    nlohmann::json root;
    in_json >> root;
    in_json.close();
    std::filesystem::remove(temp_json);
    TEST_ASSERT(root.is_array(), "Expected JSON format when extension is .json");

    std::cout << "PASS: test_export_dispatcher_extension_detection\n";
}

int main()
{
    std::cout << "=== Running Exporter Tests ===\n";
    test_export_json_validity();
    test_export_csv_validity();
    test_export_empty_collection();
    test_export_dispatcher_extension_detection();
    std::cout << "All Exporter tests passed successfully!\n";
    return 0;
}
