[**home🏠**](../README.md)

### *namespace Core*
# [[abstract]] class App

From *[spaar.h](../../source/spaar.hpp)*
```cpp
class App
{
public:
	explicit App(SpårConfiguration configuration);
	virtual ~App();

	void launch();
	virtual void start();
	virtual void run();

	bool keyDown(SDL_Scancode code) const;
}
```

## Description
The App class is an abstract class which encapsulates the main logic of your game.
It has three main parts:
- [App(SpårConfiguration configuration)](#appspårconfiguration-configuration)
- [virtual void start()](#virtual-void-start)
- [virtual void run()](#virtual-void-run)

## App(SpårConfiguration configuration)

## virtual void start()

## virtual void run()