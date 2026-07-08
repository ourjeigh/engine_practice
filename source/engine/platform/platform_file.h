#ifndef __PLATFORM_FILE_H__
#define __PLATFORM_FILE_H__
#pragma once

#include "platform_handle.h"
#include "engine/file_system/file.h"

c_platform_handle platform_file_open(const c_file_path& file_path, t_file_open_mode_flags flags);
void platform_file_close(c_platform_handle& handle);
bool platform_file_delete(const c_file_path& file_path);
void platform_file_directory_get_files(const c_file_path& directory, c_stack<c_file_path> out_files);

int32 platform_file_read_bytes(
	c_platform_handle& file_handle,
	t_file_open_mode_flags flags,
	int32 start,
	int32 length,
	c_array<byte>& out_buffer);

int32 platform_file_write_bytes(
	c_platform_handle& file_handle,
	t_file_open_mode_flags flags,
	int32 start,
	const c_array<byte>& buffer);

int32 platform_file_read_string(
	c_platform_handle& file_handle,
	t_file_open_mode_flags flags,
	int32 start,
	int32 length,
	c_array<char>& out_buffer);

int32 platform_file_write_string(
	c_platform_handle& file_handle,
	t_file_open_mode_flags flags,
	int32 start,
	const c_array<char>& buffer);


bool platform_file_copy(const c_file_path& source, const c_file_path& dest, bool overwrite);
s_file_info platform_file_get_info(const c_file_path& file_path);
#endif // __PLATFORM_FILE_H__