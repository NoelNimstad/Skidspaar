#pragma once

#ifdef EN
	#define Spaar Spår
#endif

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#ifdef __APPLE__
	#include <MoltenVK/mvk_vulkan.h>
#else
	#include <vulkan/vulkan.h>
#endif
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "defaults.hpp"
#include "types.hpp"
#include "debug.hpp"
#include "fs.hpp"
#include "ECS.hpp"

namespace Spår::Core
{
struct SpårConfiguration
{
	const char *title = "Spår Application";
	uint16_t width = 320;
	uint16_t height = 240;
	std::vector<const char *> validationLayers;
};

class Engine
{
public:
	explicit Engine(SpårConfiguration configuration);
	~Engine();

	Engine(const Engine &) = delete;
	Engine &operator=(const Engine &) = delete;
	
	Engine(Engine &&other) noexcept;
	Engine &operator=(Engine &&other) noexcept;

	bool running;
	void pollEvents();

private:
	class EngineImplementation; // PImpl
	std::unique_ptr<EngineImplementation> implementation;
};

class App
{
public:
	explicit App(SpårConfiguration configuration);
	virtual ~App() = default;

	void launch();
	virtual void start();
	virtual void run();

	bool keyDown(SDL_Scancode code) const;

	template<typename T, typename ...Args> requires std::is_base_of<ECS::Scene, T>::value
	void setActiveScene(Args &&...args)
	{
		activeScene = std::make_unique<T>(std::forward<Args>(args)...);
		activeScene->app = this;
	}

private:
	Engine engine;
	std::unique_ptr<ECS::Scene> activeScene;	

	const bool *keyboardState;
};
}

#include "graphics.hpp"