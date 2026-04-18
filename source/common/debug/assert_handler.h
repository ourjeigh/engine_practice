#ifndef __ASSERT_HANDLER_H__
#define __ASSERT_HANDLER_H__
#pragma once

typedef void(*f_assert_handler)(const char* condition, const char* file, const long line);

inline extern f_assert_handler g_assert_handler = nullptr;

#endif // !__ASSERT_HANDLER_H__

