#ifndef __FILE_PATH_H__
#define __FILE_PATH_H__
#pragma once

#include "types/types.h"
#include "structures/string/string.h"

class c_file_path
{
public:
	c_file_path() { m_data.clear(); }
	c_file_path(const char* path);
	c_file_path(const t_string_256& path);
	c_file_path(const c_file_path& other);
	c_file_path& operator=(const c_file_path& other);

	~c_file_path() {}

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

//#include "file/file_path.inl"
#endif // !__FILE_PATH_H__