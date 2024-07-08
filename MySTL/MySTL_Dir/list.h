#ifndef MYSTL_LIST_H
#define MYSTL_LIST_H

#include <initializer_list>
#include "iterator.h"
#include "memory.h"
#include "functional.h"
#include "util.h"
#include "exceptdef.h"

namespace mystl {

	// 外面调用public函数时应该public函数应该检查传入参数的错误
	// public函数调用private函数时private函数应该检查系统的错误

	template <class T> struct list_node_base;
	template <class T> struct list_node;

	template <class T>
	struct node_traits 
	{
		using base_ptr = list_node_base<T>*;
		using node_ptr = list_node<T>*;
	};

	template <class T>
	struct list_node_base 
	{
		using base_ptr = typename node_traits<T>::base_ptr;
		using node_ptr = typename node_traits<T>::node_ptr;

		base_ptr pre, next;

		list_node_base() = default;

		base_ptr self()
		{
			return static_cast<base_ptr>(&*this);
		}

		void unlink()
		{
			pre = next = self();
		}

		node_ptr as_node()
		{
			return static_cast<node_ptr>(self());
		}

	};

	template <class T>
	struct list_node : public list_node_base<T>
	{
		using base_ptr = typename node_traits<T>::base_ptr;
		using node_ptr = typename node_traits<T>::node_ptr;

		T value;

		list_node() = default;

		list_node(const T& v): value(v) {}

		list_node(T&& v): value(mystl::move(v)) {}

		base_ptr as_base()
		{
			return static_cast<base_ptr>(self());
		}

		node_ptr self()
		{
			return static_cast<node_ptr>(&*this);
		}

	};

	template <class T>
	struct list_iterator: public iterator<bidirectional_iterator_tag, T>
	{
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using base_ptr = typename node_traits<T>::base_ptr;
		using node_ptr = typename node_traits<T>::node_ptr;
		using self = list_iterator<T>;

		base_ptr mNode;

		list_iterator() = default;

		list_iterator(base_ptr x): mNode(x) {}

		list_iterator(node_ptr x): mNode(x->as_base()) {}

		list_iterator(const list_iterator& rhs): mNode(rhs.mNode) {}

		reference operator*() const 
		{
			return mNode->as_node()->value;
		}

		pointer operator->() const
		{
			return &(operator*());
		}

		self& operator++()
		{
			MYSTL_DEBUG(mNode != nullptr);
			mNode = mNode->next;
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
			MYSTL_DEBUG(mNode != nullptr);
			mNode = mNode->pre;
			return *this;
		}

		self operator--(int)
		{
			self tmp = *this;
			--(*this);
			return tmp;
		}

		bool operator==(const list_iterator& rhs) const { return mNode == rhs.mNode; }

		bool operator!=(const list_iterator& rhs) const { return !(*this == rhs); }

	};

	template <class T>
	struct list_const_iterator : public iterator<bidirectional_iterator_tag, T>
	{
		using value_type = T;
		using pointer = const T*;
		using reference = const T&;
		using base_ptr = typename node_traits<T>::base_ptr;
		using node_ptr = typename node_traits<T>::node_ptr;
		using self = list_iterator<T>;

		base_ptr mNode;

		list_const_iterator() = default;

		list_const_iterator(base_ptr x) : mNode(x) {}

		list_const_iterator(node_ptr x) : mNode(x->as_base()) {}

		list_const_iterator(const list_iterator& rhs) : mNode(rhs.mNode) {}

		list_const_iterator(const list_const_iterator& rhs) : mNode(rhs.mNode) {}

		reference operator*() const
		{
			return mNode->as_node()->value;
		}

		pointer operator->() const
		{
			return &(operator*());
		}

		self& operator++()
		{
			MYSTL_DEBUG(mNode != nullptr);
			mNode = mNode->next;
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
			MYSTL_DEBUG(mNode != nullptr);
			mNode = mNode->pre;
			return *this;
		}

		self operator--(int)
		{
			self tmp = *this;
			--(*this);
			return tmp;
		}

		bool operator==(const list_iterator& rhs) const { return mNode == rhs.mNode; }

