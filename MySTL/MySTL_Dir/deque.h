#ifndef MYSTL_DEQUE_H
#define MYSTL_DEQUE_H

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"


namespace {

#ifdef max
#pragma message("#undefing marco max")
#undef
#endif

#ifdef min
#pragma message("#undefing marco min")
#undef
#endif


#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif


	template <class T>
	struct	deque_buf_size
	{
		static const size_t value = (sizeof(T) < 256) ? 4096 / sizeof(T) : 16;
	};


	template <class T, class Ref = T&, class Ptr = T*>
	struct deque_iterator
	{
		typedef deque_iterator<T, T&, T*>				iterator;
		typedef deque_iterator<T, const T&, const T*>	const_iterator;
		typedef deque_iterator							self;

		typedef T			value_type;
		typedef Ptr			pointer;
		typedef Ref			reference;
		typedef size_t		size_type;
		typedef ptrdiff_t	difference_type;
		typedef T* value_pointer;
		typedef T** map_pointer;

		static const size_type buf_size = deque_buf_size<value_type>::value;


		value_pointer*	m_begin;
		value_pointer*	m_end;
		value_pointer*	m_cur;
		map_pointer		m_node;


		deque_iterator() noexcept :
			m_begin(nullptr),
			m_end(nullptr),
			m_cur(nullptr),
			m_node(nullptr)
		{
		}

		deque_iterator(value_pointer v, map_pointer n) :
			m_begin(*n),
			m_end(*n + buf_size),
			m_cur(v),
			m_node(n) {}

		deque_iterator(iterator&& rhs) noexcept :
			m_begin(rhs.m_begin),
			m_end(rhs.m_end),
			m_cur(rhs.m_cur),
			m_node(rhs.m_node)
		{
			rhs.m_begin = nullptr;
			rhs.m_end = nullptr;
			rhs.m_cur = nullptr;
			rhs.m_node = nullptr;
		}

		deque_iterator(const iterator& rhs) :
			m_begin(rhs.m_begin),
			m_end(rhs.m_end),
			m_cur(rhs.m_cur),
			m_node(rhs.m_node)
		{

		}

		deque_iterator(const const_iterator& rhs) :
			m_begin(rhs.m_begin),
			m_end(rhs.m_end),
			m_cur(rhs.m_cur),
			m_node(rhs.m_node)
		{

		}

		self& operator=(const iterator& rhs)
		{
			if (&rhs != this) {
				m_begin = rhs.m_begin;
				m_end = rhs.m_end;
				m_cur = rhs.m_cur;
				m_node = rhs.m_node;
			}
			return *this;
		}

		void set_node(map_pointer node)
		{
			m_begin = *node;
			m_end = m_begin + buf_size;
			m_node = node;
		}

		reference operator*()
		{
			return *cur;
		}

		pointer operator->()
		{
			return cur;
		}

		difference_type operator-(const self& rhs) const noexcept
		{
			return static_cast<difference_type>(buf_size) * (rhs.m_node - m_node)
				+ (m_cur - m_begin) - (rhs.m_cur - rhs.m_begin);
		}

		self& operator++()
		{
			++cur;
			if (cur == m_end) {
				set_node(m_node + 1);
				m_cur = m_begin;
			}
			return *this;
		}

		self operator++(int)
		{
			self tmp = *this;
			++(*this);
			return tmp;
		}

		self& operator--()
		{
			if (m_begin == m_cur) {
				set_node(node - 1);
				m_cur = m_end;
			}
			m_cur--;
			return *this;
		}

		self operator--(int)
		{
			self tmp = *this;
			--(*this);
			return tmp;
		}

		self& operator+=(difference_type n)
		{
			const difference_type offset = n + (cur - first);
			if (offset >= 0 && static_cast<difference_type>(buf_size) > offset) {
				cur += n;
			}
			else {
				const difference_type node_offset = offset > 0
					? offset / static_cast<difference_type> (buf_size)
					: -static_cast<difference_type> ((-offset - 1) / buf_size) - 1;
				set_node(m_node + node_offset);
				cur = first + (offset - node_offset * static_cast<difference_type>(buf_size));
			}
			return *this;
		}

		self operator+(difference_type n)
		{
			self tmp = *this;
			tmp += n;
			return tmp;
		}

