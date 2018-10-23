#ifndef CONST_HPP
#define CONST_HPP

#include "../database/rect.hpp"

#include <limits>
#include <string>

const std::string UNDEFINED_STRING = "UNDEFINED";
const int UNDEFINED_ID = -1;
const Point ORIGIN_POINT(0.0, 0.0);
const Point MAX_POINT(std::numeric_limits<double>::max(),
                      std::numeric_limits<double>::max());
const Point MIN_POINT(std::numeric_limits<double>::lowest(),
                      std::numeric_limits<double>::lowest());

#endif