		bool operator!=(const list_iterator& rhs) const { return !(*this == rhs); }


	};

	template <class T>
	class list
	{
	public:
		using allocator_type = mystl::allocator<T>;
		using data_allocator = mystl::allocator<T>;
		using base_allocator = mystl::allocator<list_node_base<T>>;
		using node_allocator = mystl::allocator<list_node<T>>;

		using value_type = typename allocator_type::value_type;
		using pointer = typename allocator_type::pointer;
		using const_pointer = typename allocator_type::const_pointer;
		using reference = typename allocator_type::reference;
		using const_reference = typename allocator_type::const_reference;
		using difference_type = typename allocator_type::difference_type;
		using size_type = typename allocator_type::size_type;

		using iterator = list_iterator<T>;
		using const_iterator = list_const_iterator<T>;
		using reverse_iterator = mystl::reverse_iterator<iterator>;
		using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

		using base_ptr = typename node_traits<T>::base_ptr;
		using node_ptr = typename node_traits<T>::node_ptr;

		allocator_type get_allocate() { return node_allocator(); }

	private:

		base_ptr mNode;
		size_type mSize;

	public:

		list()
		{
			fill_init(0, value_type());
		}

		explicit list(size_type n)
		{
			fill_init(0, value_type());
		}

		list(size_type n, const value_type& value)
		{
			fill_init(0, value);
		}

		template <class Iter, typename std::enable_if<
			mystl::is_input_iterator<Iter>::value, int>::type = 0>
		list(Iter first, Iter last)
		{
			copy_init(first, last);
		}

		list(std::initializer_list<T> ilist)
		{
			copy_init(ilist.begin(), ilist.end());
		}

		list(const list& rhs)
		{
			copy_init(rhs.cbegin(), rhs.cend());
		}

		list(list&& rhs) noexcept
			: mNode(mystl::move(rhs.mNode)), mSize(rhs.mSize)
		{
			rhs.mNode = nullptr;
			rhs.mSize = 0;
		}

		list& operator=(const list& rhs)
		{
			if (this != &rhs) {
				assign(rhs.begin(), rhs.end());
			}
			return *this;
		}

		list& operator=(list&& rhs)
		{

			return *this;
		}

		list& operator=(std::initializer_list<T> ilist)
		{
			list tmp(ilist);
			swap(ilist);
			return *this;
		}

		~list()
		{
			if (mNode != nullptr) {
				clear();
				base_allocator::deallocate(mNode);
				mNode = nullptr;
				mSize = 0;
			}
		}

	public:

		iterator begin() noexcept
		{
			return mNode->next;
		}

		const_iterator begin() const noexcept
		{
			return mNode->next;
		}

		iterator end() noexcept
		{
			return mNode;
		}

		const_iterator end() const noexcept
		{
			return mNode;
		}

		reverse_iterator rbegin() noexcept
		{
			return reverse_iterator(mNode);
		}

		const_reverse_iterator rbegin() const noexcept
		{
			return const_reverse_iterator(mNode);
		}

		reverse_iterator rend() noexcept
		{
			return reverse_iterator(mNode->next);
		}

		const_reverse_iterator rend() const noexcept
		{
			return const_reverse_iterator(mNode->next);
		}

		const_iterator cbegin() const noexcept
		{
			return const_iterator(mNode->next);
		}

		const_iterator cend() const noexcept
		{
			return const_iterator(mNode);
		}

		const_reverse_iterator crbegin() const noexcept
		{
			return rbegin();
		}

		const_reverse_iterator crend() const noexcept
		{
			return rend();
		}

		bool empty() const noexcept
		{
			return mNode->next == mNode;
		}

		size_type size() const noexcept
		{
			return mSize;
		}

		size_type max_size() const noexcept
		{
			return static_cast<size_type>(-1);
		}

		void swap(list& rhs) noexcept
		{
			mystl::swap(mNode, rhs.mNode);
			mystl::swap(mSize, rhs.mSize);
		}

	public:

		reference front()
		{
			MYSTL_DEBUG(!empty());
			return *(mNode->next);
		}

		const_reference front() const
		{
			MYSTL_DEBUG(!empty());
			return *(mNode->next);
		}

