#ifndef MYSTL_STACK_H
#define MYSTL_STACK_H

#include "deque.h"

namespace mystl {

	template <class T, class Container = deque<T>>
	class stack
	{
	public:

		using container_type = Container;
		using value_type = typename container_type::value_type;
		using size_type = typename container_type::size_type;
		using reference = typename container_type::reference;
		using const_reference = typename container_type::const_reference;

		static_assert(std::is_same<T, value_type>::value, "");

	private:

		container_type c;

	public:

		stack() = default;

		explicit stack(size_type n) : c(n)
		{

		}

		stack(size_type n, const value_type& value) : c(n, value)
		{

		}

		template <class Iter>
		stack(Iter first, Iter last) : c(first, last)
		{

		}

		stack(std::initializer_list<T> ilist) : c(ilist.begin(), ilist.end())
		{

		}

		stack(const stack& rhs) : c(rhs.c)
		{

		}

		stack(stack&& rhs) : c(mystl::move(rhs.c))
		{

		}

		stack& operator=(stack&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value)
		{
			c = mystl::move(rhs.c);
			return *this;
		}

		stack& operator=(const stack& rhs)
		{
			c = rhs.c;
			return *this;
		}

		stack& operator=(std::initializer_list<T> ilist)
		{
			c = ilist;
			return *this;
		}

		~stack() = default;

		reference top()
		{
			return c.back();
		}

		const_reference top() const
		{
			return c.back();
		}

		bool empty() const noexcept
		{
			return c.empty();
		}

		size_type size() const noexcept
		{
			return c.size();
		}

		template <class ...Args>
		void emplace(Args&& ...args)
		{
			c.emplace(forward<Args>(args));
		}

		void push(const value_type& value)
		{
			c.push_back(value);
		}

		void push(value_type&& value)
		{
			c.push_back(mystl::move(value));
		}

		void pop()
		{
			c.pop_back();
		}

		void clear()
		{
			while (!empty()) {
				pop();
			}
		}

		void swap(stack& rhs) noexcept(noexcept(mystl::swap(c, rhs.c)))
		{
			mystl::swap(c, rhs.c);
		}

	public:

		friend bool operator==(const stack& lhs, const stack& rhs) { return lhs.c == rhs.c; }
		friend bool operator<(const stack& lhs, const stack& rhs) { return lhs.c < rhs.c; }

	};

	template <class T, class Container>
	bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return lhs == rhs;
	}

	template <class T, class Container>
	bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class T, class Container>
	bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return (lhs < rhs);
	}

	template <class T, class Container>
	bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return (rhs < lhs);
	}

	template <class T, class Container>
	bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return !(lhs > rhs);
	}

	template <class T, class Container>
	bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return !(lhs < rhs);
	}

	template <class T, class Container>
	void swap(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		lhs.swap(rhs);
	}


}




#endif // !MYSTL_STACK_H
