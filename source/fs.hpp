#pragma once

#include <string>
#include "types.hpp"

namespace Spår::Utility
{
std::string readFile(const std::string &path);
void readFileStream(const std::string &path, std::ostream &output);
}