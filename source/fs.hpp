#pragma once

#include <string>
#include "types.hpp"

namespace Spår::Utility
{
std::string readFile(const std::string &path, size_t maxSize = 2 << 23); // 16 MB
void readFileStream(const std::string &path, std::ostream &output);
}