#include "file_path.h"

const char k_extension_separator = '.';
const char k_path_separator = '\\';

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
}

c_file_path& c_file_path::operator=(const c_file_path& other)
{
	m_data.copy_from(other.m_data);
	return *this;
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
	out_file_name.terminate();
}

void c_file_path::get_directory_path(t_string_256& out_directory_path) const
{
	out_directory_path.clear();

	uint8 directory_name_index;
	uint8 file_name_index;
	uint8 ext_index;
	split_path(directory_name_index, file_name_index, ext_index);

	out_directory_path.copy_from_range(m_data, 0, file_name_index - 1);
	out_directory_path.terminate();
}

void c_file_path::get_directory_name(t_string_256& out_directory_name) const
{
	out_directory_name.clear();

	uint8 directory_name_index;
	uint8 file_name_index;
	uint8 ext_index;
	split_path(directory_name_index, file_name_index, ext_index);

	out_directory_name.copy_from_range(m_data, directory_name_index, file_name_index - 1);
	out_directory_name.terminate();
}

void c_file_path::split_path(uint8& out_directory_name_index, uint8& out_filename_index, uint8& out_ext_index) const
{
	out_directory_name_index = k_invalid;
	out_filename_index = k_invalid;
	out_ext_index = k_invalid;

	for (int8 i = int32_to_uint8(m_data.used()) - 1; i >= 0; i--)
	{
		char ch = m_data[i];
		if (out_ext_index == k_invalid && ch == k_extension_separator)
		{
			out_ext_index = i + 1;
		}
		else if (out_filename_index == k_invalid && ch == k_path_separator)
		{
			out_filename_index = i + 1;
		}
		else if (out_directory_name_index == k_invalid &&
			out_filename_index != k_invalid &&
			ch == k_path_separator)
		{
			out_directory_name_index = i + 1;
			break;
		}
	}

	ASSERT(out_directory_name_index != k_invalid);
	ASSERT(out_filename_index != k_invalid);
	ASSERT(out_ext_index != k_invalid);
}