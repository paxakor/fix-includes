// Copyright 2017, Pavel Korozevtsev.

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <list>
#include <tuple>

class Line : public std::string {
private:
    inline static const/*expr*/ std::string_view Directive = "#include";

public:
    enum IncludeCategory { LOCAL, GLOBAL, EXPCPP, STDCPP, STDC, POSIX, DEPRECATED, UNKNOWN };
    IncludeCategory Type = UNKNOWN;

public:
    std::tuple<std::string_view, bool> IncludePath() const;  // returns [path, isGlobal]

    bool IsInclude() const {
        return std::string_view(*this).substr(0, Directive.size()) == Directive;
    }

    bool IsEmpty() const {
        return empty();
    }
};

class SourceFile : public std::list<Line> {
public:
    void ReadFromFile(std::string_view filename);
    void WriteToFile(std::string_view filename);
};
