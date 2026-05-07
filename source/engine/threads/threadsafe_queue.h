#include "structures/array.h"
#include "threads/atomic.h"

template<typename t_type>
class c_spsc_queue
{
public:
	c_spsc_queue() : m_data()
	{
		clear();
	}

	c_spsc_queue(c_array<t_type> data) : m_data(data)
	{
		clear();
	}

	bool push_back(const t_type& value)
	{
		int64 back = m_back.load(atomic_memory_order_relaxed);
		int64 next = (back + 1) % m_data.capacity();

		if (next == m_front.load(atomic_memory_order_acquire))
		{
			// full
			return false;
		}

		m_data[back] = value;
		m_back.store(next, atomic_memory_order_release);

		return true;
	}

	bool pop_front(t_type& out_value)
	{
		int64 front = m_front.load(atomic_memory_order_relaxed);

		if (front == m_back.load(atomic_memory_order_acquire))
		{
			// empty
			return false;
		}

		out_value = m_data[front];

		int64 next = (front + 1) % m_data.capacity();
		m_front.store(next, atomic_memory_order_release);
		return true;
	}

	bool empty()
	{
		return m_front.load(atomic_memory_order_acquire) == m_back.load(atomic_memory_order_acquire);
	}

	bool full()
	{
		int64 back = m_back.load(atomic_memory_order_relaxed);
		int64 next = (back + 1) % m_data.capacity();

		return next == m_front.load(atomic_memory_order_acquire);
	}

	void clear()
	{
		m_front.store(0);
		m_back.store(0);
	}

protected:
	c_array<t_type> m_data;
	alignas(64) c_atomic<int32> m_front;
	alignas(64) c_atomic<int64> m_back;
};

template<typename t_type, int32 k_size>
class c_static_spsc_queue : public c_spsc_queue<t_type>
{
public:
	c_static_spsc_queue() 
	{
		this->m_data = m_storage;
	}

private:
	c_static_array<t_type, k_size> m_storage;
};