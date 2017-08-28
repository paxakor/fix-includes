// Copyright 2017, Pavel Korozevtsev.

#include "category.hpp"
#include "source_file.hpp"

#include <algorithm>
#include <tuple>
#include <vector>

class Fixer {
private:
    static auto FindRanges(SourceFile& file) {
        std::vector<std::tuple<SourceFile::iterator, SourceFile::iterator>> ranges;
        for (auto iter = file.begin(); iter != file.end();) {
            while (iter != file.end() && !iter->IsInclude())
                ++iter;
            if (iter == file.end())
                break;
            const auto begin = iter++;
            auto lats_include = begin;
            while (iter != file.end() && (iter->IsInclude() || iter->IsEmpty()))
                if (iter->IsInclude())
                    lats_include = iter++;
                else
                    ++iter;
            ranges.emplace_back(begin, ++lats_include);
        }
        return ranges;
    }

    static void RemoveEmptyLinesInRange(SourceFile& file, SourceFile::iterator begin, SourceFile::iterator end) {
        while (begin != end)
            if (begin->IsEmpty())
                begin = file.erase(begin);
            else
                ++begin;
    }

    static void AddEmptyLinesToRange(SourceFile& file, SourceFile::iterator begin, SourceFile::iterator end) {
        if (begin == end)
            return;
        auto currType = begin->Type;
        for (; begin != end; ++begin) {
            if (begin->Type != currType) {
                currType = begin->Type;
                file.emplace(begin);
            }
        }
    }

    static void FixIncludesInRange(SourceFile& file, SourceFile::iterator begin, SourceFile::iterator end) {
        RemoveEmptyLinesInRange(file, begin, end);
        std::list<Line> to_sort;
        to_sort.splice(to_sort.end(), file, begin, end);
        to_sort.sort([](const auto& a, const auto& b) {
            if (a.Type != b.Type)
                return static_cast<int>(a.Type) < static_cast<int>(b.Type);
            return dynamic_cast<const std::string&>(a) < dynamic_cast<const std::string&>(b);
        });
        begin = to_sort.begin();
        file.splice(end, to_sort, to_sort.begin(), to_sort.end());
        file.erase(std::unique(begin, end), end);
        AddEmptyLinesToRange(file, begin, end);
    }

public:
    static auto FixIncludes(SourceFile& file) {
        for (auto& line : file)
            if (line.IsInclude())
                line.Type = GetIncludeCategory(line);

        const auto ranges = FindRanges(file);
        for (const auto[begin, end] : ranges)
            FixIncludesInRange(file, begin, end);
    }
};

void FixIncludes(SourceFile& file) {
    Fixer::FixIncludes(file);
}
