#pragma once

#include <Noesis_pch.h>

namespace noelog {
    void logHandler(const char* filename, uint32_t line, uint32_t level, const char* channel, const char* message);

}