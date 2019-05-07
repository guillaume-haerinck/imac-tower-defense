#ifdef _WIN32
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#include <cstdlib>
#include <iostream>
#include <debugbreak/debugbreak.h>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

struct AnEvent { int value; };

struct Listener1
{
	void receive(const AnEvent& event) { 
		spdlog::info("Works ! {}", event.value);
	}
};

struct Listener2
{
	void receive(const AnEvent& event) {
		spdlog::info("2 Works ! {}", event.value);
	}
};

int main(int argc, char** argv) {
    #ifdef _WIN32 // Check memory leaks
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

	entt::Dispatcher dispatcher{};
	Listener1 listener1;
	Listener2 listener2;
	dispatcher.sink<AnEvent>().connect(&listener1);
	dispatcher.sink<AnEvent>().connect(&listener2);

	dispatcher.trigger<AnEvent>(42);

    return EXIT_SUCCESS;
}