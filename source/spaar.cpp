#include "spaar.hpp"

#include <memory>

namespace Spår::Core
{
class Engine::EngineImplementation
{
public:
	SDL_Window *window;
	VkInstance instance;
	SpårConfiguration configuration;

	EngineImplementation(SpårConfiguration config);
	~EngineImplementation();

	EngineImplementation(const EngineImplementation &) = delete;
	EngineImplementation &operator=(const EngineImplementation &) = delete;
};

Engine::EngineImplementation::EngineImplementation(SpårConfiguration config)
	: window(nullptr), instance(VK_NULL_HANDLE), configuration(std::move(config))
{
	if(!SDL_Init(SDL_INIT_VIDEO))
	{
		throw Debug::printError("Core::Engine::EngineImplementation::EngineImplementation", "SDL_Init error: " + std::string(SDL_GetError()));
	}

	window = SDL_CreateWindow(configuration.title,
							configuration.width, configuration.height,
							SDL_WINDOW_VULKAN);
	if(!window)
	{
		SDL_Quit();
		throw Debug::printError("Core::Engine::EngineImplementation::EngineImplementation", "SDL_CreateWindow error: " + std::string(SDL_GetError()));
	}

	if(Graphics::initialiseVulkan(&instance, configuration) != VK_SUCCESS)
	{
		SDL_DestroyWindow(window);
		SDL_Quit();

		throw Debug::printError("Core::Engine::EngineImplementation::EngineImplementation", "Vulkan initialisation failed");
	}
}

Engine::EngineImplementation::~EngineImplementation()
{
	if(instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(instance, nullptr);
	}

	if(window)
	{
		SDL_DestroyWindow(window);
	}

	SDL_Quit();
}

Engine::Engine(SpårConfiguration configuration)
	: running(true), implementation(std::make_unique<Engine::EngineImplementation>(configuration))
{
	#ifdef DEBUG
		Debug::printMessage("Core::Engine::Engine", "Spår debug mode enabled.");
		Debug::printMessage("Core::Engine::Engine", "Starting Spår.");
	#endif
}

Engine::~Engine() = default;

Engine::Engine(Engine &&other) noexcept = default;
Engine &Engine::operator=(Engine &&other) noexcept = default;

void Engine::pollEvents()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_EVENT_QUIT:
				running = false;
				return;
		}
	}
}

App::App(SpårConfiguration configuration)
	: engine(configuration), keyboardState(nullptr)
{
}

void App::launch()
{
	#ifdef DEBUG
		Debug::printMessage("Core::App::launch", "Starting application.");
	#endif

	keyboardState = SDL_GetKeyboardState(nullptr);
	start();

	while(engine.running)
	{
		run();
	}
}

void App::start()
{
	Debug::printMessage("Core::App::start", "Default start function executed.");

	activeScene->propagateStart();
}

void App::run()
{
	engine.pollEvents();

	activeScene->propagateUpdate();
}

bool App::keyDown(SDL_Scancode code) const
{
	return keyboardState && keyboardState[code];
}
}