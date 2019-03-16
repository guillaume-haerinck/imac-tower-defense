////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __CORE_LOG_H__
#define __CORE_LOG_H__


#include <NsCore/Noesis.h>
#include <NsCore/KernelApi.h>


namespace Noesis
{

/// Available logging levels
#define NS_LOG_LEVEL_TRACE      0
#define NS_LOG_LEVEL_DEBUG      1
#define NS_LOG_LEVEL_INFO       2
#define NS_LOG_LEVEL_WARNING    3
#define NS_LOG_LEVEL_ERROR      4

/// Compile-time switch to control minimum log level output. Logging is off by default
#ifndef NS_MINIMUM_LOG_LEVEL
    #define NS_MINIMUM_LOG_LEVEL 0xFFFF
#endif

/// An identifier for the channel from which the log originated, for example "Binding"
#ifndef NS_LOG_CHANNEL
    #define NS_LOG_CHANNEL ""
#endif

/// Installs custom logging handler and returns previous one
typedef void (*LogHandler)(const char* file, uint32_t line, uint32_t level, const char* channel,
    const char* message);
NS_CORE_KERNEL_API LogHandler SetLogHandler(LogHandler handler);

/// Executes currently installed logging handler
NS_CORE_KERNEL_API void InvokeLogHandler(const char* file, uint32_t line, uint32_t level,
    const char* channel, NS_FORMAT_PRINTF const char* format, ...) NS_FORMAT_PRINTF_ATTR(5);

#ifndef NS_LOG_BASE
    #define NS_LOG_BASE(level, ...) \
        NS_MACRO_BEGIN \
            if (NS_MINIMUM_LOG_LEVEL <= level) \
            { \
                Noesis::InvokeLogHandler(__FILE__, __LINE__, level, NS_LOG_CHANNEL, __VA_ARGS__); \
            } \
        NS_MACRO_END
#endif

#define NS_LOG_TRACE(...) NS_LOG_BASE(NS_LOG_LEVEL_TRACE, __VA_ARGS__)
#define NS_LOG_DEBUG(...) NS_LOG_BASE(NS_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define NS_LOG_INFO(...) NS_LOG_BASE(NS_LOG_LEVEL_INFO, __VA_ARGS__)
#define NS_LOG_WARNING(...) NS_LOG_BASE(NS_LOG_LEVEL_WARNING, __VA_ARGS__)
#define NS_LOG_ERROR(...) NS_LOG_BASE(NS_LOG_LEVEL_ERROR, __VA_ARGS__)

}

#endif
