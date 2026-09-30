# Github Crawler

- GitHub Project Crawler

## Team Members

| Name         | A Number  | Email              |
| ------------ | --------- | ------------------ |
| Ethan Tatton | A02468210 | A02468210@usu.edu  |
| Ethan Huff   | A02386051 | ethan.huff@usu.edu |

**Team Lead:** Ethan Tatton

## Requirements

- Clang / Clang++ (supporting C++20)
- CMake 3.20+ (we're using `3.28.3`)
- Git
- libcpp-httplib-dev 
- nlohmann-json3-dev 
- libssl-dev
- GITHUB_TOKEN exported to environment

## Getting Started

For Ubuntu, run to install requirements:

```bash
sudo apt install clang cmake git libcpp-httplib-dev libssl-dev zlib1g-dev libbrotli-dev nlohmann-json3-dev
```

Clone the repo:

```bash
git clone "https://github.com/CS3460/github-crawler"
```

### Building

To build from a clean checkout, execute:

```bash
./build.sh
```

### Running

Run instructions will be provided at the end of the `build.sh` output, but by default will be:
```bash
./build/github_crawler
```