		reference back()
		{
			MYSTL_DEBUG(!empty());
			return *(--end());
		}

		const_reference back() const
		{
			MYSTL_DEBUG(!empty());
			return *(--end());
		}

		void assign(size_type n, const value_type& value)
		{
			fill_assign(n, value);
		}

		template <class Iter, typename std::enable_if<
			mystl::is_input_iterator<Iter>::value, int>::type = 0>
		void assign(Iter first, Iter last)
		{
			copy_assign(first, last);
		}

		void assign(std::initializer_list<value_type> ilist)
		{
			copy_assign(ilist.begin(), ilist.end());
		}

		template <class ...Args>
		void emplace_back(Args && ...args)
		{
			THROW_LENGTH_ERROR_IF(mSize > max_size(), "");
			node_ptr link_node = create_node(mystl::forward<Args>(args)...);
			link_nodes_at_back(link_node->as_base(), link_node->as_base());
			++mSize;

		}

		template <class ...Args>
		void emplace_front(Args && ...args)
		{
			THROW_LENGTH_ERROR_IF(mSize > max_size(), "");
			node_ptr link_node = create_node(mystl::forward<Args>(args)...);
			link_nodes_at_front(link_node->as_base(), link_node->as_base());
			++mSize;
		}

		template <class ...Args>
		iterator emplace(const_iterator pos, Args && ...args)
		{
			THROW_LENGTH_ERROR_IF(mSize > max_size(), "");
			node_ptr link_node = create_node(mystl::forward<Args>(args)...);
			link_nodes(pos.mNode, link_node->as_base(), link_node->as_base());
			++mSize;
			return iterator(node_ptr);
		}

		iterator insert(const_iterator pos, const value_type& value)
		{
			THROW_LENGTH_ERROR_IF(size() > max_size() - 1, "");
			auto link_node = create_node(value);
			++mSize;
			return link_iter_node(pos, link_node->as_base());
		}

		iterator insert(const_iterator pos, value_type&& value)
		{
			THROW_LENGTH_ERROR_IF(size() > max_size() - 1, "");
			auto link_node = create_node(mystl::move(value));
			++mSize;
			return link_iter_node(pos, link_node->as_base());
		}

		iterator insert(const_iterator pos, size_type n, const value_type& value)
		{
			THROW_LENGTH_ERROR_IF(mSize > max_size() - n, "");
			return fill_insert(pos, n, value);
		}

		template <class Iter, typename std::enable_if<
			mystl::is_input_iterator<Iter>::value, int>::type = 0>
		iterator insert(const_iterator pos, Iter first, Iter last)
		{
			size_type n = mystl::distance(first, last);
			THROW_LENGTH_ERROR_IF(mSize > max_size() - n, "");
			return copy_insert(pos, first, last);
		}

		void push_front(const value_type& value)
		{
			THROW_LENGTH_ERROR_IF(mSize > max_size() - 1, "");
			node_ptr link_node = create_node(value);
			link_nodes_at_front(link_node->as_base(), link_node->as_base());
			++mSize;
		}

		void push_front(value_type&& value)
		{
			emplace_front(mystl::move(value));
		}

		void push_back(const value_type& value)
		{
			THROW_LENGTH_ERROR_IF(mSize > max_size() - 1, "");
			node_ptr link_node = create_node(value);
			link_nodes_at_back(link_node->as_base(), link_node->as_base());
			++mSize;
		}

		void push_back(value_type&& value)
		{
			emplace_back(mystl::move(value));
		}

		void pop_front()
		{
			MYSTL_DEBUG(!empty());
			base_ptr n = mNode->next;
			unlink_node(n, n);
			destroy_node(n->as_node());
			--mSize;
		}

		void pop_back()
		{
			MYSTL_DEBUG(!empty());
			base_ptr n = mNode->pre;
			unlink_node(n, n);
			destroy_node(n->as_node());
			--mSize;
		}

		iterator erase(const_iterator pos)
		{
			MYSTL_DEBUG(pos != cend());
			base_ptr node = pos.mNode;
			base_ptr next = node->next;
			unlink_node(node, node);
			destroy_node(node->as_node());
			--mSize;
			return iterator(next);
		}

