// Copyright 2017, Pavel Korozevtsev.

#pragma once

#include <fstream>
#include <string_view>

std::string_view Strip(std::string_view s);

template <typename Iter, typename DelimType>
void JoinToStream(std::ostream& out, Iter begin, Iter end, const DelimType& delim) {
    if (begin == end)
        return;
    out << *begin++;
    while (begin != end)
        out << delim << *begin++;
}
