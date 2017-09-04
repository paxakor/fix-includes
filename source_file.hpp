// Copyright 2017, Pavel Korozevtsev, Kutenin Danila.

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
    using std::string::string;

    bool IsInclude() const;
    bool IsEmpty() const {
        return empty();
    }

    std::tuple<std::string_view, bool> IncludePath() const;  // returns [path, isGlobal]
};

enum Lang { C, CPP, AUTO };

class SourceFile : public std::list<Line> {
private:
    std::string Filename;
    Lang Mode = AUTO;

public:
    using std::list<Line>::list;

    Lang GetLang() const;
    void SetLang(Lang mode) {
        Mode = mode;
    }

    void ReadFromFile(std::string_view filename);
    void WriteToFile(std::string_view filename) const;
};
