#pragma once

#include <Noesis_pch.h>

namespace noelog {
    /**
     * @brief Callback function to handle noesis gui events
     * 
     * @param filename 
     * @param line 
     * @param level 
     * @param channel 
     * @param message 
     */
    void messageCallback(const char* filename, uint32_t line, uint32_t level, const char* channel, const char* message);
}