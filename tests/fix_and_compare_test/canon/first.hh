#pragma once

#include <algorithm>
#include <vector>

signed foo(signed a, signed b, signed c) {
    std::vector<signed> abc = {a, b, c};
    return std::find(abc.begin(), abc.end(), 5) == abc.end() ? 0 : a + b + c;
}
