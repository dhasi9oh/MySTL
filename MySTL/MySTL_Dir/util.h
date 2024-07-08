#ifndef MYSTL_UTIL_H
#define MYSTL_UTIL_H

#include <cstddef>
#include "type_traits.h"

namespace mystl {

	// noexcept(true) 表示该函数不会抛异常
	// noexcept(false) 表示该函数可能抛异常
	// noexcept 相当于 noexcept(true) 如果抛错的话进程会直接结束不会抛异常
	// 因为本来编译器会生成代码来提示抛错信息的，但声明noexcept后编译器不会声明这个函数的抛错代码
	// 因为不会生成这个函数的抛错代码，所以减小生成文件的大小


	// 根据引用折叠
	// 传入参数是左值或左值引用形参都是左值引用，函数为左值引用转右值引用
	// 传入参数是右值或右值引用形参都是右值引用，函数为右值引用转右值引用

	// 如果传入左值，那么就意味着放弃对该左值再次使用
	template <class T>
	typename std::remove_reference<T>::type&& move(T&& arg) noexcept {
		return static_cast<typename std::remove_reference<T>::type&&> (arg);
	}

	// 怎么理解完美转发？
	// 如果 T 是类型 Tp 的话 Tp&& 会折叠成 Tp&
	// 如果 T 是类型 Tp& 的话 Tp& && 会折叠成 Tp&
	// 如果 T 是类型 Tp&& 的话 Tp&& && 会折叠成 Tp&&
	// 在 static_cast<T&&> 里进行折叠
	template <class T>
	T&& forward(typename std::remove_reference<T>::type& arg) noexcept {
		return static_cast<T&&> (arg);
	}

	// 为什么这个函数需要加一个static_assert来弹错误呢？
	// 因为根据引用折叠，编译器是有可能选到这个模板的
	template <class T>
	T&& forward(typename std::remove_reference<T>::type&& arg) noexcept {
		static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
		return static_cast<T&&> (arg);
	}

	template <class T>
	void swap(T& lhs, T& rhs) {
		// 这里用move函数主要是触发移动构造和移动赋值，减小成本
		auto tmp(mystl::move(lhs));
		lhs = mystl::move(rhs);
		rhs = mystl::move(tmp);
	}

	template <class Iter,class iterator>
	iterator swap_range(Iter begin, Iter end, iterator first) {
		for (; begin != end; begin++, first++) {
			swap(*begin, *first);
		}
		return first;
	}


	template <class Tp,size_t N>
	void swap(Tp(&a)[N], Tp(&b)[N]) {
		mystl::swap_range(a, a + N, b);
	}


	template <class T1,class T2>
	class pair {
	public:
		
		typedef T1 first_type;
		typedef T2 second_type;

		first_type first;
		second_type second;
		

		// 模板类 std::enable_if<bool, T>::type
		// 如果bool为true则可以返回类型T,为false则不存在type，由编译器抛错
		 
		// 模板类std::is_constructible<T>::value是判断一个类型是否具备构造函数，如果具备则value为true

		// 模板类std::is_convertible<T1,T2>::value是判断T1是否可以转化为T2，如果可以则value为true


		// 默认构造
		template <class Other1 = first_type,class Other2 = second_type,
		typename std::enable_if<std::is_constructible<Other1>::value &&
			std::is_constructible<Other2>::value, void>::type>
		constexpr pair()
			: first(), second()
		{

		}

		template <class U1 = first_type,class U2 = second_type,
		typename std::enable_if<
			std::is_copy_constructible<U1>::value &&
			std::is_copy_constructible<U2>::value &&
			std::is_convertible<const U1&, first_type>::value &&
			std::is_convertible<const U2&, second_type>::value, int>::type = 0>
		constexpr pair(const first_type& a, const second_type& b) :
			first(a), second(b)
		{

		}

		template <class U1 = first_type, class U2 = second_type,
			typename std::enable_if<
			std::is_copy_constructible<U1>::value&&
			std::is_copy_constructible<U2>::value && (
				!std::is_convertible<const U1&, first_type>::value ||
				!std::is_convertible<const U2&, second_type>::value
				)
			, int>::type = 0>
		explicit constexpr pair(const first_type& a, const second_type& b) :
			first(a), second(b)
		{

		}


		pair(const pair&) = default;
		pair(pair&&) = default;


