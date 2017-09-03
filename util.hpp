// Copyright 2017, Pavel Korozevtsev, Kutenin Danila.

#pragma once

#include <fstream>
#include <optional>
#include <string_view>
#include <tuple>

std::string_view Strip(std::string_view s);
std::string_view StripLeft(std::string_view s);
std::string_view StripRight(std::string_view s);
std::optional<std::string_view> ParseDirective(std::string_view line, std::string_view directive);
std::tuple<size_t, bool> IntFromString(std::string_view s, int& val);

template <typename Iter, typename DelimType>
void JoinToStream(std::ostream& out, Iter begin, Iter end, const DelimType& delim) {
    if (begin == end)
        return;
    out << *begin++;
    while (begin != end)
        out << delim << *begin++;
}
