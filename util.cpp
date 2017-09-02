// Copyright 2017, Pavel Korozevtsev, Kutenin Danila.

#include "util.hpp"

#include <cctype>

std::string_view Strip(std::string_view s) {
    while (!s.empty() && std::isspace(s.front()))
        s.remove_prefix(1);
    while (!s.empty() && std::isspace(s.back()))
        s.remove_suffix(1);
    return s;
}