		self& operator-=(difference_type n)
		{
			(*this) += -n;
			return *this;
		}

		self operator-(difference_type n)
		{
			self tmp = *this;
			tmp -= n;
			return tmp;
		}

		reference operator[](difference_type n)
		{
			return *(*this + n);
		}

		bool operator==(const self& rhs) const 
		{ return m_cur == rhs.m_cur; }
		bool operator!=(const self& rhs) const 
		{ return !(*this == rhs); }
		bool operator<(const self& rhs)	 const 
		{
			return m_node == rhs.m_node ? (m_node < rhs.m_node) : (m_cur < rhs.m_cur);
		}
		bool operator>(const self& rhs)  const
		{
			return rhs > (*this);
		}
		bool operator<=(const self& rhs) const
		{
			return !((*this) > rhs);
		}
		bool operator>=(const self& rhs) const
		{
			return !((*this) < rhs);
		}

	};

	
	template <class T>
	class deque
	{
	public:

		typedef	mystl::allocator<T>				allocator_type;
		typedef mystl::allocator<T>				data_allocator;
		typedef mystl::allocator<T*>			map_allocator;


		typedef typename allocator_type::value_type			value_type;
		typedef typename allocator_type::pointer			pointer;
		typedef typename allocator_type::const_pointer		const_pointer;
		typedef typename allocator_type::reference			reference;
		typedef typename allocator_type::const_reference	const_reference;
		typedef typename allocator_type::size_type			size_type;
		typedef typename allocator_type::difference_type	difference_type;

		typedef typename pointer* map_pointer;
		typedef typename const_pointer* const_map_pointer;

		typedef typename deque_iterator<T, T&, T*>				iterator;
		typedef typename deque_iterator<T, const T&, const T*>	const_iterator;
		typedef mystl::reverse_iterator<iterator>				reverse_iterator;
		typedef mystl::reverse_iterator<const_pointer>			const_reverse_iterator;


		allocator_type get_allocator() { return allocator_type(); }

		static const size_type buf_size = deque_buf_size<T>::value;


	private:

		iterator	mBegin;
		iterator	mEnd;
		map_pointer	mMap;
		size_type	mMapSize;


	public:

		deque()
		{
			fill_init(0, value_type());
		}

		explicit deque(size_type n)
		{
			fill_init(n, value_type());
		}

		deque(size_type n, const value_type& value)
		{
			fill_init(n, value);
		}

		template <class Iter, typename std::enable_if<
			mystl::is_input_iterator<Iter>::value, int>::value = 0>
		deque(Iter first, Iter last)
		{
			copy_init(first, last, mystl::iterator_category(first));
		}

		deque(std::initializer_list<value_type> list)
		{
			copy_init(list.begin(), list.end());
		}

		deque(const deque& rhs)
		{
			copy_init(rhs.mBegin, rhs.mEnd, mystl::iterator_category(rhs.mBegin));
		}

		deque(deque&& rhs) noexcept :
			mBegin(mystl::move(rhs.mBegin)),
			mEnd(mystl::move(rhs.mEnd)),
			mMap(rhs.mMap),
			mMapSize(rhs.mMapSize)
		{
			rhs.mMap = nullptr;
			rhs.buf_size = 0;
		}

		self& operator=(const deque& rhs)
		{
			if (this != &rhs) {
				const size_type n = size();
				if (n >= rhs.size()) {
					erase(mystl::copy(rhs.begin(), rhs.end(), mBegin), mEnd);
				}
				else {
					const iterator mid = rhs.size() + rhs.begin();
					mystl::copy(rhs.begin(), mid, mBegin);
					insert(mEnd, mid, rhs.end());
				}
			}
			return *this;
		}

		self& operator=(deque&& rhs)
		{
			clear();
			mBegin = mystl::move(rhs.mBegin);
			mEnd = mystl::move(rhs.mEnd);
			mMap = rhs.mMap;
			mMapSize = rhs.mMapSize;
			rhs.mMap = nullptr;
			rhs.mMapSize = 0;
			return *this;
		}

		self& operator=(std::initializer_list<value_type> list)
		{
			deque tmp(list);
			this->swap(tmp);
			return *this;
		}

