#ifndef __ENGINE_FEATURES_H__
#define __ENGINE_FEATURES_H__
#pragma once

#include "config.h"

#ifdef CONFIG_DEBUG

// Debug specific features
#define MEMORY_TRACKING_ENABLED

#elif defined(CONFIG_RELEASE)
// Release
// 
#else
#error NO CONFIGURATION DEFINED
#endif

#endif // !__ENGINE_FEATURES_H__