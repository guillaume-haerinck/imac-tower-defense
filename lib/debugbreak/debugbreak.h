#ifndef DEBUG_BREAK_H
#define DEBUG_BREAK_H

/* TODO update this quick fix for global platform */

#ifdef _MSC_VER
	#define debug_break __debugbreak

#else
	#define debug_break __builtin_trap

#endif // ifdef _MSC_VER
#endif // DEBUG_BREAK_H
