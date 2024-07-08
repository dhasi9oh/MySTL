#ifndef MYSTL_VECTOR_H
#define MYSTL_VECTOR_H

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"
#include "algo.h"

namespace mystl {

#ifdef max
#pragma message("#undefing marco max")
#undef
#endif

#ifdef min
#pragma message("#undefing marco min")
#undef
#endif


	// 对外接口不实现主要逻辑
	// 内置接口来实现具体逻辑


	template <class T>
	class vector {

		static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in mystl");

	public:

		typedef mystl::allocator<T>						allocator_type;
		typedef mystl::allocator<T>						data_allocator;

		typedef typename allocator_type::value_type				value_type;
		typedef typename allocator_type::pointer				pointer;
		typedef typename allocator_type::const_pointer			const_pointer;
		typedef typename allocator_type::reference				reference;
		typedef typename allocator_type::const_reference		const_reference;
		typedef typename allocator_type::size_type				size_type;
		typedef typename allocator_type::difference_type		difference_type;

		typedef value_type* iterator;
		typedef const value_type* const_iterator;
		typedef mystl::reverse_iterator<iterator>	reverse_iterator;
		typedef mystl::reverse_iterator<iterator>	const_reverse_iterator;



	private:
		iterator _begin;
		iterator _end;
		iterator _cap;


	public:

		vector() noexcept
		{
			try_init();
		}

		explicit vector(size_type n)
		{
			fill_init(n, value_type());
		}

		vector(size_type n, const value_type& value)
		{
			fill_init(n, value);
		}

		template <class Iter, typename std::enable_if<
			mystl::is_input_iterator<Iter>::value, int>::type = 0>
		vector(Iter first, Iter last)
		{
			MYSTL_DEBUG(!(last < first));
			range_init(first, last);
		}

		vector(const vector& rhs)
		{
			range_init(rhs._begin, rhs._end);
		}

		vector(vector&& rhs) noexcept :
			_begin(rhs._begin),
			_end(rhs._end),
			_cap(rhs._cap)
		{
			rhs._begin = nullptr;
			rhs._end = nullptr;
			rhs._cap = nullptr;
		}

		//因为初始化列表肯定是手写的，所以肯定不会含有太多资源，采用拷贝

		vector(std::initializer_list<value_type> rhs)
		{
			range_init(rhs.begin(), rhs.end());
		}

		vector& operator=(std::initializer_list<value_type> list)
		{
			vector tmp(list);
			swap(tmp);
			return *this;
		}

		vector& operator=(const vector& rhs)
		{
			if (this != &rhs) {
				const size_type len = rhs.size();
				if (len > capacity()) {
					vector tmp(rhs.begin(), rhs.end());
					this->swap(tmp);
				}
				else if (size() >= len) {
					auto last = mystl::copy(rhs.begin(), rhs.end(), _begin);
					data_allocator::destroy(last, _end);
					_end = _begin + len;
				}
				else {
					mystl::copy(rhs.begin(), rhs.begin() + len, _begin);
					mystl::uninitialized_copy(rhs.begin() + len, rhs.end(), _end);
					_end = _begin + len;
				}
			}
			return *this;
		}

		vector& operator=(vector&& rhs) noexcept
		{
			destroy_and_recover(_begin, _end, capacity());
			_begin = rhs._begin;
			_end = rhs._end;
			_cap = rhs._cap;
			rhs._begin = nullptr;
			rhs._end = nullptr;
			rhs._cap = nullptr;
			return *this;
		}

		~vector()
		{
			destroy_and_recover(_begin, _end, (_cap - _begin));
			_begin = _end = _cap = nullptr;
		}


	public:
		// const 版本是为了 const 变量准备的
		iterator begin() noexcept
		{
			return _begin;
		}

		const_iterator begin() const noexcept
		{
			return _begin;
		}

		iterator end() noexcept
		{
			return _end;
		}

		const_iterator end() const noexcept
		{
			return _end;
		}

		reverse_iterator rbegin() noexcept
		{
			return reverse_iterator(end());
		}

		const_reverse_iterator rbegin() const noexcept
		{
			return const_reverse_iterator(end());
		}

		reverse_iterator rend() noexcept
		{
			return reverse_iterator(begin());
		}

