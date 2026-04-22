#ifndef __FILE_H__
#define __FILE_H__
#pragma once

#include "structures/string/string.h"
#include "structures/array.h"
#include <platform/platform_handle.h>

const uint32 k_file_path_max = 256;

struct s_file_info
{
	bool exists;
	uint64 size_bytes;
	uint64 creation_time;
	uint64 write_time;
};

class c_file_path
{
public:
	c_file_path() { m_data.clear(); }
	c_file_path(const char* path);
	c_file_path(const t_string_256& path);
	c_file_path(const c_file_path& other);
	c_file_path& operator=(const c_file_path& other);

	~c_file_path() {}

	bool exists() const;
	uint64 get_file_size_bytes() const;
	const char* get_full_path() const { return m_data.get_const_char(); }
	void get_file_name(t_string_256& out_file_name) const;
	void get_file_ext(t_string_256& out_file_ext) const;
	void get_file_name_no_ext(t_string_256& out_file_name) const;
	void get_directory_path(t_string_256& out_directory_path) const;
	void get_directory_name(t_string_256& out_directory_name) const;
private:
	void split_path(uint8& out_parent_directory_index, uint8& out_filename_index, uint8& out_ext_index) const;

	t_string_256 m_data;
};

enum e_file_open_mode
{
	file_open_mode_read,
	file_open_mode_write,
	file_open_mode_append,
	file_open_mode_replace,
	file_open_mode_exclusive,

	k_file_open_mode_count
};
typedef c_flags<k_file_open_mode_count> t_file_open_mode_flags;

class c_file
{
public:
	c_file() : m_file_handle(), m_file_size(0) {}

	virtual bool open(const c_file_path& file_path, t_file_open_mode_flags flags);
	bool close();

	bool is_open() const { return m_file_handle.is_valid(); }

	int32 read_bytes(int32 start, int32 length, c_array<byte>& out_buffer);

	// todo: the default writes should be append (start = k_invalid), make new write_at_position or something for explicit start
	int32 write_bytes(int32 start, const c_array<byte>& buffer);
	int32 write_string(int32 start, const c_array<char>& buffer);

protected:
	c_file_path m_path;
	t_file_open_mode_flags m_flags;
	c_platform_handle m_file_handle;
	uint64 m_file_size;
};

class c_file_buffered : public c_file
{
public:
	c_file_buffered() { reset(); }
	c_file_buffered(c_array<byte>& buffer) : m_buffer(buffer) { reset(); }

	void set_buffer(c_array<byte>& buffer) 
	{ 
		m_buffer = buffer; 
	}

	void set_read_position(int32 read_position)
	{
		ASSERT(read_position < m_file_size);

		// force the next read to refil the buffer
		reset();

		m_file_position = read_position;
	}

	bool open(const c_file_path& file_path, t_file_open_mode_flags flags) override;
	bool close() 
	{
		reset();
		return c_file::close(); 
	}

	int32 read_bytes(int32 length, c_array<byte>& out_buffer);
	int32 read_bytes_unbuffered(int32 start, int32 length, c_array<byte>& out_buffer) 
	{
		return c_file::read_bytes(start, length, out_buffer);
	}

	int32 write_string(int32 start, const c_array<char>& buffer)
	{
		return c_file::write_string(start, buffer);
	}

	bool eof() const { return m_file_position == m_file_size; }

protected:
	void reset()
	{
		m_buffer_position = 0;
		m_buffer_end = 0;
		m_file_position = 0;
	}

	// consider switching this to a stack since m_buffer_end is essentially tracking top
	c_array<byte> m_buffer;
	int32 m_buffer_position;
	int32 m_buffer_end;
	int32 m_file_position;

	// todo: add some tracking metrics for read/write : buffer size ratio
	// that would allow us to evaluate the efficiency of the buffer.
};

template<int32 k_size>
class c_file_static_buffered : public c_file_buffered
{
public:
	c_file_static_buffered() : c_file_buffered()
	{
		// since m_storage is not initialized until after the parent is, we must set the buffer explicitly after
		set_buffer(m_storage);
	}

private:
	c_static_array<byte, k_size> m_storage;
};

char get_path_separator();
char get_ext_separator();
s_file_info get_file_info(const c_file_path& file_path);
bool file_copy(const c_file_path& source, const c_file_path& dest);
#endif // __FILE_H__
