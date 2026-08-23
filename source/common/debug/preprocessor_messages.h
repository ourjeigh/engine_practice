#ifndef __PREPROCESSOR_MESSAGES_H__
#define __PREPROCESSOR_MESSAGES_H__
#pragma once

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

// usage:
//#pragma CUSTOM_MESSAGE("message string")
//#pragma CUSTOM_ERROR_FILE_LINE("...\\perf_system.h", 10, "error string")

#define CUSTOM_MESSAGE(msg) \
	message(__FILE__ "(" STRINGIFY(__LINE__) ") : message : " msg)

#define CUSTOM_MESSAGE_FILE_LINE(file, line, msg) \
	message(file "(" STRINGIFY(line) ") : message : " msg)

#define CUSTOM_WARNING(msg) \
	message(__FILE__ "(" STRINGIFY(__LINE__) ") : warning : " msg)

#define CUSTOM_WARNING_FILE_LINE(file, line, msg) \
	message(file "(" STRINGIFY(line) ") : warning : " msg)

#define CUSTOM_ERROR(msg) \
	message(__FILE__ "(" STRINGIFY(__LINE__) ") : error : " msg)

#define CUSTOM_ERROR_FILE_LINE(file, line, msg) \
	message(file "(" STRINGIFY(line) ") : error : " msg)

#endif // !__PREPROCESSOR_MESSAGES_H__
