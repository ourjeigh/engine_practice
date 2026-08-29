#ifndef __CONFIG_H__
#define __CONFIG_H__
#pragma once

// internal defines for common ones
#ifdef _DEBUG
#define CONFIG_NAME "debug"
#define CONFIG_DEBUG

#define FEATURE_LOGGING
#define FEATURE_PERF_MEASUREMENT
#define FEATURE_REPLAY
//#define FEATURE_TODO_MESSAGES
#endif //_DEBUG



#ifdef CONFIG_DEBUG

#define IF_DEBUG(x) x
#define DEBUG_ONLY_MEMBER(x) x
#define DEBUG_ONLY_PARAM_LEFT_COMMA(x) , x
#define DEBUG_ONLY_PARAM_RIGHT_COMMA(x)  x ,

#else 
#define CONFIG_NAME "release"
#define CONFIG_RELEASE

#define IF_DEBUG(x)
#define DEBUG_ONLY_MEMBER(x)
#define DEBUG_ONLY_PARAM_LEFT_COMMA(x)
#define DEBUG_ONLY_PARAM_RIGHT_COMMA(x)

#endif
#endif //__CONFIG_H__