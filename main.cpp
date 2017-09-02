// Copyright 2017, Pavel Korozevtsev, Kutenin Danila.

#include "fix_includes.hpp"
#include "source_file.hpp"
#include "util.hpp"

// TODO (@paxakor): clean up here when someone implement <charconv>
#if __has_include(<charconv>)
#include <charconv>
#else
#include <cstdlib>
#endif

#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <tuple>
#include <vector>
namespace fs = std::experimental::filesystem;

static std::string FindConfig() {
    static constexpr auto cfgName = ".fix-includes";
    auto path = fs::current_path();
    while (!path.empty())
        if (fs::exists(path / cfgName))
            return path / cfgName;
        else
            path = path.parent_path();
    return "";
}

static auto ReadConfig() {
    std::vector<std::tuple<std::string, IncludeCategory>> additionalCategories;
    Lang mode = AUTO;
    const auto cfgPath = FindConfig();
    if (!cfgPath.empty()) {
        SourceFile cfg;
        cfg.ReadFromFile(cfgPath);
        size_t number = 0;
        for (const auto& string : cfg) {
            ++number;
            std::string_view line = string;
            if (line.size() > 6 && line.substr(0, 6) == "Lang: ") {
                line.remove_prefix(6);
                if (line == "C")
                    mode = C;
                else if (line == "C++")
                    mode = CPP;
                else if (line == "AUTO")
                    mode = AUTO;
                else
                    std::cerr << "Parse error in config (line " << number << "): " << string << '\n';
            } else {
                const auto delimPos = line.find(' ');
                if (delimPos == std::string_view::npos) {
                    std::cerr << "Parse error in config (line " << number << "): " << string << '\n';
                    continue;
                }
                const auto delim = line.data() + delimPos;
                IncludeCategory cat;
// TODO (@paxakor): clean up here when someone implement <charconv>
#if __has_include(<charconv>)
                const auto[std::ignore, ec] = std::from_chars(line.data(), delim, cat);
                if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
                    std::cerr << "Parse error in config (line " << number << "): " << string << '\n';
                    continue;
                }
#else
                cat = static_cast<IncludeCategory>(std::atoi(line.data()));
#endif
                additionalCategories.emplace_back(Strip({delim, line.size() - delimPos}), cat);
            }
        }
    }
    return std::make_tuple(mode, additionalCategories);
}

static void FixIncludesInFile(Lang mode, std::string_view src, std::string_view dst) {
    SourceFile sf;
    sf.SetLang(mode);
    sf.ReadFromFile(src);
    FixIncludes(sf);
    sf.WriteToFile(dst);
}

static void FixIncludesInFile(Lang mode, std::string_view filename) {
    return FixIncludesInFile(mode, filename, filename);
}

static void PrintUsage(std::string_view bin) {
    std::cout << "Usage:\n"
              << '\t' << bin << "[options]... -i [FILE]...    Inplace edit [FILE]s\n"
              << '\t' << bin << "[options]... [SRC] [DST]     Write fixed [SRC] to [DST]\n"
              << "Options:\n"
              << "\t-I[path] - search headers in [path]\n";
}

int main(int argc, const char** argv) {
    const auto[mode, addCats] = ReadConfig();
    (void) addCats;
    if (argc > 1)
        if (std::string_view(argv[1]) == "-i")
            for (int i = 2; i < argc; ++i)
                FixIncludesInFile(mode, argv[i]);
        else if (argc == 3)
            FixIncludesInFile(mode, argv[1], argv[2]);
        else
            PrintUsage(argv[0]);
    else
        PrintUsage(argv[0]);
    return 0;
}
