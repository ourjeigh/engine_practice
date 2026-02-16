#ifndef __CONFIG_H__
#define __CONFIG_H__
#pragma once

// internal defines for common ones
#ifdef _DEBUG
#define CONFIG_DEBUG

#define FEATURE_LOGGING
#endif 

#ifdef _WIN64
#define PLATFORM_WINDOWS
#endif // _WIN64

#if defined(CONFIG_DEBUG)

// Debug specific features
#define MEMORY_TRACKING_ENABLED

#define IF_DEBUG(x) x
#define DEBUG_ONLY_MEMBER(x) x
#define DEBUG_ONLY_PARAM_LEFT_COMMA(x) , x
#define DEBUG_ONLY_PARAM_RIGHT_COMMA(x)  x ,

#else 
#message no config detected, RELEASE assumed
#define CONFIG_RELEASE

#define IF_DEBUG(x)
#define DEBUG_ONLY_MEMBER(x)
#define DEBUG_ONLY_PARAM_LEFT_COMMA(x)
#define DEBUG_ONLY_PARAM_RIGHT_COMMA(x)

#endif




#endif //__CONFIG_H__