		iterator erase(const_iterator first, const_iterator last)
		{
			if (first != last)
			{
				unlink_node(first.mNode, last.mNode->pre);
				while (first != last) {
					base_ptr cur = first.mNode;
					first++;
					destroy_node(cur->as_node());
					--mSize;
				}
			}
			return iterator(last.mNode);
		}

		void clear()
		{
			if (mSize != 0) {
				base_ptr cur = mNode->next;
				for (base_ptr next = cur->next; cur != mNode; cur = next, next = cur->next) {
					destroy_node(cur->as_node());
					--mSize;
				}
				mNode->unlink();
				mSize = 0;
			}
		}

		void resize(size_type new_size)
		{
			resize(new_size, value_type());
		}

		void resize(size_type new_size, const value_type& value)
		{
			iterator iter = begin();
			size_type n = 0;
			while (iter != end() && n != new_size) {
				++iter;
				++n;
			}
			if (n == new_size) {
				erase(iter, end());
			}
			else {
				insert(iter, new_size - n, value);
			}
		}

		void splice(const_iterator pos, list& other)
		{
			MYSTL_DEBUG(this != &other);
			if (other.empty() == false) {
				THROW_LENGTH_ERROR_IF(mSize > max_size() - other.size(), "");
				auto first = pos.mNode->next;
				auto last = pos.mNode->pre;
				other.unlink_node(first, last);
				link_nodes(pos.mNode, first, last);
				mSize += other.size();
				other.mSize = 0;
			}
		}

		void splice(const_iterator pos, list& other, const_iterator it)
		{
			MYSTL_DEBUG(this != &other);
			if (other.empty() == false) {
				THROW_LENGTH_ERROR_IF(mSize > max_size() - 1, "");
				auto cur = pos.mNode;
				other.unlink_node(cur, cur);
				link_nodes(pos.mNode, cur, cur);
				mSize++;
				other.mSize--;
			}
		}

		void splice(const_iterator pos, list& other, const_iterator first, const_iterator last)
		{
			MYSTL_DEBUG(this != &other);
			if (other.empty() == false) {
				size_type n = distance(first, last);
				THROW_LENGTH_ERROR_IF(mSize > max_size() - n, "");
				auto f = first.mNode;
				auto l = last.mNode->pre;
				other.unlink_node(f, l);
				link_nodes(pos.mNode, f, l);
				mSize += n;
				other.mSize -= n;
			}
		}

		void remove(const value_type& value)
		{
			remove_if([&](const value_type& v) {return v == value; });
		}

		template <class UnaryPredicate>
		void remove_if(UnaryPredicate pred)
		{
			auto cur = begin();
			for (auto next = cur; cur != end(); cur = next)
			{
				next++;
				if (pred(*cur)) {
					erase(cur);
				}
			}
		}

		void unique()
		{
			unique(mystl::equal_to<T>());
		}

		template <class BinaryPredicate>
		void unique(BinaryPredicate pred)
		{
			auto cur = begin();
			auto next = cur;
			++next;
			while (next != end()) {
				if (pred(*cur, *next)) {
					erase(next);
				}
				else {
					next = cur;
				}
				cur = next;
				next++;
			}
		}

		void merge(list& x)
		{
			merge(x, less<T>());
		}

		template <class Compared>
		void merge(list& x, Compared cmp)
		{
			iterator f1 = begin(), l1 = end();
			iterator f2 = x.begin(), l2 = end();
			while (f1 != l1 && f2 != l2) {
				if (cmp(*f1, *f2)) {
					iterator next = f2;
					next++;
					for (; next != l2 && cmp(*next, *f1); next++) {}
					base_ptr last = next.mNode->pre;
					x.unlink_node(f2.mNode, last);
					link_nodes(f1.mNode, f2.mNode, last);
					f2 = next;
				}
				++f1;
			}
			if (f2 != l2) {
				iterator first = x.begin();
				iterator last = x.end();
				--last;
				x.unlink_node(first.mNode, last.mNode);
				link_nodes(l1.mNode, first.mNode, last.mNode);
			}
			mSize += x.mSize();
			x.mSize = 0;
		}

