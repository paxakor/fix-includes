// Copyright 2017, Pavel Korozevtsev, Kutenin Danila.

#pragma once

#include "source_file.hpp"

#include <tuple>
#include <string>

using CaterogySpec = std::tuple<std::string, WeightType>;

void FixIncludes(SourceFile& file);