		template <class Other1,class Other2,
			typename std::enable_if<
			std::is_constructible<first_type, Other1>::value &&
			std::is_constructible<second_type, Other2>::value &&
			std::is_constructible<Other1&&, first_type>::value &&
			std::is_constructible<Other2&&, second_type>::value, int>::type = 0>
		constexpr pair(Other1&& a,Other2&& b) : 
			first(forward<Other1>(a)), second(forward<Other2>(b))
		{

		}

		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<first_type, Other1>::value &&
			std::is_constructible<second_type, Other2>::value && (
				!std::is_constructible<Other1&&, first_type>::value ||
				!std::is_constructible<Other2&&, second_type>::value
				), int>::type = 0>
		explicit constexpr pair(Other1&& a, Other2&& b) :
			first(forward<Other1>(a)), second(forward<Other2>(b))
		{

		}


		template <class Other1,class Other2,
			typename std::enable_if<
			std::is_constructible<const Other1&, first_type>::value &&
			std::is_constructible<const Other2&, second_type>::value &&
			std::is_convertible<const Other1&, first_type>::value &&
			std::is_convertible<const Other2&, second_type>::value, void>::type = 0>
		constexpr pair(const pair<Other1, Other2>& other) :
			first(other.first), second(other.second)
		{

		}


		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<const Other1&, first_type>::value &&
			std::is_constructible<const Other2&, second_type>::value &&
			(!std::is_convertible<const Other1&, first_type>::value ||
				!std::is_convertible<const Other2&, second_type>::value)
			, void>::type = 0>
		explicit constexpr pair(const pair<Other1, Other2>& other) :
			first(other.first), second(other.second)
		{

		}


		template <class Other1,class Other2,
			typename std::enable_if<
			std::is_constructible<Other1, first_type>::value &&
			std::is_constructible<Other2, second_type>::value &&
			std::is_convertible<Other1, first_type>::value &&
			std::is_convertible<Other2, second_type>::value, void>::type = 0>
		constexpr pair(const pair<Other1, Other2>&& other) :
			first(forward(other.first)), second(forward(other.second))
		{

		}


		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<Other1, first_type>::value&&
			std::is_constructible<Other2, second_type>::value&&
			(!std::is_convertible<Other1, first_type>::value ||
				!std::is_convertible<Other2, second_type>::value), void>::type = 0>
		explicit constexpr pair(pair<Other1, Other2>&& other) :
			first(forward(other.first)), second(forward(other.second))
		{

		}


		pair& operator=(pair&& rhs) {
			if (this != &rhs) {
				first = mystl::move(rhs.first);
				second = mystl::move(rhs.second);
			}
			return *this;
		}

		pair& operator=(const pair& rhs) {
			if (this != &rhs) {
				first = rhs.first;
				second = rhs.second;
			}
			return *this;
		}


		template <class Other1,class Other2> 
		typename pair<Other1, Other2>& operator=(const pair<Other1, Other2>& rhs) {
			first = mystl::forward<Other1>(rhs.first);
			second = mystl::forward<Other2>(rhs.second);
			return *this;
		}


		template <class Other1,class Other2>
		typename pair<Other1, Other2>& operator=(pair<Other1, Other2>&& rhs) {
			first = mystl::forward<Other1>(rhs.first);
			second = mystl::forward<Other2>(rhs.second);
			return *this;
		}

		~pair() = default;

		void swap(pair& rhs) {
			if (&rhs != this) {
				swap(first, rhs.first);
				swap(second, rhs.second);
			}
		}


	};
	template <class T1, class T2>
	bool operator==(const pair<T1, T2>& a, const pair<T1, T2>& b) {
		return (a.first == b.first && a.second == b.second);
	}

	template <class T1, class T2>
	bool operator!=(const pair<T1, T2>& a, const pair<T1, T2>& b) {
		return !(a == b);
	}

	template <class T1, class T2>
	bool operator<(const pair<T1, T2>& a, const pair<T1, T2>& b) {
		return (a.first < b.first || (a.first == b.first && a.second < b.second));
	}

	template <class T1, class T2>
	bool operator<=(const pair<T1, T2>& a, const pair<T1, T2>& b) {
		return (a < b) || (a == b);
	}

	template <class T1, class T2>
	bool operator>(const pair<T1, T2>& a, const pair<T1, T2>& b) {
		return !(a <= b);
	}

	template <class T1, class T2>
	bool operator>=(const pair<T1, T2>& a, const pair<T1, T2>& b) {
		return !(a < b);
	}

	template <class T1, class T2>
	void swap(pair<T1, T2>& a, pair<T1, T2>& b) {
		a.swap(b);
	}

	template <class T1, class T2>
	pair<T1, T2> make_pair(const T1& a, const T2& b) {
		return pair<T1, T2>(mystl::forward(a), mystl::forward(b));
	}

}



#endif // !MYSTL_UTIL_H
