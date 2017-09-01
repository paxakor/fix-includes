// Copyright 2017, Pavel Korozevtsev.

#include "category.hpp"
#include "source_file.hpp"

#include <algorithm>
#include <tuple>
#include <unordered_set>
#include <vector>
#include <iostream>

class Fixer {
private:
    static auto FormatInclude(std::string& cur) {
        // we know that cur has '#\s*include \s* something'
        static const auto include = static_cast<const std::string&>("include");
        auto afterInclude = cur.find(include) + include.size();
        while (std::isspace(cur.back()))
            cur.pop_back();

        cur.erase(std::remove_if(cur.begin(), cur.begin() + afterInclude, [](auto x) {
            return std::isspace(x);
        }), cur.begin() + afterInclude);
        afterInclude = cur.find(include) + include.size();
        // if we haven't found the space
        if (!std::isspace(cur[afterInclude])) {
            cur += " ";
            auto cursize = cur.size() - 1;
            while (cursize > afterInclude) {
                cur[cursize] = cur[cursize - 1];
                --cursize;
            }
            cur[cursize] = ' ';
        } else {
            // if we have found it
            auto endSpaces = afterInclude;
            while (std::isspace(cur[endSpaces])) {
                ++endSpaces;
            }
            cur.erase(cur.begin() + afterInclude + 1, cur.begin() + endSpaces);
        }
    }

    static auto FindRanges(SourceFile& file) {
        std::vector<std::tuple<SourceFile::iterator, SourceFile::iterator>> ranges;
        for (auto iter = file.begin(); iter != file.end();) {
            while (iter != file.end() && !iter->IsInclude())
                ++iter;
            if (iter == file.end())
                break;
            FormatInclude(*iter);
            const auto begin = iter++;
            auto lastInclude = begin;
            while (iter != file.end() && (iter->IsInclude() || iter->IsEmpty()))
                if (iter->IsInclude()) {
                    FormatInclude(*iter);
                    lastInclude = iter++;
                }
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
        auto currType = begin->Type;
        for (; begin != end; ++begin) {
            if (begin->Type != currType) {
                currType = begin->Type;
                file.emplace(begin);
            }
        }
        // add empty line before function/commentary if needed
        if (end != file.end() && !end->IsEmpty()) {
            file.emplace(end);
        }
    }

    static auto RemoveRepeatedLines(SourceFile& file, SourceFile::iterator begin, SourceFile::iterator end,
                                    std::unordered_set<std::string>& inserted) {
        while (begin != end && inserted.count(*begin))
            begin = file.erase(begin);
        for (auto iter = begin; iter != end;)
            if (inserted.count(*iter))
                iter = file.erase(iter);
            else {
                inserted.insert(*iter++);
            }
        return std::tuple(begin, end);
    }

    static void FixIncludesInRange(SourceFile& file, SourceFile::iterator begin, SourceFile::iterator end,
                                   std::unordered_set<std::string>& inserted) {
        RemoveEmptyLinesInRange(file, begin, end);
        std::list<Line> tosort;
        tosort.splice(tosort.end(), file, begin, end);
        tosort.sort([](const auto& a, const auto& b) {
            if (a.Type != b.Type)
                return static_cast<int>(a.Type) < static_cast<int>(b.Type);
            return static_cast<const std::string&>(a) < static_cast<const std::string&>(b);
        });
        begin = tosort.begin();
        file.splice(end, tosort, tosort.begin(), tosort.end());
        std::tie(begin, end) = RemoveRepeatedLines(file, begin, end, inserted);
        AddEmptyLinesToRange(file, begin, end);
    }

public:
    static auto FixIncludes(SourceFile& file) {
        for (auto& line : file)
            if (line.IsInclude())
                line.Type = GetIncludeCategory(line, file);

        const auto ranges = FindRanges(file);
        std::unordered_set<std::string> inserted;
        for (const auto[begin, end] : ranges)
            FixIncludesInRange(file, begin, end, inserted);
    }
};

void FixIncludes(SourceFile& file) {
    Fixer::FixIncludes(file);
}
