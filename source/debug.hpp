#pragma once

#include <iostream>
#include <string>
#include "types.hpp"

namespace Spår::Debug
{
constexpr size_t TAB_SIZE = 4;
constexpr size_t tabAmount = 16;

void printMessage(const std::string &from, const std::string &message);
void printMessage(const std::string &message);
std::runtime_error printError(const std::string &from, const std::string &message);
}