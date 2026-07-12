template<typename t_type>
void quick_sort(c_array<t_type> array, f_comparitor comparitor, int32 left, int32 right)
{
	if (left + 1 >= right) return;
	if (array.capacity() < 2) return;

	int32 pivot = right - 1;
	{
		int32 partition_left = left;
		int32 partition_right = right;

		while (true)
		{
			while (comparitor(array.get_item_const(partition_left), array.get_item_const(pivot)))
			{
				partition_left++;
			}

			while (partition_left < partition_right && !comparitor(array.get_item_const(partition_right - 1), array.get_item_const(pivot)))
			{
				partition_right--;
			}

			if (partition_left >= partition_right)
			{
				break;
			}

			memory_swap(array.get_item(partition_left), array.get_item(partition_right - 1));
		}

		memory_swap(array.get_item(partition_left), array.get_item(pivot));
		pivot = partition_left;
	}

	// sort the smaller partition first to ensure the deeper recursive call is tail recursive.
	if (pivot - left < right - pivot) 
	{
		quick_sort(array, comparitor, left, pivot);
		quick_sort(array, comparitor, pivot + 1, right);
	}
	else
	{
		quick_sort(array, comparitor, pivot + 1, right);
		quick_sort(array, comparitor, left, pivot);
	}
	
}