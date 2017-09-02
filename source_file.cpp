// Copyright 2017, Pavel Korozevtsev.

#include "source_file.hpp"
#include "util.hpp"

#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_set>

// returns text after "\s*#\s*include\s*" or empty string if line don't match
static std::string_view TryParseInclude(std::string_view sv) {
    const auto removePrefix = [&sv](std::string_view prefix) {
        if (sv.size() < prefix.size() || sv.substr(0, prefix.size()) != prefix)
            return false;
        sv.remove_prefix(prefix.size());
        sv = Strip(sv);
        return true;
    };
    sv = Strip(sv);
    return removePrefix("#") && removePrefix("include") ? sv : "";
}

std::tuple<std::string_view, bool> Line::IncludePath() const {
    auto sv = TryParseInclude(*this);
    const auto isGlobal = sv[0] == '<';
    sv.remove_prefix(1);
    const auto closingSymPos = sv.find(isGlobal ? '>' : '"');
    if (closingSymPos == std::string_view::npos)
        return {"", false};
    else
        return {{sv.begin(), closingSymPos}, isGlobal};
}

bool Line::IsInclude() const {
    return !TryParseInclude(*this).empty();
}

static auto GetLangByExtention(std::string_view filename) {
    const static std::unordered_set<std::string_view> CppExt = {"cpp", "cc", "cxx", "hpp", "hh", "hxx"};
    const auto dotPos = filename.rfind('.');
    if (dotPos != std::string_view::npos) {
        return CppExt.count(filename.substr(dotPos)) ? CPP : C;
    }
    return CPP;
}

Lang SourceFile::GetLang() const {
    return Mode == AUTO ? GetLangByExtention(Filename) : Mode;
}

void SourceFile::ReadFromFile(std::string_view filename) {
    clear();
    Filename = filename;
    std::ifstream fin(filename.data());
    while (fin.good()) {
        emplace_back();
        std::getline(fin, back());
    }
}

void SourceFile::WriteToFile(std::string_view filename) const {
    std::ofstream fout(filename.data());
    JoinToStream(fout, begin(), end(), '\n');
}
