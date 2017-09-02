// Copyright 2017, Pavel Korozevtsev.

#pragma once

#include <list>
#include <string>
#include <string_view>
#include <tuple>

/* IncludeCategory -- include's weight for sort.
** Default weight calculates as (Category + 1) * 1000.
** Feel free to add new categories in .fix-includes in any parent directory.
** For more info see README.
*/
using WeightType = int;
enum IncludeCategory : WeightType {
    PAIRED_HEADER,
    LOCAL,
    SAME_PROJ,
    GLOBAL,
    EXPCPP,
    STDCPP,
    STDC,
    POSIX,
    DEPRECATED,
    UNKNOWN
};

class Line : public std::string {
public:
    WeightType Weight = UNKNOWN;

public:
    std::tuple<std::string_view, bool> IncludePath() const;  // returns [path, isGlobal]

    bool IsInclude() const;

    bool IsEmpty() const {
        return empty();
    }
};


enum Lang { C, CPP, AUTO };

class SourceFile : public std::list<Line> {
private:
    std::string Filename;
    Lang Mode = AUTO;

public:
    void SetLang(Lang mode) {
        Mode = mode;
    }
    Lang GetLang() const;
    void ReadFromFile(std::string_view filename);
    void WriteToFile(std::string_view filename) const;
};
