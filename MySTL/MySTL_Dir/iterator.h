#ifndef MYSTL_ITERATOR_H
#define MYSTL_ITERATOR_H


#include <cstddef>
#include "type_traits.h"

namespace mystl
{
	// 五种迭代器类型
	class input_iterator_tag{};
	class output_iterator_tag{};
	class forward_iterator_tag: public input_iterator_tag{};
	class bidirectional_iterator_tag: public forward_iterator_tag{};
	class radom_access_iterator_tag: public bidirectional_iterator_tag{};

	// 用这个类继承上面的类来区分不同的迭代器
	template <class Category,class T,class Distance = ptrdiff_t,class Pointer = T*,class Reference = T&>
	class iterator {
	public:
		using value_type		= T;
		using pointer			= Pointer;
		using reference			= Reference;
		using different_type	= Distance;
		using iterator_category = Category;
	};
	
	// 判断这个类是不是一个迭代器
	template <class Iterator>
	class has_iterator_cat {
	private:
		struct two { char a; char b; };
		template <class T> static two test(...);
		template <class T> static char test(typename T::iterator_category* = nullptr);
	public:
		static constexpr bool value = (sizeof(test<Iterator>(0)) == sizeof(char));
	};

	// 这个类是做一个过滤
	template <class Iterator, bool>
	class iterator_traits_impl{};

	// 迭代器的基础
	template <class Iterator>
	class iterator_traits_impl <Iterator, true> {
	public:
		using pointer			= typename Iterator::pointer;
		using reference			= typename Iterator::reference;
		using value_type		= typename Iterator::value_type;
		using different_type	= typename Iterator::different_type;
		using iterator_category = typename Iterator::iterator_category;
	};

	// 如果不是一个迭代器就什么也不做
	template <class Iterator, bool>
	class iterator_traits_helper{};

	// 如果是一个输出或输入迭代器就继承它
	template <class Iterator>
	class iterator_traits_helper<Iterator, true>: public iterator_traits_impl<Iterator,
		std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
		std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>
	{};

	// 继承一个类，它是不是迭代器由模板判断
	template <class Iterator>
	class iterator_traits: public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

	// 对指针的特例化
	template <class T>
	class iterator_traits<T*> {
	public:
		using pointer			= T*;
		using reference			= T&;
		using value_type		= T;
		using different_type	= ptrdiff_t;
		using iterator_category = radom_access_iterator_tag;
	};

	// 对引用的特例化
	template <class T>
	class iterator_traits<T&> {
	public:
		using pointer			= T*;
		using reference			= T&;
		using value_type		= T;
		using different_type	= ptrdiff_t;
		using iterator_category = radom_access_iterator_tag;
	};

	// 对const指针的特例化
	template <class T>
	class iterator_traits<const T*> {
	public:
		using pointer			= T*;
		using reference			= T&;
		using value_type		= T;
		using different_type	= ptrdiff_t;
		using iterator_category = radom_access_iterator_tag;
	};

	// 判断类的基类
	template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
	class has_iterator_cat_of : public m_bool_constant<std::is_convertible
		<typename iterator_traits<T>::iterator_category, U>::value> {};


	template <class T,class U>
	class has_iterator_cat_of<T, U, false> : public m_false_tpye {};

	template <class Iter>
	class is_input_iterator : public has_iterator_cat_of<Iter, mystl::input_iterator_tag> {};

	template <class Iter>
	class is_output_iterator : public has_iterator_cat_of<Iter, mystl::output_iterator_tag> {};

	template <class Iter>
	class is_forward_iterator : public has_iterator_cat_of<Iter, mystl::forward_iterator_tag> {};

	template <class Iter>
	class is_bidirectional_iterator : public has_iterator_cat_of<Iter, mystl::bidirectional_iterator_tag> {};

	template <class Iter>
	class is_radom_access_iterator : public has_iterator_cat_of<Iter, mystl::radom_access_iterator_tag> {};

	// 目的是返回一个迭代器的类型，通过这个类型去调用其它函数
	template <class Iter>
	typename iterator_traits<Iter>::iterator_category iterator_category(const Iter&) {
		using Category = iterator_traits<Iter>::iterator_category;
		return Category();
	}

	template <class Iter>
	typename iterator_traits<Iter>::different_type* distance_type(const Iter&) {
		return static_cast<iterator_traits<Iter>::different_type*>(0);
	}

	template <class Iter>
	typename iterator_traits<Iter>::value_type* value_type(const Iter&) {
		return static_cast<iterator_traits<Iter>::value_type*>(0);
	}

