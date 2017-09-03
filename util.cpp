// Copyright 2017, Pavel Korozevtsev, Kutenin Danila.

#include "util.hpp"

#include <cctype>

// TODO (@paxakor): clean up here when someone implement <charconv>
#if __has_include(<charconv>)
#include <charconv>
#else
#include <cstdlib>
#include <cerrno>
#endif

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

std::tuple<size_t, bool> IntFromString(std::string_view s, int& val) {
// TODO (@paxakor): clean up here when someone implement <charconv>
#if __has_include(<charconv>)
    const auto[end, ec] = std::from_chars(s.data(), s.data() + s.size(), val);
    return {end - s.data(), ec != std::errc::invalid_argument && ec != std::errc::result_out_of_range};
#else
    auto end = s.data() + s.size();
    auto vall = std::strtol(s.data(), const_cast<char**>(&end), 10);
    if (errno != ERANGE && s.data() != end) {
        val = vall;
        return {end - s.data(), true};
    }
    return {end - s.data(), false};
#endif
}
