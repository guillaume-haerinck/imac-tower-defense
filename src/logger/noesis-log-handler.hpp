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

    /**
     * @brief Callback function to handle noesis errors
     * 
     * @param file 
     * @param line 
     * @param message 
     * @param fatal 
     */
    void errorHandler(const char* file, uint32_t line, const char* message, bool fatal);
}