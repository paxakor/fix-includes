// Copyright 2017, Pavel Korozevtsev, Kutenin Danila.

#pragma once

#include "source_file.hpp"

#include <tuple>
#include <string>
#include <vector>

using CaterogySpec = std::tuple<std::string, WeightType>;
using CaterogySpecs = std::vector<CaterogySpec>;

void FixIncludes(SourceFile& file, const CaterogySpecs& cats, bool format);
