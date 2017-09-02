// Copyright 2017, Pavel Korozevtsev, Kutenin Danila.

#include "category.hpp"
#include "source_file.hpp"
#include "util.hpp"

#include <algorithm>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <vector>

class Fixer {
private:
    using LineIterator = SourceFile::iterator;

    static auto FormatInclude(std::string& line) {
        line = "#include " + std::string(Strip(*ParseDirective(line, "include")));
    }

    static auto FindBegin(LineIterator begin, LineIterator end) {
        for (auto nestingLevel = 0; begin != end && (nestingLevel || !begin->IsInclude()); ++begin) {
            // with current implementation of ParseDirective last 2 calls are useless
            if (ParseDirective(*begin, "if") /*|| ParseDirective(*begin, "ifdef") || ParseDirective(*begin, "ifndef")*/)
                ++nestingLevel;
            if (ParseDirective(*begin, "endif"))
                --nestingLevel;
        }
        return begin;
    }

    static auto FindRanges(SourceFile& file) {
        std::vector<std::tuple<LineIterator, LineIterator>> ranges;
        for (auto iter = FindBegin(file.begin(), file.end()); iter != file.end(); iter = FindBegin(iter, file.end())) {
            const auto begin = iter++;
            auto lastInclude = begin;
            while (iter != file.end() && (iter->IsEmpty() || iter->IsInclude()))
                if (iter->IsInclude())
                    lastInclude = iter++;
                else
                    ++iter;
            ranges.emplace_back(begin, ++lastInclude);
        }
        return ranges;
    }

    static void RemoveEmptyLinesInRange(SourceFile& file, LineIterator begin, LineIterator end) {
        while (begin != end)
            if (begin->IsEmpty())
                begin = file.erase(begin);
            else
                ++begin;
    }

    static void AddEmptyLinesToRange(SourceFile& file, LineIterator begin, LineIterator end) {
        if (begin == end)
            return;
        auto currWeight = begin->Weight;
        for (; begin != end; ++begin) {
            if (begin->Weight != currWeight) {
                currWeight = begin->Weight;
                file.emplace(begin);
            }
        }
        // add empty line before function/commentary if needed
        if (end != file.end() && !end->IsEmpty())
            file.emplace(end);
    }

    static auto RemoveRepeatedLines(SourceFile& file, LineIterator begin, LineIterator end,
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

    static auto SortRange(SourceFile& file, LineIterator begin, LineIterator end) {
        std::list<Line> toSort;
        toSort.splice(toSort.end(), file, begin, end);
        toSort.sort([](const auto& a, const auto& b) {
            if (a.Weight != b.Weight)
                return static_cast<int>(a.Weight) < static_cast<int>(b.Weight);
            return static_cast<const std::string&>(a) < static_cast<const std::string&>(b);
        });
        begin = toSort.begin();
        file.splice(end, toSort, toSort.begin(), toSort.end());
        return std::tuple(begin, end);
    }

    static auto FormatIncludeInRange(LineIterator begin, LineIterator end) {
        for (; begin != end; ++begin)
            FormatInclude(*begin);
    }

    static void FixIncludesInRange(SourceFile& file, LineIterator begin, LineIterator end,
                                   std::unordered_set<std::string>& inserted) {
        RemoveEmptyLinesInRange(file, begin, end);
        FormatIncludeInRange(begin, end);
        std::tie(begin, end) = SortRange(file, begin, end);
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
