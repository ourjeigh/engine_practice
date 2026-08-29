#include "file.h"
#include "debug/logging.h"
#include "platform/platform.h"
#include "platform/platform_file.h"
#include "platform/platform_handle.h"
#include "file/file_path.h"

const bool k_file_debug_logging = false;

bool c_file::open(const c_file_path& file_path, t_file_open_mode_flags flags)
{
	ASSERT(!is_open());

	bool result = false;

	m_file_handle = platform_file_open(file_path, flags);

	if (m_file_handle.is_valid())
	{
		result = true;
		m_flags = flags;
		m_file_size = get_file_info(file_path).size_bytes;
		m_path = file_path;
	}
	else
	{
		m_file_handle.invalidate();
	}

	return result;
}

bool c_file::close()
{
	bool result = false;
	if (m_file_handle.is_valid())
	{
		platform_file_close(m_file_handle);
		m_file_handle.invalidate();
		result = true;
	}

	return result;
}

int32 c_file::read_bytes(int32 start, int32 length, c_array<byte> out_buffer)
{
	ASSERT(is_open());

	return platform_file_read_bytes(
		m_file_handle,
		m_flags,
		start,
		length,
		out_buffer);
}

int32 c_file::read_memory(int32 start, int32 length, void* out_memory)
{
	c_array<byte> buffer(reinterpret_cast<byte*>(out_memory), length);

	return read_bytes(start, length, buffer);
}


int32 c_file::write_bytes(int32 start, const c_array<const byte>& buffer)
{
	ASSERT(is_open());
	ASSERT(m_flags.test(file_open_mode_write));

	return platform_file_write_bytes(m_file_handle, m_flags, start, buffer);
}

int32 c_file::write_string(int32 start, const c_array<const char>& buffer)
{
	return platform_file_write_string(m_file_handle, m_flags, start, buffer);
}

int32 c_file::write_memory(int32 start, const void* memory, int32 size)
{
	c_array<const byte> buffer(reinterpret_cast<const byte*>(memory), size);
	return write_bytes(start, buffer);
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

int32 c_file_buffered::read_bytes(int32 length, c_array<byte> out_buffer)
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

s_file_info get_file_info(const c_file_path& file_path)
{
	return platform_file_get_info(file_path);
}

bool file_exists(const c_file_path& file_path)
{
	return get_file_info(file_path).exists;
}

bool file_copy(const c_file_path& source, const c_file_path& dest, bool overwrite)
{
	return platform_file_copy(source, dest, overwrite);
}
