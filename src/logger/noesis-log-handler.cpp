#include "noesis-log-handler.hpp"
#include <spdlog/spdlog.h>
#include <debugbreak/debugbreak.h>

void noelog::messageCallback(const char* filename, uint32_t line, uint32_t level, const char* channel, const char* message) {
  	switch (level) {
		case 0: spdlog::info("[Noesis] {}", message); break; 	// Trace
		case 1: spdlog::debug("[Noesis] {}", message); break; 	// Debug
		case 2: spdlog::info("[Noesis] {}", message); break; 	// Info
		case 3: spdlog::warn("[Noesis] {}", message); break; 	// Warning
    	case 4: spdlog::error("[Noesis] {}", message); break; 	// Error
	}
}

void noelog::errorHandler(const char* file, uint32_t line, const char* message, bool fatal) {
	spdlog::error("[Noesis] {}", message);
	if (fatal)
		debug_break();
}
