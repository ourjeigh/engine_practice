#include "file.h"
#include <debug/logging.h>
#include "platform/platform.h"
#include <platform/platform_handle.h>
#include <platform/platform_handle_windows.h>

IGNORE_WINDOWS_WARNINGS_PUSH
#include "windows.h"
#include <minwinbase.h>
IGNORE_WINDOWS_WARNINGS_POP

// move into windows impl
const uint64 k_file_size_max_word = MAXWORD;

const bool k_file_debug_logging = false;

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


c_file_path::c_file_path(const char* path)
{
	m_data.print(path);
}

c_file_path::c_file_path(const t_string_256& path)
{
	m_data.copy_from(path);
}

c_file_path::c_file_path(const c_file_path& other)
{
	m_data.copy_from(other.m_data);
	//m_data = other.m_data;
}

c_file_path& c_file_path::operator=(const c_file_path& other)
{
	m_data.copy_from(other.m_data);
	return *this;
}

bool c_file_path::exists() const
{
	return get_file_info(*this).exists;
}

// remove?
uint64 c_file_path::get_file_size_bytes() const
{
	return get_file_info(*this).size_bytes;
}

void c_file_path::get_file_name(t_string_256& out_file_name) const
{
	out_file_name.clear();
	uint8 directory_name_index;
	uint8 file_name_index;
	uint8 ext_index;
	split_path(directory_name_index, file_name_index, ext_index);

	out_file_name.copy_from_range(m_data, file_name_index, m_data.used());
}

void c_file_path::get_file_ext(t_string_256& out_file_ext) const
{
	out_file_ext.clear();

	uint8 directory_name_index;
	uint8 file_name_index;
	uint8 ext_index;
	split_path(directory_name_index, file_name_index, ext_index);

	out_file_ext.copy_from_range(m_data, ext_index, m_data.used());
}

void c_file_path::get_file_name_no_ext(t_string_256& out_file_name) const
{
	out_file_name.clear();

	uint8 directory_name_index;
	uint8 file_name_index;
	uint8 ext_index;
	split_path(directory_name_index, file_name_index, ext_index);

	out_file_name.copy_from_range(m_data, file_name_index, ext_index - 1);
}

void c_file_path::get_directory_path(t_string_256& out_directory_path) const
{
	out_directory_path.clear();

	uint8 directory_name_index;
	uint8 file_name_index;
	uint8 ext_index;
	split_path(directory_name_index, file_name_index, ext_index);

	out_directory_path.copy_from_range(m_data, 0, file_name_index - 1);
}

void c_file_path::get_directory_name(t_string_256& out_directory_name) const
{
	out_directory_name.clear();

	uint8 directory_name_index;
	uint8 file_name_index;
	uint8 ext_index;
	split_path(directory_name_index, file_name_index, ext_index);

	out_directory_name.copy_from_range(m_data, directory_name_index, file_name_index - 1);
}

void c_file_path::split_path(uint8& out_directory_name_index, uint8& out_filename_index, uint8& out_ext_index) const
{
	out_directory_name_index = k_invalid;
	out_filename_index = k_invalid;
	out_ext_index = k_invalid;

	for (int8 i = int32_to_uint8(m_data.used()) - 1; i >= 0; i--)
	{
		char ch = m_data[i];
		if (out_ext_index == k_invalid && ch == get_ext_separator())
		{
			out_ext_index = i + 1;
		}
		else if (out_filename_index == k_invalid && ch == get_path_separator())
		{
			out_filename_index = i + 1;
		}
		else if (out_directory_name_index == k_invalid &&
			out_filename_index != k_invalid &&
			ch == get_path_separator())
		{
			out_directory_name_index = i + 1;
			break;
		}
	}

	ASSERT(out_directory_name_index != k_invalid);
	ASSERT(out_filename_index != k_invalid);
	ASSERT(out_ext_index != k_invalid);
}

bool c_file::open(const c_file_path& file_path, t_file_open_mode_flags flags)
{
	ASSERT(!is_open());

	bool result = false;

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

	if (file_handle != INVALID_HANDLE_VALUE)
	{
		result = true;
		m_file_handle = c_platform_handle_factory::get_platform_handle_from_native_handle(file_handle);
		m_flags = flags;
		m_file_size = file_path.get_file_size_bytes();
		m_path = file_path;
	}

	return result;
}

bool c_file::close()
{
	bool result = false;
	if (m_file_handle.is_valid())
	{
		CloseHandle(c_platform_handle_factory::get_native_handle_from_platform_handle(m_file_handle));
		m_file_handle.invalidate();
		result = true;
	}

	return result;
}