	// 链表的长度
	template <class Iter>
	typename iterator_traits<Iter>::different_type distance_dispatch(Iter first, Iter end, input_iterator_tag) {
		typename iterator_traits<Iter>::different_type size = 0;
		while (first != end) {
			++first;
			size++;
		}
		return size;
	}

	// 数组的长度
	template <class Iter>
	typename iterator_traits<Iter>::different_type distance_dispatch(Iter first, Iter end, radom_access_iterator_tag) {
		return end - first;
	}

	// 通过迭代器类型决定调用哪个函数
	template <class Iter>
	typename iterator_traits<Iter>::different_type distance(Iter first, Iter end) {
		return distance_dispatch(first, end, iterator_category(first));
	}

	template <class Iter,class Distance>
	void advance_dispath(Iter &i,Distance n,input_iterator_tag) {
		while (n--) {
			++i;
		}
	}

	template <class Iter,class Distance>
	void advance_dispath(Iter &i, Distance n, bidirectional_iterator_tag) {
		if (n > 0) {
			while (n--) ++i;
		}
		else {
			while (n++) --i;
		}
	}

	template <class Iter, class Distance>
	void advance_dispath(Iter &i, Distance n, radom_access_iterator_tag) {
		i += n;
	}


	template <class Iter, class Distance>
	void advance(Iter &i, Distance n) {
		advance_dispath(i, n, iterator_category(i));
	}


	/*------------------------------------------------------------------------------------------*/
	// 反向迭代器

	

	template <class Iter>
	class reverse_iterator {
	private:
		Iter cur;

	public:

		// 以后尽量用typedef，不要用using，容易报错
		using iterator_type		= typename Iter;
		typedef typename reverse_iterator<typename Iter>					self;
		typedef typename iterator_traits<typename Iter>::pointer			pointer;
		typedef typename iterator_traits<typename Iter>::reference			reference;
		typedef typename iterator_traits<typename Iter>::value_type			value_type;
		typedef typename iterator_traits<typename Iter>::different_type		different_type;
		typedef typename iterator_traits<typename Iter>::iterator_category	iterator_category;

		/*using self				= typename reverse_iterator<Iter>;
		template <class Iter>
		using pointer			= typename iterator_traits<Iter>::pointer;
		template <class Iter>
		using reference			= typename iterator_traits<Iter>::reference;
		template <class Iter>
		using value_type		= typename iterator_traits<Iter>::value_type;
		template <class Iter>
		using different_type	= typename iterator_traits<Iter>::different_type;
		template <class Iter>
		using iterator_category = typename iterator_traits<Iter>::iterator_category;*/

	public:

		reverse_iterator() {}

		reverse_iterator(const self& rhs) { cur = rhs.cur; }

		reverse_iterator(Iter i) : cur(i) { }

	public:

		iterator_type base() const { return cur; }

		reference operator*() const { auto iter = cur; return *(--iter); }

		pointer operator->() const { return &(operator*()); }

		self& operator++() { --cur; return *(this); }

		self operator++(int) { auto iter = *this; operator++(); return iter; }

		self& operator--() { ++cur; return *(this); }

		self operator--(int) { auto iter = *this; operator--(); return iter; }

		self operator+(different_type i) { return self(cur - i); }

		self operator-(different_type i) { return self(cur + i); }

		self& operator+=(different_type i) { cur -= i; return *this; }

		self& operator-=(different_type i) { cur += i; return *this; }

		reference operator[](different_type i) { return *(*this + i); }

	};

	template <class Iter>
	typename reverse_iterator<Iter>::different_type
		operator-(Iter lhs, Iter rhs) {
		return rhs.base() - lhs.base();
	}

	template <class Iter>
	typename bool operator==(Iter lhs, Iter rhs) {
		return rhs.base() == lhs.base();
	}

	template <class Iter>
	typename bool operator!=(Iter lhs, Iter rhs) {
		return !operator==(lhs, rhs);
	}

	template <class Iter>
	typename bool operator<(Iter lhs, Iter rhs) {
		return lhs.base() < rhs.base();
	}

	template <class Iter>
	typename bool operator<=(Iter lhs, Iter rhs) {
		return operator==(lhs, rhs) || operator<(lhs, rhs);
	}

	template <class Iter>
	typename bool operator>(Iter lhs, Iter rhs) {
		return !operator<=(lhs, rhs);
	}

	template <class Iter>
	typename bool operator>=(Iter lhs, Iter rhs) {
		return !operator<(lhs, rhs);
	}
}

#endif // !MYSTL_ITERATOR_H