		const_reverse_iterator rend() const noexcept
		{
			return const_reverse_iterator(begin());
		}

		const_iterator cbegin() const noexcept
		{
			return begin();
		}

		const_iterator cend() const noexcept
		{
			return end();
		}

		const_reverse_iterator crbegin() const noexcept
		{
			return rend();
		}

		const_reverse_iterator crend() const noexcept
		{
			return rbegin();
		}

	public:

		bool empty() const noexcept
		{
			return _begin == _end;
		}

		size_type size() const noexcept
		{
			return _end - _begin;
		}

		size_type max_size() const noexcept
		{
			return static_cast<size_type>(-1) / sizeof(T);
		}

		size_type capacity() const noexcept
		{
			return _cap - _begin;
		}

		void reserve(size_type n)
		{
			if (n > capacity()) {
				THROW_LENGTH_ERROR_IF(n > max_size(),
					"n can not larger than max_size() in vector<T>::reserve(n)");
				auto tmp = data_allocator::allocate(n);
				auto len = size();
				mystl::uninitialized_move(_begin, _end, tmp);
				data_allocator::deallocate(_begin, _cap - _begin);
				_begin = tmp;
				_end = _begin + len;
				_cap = _begin + n;
			}
		}

		// cap 缩小到 size
		void shrink_to_fit()
		{
			if (_end < _cap) {
				reinsert(size());
			}
		}


	public:
		//这里用将一个函数分成两个版本的目的是供const变量调用
		//const变量只能调用const函数

		reference operator[](size_type n)
		{
			MYSTL_DEBUG(n < size());
			return *(_begin + n);
		}

		const_reference operator[](size_type n) const
		{
			MYSTL_DEBUG(n < size());
			return *(_begin + n);
		}

		reference at(size_type n)
		{
			THROW_LENGTH_ERROR_IF(!(n < size()), "vector<T>::at() subscript out of range");
			return (*this)[n];
		}

		const_reference at(size_type n) const
		{
			THROW_LENGTH_ERROR_IF(!(n < size()), "vector<T>::at() subscript out of range");
			return (*this)[n];
		}

		reference front()
		{
			MYSTL_DEBUG(!empty());
			return *_begin;
		}

		const_reference front() const
		{
			MYSTL_DEBUG(!empty());
			return *_begin;
		}

		reference back()
		{
			MYSTL_DEBUG(!empty());
			return *(_end - 1);
		}

		const_reference back() const
		{
			MYSTL_DEBUG(!empty());
			return *(_end - 1);
		}

		pointer data() noexcept
		{
			return _begin;
		}

		const_pointer data() const noexcept
		{
			return _begin;
		}

	public:

		void assign(size_type n, const value_type& value)
		{
			fill_assign(n, value);
		}

		template <class Iter, typename std::enable_if<
			mystl::is_input_iterator<Iter>::value, int>::type = 0>
		void assign(Iter first, Iter last)
		{
			MYSTL_DEBUG(!(last < first));
			copy_assign(first, last, mystl::iterator_category(first));
		}

		void assign(std::initializer_list<value_type>& list)
		{
			copy_assign(list.begin(), list.end(), forward_iterator_tag());
		}

		// 这里用const_iterator主要是因为平常调用这个
		// 函数的时候传入的变量一般都是一个临时值
		template <class... Args>
		iterator emplace(const_iterator pos, Args&& ...args)
		{
			MYSTL_DEBUG(begin() <= pos && pos <= end());
			auto xpos = const_cast<iterator>(pos);
			const size_type n = pos - _begin;
			if (_cap != _end && xpos == _end) {
				data_allocator::construct(mystl::address_of(*_end), mystl::forward<Args>(args)...);
				++_end;
			}
			else if (_end != _cap) {
				auto new_pos = _end;
				// 这里构造的原因是因为要先构造以后才能进行拷贝
				data_allocator::construct(mystl::address_of(*new_pos), *(_end - 1));
				new_pos++;
				mystl::copy_backward(xpos, (_end - 1), _end);
				_end = new_pos;
				*xpos = value_type(mystl::forward<Args>(args)...);
			}
			else {
				this->reallocate_emplace(xpos, mystl::forward<Args>(args)...);
			}
			return begin() + n;
		}

