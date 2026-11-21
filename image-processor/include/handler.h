#pragma once

#include "image.h"
#include <memory>
#include "parser.h"

std::shared_ptr<Image> ApplyFilters(std::shared_ptr<Image> img, Parser& parser);
