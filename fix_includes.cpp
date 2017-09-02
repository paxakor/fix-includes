// Copyright 2017, Pavel Korozevtsev.

#include "category.hpp"
#include "source_file.hpp"

#include <algorithm>
#include <tuple>
#include <unordered_set>
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
            auto lastInclude = begin;
            while (iter != file.end() && (iter->IsInclude() || iter->IsEmpty()))
                if (iter->IsInclude())
                    lastInclude = iter++;
                else
                    ++iter;
            ranges.emplace_back(begin, ++lastInclude);
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
        auto currWeight = begin->Weight;
        for (; begin != end; ++begin) {
            if (begin->Weight != currWeight) {
                currWeight = begin->Weight;
                file.emplace(begin);
            }
        }
    }

    static auto RemoveRepeatedLines(SourceFile& file, SourceFile::iterator begin, SourceFile::iterator end,
                                    std::unordered_set<std::string>& inserted) {
        while (begin != end && inserted.count(*begin))
            begin = file.erase(begin);
        for (auto iter = begin; iter != end;)
            if (inserted.count(*iter))
                iter = file.erase(iter);
            else
                inserted.insert(*iter++);
        return std::tuple(begin, end);
    }

    static void FixIncludesInRange(SourceFile& file, SourceFile::iterator begin, SourceFile::iterator end,
                                   std::unordered_set<std::string>& inserted) {
        RemoveEmptyLinesInRange(file, begin, end);
        std::list<Line> to_sort;
        to_sort.splice(to_sort.end(), file, begin, end);
        to_sort.sort([](const auto& a, const auto& b) {
            if (a.Weight != b.Weight)
                return static_cast<int>(a.Weight) < static_cast<int>(b.Weight);
            return static_cast<const std::string&>(a) < static_cast<const std::string&>(b);
        });
        begin = to_sort.begin();
        file.splice(end, to_sort, to_sort.begin(), to_sort.end());
        std::tie(begin, end) = RemoveRepeatedLines(file, begin, end, inserted);
        AddEmptyLinesToRange(file, begin, end);
    }

public:
    static auto FixIncludes(SourceFile& file) {
        for (auto& line : file)
            if (line.IsInclude())
                line.Weight = GetIncludeCategory(line, file.GetLang());

        const auto ranges = FindRanges(file);
        std::unordered_set<std::string> inserted;
        for (const auto[begin, end] : ranges)
            FixIncludesInRange(file, begin, end, inserted);
    }
};

void FixIncludes(SourceFile& file) {
    Fixer::FixIncludes(file);
}