		template <class... Args>
		void emplace_back(Args&& ...args)
		{
			emplace(_end, mystl::forward<Args>(args)...);
		}

		void push_back(const value_type& value)
		{
			if (_cap != _end) {
				data_allocator::construct(address_of(*_end), value);
				++_end;
			}
			else {
				reallocate_insert(_end, value);
			}
		}

		void push_back(value_type&& value)
		{
			emplace_back(mystl::move(value));
		}

		void pop_back()
		{
			MYSTL_DEBUG(!empty());
			data_allocator::destroy((_end - 1));
			--_end;
		}

		iterator insert(const_iterator pos, const value_type& value)
		{
			MYSTL_DEBUG(begin() <= pos && pos <= end());
			iterator xpos = const_cast<iterator>(pos);
			const size_type n = pos - _begin;
			if (_cap != _end && xpos == _end) {
				data_allocator::construct(mystl::address_of(*_end), mystl::forward<Args>(args)...);
				++_end;
			}
			else if (_end != _cap) {
				auto new_pos = _end;
				// 这里构造的原因是因为要先构造以后才能进行拷贝
				data_allocator::construct(mystl::address_of(*new_pos), *(_end - 1));
				new_pos++;
				// 避免因为复制操作符改变 value
				auto value_copy = value;
				mystl::copy_backward(xpos, (_end - 1), _end);
				_end = new_pos;
				*xpos = mystl::move(value_copy);
			}
			else {
				this->reallocate_emplace(xpos, mystl::forward<Args>(args)...);
			}
			return begin() + n;
		}

		iterator insert(const_iterator pos, size_type n, const value_type& value)
		{
			MYSTL_DEBUG(pos >= begin() && pos <= end());
			return fill_insert(const_cast<iterator>(pos), value, n);
		}

		iterator insert(const_iterator pos, value_type&& value)
		{
			return emplace(pos, mystl::forward<value_type>(value));
		}

		template <class Iter, typename std::enable_if<
			mystl::is_input_iterator<Iter>::value, int>::type = 0>
		void insert(const_iterator pos, Iter first, Iter last)
		{
			MYSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
			copy_insert(const_cast<iterator>(pos), first, last);
		}

		iterator erase(const_iterator pos)
		{
			MYSTL_DEBUG(begin() <= pos && pos < end());
			// 不能直接赋值主要是因为 pos 是 const
			iterator xpos = _begin + (pos - _begin);
			mystl::move((xpos + 1), _end, xpos);
			--_end;
			data_allocator::destroy(end());
			return xpos;

		}

		iterator erase(const_iterator first, const_iterator last)
		{
			MYSTL_DEBUG(begin() <= first && last <= end() && !(last < first));
			const auto n = first - begin();
			iterator r = begin() + n;
			data_allocator::destroy(mystl::move(r + (last - first), _end, r), _end);
			_end = _end - (last - first);
			return begin() + n;
		}

		void clear()
		{
			erase(begin(), end());
		}

		void resize(size_type n)
		{
			resize(n, value_type());
		}

		void resize(size_type n, const value_type& value)
		{
			if (n < size()) {
				erase(begin() + n, end());
			}
			else {
				insert(end(), n - size(), value);
			}
		}

		void reverse()
		{
			mystl::reverse(begin(), end());
		}

		void swap(vector& rhs) noexcept
		{
			if (&rhs != this) {
				mystl::swap(_begin, rhs._begin);
				mystl::swap(_end, rhs._end);
				mystl::swap(_cap, rhs._cap);
			}
		}

	private:

		// 申请 16 个内存空间的数组
		void try_init() noexcept
		{
			try {
				_begin = data_allocator::allocate(16);
				_end = _begin;
				_cap = _begin + 16;
			}
			catch (...) {
				_begin = _end = _cap = nullptr;
			}
		}

		// 申请 cap 个空间，其中 size 个是用了的
		void init_space(size_type size, size_type cap)
		{
			try {
				_begin = data_allocator::allocate(cap);
				_end = _begin + size;
				_cap = _begin + cap;
			}
			catch (...) {
				_begin = _end = _cap = nullptr;
				throw;
			}
		}

		void fill_init(size_type n, const value_type& value)
		{
			const size_type len = mystl::max(static_cast<size_type>(16), n);
			init_space(n, len);
			mystl::uninitialized_fill_n(begin(), n, value);
		}

