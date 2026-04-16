
#ifdef USING_STD_ATOMIC

COMPILE_ASSERT(static_cast<std::memory_order>(atomic_memory_order_relaxed) == std::memory_order_relaxed);
COMPILE_ASSERT(static_cast<std::memory_order>(atomic_memory_order_consume) == std::memory_order_consume);
COMPILE_ASSERT(static_cast<std::memory_order>(atomic_memory_order_acquire) == std::memory_order_acquire);
COMPILE_ASSERT(static_cast<std::memory_order>(atomic_memory_order_release) == std::memory_order_release);
COMPILE_ASSERT(static_cast<std::memory_order>(atomic_memory_order_aquire_release) == std::memory_order_acq_rel);
COMPILE_ASSERT(static_cast<std::memory_order>(atomic_memory_order_sequential) == std::memory_order_seq_cst);

template<typename t_type>
t_type c_atomic<t_type>::load(e_atomic_memory_order order)
{
	return m_data.load(static_cast<std::memory_order>(order));
}

template<typename t_type>
void c_atomic<t_type>::store(t_type value, e_atomic_memory_order order)
{
	m_data.store(value, static_cast<std::memory_order>(order));
}

template<typename t_type>
t_type c_atomic<t_type>::exchange(t_type value, e_atomic_memory_order order)
{
	return m_data.exchange(value, static_cast<std::memory_order>(order));
}

template<typename t_type>
bool c_atomic<t_type>::compare_exchange(t_type& expected, t_type desired, e_atomic_memory_order order)
{
	return m_data.compare_exchange_strong(expected, desired, static_cast<std::memory_order>(order));
}

#else // USING_STD_ATOMIC

// based on std::atomic but doesn't respect memory order input. for now everything behaves as tho
// the ordering is atomic_memory_order_sequential. maybe someday we can improve that.

template<typename t_type>
t_type c_atomic<t_type>::load(e_atomic_memory_order order)
{
	t_type out;
	
	if constexpr (sizeof(t_type) == 4)
	{
		out = atomic_compare_exchange_32(&m_data, 0, 0);
	}
	else if constexpr (sizeof(t_type) == 8)
	{
		out = atomic_compare_exchange_64(&m_data, 0, 0);
	}
	else
	{
		HALT_UNIMPLEMENTED();
	}

	return out;
}

template<typename t_type>
void c_atomic<t_type>::store(t_type value, e_atomic_memory_order order)
{
	if constexpr (sizeof(t_type) == 4)
	{
		atomic_exchange_32(&m_data, value);
	}
	else if constexpr (sizeof(t_type) == 8)
	{
		atomic_exchange_64(&m_data, value);
	}
	else
	{
		HALT_UNIMPLEMENTED();
	}
}

template<typename t_type>
t_type c_atomic<t_type>::exchange(t_type value, e_atomic_memory_order order)
{
	t_type out;
	if constexpr (sizeof(t_type) == 4)
	{
		out = atomic_exchange_32(&m_data, value);
	}
	else if constexpr (sizeof(t_type) == 8)
	{
		out = atomic_exchange_64(&m_data, value);
	}
	else
	{
		HALT_UNIMPLEMENTED();
	}

	return out;
}

template<typename t_type>
bool c_atomic<t_type>::compare_exchange(t_type& expected, t_type desired, e_atomic_memory_order order)
{
	t_type old;
	if constexpr (sizeof(t_type) == 4)
	{
		old = atomic_compare_exchange_32(&m_data, desired, expected);
	}
	else if constexpr (sizeof(t_type) == 8)
	{
		old = atomic_compare_exchange_64(&m_data, desired, expected);
	}
	else
	{
		HALT_UNIMPLEMENTED();
	}
	
	if (old == expected)
	{
		return true;
	}

	expected = old;

	return false;
}
#endif // USING_STD_ATOMIC 