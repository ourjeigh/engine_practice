#ifndef __DELEGATES_H__
#define __DELEGATES_H__
#pragma once

#include <platform/platform.h>

#define MAKE_DELEGATE(signature, object, method) \
	c_delegate<signature>::bind<decltype(object), &decltype(object)::method>(&object)

#define MAKE_DELEGATE_STATIC(signature, method) \
	c_delegate<signature>::bind<method>()

template<typename>
class c_delegate;

template<typename t_return, typename... t_args>
class c_delegate<t_return(t_args...)>
{
public:
	using t_thunk = t_return(*)(void*, t_args...);

	c_delegate() : m_object(nullptr), m_thunk(nullptr) {}

	// instanced member function
	template<class t_class, t_return(t_class::*t_method)(t_args...)>
	static c_delegate bind(t_class* object)
	{
		return c_delegate(object, &invoke<t_class, t_method>);
	}

	// const memeber not implemented
	/*template<class t_class, t_return(t_class::* t_method)(t_args...)>
	static c_delegate bind(t_class* object) = delete;*/

	template<t_return (*t_function)(t_args...)>
	static c_delegate bind()
	{
		return c_delegate(nullptr, &invoke_static<t_function>);
	}

	t_return operator()(t_args... args) const
	{
		return m_thunk(m_object, args...);
	}

	//explicit operator bool() const
	//{
	//	return m_thunk != nullptr;
	//}

	bool is_valid()
	{
		return m_thunk != nullptr;
	}

private:
	c_delegate(void* object, t_thunk thunk) : m_object(object), m_thunk(thunk) {}

	template<class t_class, t_return(t_class::*t_method)(t_args...)>
	static t_return invoke(void* object, t_args... args)
	{
		return (static_cast<t_class*>(object)->*t_method)(args...);
	}

	template<t_return(*t_function)(t_args...)>
	static t_return invoke_static(void*, t_args... args)
	{
		return (*t_function)(args...);
	}

	void* m_object;
	t_thunk m_thunk;
};

#endif //__DELEGATES_H__
