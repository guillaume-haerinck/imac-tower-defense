#include <SDL2/SDL.h>

namespace evnt {
	struct ChangeCursor {
		ChangeCursor(SDL_SystemCursor type) : type(type) {}

		SDL_SystemCursor type;
	};
}
