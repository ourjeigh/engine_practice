#ifndef __PLATFORM_ASSERT_H__
#define __PLATFORM_ASSERT_H__
#pragma once

void assert_internal(const char* condition, const char* file, const long line);
void halt_internal(const char* message, const char* file, const long line);

#endif // !__PLATFORM_ASSERT_H__
