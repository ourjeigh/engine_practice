#ifndef __WIN64_INCLUDES_H__
#define __WIN64_INCLUDES_H__
#pragma once

#include "config.h"

#ifdef PLATFORM_WIN64
#define IGNORE_WINDOWS_WARNINGS_PUSH	\
	_Pragma("warning(push)")			\
	_Pragma("warning(disable: 5105)")

#define IGNORE_WINDOWS_WARNINGS_POP		\
	_Pragma("warning(pop)")
#endif PLATFORM_WIN64

IGNORE_WINDOWS_WARNINGS_PUSH
#include "windows.h"
#include "windowsx.h"
#include "comdef.h"
#include "combaseapi.h"
#include "mmdeviceapi.h"
#include "audioclient.h"
#include "libloaderapi.h"
#include "timeapi.h"
#include "profileapi.h"
#include "intrin.h"
#include "debugapi.h"
#include "comdef.h"
IGNORE_WINDOWS_WARNINGS_POP

#endif // !__WIN64_INCLUDES_H__

