// Copyright 2017, Pavel Korozevtsev, Kutenin Danila.

#include "fix_includes.hpp"
#include "category.hpp"
#include "source_file.hpp"
#include "util.hpp"

#include <algorithm>
#include <stack>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

class Fixer {
private:
    using LineIterator = SourceFile::iterator;

    static auto FormatInclude(std::string& line) {
        line = "#include " + std::string(Strip(*ParseDirective(line, "include")));
    }

    static auto FindIncludes(SourceFile& file) {
        std::vector<std::vector<LineIterator>> sequences;
        std::stack<std::vector<LineIterator>> nestedRanges;

        const auto popNested = [&]() {
            sequences.emplace_back(std::move(nestedRanges.top()));
            nestedRanges.pop();
        };

        nestedRanges.emplace();
        for (auto iter = file.begin(); iter != file.end(); ++iter) {
            // with current implementation of ParseDirective last 2 calls are useless
            if (ParseDirective(*iter, "if") || ParseDirective(*iter, "ifdef") || ParseDirective(*iter, "ifndef"))
                nestedRanges.emplace();
            else if (ParseDirective(*iter, "elif") || ParseDirective(*iter, "else")) {
                popNested();
                nestedRanges.emplace();
            } else if (ParseDirective(*iter, "endif"))
                popNested();
            else if (iter->IsInclude())
                nestedRanges.top().push_back(iter);
        }

        while (!nestedRanges.empty())
            popNested();
        return sequences;
    }

    static auto MakeRanges(SourceFile& file) {
        const auto sequences = FindIncludes(file);
        std::vector<std::tuple<LineIterator, LineIterator>> ranges;
        for (auto& seq : sequences) {
            const auto lstIncPos = [&]() {
                auto i = static_cast<int>(seq.size()) - 1;
                for (; i >= 0; --i)
                    if (!seq[i]->IsEmpty())
                        break;
                return i;
            }();
            const auto fstIncPos = [&]() {
                auto i = 0;
                for (; i < static_cast<int>(seq.size()); ++i)
                    if (!seq[i]->IsEmpty())
                        break;
                return i;
            }();
            if (fstIncPos <= lstIncPos) {
                auto & [ first, last ] = ranges.emplace_back(seq[fstIncPos], seq[fstIncPos]);
                for (auto i = fstIncPos + 1; i <= lstIncPos; ++i) {
                    file.splice(first, file, seq[i]);
                    first = seq[i];
                }
                ++last;
            }
        }
        return ranges;
    }

    static void AddEmptyLinesToRange(SourceFile& file, LineIterator begin, LineIterator end) {
        if (begin == end)
            return;
        for (auto currWeight = begin->Weight; begin != end; currWeight = begin->Weight, ++begin)
            if (begin->Weight != currWeight)
                file.emplace(begin);
        // add empty line before function/commentary if needed
        if (end != file.end() && !end->IsEmpty() &&
            !(ParseDirective(*end, "endif") || ParseDirective(*end, "else") || ParseDirective(*end, "elif")))
            file.emplace(end);
    }

    static auto RemoveRepeatedLines(SourceFile& file, LineIterator begin, LineIterator end) {
        std::unordered_set<std::string_view> inserted;
        while (begin != end && inserted.count(*ParseDirective(*begin, "include")))
            begin = file.erase(begin);
        for (auto iter = begin; iter != end;)
            if (inserted.count(*ParseDirective(*iter, "include")))
                iter = file.erase(iter);
            else
                inserted.insert(*ParseDirective(*iter++, "include"));
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

    static auto FormatIncludesInRange(LineIterator begin, LineIterator end) {
        for (; begin != end; ++begin)
            FormatInclude(*begin);
    }

    static auto RemoveEmptyLinesAfterIncludes(SourceFile& file) {
        for (auto iter = file.begin(); iter != file.end();) {
            auto prev = iter++;
            if (prev->IsInclude() && iter->IsEmpty())
                iter = file.erase(iter);
        }
    }

    static void FixIncludesInRange(SourceFile& file, LineIterator begin, LineIterator end, bool format) {
        if (format)
            FormatIncludesInRange(begin, end);
        std::tie(begin, end) = SortRange(file, begin, end);
        std::tie(begin, end) = RemoveRepeatedLines(file, begin, end);
        AddEmptyLinesToRange(file, begin, end);
    }

public:
    static auto FixIncludes(SourceFile& file, const CaterogySpecs& cats, bool format) {
        for (auto& line : file)
            if (line.IsInclude())
                line.Weight = GetIncludeCategory(line, cats, file.GetLang());

        RemoveEmptyLinesAfterIncludes(file);
        const auto ranges = MakeRanges(file);
        for (const auto[begin, end] : ranges)
            FixIncludesInRange(file, begin, end, format);
    }
};

void FixIncludes(SourceFile& file, const CaterogySpecs& cats, bool format) {
    Fixer::FixIncludes(file, cats, format);
}
