// Copyright 2017, Pavel Korozevtsev.

#include "source_file.hpp"
#include "util.hpp"

#include <fstream>
#include <stdexcept>

std::tuple<std::string_view, bool> Line::IncludePath() const {
    if (IsInclude()) {
        const auto sub = Strip(std::string_view(*this).substr(Directive.size()));
        if (sub.size() >= 2)
            return {sub.substr(1, sub.size() - 2), !(sub.front() == '"' && sub.back() == '"')};
    }
    return {"", false};
}

void SourceFile::ReadFromFile(std::string_view filename) {
    clear();
    std::ifstream fin(filename.data());
    while (fin.good()) {
        emplace_back();
        std::getline(fin, back());
    }
}

void SourceFile::WriteToFile(std::string_view filename) {
    std::ofstream fout(filename.data());
    JoinToStream(fout, begin(), end(), '\n');
}
