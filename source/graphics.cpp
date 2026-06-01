#include "graphics.hpp"

namespace Spår::Graphics
{
// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers
bool checkVulkanValidationLayerSupport(const std::vector<const char *> &validationLayers)
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for(const auto &layer : validationLayers)
	{
		bool found = false;
		for(const auto &available : availableLayers)
		{
			if(std::strcmp(layer, available.layerName) == 0)
			{
				found = true;
				break;
			}
		}

		if(!found)
		{
			Debug::printError("Graphics::checkValidationLayerSupport", 
							std::string(layer) + " is not an available validation layer.");
			return false;
		}
	}

	return true;
}

VkResult initialiseVulkan(VkInstance *instance, Core::SpårConfiguration configuration)
{
	VkApplicationInfo applicationInformation =
	{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = configuration.title,
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "Spår Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};

	VkInstanceCreateInfo createInformation =
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &applicationInformation,
		.enabledLayerCount = 0
	};

	createInformation.ppEnabledExtensionNames
		= SDL_Vulkan_GetInstanceExtensions(&createInformation.enabledExtensionCount);

	#ifdef DEBUG
		if(!checkVulkanValidationLayerSupport(configuration.validationLayers))
		{
			Debug::printError("Graphics::initialiseVulkan", 
							"Not all requested Vulkan validation layers are available.");
			return VK_NOT_READY;
		}

		createInformation.enabledLayerCount = static_cast<uint32_t>(configuration.validationLayers.size());
		createInformation.ppEnabledLayerNames = configuration.validationLayers.data();
	#else
		createInformation.enabledLayerCount = 0;
	#endif

	if(vkCreateInstance(&createInformation, nullptr, instance) != VK_SUCCESS)
	{
		Debug::printError("Graphics::initialiseVulkan", "Failed to create Vulkan instance.");
		return VK_NOT_READY;
	}

	#ifdef DEBUG
		Debug::printMessage("Graphics::initialiseVulkan", "Created Vulkan instance.");
	#endif

	return VK_SUCCESS;
}
}