// Copyright 2017, Pavel Korozevtsev, Kutenin Danila.

#include "util.hpp"

#include <cctype>

std::string_view Strip(std::string_view s) {
    return StripRight(StripLeft(s));
}

std::string_view StripLeft(std::string_view s) {
    while (!s.empty() && std::isspace(s.front()))
        s.remove_prefix(1);
    return s;
}

std::string_view StripRight(std::string_view s) {
    while (!s.empty() && std::isspace(s.back()))
        s.remove_suffix(1);
    return s;
}

// returns text after "\s*#\s*directive\s*" if line match
std::optional<std::string_view> ParseDirective(std::string_view line, std::string_view directive) {
    const auto removePrefix = [&line](std::string_view prefix) {
        if (line.size() < prefix.size() || line.substr(0, prefix.size()) != prefix)
            return false;
        line.remove_prefix(prefix.size());
        line = StripLeft(line);
        return true;
    };
    line = StripLeft(line);
    if (removePrefix("#") && removePrefix(directive))
        return line;
    else
        return {};
}