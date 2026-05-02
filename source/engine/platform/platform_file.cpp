#include "platform_file.h"
#include "platform.h"
#include "platform/win64/platform_handle_win64.h"

IGNORE_WINDOWS_WARNINGS_PUSH
#include "windows.h"
#include "minwinbase.h"
IGNORE_WINDOWS_WARNINGS_POP

const uint64 k_file_size_max_word = MAXWORD;

template<class t_type>
int32 read_file_internal(
	c_platform_handle& file_handle,
	t_file_open_mode_flags flags,
	int32 start,
	int32 length,
	c_array<t_type>& out_buffer);

template<class t_type>
int32 write_file_internal(
	c_platform_handle& file_handle,
	t_file_open_mode_flags flags,
	int32 start,
	const c_array<t_type>& buffer);

c_platform_handle platform_file_open(const c_file_path& file_path, t_file_open_mode_flags flags)
{
	HANDLE file_handle = nullptr;

	DWORD access = 0;

	if (flags.test(file_open_mode_read))
	{
		access |= GENERIC_READ;
	}

	if (flags.test(file_open_mode_write))
	{
		access |= GENERIC_WRITE;
	}

	// TODO handle read and write sharing separately (maybe delete also??)
	DWORD share_mode = flags.test(file_open_mode_exclusive) ? 0 : FILE_SHARE_READ | FILE_SHARE_WRITE;
	LPSECURITY_ATTRIBUTES security = nullptr;

	DWORD creation_disposition = NULL;

	if (flags.test(file_open_mode_read))
	{
		creation_disposition = OPEN_EXISTING;
	}
	else if (flags.test(file_open_mode_replace))
	{
		creation_disposition = CREATE_ALWAYS;
	}
	else if (flags.test(file_open_mode_write))
	{
		creation_disposition = OPEN_ALWAYS;
	}

	DWORD attributes = FILE_ATTRIBUTE_NORMAL;
	HANDLE template_file = nullptr;

	file_handle = CreateFileA(
		file_path.get_full_path(),
		access,
		share_mode,
		security,
		creation_disposition,
		attributes,
		template_file);

	return c_platform_handle_factory::get_platform_handle_from_native_handle(file_handle);
}

void platform_file_close(c_platform_handle& handle)
{
	CloseHandle(c_platform_handle_factory::get_native_handle_from_platform_handle<HANDLE>(handle));
}

int32 platform_file_read_bytes(
	c_platform_handle& file_handle,
	t_file_open_mode_flags flags,
	int32 start,
	int32 length,
	c_array<byte>& out_buffer)
{
	return read_file_internal(file_handle, flags, start, length, out_buffer);
}

int32 platform_file_write_bytes(
	c_platform_handle& file_handle,
	t_file_open_mode_flags flags,
	int32 start,
	const c_array<byte>& buffer)
{
	return write_file_internal(file_handle, flags, start, buffer);
}

int32 platform_file_read_string(
	c_platform_handle& file_handle,
	t_file_open_mode_flags flags,
	int32 start,
	int32 length,
	c_array<char>& out_buffer)
{
	return read_file_internal(file_handle, flags, start, length, out_buffer);
}

int32 platform_file_write_string(
	c_platform_handle& file_handle,
	t_file_open_mode_flags flags,
	int32 start,
	const c_array<char>& buffer)
{
	return write_file_internal(file_handle, flags, start, buffer);
}

bool platform_file_copy(const c_file_path& source, const c_file_path& dest, bool overwrite)
{
	return CopyFileA(source.get_full_path(), dest.get_full_path(), !overwrite);
}

s_file_info platform_file_get_info(const c_file_path& file_path)
{
	s_file_info out_info;
	zero_object(out_info);

	WIN32_FIND_DATA file_data;
	HANDLE find_handle;

	find_handle = FindFirstFileA(file_path.get_full_path(), &file_data);
	if (find_handle != INVALID_HANDLE_VALUE)
	{
		out_info.exists = true;

		const uint64 high_shift = k_file_size_max_word + 1;
		out_info.size_bytes = (file_data.nFileSizeHigh * (high_shift)) + file_data.nFileSizeLow;

		out_info.creation_time = (file_data.ftCreationTime.dwHighDateTime * high_shift) + file_data.ftCreationTime.dwLowDateTime;
		out_info.write_time = (file_data.ftLastWriteTime.dwHighDateTime * high_shift) + file_data.ftLastWriteTime.dwLowDateTime;

		FindClose(find_handle);
	}

	return out_info;
}

// private 
template<class t_type>
int32 read_file_internal(
	c_platform_handle& file_handle,
	t_file_open_mode_flags flags,
	int32 start,
	int32 length,
	c_array<t_type>& out_buffer)
{
	if (length == 0)
	{
		length = out_buffer.capacity();
	}

	ASSERT(in_range_int32(0, out_buffer.capacity(), length));
	ASSERT(flags.test(file_open_mode_read));


	uint32 bytes_read = 0;
	OVERLAPPED overlapped;
	zero_object(overlapped);
	overlapped.Offset = start;

	bool result = ReadFile(
		c_platform_handle_factory::get_native_handle_from_platform_handle<HANDLE>(file_handle),
		out_buffer.data(),
		length,
		&bytes_read,
		&overlapped);

	return bytes_read;
}

template<class t_type>
int32 write_file_internal(
	c_platform_handle& file_handle,
	t_file_open_mode_flags flags,
	int32 start,
	const c_array<t_type>& buffer)
{
	uint32 bytes_written = 0;

	OVERLAPPED* overlapped_ptr = nullptr;

	if (start != k_invalid)
	{
		OVERLAPPED overlapped;
		zero_object(overlapped);
		overlapped.Offset = start;
		overlapped_ptr = &overlapped;
	}

	bool result = WriteFile(
		c_platform_handle_factory::get_native_handle_from_platform_handle<HANDLE>(file_handle),
		buffer.data(),
		buffer.capacity(),
		&bytes_written,
		overlapped_ptr);

	return bytes_written;
}
