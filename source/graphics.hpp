#pragma once

#include "spaar.hpp"
#include "types.hpp"

namespace Spår::Graphics
{
VkResult initialiseVulkan(VkInstance *instance, Core::SpårConfiguration configuration);
}