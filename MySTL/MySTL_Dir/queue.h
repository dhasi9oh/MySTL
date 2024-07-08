#ifndef MYSTL_QUEUE_H
#define MYSTL_QUEUE_H


#include "deque.h"
#include "functional.h"
#include "heap_algo.h"
#include "vector.h"

namespace mystl {

	template <class T, class Container = deque<T>>
	class queue
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

		queue() = default;

		explicit queue(size_type n) : c(n)
		{

		}

		queue(size_type n, const value_type& value) : c(n, value)
		{

		}

		template <class Iter>
		queue(Iter first, Iter last) : c(first, last)
		{

		}

		queue(std::initializer_list<T> ilist) : c(ilist.begin(), ilist.end())
		{

		}

		queue(const queue& rhs) : c(rhs.c)
		{

		}

		queue(queue&& rhs) : c(mystl::move(rhs.c))
		{

		}

		queue& operator=(queue&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value)
		{
			c = mystl::move(rhs.c);
			return *this;
		}

		queue& operator=(const queue& rhs)
		{
			c = rhs.c;
			return *this;
		}

		queue& operator=(std::initializer_list<T> ilist)
		{
			c = ilist;
			return *this;
		}

		~queue() = default;

		reference front()
		{
			return c.back();
		}

		const_reference front() const
		{
			return c.back();
		}

		reference back()
		{
			return c.back();
		}

		const_reference back() const
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
			c.pop_front();
		}

		void clear()
		{
			while (!empty()) {
				pop();
			}
		}

		void swap(queue& rhs) noexcept(noexcept(mystl::swap(c, rhs.c)))
		{
			mystl::swap(c, rhs.c);
		}

	public:

		friend bool operator==(const queue& lhs, const queue& rhs) { return lhs.c == rhs.c; }
		friend bool operator<(const queue& lhs, const queue& rhs) { return lhs.c < rhs.c; }


	};

	template <class T, class Container>
	bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return lhs == rhs;
	}

	template <class T, class Container>
	bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class T, class Container>
	bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return (lhs < rhs);
	}

	template <class T, class Container>
	bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return (rhs < lhs);
	}

	template <class T, class Container>
	bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return !(lhs > rhs);
	}

	template <class T, class Container>
	bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return !(lhs < rhs);
	}

	template <class T, class Container>
	void swap(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		lhs.swap(rhs);
	}


	template <class T, class Container = vector<T>,
	class Compared = less<typename Container::value_type>>
	class priority_queue
	{
	public:

		using container_type = Container;
		using value_compare = Compared;
		using value_type = typename container_type::value_type;
		using size_type = typename container_type::size_type;
		using reference = typename container_type::reference;
		using const_reference = typename container_type::const_reference;

		static_assert(std::is_same<T, value_type>::value, "");

	private:

		container_type c;
		value_compare cmp;

	public:

		priority_queue() = default;

		priority_queue(const value_compare& d) : cmp(d)
		{

		}

		priority_queue(const container_type& d) : cmp(d)
		{
			make_heap(c.begin(), c.end(), cmp);
		}

		priority_queue(container_type&& d) : cmp(d)
		{
			make_heap(c.begin(), c.end(), cmp);
		}

		explicit priority_queue(size_type n) : c(n)
		{
			make_heap(c.begin(), c.end(), cmp);
		}

		priority_queue(size_type n, const value_type& value) : c(n, value)
		{
			make_heap(c.begin(), c.end(), cmp);
		}

		template <class Iter>
		priority_queue(Iter first, Iter last) : c(first, last)
		{
			make_heap(c.begin(), c.end(), cmp);
		}

		priority_queue(std::initializer_list<T> ilist) : c(ilist.begin(), ilist.end())
		{
			make_heap(c.begin(), c.end(), cmp);
		}

		priority_queue(const priority_queue& rhs) : c(rhs.c), cmp(rhs.cmp)
		{
			make_heap(c.begin(), c.end(), cmp);
		}

		priority_queue(priority_queue&& rhs) : c(mystl::move(rhs.c)), cmp(rhs.cmp)
		{
			make_heap(c.begin(), c.end(), cmp);
		}

		priority_queue& operator=(priority_queue&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value)
		{
			c = mystl::move(rhs.c);
			cmp = rhs.cmp;
			make_heap(c.begin(), c.end(), cmp);
			return *this;
		}

		priority_queue& operator=(const priority_queue& rhs)
		{
			c = rhs.c;
			cmp = rhs.cmp;
			make_heap(c.begin(), c.end(), cmp);
			return *this;
		}

		priority_queue& operator=(std::initializer_list<T> ilist)
		{
			c = ilist;
			make_heap(c.begin(), c.end(), cmp);
			return *this;
		}

		~priority_queue() = default;

		const_reference top() const
		{
			return c.front();
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
			make_heap(c.begin(), c.end(), cmp);
		}

		void push(const value_type& value)
		{
			c.push_back(value);
			make_heap(c.begin(), c.end(), cmp);
		}

		void push(value_type&& value)
		{
			c.push_back(mystl::move(value));
			make_heap(c.begin(), c.end(), cmp);
		}

		void pop()
		{
			pop_heap(c.begin(), c.end(), cmp);
			c.pop_back();
		}

		void clear()
		{
			while (!empty()) {
				pop();
			}
		}

		void swap(priority_queue& rhs) noexcept(noexcept(mystl::swap(c, rhs.c)))
		{
			mystl::swap(c, rhs.c);
		}

	public:

		friend bool operator==(const priority_queue& lhs, const priority_queue& rhs) { return lhs.c == rhs.c; }
		friend bool operator!=(const priority_queue& lhs, const priority_queue& rhs) { return lhs.c != rhs.c; }


	};

	template <class T, class Container>
	bool operator==(const priority_queue<T, Container>& lhs, const priority_queue<T, Container>& rhs)
	{
		return lhs == rhs;
	}

	template <class T, class Container>
	bool operator!=(const priority_queue<T, Container>& lhs, const priority_queue<T, Container>& rhs)
	{
		return (lhs != rhs);
	}

	template <class T, class Container>
	void swap(const priority_queue<T, Container>& lhs, const priority_queue<T, Container>& rhs)
	{
		lhs.swap(rhs);	
	}

}



#endif // !MYSTL_QUEUE_H