		~deque()
		{
			if (mMap != nullptr) {
				clear();
				data_allocator::deallocate(*(mBegin.m_node), buf_size);
				mBegin = mEnd = nullptr;
				data_allocator::deallocate(mMap, mMapSize);
				mMap = nullptr;
			}
		}

		reference operator[](size_type n)
		{
			MYSTL_DEBUG(n < size());
			return mBegin[n];
		}

		const_reference operator[](size_type n) const
		{
			MYSTL_DEBUG(n < size());
			return mBegin[n];
		}

	public:

		iterator begin() noexcept
		{
			return mBegin;
		}

		iterator end()
		{
			return mEnd;
		}

		const_iterator begin() const noexcept
		{
			return mBegin;
		}

		const_iterator end() const noexcept
		{
			return mEnd;
		}

		reverse_iterator rbegin() noexcept
		{
			return reverse_iterator(mEnd);
		}

		reverse_iterator rend() noexcept
		{
			return reverse_iterator(mBegin);
		}

		const_iterator cbegin() const noexcept
		{
			return mBegin;
		}

		const_iterator cend() const noexcept
		{
			return mEnd;
		}

		const_reverse_iterator crbegin() const noexcept
		{
			return rbegin();
		}

		const_reverse_iterator crend() const noexcept
		{
			return rend();
		}


	public:

		bool empty() const noexcept
		{
			return mBegin == mEnd;
		}

		size_type size() const noexcept
		{
			return mEnd - mBegin;
		}

		size_type max_size() const noexcept
		{
			return static_cast<size_type> (-1);
		}

		void resize(size_type n)
		{
			resize(n, value_type());
		}

		void resize(size_type n, const value_type& value)
		{
			const size_type len = size();
			if (n <= len) {
				erase(mBegin + n, mEnd);
			}
			else {
				insert(mEnd, n - len, value);
			}
		}

		// 归还前后没有装有元素的空间
		void shrink_to_fit() noexcept
		{
			for (auto cur = mMap; cur < mBegin.m_node; cur++) {
				data_allocator::deallocate(*cur, buf_size);
				*cur = nullptr;
			}
			for (auto cur = mEnd.m_node + 1; cur < mMap + mMapSize; cur++) {
				data_allocator::deallocate(*cur, buf_size);
				*cur = nullptr;
			}
		}


	public:

		reference at(size_type n)
		{
			THROW_OUT_RANGE_IF(!(n < size()), "超出范围");
			return (*this)[n];
		}

		const_reference at(size_type n) const
		{
			HROW_OUT_RANGE_IF(!(n < size()), "超出范围");
			return (*this)[n];
		}

		reference front()
		{
			MYSTL_DEBUG(!empty());
			return mBegin[0];
		}

		const_reference front()
		{
			MYSTL_DEBUG(!empty());
			return mBegin[0];
		}

		reference back()
		{
			MYSTL_DEBUG(!empty());
			return *(mEnd - 1);
		}

		const_reference back() const
		{
			MYSTL_DEBUG(!empty());
			return *(mEnd - 1);
		}

		void assign(size_type n, const value_type& value)
		{
			fill_assign(n, value);
		}

		template <class Iter, std::enable_if<
			mystl::is_input_iterator<Iter>::value, int>::value = 0>
		void assign(Iter first, Iter last)
		{
			copy_assign(first, last, mystl::iterator_category(first));
		}

		void assign(std::initializer_list<T> list)
		{
			copy_assign(list.begin(), list.end(), mystl::forward_iterator_tag());
		}

		template <class ...Args>
		void emplace_front(Args&& ...args)
		{
			if (mBegin.m_cur != mBegin.m_begin) {
				data_allocator::construct(mBegin.m_cur - 1, mystl::forward<Args>(args));
				--mBegin.m_cur;
			}
			else {
				requiue_capacity(1, true);
				try {
					--mBegin;
					data_allocator::construct(mBegin.m_cur, mystl::forward<Args>(args));
				}
				catch (...) {
					++mBegin;
					throw;
				}
			}
		}

		template <class ...Args>
		void emplace_back(Args&& ...args)
		{
			if (mEnd.m_cur != mEnd.m_end - 1) {
				data_allocator::construct(mEnd.m_cur, mystl::forward<Args>(...args));
				++mEnd.m_cur;
			}
			else {
				requiue_capacity(1, false);
				data_allocator::construct(mEnd.m_cur, mystl::forward<Args>(...args));
				++mEnd;
			}
		}

