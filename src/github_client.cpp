#include "github_client.hpp"

#include <httplib.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

namespace
{
    // URL-encode a query component for use in a GitHub API request.
    std::string encode_query_component(const std::string &value)
    {
        constexpr char hex_digits[] = "0123456789ABCDEF";
        std::string encoded;
        encoded.reserve(value.size() * 3);

        for (const unsigned char character : value)
        {
            const bool is_unreserved =
                (character >= 'A' && character <= 'Z') ||
                (character >= 'a' && character <= 'z') ||
                (character >= '0' && character <= '9') ||
                character == '-' || character == '_' || character == '.' || character == '~';
            if (is_unreserved)
            {
                encoded.push_back(static_cast<char>(character));
            }
            else
            {
                encoded.push_back('%');
                encoded.push_back(hex_digits[character >> 4]);
                encoded.push_back(hex_digits[character & 0x0F]);
            }
        }

        return encoded;
    }

}

GitHubClient::GitHubClient(std::string base_url)
    : base_url_(std::move(base_url))
{
}

std::string GitHubClient::fetch_repositories_json(const SearchOptions &options) const
{
    return fetch_repositories_json(options.query, options.count);
}

std::string GitHubClient::fetch_repositories_json(const std::string &query, std::size_t count) const
{
    httplib::Client client(base_url_);
    client.set_connection_timeout(10, 0);
    client.set_read_timeout(30, 0);

    const std::string path = "/search/repositories?q=" +
                             encode_query_component(query) + "&per_page=" + std::to_string(count);

    httplib::Headers headers{
        {"Accept", "application/vnd.github+json"},
        {"X-GitHub-Api-Version", "2026-03-10"},
        {"User-Agent", "cs3460-github-crawler"}};

    if (const char *token = std::getenv("GITHUB_TOKEN"))
    {
        headers.emplace("Authorization", std::string("Bearer ") + token);
    }

    const auto result = client.Get(path, headers);
    if (!result)
    {
        std::cerr << "GitHub request failed due to a network or TLS error.\n";
        return {};
    }
    if (result->status != 200)
    {
        std::cerr << "GitHub API returned HTTP " << result->status << ".\n"
                  << result->body << '\n';
        return {};
    }

    return result->body;
}