		template <class Iter>
		void range_init(Iter first, Iter last)
		{
			size_type len = mystl::distance(first, last);
			const auto n = mystl::max(len, static_cast<size_type>(16));
			init_space(len, n);
			mystl::uninitialized_copy(first, last, begin());
		}

		void destroy_and_recover(iterator first, iterator last, size_type n)
		{
			MYSTL_DEBUG(begin() <= first && last <= end() && !(last < first));
			data_allocator::destroy(first, last);
			data_allocator::deallocate(_begin);
			_begin = _end = _cap = nullptr;
		}

		// 每次扩充的时候添加 add_size 个位置或者扩充成 1.5 倍
		size_type get_new_cap(size_type add_size)
		{
			const size_type old = capacity();
			THROW_LENGTH_ERROR_IF(old + add_size > max_size(), "vector<T>'s size too big");
			// 如果扩充成 1.5 倍太大了，就不选择这个方案了
			if (old > max_size() - old / 2) {
				return  old + add_size > max_size() - 16 ?
					old + add_size : old + add_size + 16;
			}
			return (old == 0 
				? mystl::max(add_size, static_cast<size_type>(0)) 
				: mystl::max(old + old / 2, old + add_size));
		}

		void fill_assign(size_type n, const value_type& value)
		{
			if (n > capacity()) {
				vector tmp(n, value);
				this->swap(tmp);
			}
			else if (n > size()){
				mystl::fill(begin(), end(), value);
				_end = mystl::uninitialized_fill_n(end(), n - size(), value);
			}
			else {
				erase(mystl::fill_n(first, n, value), end());
			}
		}

		template <class Iter>
		void copy_assign(Iter first, Iter last, input_iterator_tag)
		{
			auto cur = begin();
			for (; first != last && cur != end(); first++, cur++) {
				*cur = *first;
			}
			if (cur == end()) {
				insert(first, last, cur);
			}
			if (first == last) {
				erase(cur, end());
			}
		}

		template <class Iter>
		void copy_assign(Iter first, Iter last, forward_iterator_tag)
		{
			const size_type len = mystl::distance(first, last);
			if (len > capacity()) {
				vector tmp(first, last);
				swap(tmp);
			}
			else if (size() >= len) {
				auto new_end = mystl::copy(first, last, begin());
				data_allocator::destroy(new_end, end());
				_end = new_end;
			}
			else {
				auto mid = first;
				mystl::advance(mid, size());
				auto new_end = mystl::copy(first, mid, begin());
				mystl::uninitialized_copy(mid, last, new_end);
				_end = new_end;

			}
		}

		template <class ...Args>
		void reallocate_emplace(iterator pos, Args&& ...args)
		{
			const auto new_size = get_new_cap(1);
			auto new_begin = data_allocator::allocate(new_size);
			auto new_end = new_begin;
			try {
				new_end = mystl::uninitialized_move(begin(), pos, new_begin);
				data_allocator::construct(mystl::address_of(*pos), mystl::forward<Args>(args)...);
				new_end++;
				new_end = mystl::uninitialized_move(pos, end(), new_end);
			}
			catch (...) {
				data_allocator::deallocate(new_begin, new_size);
				throw;
			}
			destroy_and_recover(begin(), end(), capacity());
			_begin = new_begin;
			_end = new_end;
			_cap = _begin + new_size;
		}

		void reallocate_insert(iterator pos, const value_type& value)
		{
			const auto new_size = get_new_cap(1);
			auto new_begin = data_allocator::allocate(new_size);
			auto new_end = new_begin;
			try {
				new_end = mystl::uninitialized_move(begin(), pos, new_begin);
				data_allocator::construct(mystl::address_of(*pos), value);
				new_end++;
				new_end = mystl::uninitialized_move(pos, end(), new_end);
			}
			catch (...) {
				data_allocator::deallocate(new_begin, new_size);
				throw;
			}
			destroy_and_recover(begin(), end(), capacity());
			_begin = new_begin;
			_end = new_end;
			_cap = _begin + new_size;
		}

