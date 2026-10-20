#ifndef MANGO_COLLISIONDEBUG_H
#define MANGO_COLLISIONDEBUG_H

#include <iostream>
#include "EngineDebug.h"

#ifdef TOMATO_DEBUG
#define TOMATO_DEBUG_COLLISION
#endif

#ifdef TOMATO_DEBUG_COLLISION
#define TOMATO_DEBUG_COLLISION_DETECT   // for collision detection
// #define TOMATO_DEBUG_COLLISION_DETECT_VERBOSE
// #define TOMATO_DEBUG_COLLISION_RESOLVE  // for collision resolution
#endif

#ifdef TOMATO_DEBUG_COLLISION_DETECT_VERBOSE
#ifndef TOMATO_DEBUG_COLLISION_DETECT
#define TOMATO_DEBUG_COLLISION_DETECT
#endif
#endif

#ifdef TOMATO_DEBUG_COLLISION_DETECT
#define COL_DETECT_LOG(msg) \
    do { std::cout << "   [COL DETECT  ] " << msg << "\n"; } while (0)
#else
#define COL_DETECT_LOG(msg) do { } while (0)
#endif

#ifdef TOMATO_DEBUG_COLLISION_DETECT_VERBOSE
#define COL_DETECT_VERBOSE_LOG(msg) \
    do { std::cout << "   [COL DETECT V] " << msg << "\n"; } while (0)
#else
#define COL_DETECT_VERBOSE_LOG(msg) do { } while (0)
#endif

#ifdef TOMATO_DEBUG_COLLISION_RESOLVE
#define COL_RESOLVE_LOG(msg)\
    do { std::cout << "   [COL RESOLVE ] " << msg << "\n"; } while (0)
#else
#define COL_RESOLVE_LOG(msg) do { } while (0)
#endif

#endif //MANGO_COLLISIONDEBUG_H