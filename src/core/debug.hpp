#pragma once

/*
 * @brief Detailled memory leak informations for objects created on the heap
 * 
 */
#if defined (_DEBUG) && defined (_WIN32)
	#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
	#define new new
#endif // _DEBUG && _WIN32

/*
 * @brief Remove what's inside of the function if not in debug mode
 */
#ifdef _DEBUG
	#define DebugOnly(x) x
#else
	#define DebugOnly(x)
#endif // _DEBUG