		iterator fill_insert(iterator pos, const value_type& value, size_type n)
		{
			if (n == 0) {
				return pos;
			}
			if ((capacity() - end()) >= n) {
				const auto len = end() - pos;
				auto old_end = end();
				if (len > n) {
					mystl::uninitialized_copy(end() - n, end(), end());
					_end += n;
					mystl::move_backward(pos, old_end - n, old_end);
					mystl::fill_n(pos, n, value);
				}
				else {
					_end = mystl::uninitialized_fill_n(end(), n, value);
					_end = mystl::uninitialized_move(pos, old_end, end());
					mystl::fill_n(pos, n, value);
				}
			}
			else {
				const auto new_size = get_new_cap(n);
				auto new_begin = data_allocator::allocate(new_size);
				auto new_end = new_begin;
				const auto len = pos - begin();
				try {
					new_end = mystl::uninitialized_move(begin(), pos, new_begin);
					new_end = mystl::uninitialized_fill_n(new_begin + len, n, value);
					new_end = mystl::uninitialized_move(pos, end(), new_begin + len + n);
				}
				catch (...) {
					data_allocator::deallocate(new_begin, new_size);
					new_begin = new_end = nullptr;
					throw;
				}
				destroy_and_recover(begin(), end(), capacity());
				_begin = new_begin;
				_end = new_end;
				_cap = _begin + new_size;
				new_begin = new_end = nullptr;
			}
		}

		template <class Iter>
		void copy_insert(iterator pos, Iter first, Iter last)
		{
			const auto n = distance(first, last);
			if (n == 0) {
				return pos;
			}
			if ((capacity() - end()) >= n) {
				const auto len = end() - pos;
				auto old_end = end();
				if (len > n) {
					_end = mystl::uninitialized_copy(end() - n, end(), end());
					mystl::move_backward(pos, old_end - n, old_end);
					mystl::uninitialized_copy(first, last, pos);
				}
				else {
					_end = mystl::uninitialized_fill_n(end(), n, value);
					_end = mystl::uninitialized_move(pos, old_end, end());

				}
			}
			else {
				const auto new_size = get_new_cap(n);
				auto new_begin = data_allocator::allocate(new_size);
				auto new_end = new_begin;
				try {
					new_end = mystl::uninitialized_move(begin(), pos, new_begin);
					new_end = mystl::uninitialized_copy(first, last, new_end);
					new_end = mystl::uninitialized_move(pos, end(), new_begin);
				}
				catch (...) {
					data_allocator::deallocate(new_begin, new_size);
					new_begin = new_end = nullptr;
					throw;
				}
				destroy_and_recover(begin(), end(), capacity());
				_begin = new_begin;
				_end = new_end;
				_cap = _begin + new_size;
				new_begin = new_end = nullptr;
			}
		}

		void reinsert(size_type n)
		{
			const auto len = get_new_cap(n);
			auto new_begin = data_allocator::allocate(len);
			auto new_end = new_begin;
			try {
				new_end = mystl::uninitialized_move(begin(), end(), new_begin);
			}
			catch (...) {
				data_allocator::deallocate(new_begin, len);
				new_begin = new_end = nullptr;
				throw;
			}
			data_allocator::deallocate(begin(), capacity());
			_begin = new_begin;
			_end = new_end;
			_cap = _begin + len;
			new_begin = new_end = nullptr;
		}

	};


	template <class T>
	bool operator==(const vector<T>& lhs, const vector<T>& rhs)
	{
		return lhs.size() == rhs.size() &&
			mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template <class T>
	bool operator<(const vector<T>& lhs, const vector<T>& rhs)
	{
		return mystl::lexicographical_compare(lhs.begin(), lhs.end(),
			rhs.begin(), rhs.end());
	}

	template <class T>
	bool operator!=(const vector<T>& lhs, const vector<T>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class T>
	bool operator>(const vector<T>& lhs, const vector<T>& rhs)
	{
		return rhs < lhs;
	}

	template <class T>
	bool operator<=(const vector<T>& lhs, const vector<T>& rhs)
	{
		return (lhs < rhs) || (lhs == rhs);
	}

	template <class T>
	bool operator>=(const vector<T>& lhs, const vector<T>& rhs)
	{
		return !(lhs < rhs);
	}

	template <class T>
	bool swap(vector<T>& lhs, vector<T>& rhs)
	{
		lhs.swap(rhs);
	}

}


#endif // !MYSTL_VECTOR_H
