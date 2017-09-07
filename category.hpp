// Copyright 2017, Pavel Korozevtsev, Kutenin Danila.

#pragma once

#include "source_file.hpp"

#include <string>
#include <tuple>
#include <vector>

using CaterogySpec = std::tuple<std::string, WeightType>;
using CaterogySpecs = std::vector<CaterogySpec>;

WeightType GetIncludeCategory(const Line& line, const CaterogySpecs& cats, Lang mode);
