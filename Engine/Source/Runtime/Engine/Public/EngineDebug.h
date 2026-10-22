#ifndef MANGO_ENGINEDEBUG_H
#define MANGO_ENGINEDEBUG_H

#if defined(_DEBUG) || !defined(NDEBUG)
#define TOMATO_DEBUG
#endif

#ifdef TOMATO_DEBUG
// #define TOMATO_DEBUG_TICK
#endif

#ifdef TOMATO_DEBUG_TICK
#define TICK_LOG(msg) std::cout << msg << "\n"
#else
#define TICK_LOG(msg)
#endif

#endif //MANGO_ENGINEDEBUG_H