int32 c_file::read_bytes(int32 start, int32 length, c_array<byte>& out_buffer)
{
	ASSERT(is_open());

	return read_file_internal(
		m_file_handle,
		m_flags,
		start,
		length,
		out_buffer);
}

int32 c_file::write_bytes(int32 start, const c_array<byte>& buffer)
{
	ASSERT(is_open());
	ASSERT(m_flags.test(file_open_mode_write));

	return write_file_internal(m_file_handle, m_flags, start, buffer);
}

int32 c_file::write_string(int32 start, const c_array<char>& buffer)
{
	return write_file_internal(m_file_handle, m_flags, start, buffer);
}

bool c_file_buffered::open(const c_file_path& file_path, t_file_open_mode_flags flags)
{
	ASSERT(m_buffer.is_valid());

	bool result = c_file::open(file_path, flags);

	// tbd if a big read is desired on open
	if (result && flags.test(file_open_mode_read))
	{
		uint32 bytes_read = c_file::read_bytes(m_file_position, m_buffer.capacity(), m_buffer);
		m_file_position += bytes_read;
		m_buffer_end = bytes_read;
		result = bytes_read > 0;
	}

	return result;
}

int32 c_file_buffered::read_bytes(int32 length, c_array<byte>& out_buffer)
{
	ASSERT(is_open());

	// if the read request is larger than the buffer capacity, we will be guaranteed to have to do an immediate disk read,
	// which defeats the purpose of a buffered file. eventually we could support this case, but for now we'll keep it simple 
	// and just forbid it. keeping the warning as a reminder
	ASSERT(length < m_buffer.capacity());
	/*if (length >= m_buffer.capacity())
	{
		log(warning,
			"c_file_buffere: read_bytes length called with length larger than buffer size. consider increasing the \
			file's buffer size to improve efficiency if reads of this length are needed [file: %s, buffer size: %i, read length: %i]",
			m_path,
			m_buffer.capacity(),
			length);
	}*/

	int32 out_bytes_copied = 0;
	int32 buffered_bytes_remaining = m_buffer_end - m_buffer_position;

	int32 first_size = math_min(buffered_bytes_remaining, length);
	if (first_size > 0)
	{
		out_buffer.copy_from_range(m_buffer, m_buffer_position, m_buffer_position + first_size);
		out_bytes_copied += first_size;
		m_buffer_position += first_size;
	}

	if (k_file_debug_logging)
	{
		log_message(verbose, "c_file_buffered: first read: {i} of {i} requested. file: {s}",
			first_size,
			length,
			m_path.get_full_path());

	}

	if (!eof() && out_bytes_copied < length)
	{
		// read a new chunk into the buffer
		int32 bytes_read_to_buffer = c_file::read_bytes(m_file_position, m_buffer.capacity(), m_buffer);
		m_file_position += bytes_read_to_buffer;
		m_buffer_end = bytes_read_to_buffer;

		if (k_file_debug_logging)
		{
			log_message(verbose, "c_file_buffered: refilled buffer. got {i} of {i} requested bytes file: {s}",
				bytes_read_to_buffer,
				m_buffer.capacity(),
				m_path.get_full_path());
		}

		// if we didn't read a full buffer, it should be because we hit eof
		if (bytes_read_to_buffer < m_buffer.capacity())
		{
			ASSERT(eof());
			if (k_file_debug_logging)
			{
				log_message(verbose, "c_file_buffered: eof. file: {s}", m_path.get_full_path());
			}
		}

		// copy the remaing bytes needed into the output from the beginning of the buffer
		int32 second_size = math_min<int32>(bytes_read_to_buffer, length - out_bytes_copied);
		out_buffer.copy_from_range_offset(m_buffer, 0, second_size, first_size);
		out_bytes_copied += second_size;
		m_buffer_position = second_size;

		if (k_file_debug_logging)
		{
			log_message(verbose, "c_file_buffered: second read: {i} of {i} requested. file: {s}",
				second_size,
				length,
				m_path.get_full_path());
		}
	}

	ASSERT(out_bytes_copied || eof());

	// we may have read more bytes than requested to build up the buffer
	return out_bytes_copied;
}

// todo: this all needs to move to a windows platform impl

char get_path_separator()
{
	return '\\';
}

char get_ext_separator()
{
	return '.';
}

s_file_info get_file_info(const c_file_path& file_path)
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

		// parse datetime info once we have our own internal datetime structs

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
		c_platform_handle_factory::get_native_handle_from_platform_handle(file_handle),
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
		c_platform_handle_factory::get_native_handle_from_platform_handle(file_handle),
		buffer.data(),
		buffer.capacity(),
		&bytes_written,
		overlapped_ptr);

	return bytes_written;
}