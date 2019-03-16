#include "noesis-log-handler.hpp"

void noelog::logHandler(const char* filename, uint32_t line, uint32_t level, const char* channel, const char* message) {
    if (strcmp(channel, "") == 0) {
        // [TRACE] [DEBUG] [INFO] [WARNING] [ERROR]
        const char* prefixes[] = { "T", "D", "I", "W", "E" };
        const char* prefix = level < NS_COUNTOF(prefixes) ? prefixes[level] : " ";
        fprintf(stderr, "[NOESIS/%s] %s\n", prefix, message);
    }
}
