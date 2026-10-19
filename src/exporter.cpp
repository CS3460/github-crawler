#include "exporter.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace
{
    // Escapes CSV values
    std::string escape_csv_field(std::string_view field)
    {
        if (const bool needs_quotes = field.find_first_of(",\"\r\n") != std::string_view::npos; !needs_quotes)
        {
            return std::string(field);
        }

        std::string escaped = "\"";
        for (const char c : field)
        {
            if (c == '"')
            {
                escaped += "\"\"";
            }
            else
            {
                escaped += c;
            }
        }
        escaped += "\"";
        return escaped;
    }

}

bool Exporter::export_json(const std::vector<Repository> &repositories,
                           const std::filesystem::path &path)
{
    nlohmann::json output = nlohmann::json::array();
    for (const auto &r : repositories)
    {
        output.push_back({{"owner", r.owner},
                          {"name", r.name},
                          {"description", r.description},
                          {"stars", r.stars},
                          {"forks", r.forks},
                          {"language", r.language},
                          {"size", r.size},
                          {"updated_at", r.updated_at},
                          {"url", r.url}});
    }

    std::ofstream file(path);
    if (!file.is_open())
    {
        return false;
    }
    file << output.dump(2) << '\n';
    return file.good();
}

bool Exporter::export_csv(const std::vector<Repository> &repositories,
                          const std::filesystem::path &path)
{
    std::ofstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    file << "owner,name,description,stars,forks,language,size,updated_at,url\n";
    for (const auto &r : repositories)
    {
        file << escape_csv_field(r.owner) << ','
             << escape_csv_field(r.name) << ','
             << escape_csv_field(r.description) << ','
             << r.stars << ','
             << r.forks << ','
             << escape_csv_field(r.language) << ','
             << r.size << ','
             << escape_csv_field(r.updated_at) << ','
             << escape_csv_field(r.url) << '\n';
    }
    return file.good();
}

bool Exporter::export_repositories(const std::vector<Repository> &repositories,
                                   const std::filesystem::path &path,
                                   std::string_view format)
{
    if (format == "csv" || (format.empty() && path.extension() == ".csv"))
    {
        return export_csv(repositories, path);
    }
    return export_json(repositories, path);
}
