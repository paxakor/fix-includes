// Copyright 2017, Pavel Korozevtsev.

#pragma once

#include <list>
#include <string>
#include <string_view>
#include <tuple>

class Line : public std::string {
public:
    enum IncludeCategory {
        PAIRED_HEADER,
        LOCAL,
        SAME_PROJ,
        /* pre-global specific indexes */
        GLOBAL = 1000,
        /* pre-std specific indexes */
        EXPCPP = 2000,
        STDCPP,
        STDC,
        POSIX,
        DEPRECATED,
        UNKNOWN
    };
    IncludeCategory Type = UNKNOWN;

public:
    std::tuple<std::string_view, bool> IncludePath() const;  // returns [path, isGlobal]

    bool IsInclude() const;

    bool IsEmpty() const {
        return empty();
    }
};

class SourceFile : public std::list<Line> {
public:
    std::string_view src;
    std::string_view dst;
    SourceFile(std::string_view src, std::string_view dst) : src(src), dst(dst) {};
    void ReadFromFile();
    void WriteToFile();
};