		template <class ...Args>
		iterator emplace(iterator pos, Args&& ...args)
		{
			if (pos.m_cur == mBegin.m_cur) {
				emplace_front(mystl::forward<Args>(...args));
				return mBegin;
			}
			else if (pos.m_cur == mEnd.m_cur) {
				emplace_back(mystl::forward<Args>(...args));
				return mEnd - 1;
			}
			return insert_aux(pos, mystl::forward<Args>(...args));
		}

		void push_front(const value_type& value)
		{
			if (mBegin.m_cur != mBegin.m_begin) {
				mBegin.m_cur--;
				data_allocator::construct(mBegin.m_cur, value);
			}
			else {
				requiue_capacity(1, true);
				try {
					mBegin--;
					data_allocator::construct(mBegin.m_cur, value);
				}
				catch (...) {
					data_allocator::destroy(mBegin.m_cur);
					mBegin++;
					throw;
				}
			}
		}

		void push_back(const value_type& value)
		{
			if (mEnd.m_cur != mEnd.m_cur - 1) {
				data_allocator::construct(mEnd.m_cur, value);
				mEnd.m_cur++;
			}
			else {
				requiue_capacity(1, false);
				data_allocator::construct(mEnd.m_cur, value);
				mEnd++;
			}
		}

		void push_front(value_type&& value)
		{
			emplace_front(mystl::forward<value_type>(value));
		}

		void push_back(value_type&& value)
		{
			emplace_back(mystl::forward<value_type>(value));
		}

		void pop_front()
		{
			MYSTL_DEBUG(!empty());
			if (mBegin.m_cur != mBegin.m_end - 1) {
				data_allocator::destroy(mBegin.m_cur);
				++mBegin.m_cur;
			}
			else {
				data_allocator::destroy(mBegin.m_cur);
				++mBegin;
				destroy_buffer(mBegin.m_node - 1, mBegin.m_node - 1);
			}
		}

		void pop_back()
		{
			MYSTL_DEBUG(!empty());
			if (mEnd.m_cur != mEnd.m_begin) {
				--mEnd.m_cur;
				data_allocator::destroy(mEnd.m_cur);
			}
			else {
				--mEnd;
				data_allocator::destroy(mEnd.m_cur);
				destroy_buffer(mEnd.m_node + 1, mEnd.m_node + 1);
			}
		}

		iterator insert(iterator pos, const value_type& value)
		{
			if (pos == mBegin) {
				push_front(value);
				return mBegin;
			}
			else if (pos == mEnd) {
				iterator tmp = mEnd;
				push_back(value);
				return tmp;
			}
			else {
				return insert_aux(pos, value);
			}
		}

		iterator insert(iterator pos, value_type&& value)
		{
			if (pos == mBegin) {
				emplace_front(mystl::forward<value_type>(value));
				return mBegin;
			}
			else if (pos == mEnd) {
				iterator tmp = mEnd;
				push_back(mystl::forward<value_type>(value));
				return tmp;
			}
			else {
				return insert_aux(pos, mystl::move(value));
			}
		}

		void insert(iterator pos, size_type n, const value_type& value)
		{
			if (pos == mBegin) {
				requiue_capacity(n, true);
				const iterator newBegin = mBegin - n;
				mystl::uninitialized_fill_n(newBegin, n, value);
				mBegin = newBegin;
			}
			else if (pos == mEnd) {
				requiue_capacity(n, false);
				const iterator newEnd = mEnd + n;
				mystl::uninitialized_fill_n(mEnd, n, value);
				mEnd = newEnd;
			}
			else {
				fill_insert(pos, n, value);
			}
		}

		template <class Iter, std::enable_if<
			mystl::is_input_iterator<Iter>::value, int>::value = 0>
		void insert(iterator pos, Iter first, Iter last)
		{
			insert_dispatch(pos, first, last, mystl::iterator_category(first));
		}

		iterator erase(iterator pos)
		{
			iterator next = pos;
			++next;
			const size_type n = pos - mBegin;
			if (n < size() / 2) {
				mystl::copy_backward(mBegin, pos, next);
				pop_front();
			}
			else {
				mystl::copy(next, mEnd, pos);
				pop_back();
			}
			return mBegin + n;
		}