		void sort()
		{
			sort(less<T>());
		}

		template <class Compared>
		void sort(Compared cmp)
		{
			list_sort(begin(), end(), size(), cmp);
		}

		void reverse()
		{
			if (mSize <= 1) {
				return;
			}
			iterator first = begin();
			iterator last = end();
			while (first.mNode != last.mNode) {
				mystl::swap(first.mNode->pre, first.mNode->next);
				first.mNode = first.mNode->pre;
			}
			mystl::swap(last.mNode->pre, last.mNode->next);
		}


	private:

		template <class ...Args>
		node_ptr create_node(Args && ...args)
		{
			node_ptr p = node_allocator::allocate(1);
			try {
				data_allocator::construct(mystl::address_of(p->value), mystl::forward<Args>(args)...);
				p->pre = p->next = nullptr;
			}
			catch (...) {
				node_allocator::deallocate(p);
				throw;
			}
			return p;
		}

		void destroy_node(node_ptr node)
		{
			data_allocator::destroy(mystl::address_of(node->value));
			node_allocator::deallocate(node);
		}

		void fill_init(size_type n, const value_type& value)
		{
			mNode = base_allocator::allocate(1);
			mNode->unlink();
			try {
				for (; n > 0; n--) {
					node_ptr p = create_node(value);
					link_nodes(mNode, p, p);
				}
			}
			catch (...) {
				clear();
				base_allocator::deallocate(mNode);
				mNode = nullptr;
				throw;
			}
		}

		template <class Iter>
		void copy_init(Iter first, Iter last)
		{
			mNode = base_allocator::allocate(1);
			mNode->unlink();
			mSize = mystl::distance(first, last);
			try {
				size_type n = mSize;
				for (; first != last; first++, n--) {
					node_ptr p = create_node(*first);
					link_nodes_at_back(p, p);
				}
			}
			catch (...) {
				clear();
				base_allocator::deallocate(mNode);
				mNode = nullptr;
				throw;
			}
		}

		template <class Iter>
		void copy_assign(Iter first, Iter last)
		{
			size_type n = mystl::distance(first, last);
			auto cur = begin();
			for (; cur != end() && first != last; cur++, first++) {
				*cur = *first;
			}
			if (first != last) {
				insert(cur, first, last);
			}
			else {
				erase(first, last);
			}
		}

		void fill_assign(size_type n, const value_type& value)
		{
			iterator first = begin();
			for (; first != end(); first++) {
				*first = value;
			}
			if (n >= mSize) {
				insert(first, n - mSize, value);
			}
			else {
				erase(first, end());
			}
		}

		void link_nodes(base_ptr p, base_ptr first, base_ptr last)
		{
			p->pre->next = first;
			first->pre = p->pre;
			last->next = p;
			p->pre = last;
		}

		void link_nodes_at_front(base_ptr first, base_ptr last)
		{
			first->pre = mNode;
			last->next = mNode->next;
			mNode->next->pre = last;
			mNode->next = first;
		}

		void link_nodes_at_back(base_ptr first, base_ptr last)
		{
			first->pre = mNode->pre;
			last->next = mNode;
			mNode->pre->next = first;
			mNode->pre = last;
		}

		iterator link_iter_node(const_iterator pos, base_ptr node)
		{
			if (pos == begin()) {
				link_nodes_at_front(node, node);
			}
			else if (pos == end()) {
				link_nodes_at_back(node, node);
			}
			else {
				link_nodes(pos.mNode, node, node);
			}
			return iterator(node);
		}

		iterator fill_insert(const_iterator pos, size_type n, const value_type& value)
		{
			iterator r(pos.mNode);
			if (n != 0) {
				const size_type add_size = n;
				node_ptr p = create_node(value);
				p->pre = nullptr;
				r = iterator(p);
				iterator end = r;
				try {
					for (--n; n > 0; --n, ++end) {
						node_ptr node = create_node(value);
						end.mNode->next = node->as_base();
						node->pre = end.mNode;
					}
				}
				catch (...) {
					base_ptr enode = end.mNode;
					while (true) {
						base_ptr prev = enode->pre;
						destroy_node(enode->as_node());
						if (prev == nullptr) {
							break;
						}
						enode = prev;
					}
					throw;
				}
				link_nodes(pos.mNode, r.mNode, end.mNode);
			}
			return r;
		}

