// Copyright 2017, Pavel Korozevtsev, Kutenin Danila.

#include "fix_includes.hpp"
#include "source_file.hpp"
#include "util.hpp"

#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>
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

static std::variant<CaterogySpec, Lang, bool> ParseConfigLine(std::string_view line) {
    if (line == "Lang: C")
        return C;
    else if (line == "Lang: C++")
        return CPP;
    else if (line == "Lang: Auto")
        return AUTO;
    else {
        WeightType cat = 0;
        const auto[pos, success] = IntFromString(line, cat);
        if (success && pos < line.size())
            return CaterogySpec{Strip(line.substr(pos)), cat};
    }
    return false;
}

static auto ReadConfig() {
    CaterogySpecs additionalCategories;
    Lang mode = AUTO;
    const auto cfgPath = FindConfig();
    if (!cfgPath.empty()) {
        SourceFile cfg;
        cfg.ReadFromFile(cfgPath);

        size_t number = 0;
        const auto printError = [&](std::string_view string) {
            std::cerr << "Error in config (line " << number << "): " << string << '\n';
        };

        for (const auto& line : cfg) {
            ++number;
            auto res = ParseConfigLine(line);
            switch (res.index()) {
            case 0:
                additionalCategories.emplace_back(std::get<0>(std::move(res)));
                break;
            case 1:
                mode = std::get<1>(res);
                break;
            case 2:
                if (!line.empty())
                    printError(line);
            }
        }
    }
    return std::tuple(mode, additionalCategories);
}

static void FixIncludesInFile(const CaterogySpecs& cats, Lang mode, std::string_view src, std::string_view dst) {
    SourceFile sf;
    sf.SetLang(mode);
    sf.ReadFromFile(src);
    FixIncludes(sf, cats, true);
    sf.WriteToFile(dst);
}

static void FixIncludesInFile(const CaterogySpecs& cats, Lang mode, std::string_view filename) {
    return FixIncludesInFile(cats, mode, filename, filename);
}

static void PrintUsage(std::string_view bin) {
    std::cout << "Usage:\n"
              << '\t' << bin << " -i [FILE]...    Inplace edit [FILE]s\n"
              << '\t' << bin << " [SRC] [DST]     Write fixed [SRC] to [DST]\n";
}

int main(int argc, const char** argv) {
    const auto[mode, addCats] = ReadConfig();
    if (argc > 1)
        if (std::string_view(argv[1]) == "-i")
            for (int i = 2; i < argc; ++i)
                FixIncludesInFile(addCats, mode, argv[i]);
        else if (argc == 3)
            FixIncludesInFile(addCats, mode, argv[1], argv[2]);
        else
            PrintUsage(argv[0]);
    else
        PrintUsage(argv[0]);
    return 0;
}