		iterator erase(iterator first, iterator last)
		{
			if (first == mBegin && last == mEnd) {
				clear();
				return mBegin;
			}
			const size_type len = last - first;
			const size_type elem = first - mBegin;
			if (elem < ((size() - len) / 2)) {
				mystl::copy_backward(mBegin, first, last);
				iterator newBegin = mBegin + len;
				data_allocator::destroy(mBegin.m_cur, newBegin.m_cur);
				mBegin = newBegin;
			}
			else {
				mystl::copy(last, mEnd, first);
				iterator newEnd = mEnd - len;
				data_allocator::destroy(last.m_cur, mEnd.m_cur);
				mEnd = newEnd;
			}
			return mBegin + elem;
		}

		void clear()
		{
			for (iterator cur = mBegin.m_node + 1; cur < mEnd.m_node; cur++) {
				data_allocator::destroy(cur.m_begin, cur.m_end);
			}
			if (mBegin.m_node != mEnd.m_node) {
				data_allocator::destroy(mBegin.m_cur, mBegin.m_end);
				data_allocator::destroy(mEnd.m_begin, mEnd.m_cur);
			}
			else {
				data_allocator::destroy(mEnd.m_cur, mEnd.m_cur);
			}
			shrink_to_fit();
			mEnd = mBegin;
		}

		void swap(deque& rhs) noexcept
		{
			mystl::swap(mBegin, rhs.mBegin);
			mystl::swap(mEnd, rhs.mEnd);
			mystl::swap(mMap, rhs.mMap);
			mystl::swap(mMapSize, rhs.mMapSize);
		}


	private:

		// 创建一个指针数组
		map_pointer create_map(size_type size)
		{
			map_pointer mp = nullptr;
			mp = map_allocator::allocate(size);
			for (size_type i = 0; i < size; i++) {
				*(mp + i) = nullptr;
			}
			return mp;
		}

		// 为指针数组的每一个指针指向一个数据类型的数组
		void create_buffer(map_pointer nstart, map_pointer nfinish)
		{
			map_pointer mp = nstart;
			try {
				for (; mp <= nfinish; mp++) {
					*mp = data_allocator::allocate(buf_size);
				}
			}
			catch (...) {
				while (mp != nstart) {
					--mp;
					data_allocator::deallocate(*mp, buf_size);
					*mp = nullptr;
				}
				throw;
			}
		}

		void destroy_buffer(map_pointer nstart, map_pointer nfinish)
		{
			for (map_pointer mp = nstart; mp != nfinish; mp++) {
				data_allocator::deallocate(*mp, buf_size);
				*mp = nullptr;
			}
		}

		// 根据数据量nelem来初始化缓冲区以及mBegin和mEnd
		void map_init(size_type nelem)
		{
			const size_type n = nelem / buf_size - 1;
			mMapSize = mystl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), n + 2);
			try {
				mMap = create_map(mMapSize);
			}
			catch (...) {
				map_allocator::deallocate(mMap, mMapSize);
				mMap = nullptr;
				mMapSize = 0;
				throw;
			}