		template <class Iter>
		iterator copy_insert(const_iterator pos, size_type n, Iter first)
		{
			iterator r(pos.mNode);
			if (n != 0) {
				size_type add_size = n;
				node_ptr p = create_node(*first);
				p->pre = nullptr;
				r = iterator(p);
				iterator end = r;
				try {
					for (first++, n--; n > 0; n--, first++, end++) {
						node_ptr node = create_node(*first);
						end.mNode->next = node->as_base();
						node->pre = end.mNode;
					}
					mSize += n;
				}
				catch (...) {
					base_ptr enode = end.mNode;
					while (true) {
						base_ptr prev = enode->pre;
						destroy_node(enode->as_node());
						if (prev == nullptr) {
							break;
						}
						enode = prev;
					}
					throw;
				}
				link_nodes(pos.mNode, r.mNode, end.mNode);
			}
			return r;
		}

		template <class Compared>
		void list_sort(iterator f1, iterator l2, size_type n, Compared cmp)
		{
			if (n <= 1) {
				return;
			}
			if (n == 2) {
				if (cmp(*--l2, *f1)) {
					auto ln = l2.mNode;
					unlink_node(ln, ln);
					link_nodes(f1.mNode, ln, ln);
					return l2;
				}
				return f1;
			}
			size_type n2 = n / 2;
			iterator l1 = f1;
			mystl::advance(l1, n2);
			iterator result = f1 = list_sort(f1, l1, n2, cmp);
			iterator f2 = l1 = list_sort(l1, l2, n - n2, cmp);

			if (cmp(*f2, *f1)) {
				iterator m = f2;
				++m;
				for (; m != l2 && cmp(*m, *f1); ++m);
				base_ptr f = f2.mNode;
				base_ptr l = m.mNode->pre;
				result = f2;
				l1 = f2 = m;
				unlink_node(f, l);
				m = f1;
				++m;
				link_nodes(f1.mNode, f, l);
				f1 = m;
			}
			else {
				++f1;
			}

			while (f1 != l1 && f2 != l2) {
				if (cmp(*f2, *f1)) {
					iterator m = f2;
					++m;
					for (; m != l2 && cmp(*m, *f1); ++m);
					base_ptr f = f2.mNode;
					base_ptr l = m.mNode->pre;
					if (l1 == f2) {
						l1 = m;
					}
					f2 = m;
					unlink_node(f, l);
					m = f1;
					++m;
					link_nodes(f1.mNode, f, l);
					f1 = m;
				}
				else {
					++f1;
				}
			}
			return result;
		}

		void unlink_node(base_ptr first, base_ptr last)
		{
			first->pre->next = last->next;
			last->next->pre = first->pre;
			first->pre = last->next = nullptr;
		}

	};


	template <class T>
	bool operator==(const list<T>& lhs, const list<T>& rhs)
	{
		auto f1 = lhs.cbegin(), l1 = lhs.cend();
		auto f2 = rhs.cbegin(), l2 = rhs.cend();
		for (; f1 != l1 && f2 != l2 && *f1 == *f2; ++f1, ++f2);
		return f1 == l1 && f2 == l2;
	}

	template <class T>
	bool operator<(const list<T>& lhs, const list<T>& rhs)
	{
		return lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
	}

	template <class T>
	bool operator!=(const list<T>& lhs, const list<T>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class T>
	bool operator>(const list<T>& lhs, const list<T>& rhs)
	{
		return rhs < lhs;
	}

	template <class T>
	bool operator>=(const list<T>& lhs, const list<T>& rhs)
	{
		return !(lhs < rhs);
	}

	template <class T>
	bool operator<=(const list<T>& lhs, const list<T>& rhs)
	{
		return !(lhs > rhs);
	}

	template <class T>
	void swap(list<T>& lhs, list<T>& rhs)
	{
		lhs.swap(rhs);
	}


}




#endif // !MYSTL_LIST_H
