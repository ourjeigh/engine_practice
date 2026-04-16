#include "pch.h"
#include "events/delegates.h"

using t_delegate_no_args = void();
using t_delegate_takes_int = void(int);
using t_delegate_takes_int_and_bool = void(int, bool);

class c_delegate_caller_no_args
{
public:
	void register_callback(c_delegate<t_delegate_no_args> delegate)
	{
		m_delegate = delegate;
	}

	void trigger()
	{
		m_delegate();
	}

private:
	c_delegate<t_delegate_no_args> m_delegate;
};

bool g_handled;

class c_delegate_handler_no_args
{
public:
	void handle_delegate()
	{
		g_handled = true;
	}
};


class c_delegate_caller_int_arg
{
public:
	void register_callback(c_delegate<t_delegate_takes_int> delegate)
	{
		m_delegate = delegate;
	}

	void trigger(int arg)
	{
		m_delegate(arg);
	}

private:
	c_delegate<t_delegate_takes_int> m_delegate;
};

int g_handled_int;

class c_delegate_handler_int_arg
{
public:
	void handle_delegate(int arg)
	{
		g_handled_int = arg;
	}
};


static void static_delegate_test_no_args()
{
	g_handled = true;
}
TEST(DELEGATES, DELEGATE_MEMBER_NO_ARGS)
{
	g_handled = false;

	c_delegate_handler_no_args handler;
	c_delegate_caller_no_args caller;

	//caller.register_callback(c_delegate<t_delegate_no_args>::bind<c_delegate_handler_no_args, &c_delegate_handler_no_args::handle_delegate>(&handler));
	caller.register_callback(
		MAKE_DELEGATE(
			t_delegate_no_args,
			handler,
			handle_delegate));

	caller.trigger();

	EXPECT_TRUE(g_handled);
}

TEST(DELEGATES, DELEGATE_STATIC_NO_ARGS)
{
	g_handled = false;

	//c_delegate<t_delegate_no_args> delegate = c_delegate<t_delegate_no_args>::bind<&static_delegate_test_no_args>();
	auto delegate = MAKE_DELEGATE_STATIC(
		t_delegate_no_args,
		static_delegate_test_no_args);

	delegate();

	EXPECT_TRUE(g_handled);
}

TEST(DELEGATES, DELEGATE_LAMBDA)
{
	static bool handled = false;

	//c_delegate<t_delegate_no_args> delegate = c_delegate<t_delegate_no_args>::bind <[](){ handled = true;}>();
	auto delegate = MAKE_DELEGATE_STATIC(
		t_delegate_no_args,
		[]() { handled = true;});

	delegate();

	EXPECT_TRUE(g_handled);
}

TEST(DELEGATES, DELEGATE_MEMBER_INT_ARG)
{
	g_handled_int = 0;

	c_delegate_handler_int_arg handler;
	c_delegate_caller_int_arg caller;

	caller.register_callback(
		MAKE_DELEGATE(
			t_delegate_takes_int,
			handler,
			handle_delegate));

	caller.trigger(6);

	EXPECT_EQ(g_handled_int, 6);
}

TEST(DELEGATES, DELEGATE_INT_ARG)
{
	static int result = 0;

	//c_delegate<t_delegate_takes_int> delegate = c_delegate<t_delegate_takes_int>::bind < [](int arg) { result = arg;} > ();

	auto delegate = MAKE_DELEGATE_STATIC(
		t_delegate_takes_int,
		[](int arg) { result = arg;});

	delegate(6);

	EXPECT_EQ(result, 6);
}