			map_pointer start = mMap + (mMapSize - n) / 2;
			map_pointer finish = start + n - 1;
			try {
				create_buffer(start, finish);
			}
			catch (...) {
				map_allocator::deallocate(mMap, mMapSize);
				mMap = nullptr;
				mMapSize = 0;
				throw;
			}
			mBegin.set_node(start);
			mEnd.set_node(finish);
			mBegin.m_cur = mBegin.m_begin;
			mEnd.m_cur = mEnd.m_begin + (nelem % buf_size);
		}

		void fill_init(size_type n, const value_type& value)
		{
			map_init(n);
			if (n != 0) {
				for (map_pointer cur = mBegin.m_node; cur < mEnd.m_node; cur++) {
					mystl::uninitialized_fill(*cur, *cur + buf_size, value);
				}
				mystl::uninitialized_fill(mEnd.m_begin, mEnd.m_cur, value);
			}
		}

		// 这个设计有问题感觉
		template <class Iter>
		void copy_init(Iter first, Iter last, mystl::input_iterator_tag)
		{
			const size_type n = mystl::distance(first, last);
			map_init(n);
			for (; first != last; first++) {
				emplace_back(*first);
			}
		}

		template <class Iter>
		void copy_init(Iter first, Iter last, mystl::forward_iterator_tag)
		{
			const size_type n = mystl::distance(first, last);
			map_init(n);
			for (map_pointer cur = mBegin.m_node; cur < mEnd.m_node; cur++) {
				Iter next = first;
				mystl::advance(next, buf_size);
				mystl::uninitialized_copy(first, next, *cur);
				first = next;
			}
			mystl::uninitialized_copy(first, last, mEnd.m_begin);
		}

		void fill_assign(size_type n, const value_type& value)
		{
			if (n > size()) {
				mystl::fill(mBegin.m_cur, mEnd.m_cur, value);
				insert(mEnd, n - size(), value);
			}
			else {
				mystl::fill(mBegin, mBegin + n, value);
				erase(mBegin + n, mEnd);
			}
		}

		template <class Iter>
		void copy_assign(Iter first, Iter last, mystl::input_iterator_tag)
		{
			iterator first1 = begin(), last1 = end();
			for (; first1 != last1 && first != last; first1++, first++) {
				*first1 = *first;
			}
			if (first1 == last1) {
				insert_dispatch(mEnd, first, last, mystl::input_iterator_tag());
			}
			else {
				erase(first1, last1);
			}
		}

		template <class Iter>
		void copy_assign(Iter first, Iter last, mystl::forward_iterator_tag)
		{
			const size_type len = mystl::distance(first, last);
			if (len <= size()) {
				erase(mystl::copy(first, last, mBegin), mEnd);
			}
			else {
				Iter mid = first;
				mystl::advance(mid, size());
				insert_dispatch(mEnd, mid, last, mystl::forward_iterator_tag());
			}
		}

		template <class... Args>
		iterator insert_aux(iterator pos, Args&& ...args)
		{
			const size_type elem = pos - mBegin;
			value_type value_copy = value_type(mystl::move(args));
			if (elem <= size() / 2) {
				iterator front1 = mBegin;
				emplace_front(front());
				iterator front2 = front1;
				front2++;
				mystl::copy(front2, last, front1);
			}
			else {
				iterator back1 = mEnd;
				emplace_back(back());
				iterator back2 = back1;
				--back2;
				mystl::copy_backward(pos, back2, back1);
			}
			*pos = mystl::move(value_copy);
			return pos;
		}

		void fill_insert(iterator pos, size_type n, const value_type& value)
		{
			const size_type elem = pos - mBegin;
			value_type valueCopy = value;
			if (elem < size() / 2) {
				requiue_capacity(n, true);
				// 这里的迭代器可能已经失效了
				iterator newBegin = mBegin - n;
				iterator oldBegin = mBegin;
				pos = mBegin + elem;
				try {
					if (elem >= n) {
						iterator begin_n = mBegin + n;
						mystl::uninitialized_copy(oldBegin, begin_n, newBegin);
						mBegin = newBegin;
						mystl::copy(begin_n, pos, oldBegin);
						mystl::fill(pos - n, pos, valueCopy);
					}
					else {
						// 注意这里是不能直接用copy()函数的
						// 因为没初始化过
						mystl::uninitialized_fill(
							mystl::uninitialized_copy(oldBegin, pos, newBegin), oldBegin, valueCopy
						);
						mBegin = newBegin;
						mystl::fill(oldBegin, pos, valueCopy);
					}
				}
				catch (...) {
					if (newBegin != oldBegin) {
						destroy_buffer(newBegin.m_node, oldBegin.m_node - 1);
					}
					throw;
				}
			}
			else {
				requiue_capacity(n, false);
				iterator newEnd = mEnd + n;
				iterator oldEnd = mEnd;
				pos = mEnd - elem;
				size_type len = mEnd - pos;
				try {
					if (len >= n) {
						iterator end_n = mEnd - n;
						mystl::uninitialized_copy(end_n, oldEnd, oldEnd);
						mEnd = newEnd;
						mystl::copy_backward(pos, end_n, oldEnd);
						mystl::fill(pos, pos + n, valueCopy);
					}
					else {
						mystl::uninitialized_fill(oldEnd, newEnd, valueCopy);
						mystl::copy_backward(pos, oldEnd, newEnd);
						mEnd = newEnd;
						mystl::fill(pos, oldEnd, valueCopy);
					}
				}
				catch (...) {
					if (newEnd != oldEnd) {
						destroy_buffer(oldEnd.m_node + 1, newEnd.m_node);
					}
					throw;
				}
			}
		}

		template <class Iter>
		void copy_insert(iterator pos, Iter first, Iter last, size_type n)
		{
			const size_type elem = pos - mBegin;
			if (elem < size() / 2) {
				requiue_capacity(n, true);
				// 这里的迭代器可能已经失效了
				iterator newBegin = mBegin - n;
				iterator oldBegin = mBegin;
				pos = mBegin + elem;
				try {
					if (elem >= n) {
						iterator begin_n = mBegin + n;
						mystl::uninitialized_copy(oldBegin, begin_n, newBegin);
						mBegin = newBegin;
						mystl::copy(begin_n, pos, oldBegin);
						mystl::copy(first, last, pos - n);
					}
					else {
						// 注意这里是不能直接用copy()函数的
						// 因为没初始化过
						Iter mid = first;
						mystl::advance(mid, n - elem);
						mystl::uninitialized_copy(
							first, 
							mid,
							mystl::uninitialized_copy(oldBegin, pos, newBegin)
						);
						mBegin = newBegin;
						mystl::copy(mid, last, oldBegin);
					}
				}
				catch (...) {
					if (newBegin != oldBegin) {
						destroy_buffer(newBegin.m_node, oldBegin.m_node - 1);
					}
					throw;
				}
			}
			else {
				requiue_capacity(n, false);
				iterator newEnd = mEnd + n;
				iterator oldEnd = mEnd;
				pos = mEnd - elem;
				size_type len = mEnd - pos;
				try {
					if (len >= n) {
						iterator end_n = mEnd - n;
						mystl::uninitialized_copy(end_n, oldEnd, oldEnd);
						mEnd = newEnd;
						mystl::copy_backward(pos, end_n, oldEnd);
						mystl::copy(first, last, pos);
					}
					else {
						Iter mid = first;
						mystl::advance(mid, elem);
						mystl::uninitialized_copy(pos, oldEnd,
							mystl::uninitialized_copy(mid, last, oldEnd));
						mEnd = newEnd;
						mystl::copy(first, mid, pos);
					}
				}
				catch (...) {
					if (newEnd != oldEnd) {
						destroy_buffer(oldEnd.m_node + 1, newEnd.m_node);
					}
					throw;
				}
			}
		}

		// 名字里有包含dispatch的是要操作一个范围
		template <class Iter>
		void insert_dispatch(iterator pos, Iter first, Iter last, mystl::forward_iterator_tag)
		{
			if (last <= first) {
				return;
			}
			const size_type n = mystl::distance(first, last);
			const size_type elem = pos - mBegin;
			if (elem <= size() / 2) {
				requiue_capacity(n, true);
			}
			else {
				requiue_capacity(n, false);
			}
			pos = mBegin + elem;
			Iter cur = --last;
			for (size_type i = 0; i < n; i++, cur--) {
				insert(pos, *cur);
			}
		}

		template <class Iter>
		void insert_dispatch(iterator pos, Iter first, Iter last, mystl::input_iterator_tag)
		{
			if (last <= first) {
				return;
			}
			const size_type n = mystl::distance(first, last);
			if (pos.m_cur == mBegin.m_cur) {
				requiue_capacity(n, true);
				iterator newBegin = mBegin - n;
				try {
					// 如果报错的话会在下面这句报错
					// 如何进行捕抓
					mystl::uninitialized_copy(first, last, newBegin);
					// 不会执行下面这句
					// 如果执行说明已经不会报错了
					mBegin = newBegin;
				}
				catch (...) {
					if (mBegin.m_node != newBegin.m_node) {
						destroy_buffer(newBegin.m_node, mBegin.m_node);
					}
					throw;
				}
			}
			else if (pos.m_cur == mEnd.m_cur) {
				requiue_capacity(n, false);
				iterator newEnd = mEnd + n;
				try {
					mystl::uninitialized_copy(first, last, mEnd);
					mEnd = newEnd;
				}
				catch (...) {
					if (mEnd.m_node != newEnd.m_node) {
						destroy_buffer(mEnd.m_node, newEnd.m_node);
					}
					throw;
				}
			}
			else {
				copy_insert(pos, first, last, n);
			}
		}

		void requiue_capacity(size_type n, bool flag)
		{
			if (flag == true && (static_cast<size_type>(mBegin.m_cur - mBegin.m_begin) < n)) {
				const size_type needBuf = (n - (mBegin.m_cur - mBegin.m_begin)) / buf_size + 1;
				if (needBuf > static_cast<size_type>(mBegin.m_node - mMap)) {
					reallocate_map_at_front(needBuf);
					return;
				}
				create_buffer(mBegin.m_node - needBuf, mBegin.m_node - 1);
			}
			else if (flag == false && (static_cast<size_type>(mEnd.m_end - mEnd.m_cur) < n)) {
				const size_type needBuf = (n - (mEnd.m_end - mEnd.m_cur)) / buf_size + 1;
				if (needBuf > static_cast<size_type>((mMap + mMapSize) - mEnd.m_node - 1)) {
					reallocate_map_at_back(needBuf);
					return;
				}
				create_buffer(mEnd.m_node + 1, mEnd.m_node + needBuf);
			}
		}
		
		void reallocate_map_at_front(size_type need)
		{
			const size_type newMapSize = mystl::max(mMapSize << 1, mMapSize + need + DEQUE_MAP_INIT_SIZE);
			map_pointer mp = create_map(newMapSize);
			const size_type oldBuf = mEnd.m_node - mBegin.m_node + 1;
			const size_type newBuf = oldBuf + need;

			map_pointer newBegin = mp + (newMapSize - newBuf) / 2;
			map_pointer mid = newBegin + need;
			map_pointer newEnd = mid + oldBuf;
			create_buffer(newBegin, mid);
			for (map_pointer begin1 = mid, begin2 = mBegin.m_node; begin1 != newEnd; ++begin1, ++begin2) {
				*begin1 = *begin2;
			}
			map_allocator::deallocate(mMap, mMapSize);
			mMap = mp;
			mMapSize = newMapSize;
			mBegin = iterator(*mid + (mBegin.m_cur - mBegin.m_begin), mid);
			mEnd = iterator(*(newEnd - 1) + (mEnd.m_cur - mEnd.m_begin), newEnd - 1);
		}

		void reallocate_map_at_back(size_type need)
		{
			const size_type newMapSize = mystl::max(mMapSize << 1, mMapSize + need + DEQUE_MAP_INIT_SIZE);
			map_pointer mp = create_map(newMapSize);
			const size_type oldBuf = mEnd.m_node - mBegin.m_node + 1;
			const size_type newBuf = oldBuf + need;

			map_pointer newBegin = mp + (newMapSize - newBuf) / 2;
			map_pointer mid = newBegin + oldBuf;
			map_pointer newEnd = mid + need;
			for (map_pointer begin1 = newBegin, begin2 = mBegin.m_node; begin1 != mid; ++begin1, ++begin2) {
				*begin1 = *begin2;
			}
			map_allocator::deallocate(mMap, mMapSize);
			mMap = mp;
			mMapSize = newMapSize;
			mBegin = iterator(*newBegin + (mBegin.m_cur - mBegin.m_begin), newBegin);
			mEnd = iterator(*(mid - 1) + (mEnd.m_cur - mEnd.m_begin), mid - 1);
		}

	};


	template <class T>
	bool operator==(const deque<T>& lhs, const deque<T>& rhs)
	{
		return lhs.size() == rhs.size()
			&& mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template <class T>
	bool operator!=(const deque<T>& lhs, const deque<T>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class T>
	bool operator<(const deque<T>&lhs, const deque<T>&rhs)
	{
		return mystl::lexicographical_compare(lhs.begin(), lhs.end(),
			rhs.begin(), rhs.end());
	}

	template <class T>
	bool operator>(const deque<T>& lhs, const deque<T>& rhs)
	{
		return rhs < lhs;
	}

	template <class T>
	bool operator>=(const deque<T>& lhs, const deque<T>& rhs)
	{
		return !(rhs < lhs);
	}

	template <class T>
	bool operator<=(const deque<T>& lhs, const deque<T>& rhs)
	{
		return !(rhs > lhs);
	}

	template <class T>
	void swap(deque<T>& lhs, deque<T>& rhs) {
		lhs.swap(rhs);
	}
}


#endif // !MYSTL_DEQUE_H
