#ifndef MYSTL_UTIL_H
#define MYSTL_UTIL_H

#include <cstddef>
#include "type_traits.h"

namespace mystl {

	// noexcept(true) ��ʾ�ú����������쳣
	// noexcept(false) ��ʾ�ú����������쳣
	// noexcept �൱�� noexcept(true) ����״�Ļ����̻�ֱ�ӽ����������쳣
	// ��Ϊ���������������ɴ�������ʾ�״���Ϣ�ģ�������noexcept�������������������������״����
	// ��Ϊ������������������״���룬���Լ�С�����ļ��Ĵ�С


	// ���������۵�
	// �����������ֵ����ֵ�����βζ�����ֵ���ã�����Ϊ��ֵ����ת��ֵ����
	// �����������ֵ����ֵ�����βζ�����ֵ���ã�����Ϊ��ֵ����ת��ֵ����

	// ���������ֵ����ô����ζ�ŷ����Ը���ֵ�ٴ�ʹ��
	template <class T>
	typename std::remove_reference<T>::type&& move(T&& arg) noexcept {
		return static_cast<typename std::remove_reference<T>::type&&> (arg);
	}

	// ��ô�������ת����
	// ��� T ������ Tp �Ļ� Tp&& ���۵��� Tp&
	// ��� T ������ Tp& �Ļ� Tp& && ���۵��� Tp&
	// ��� T ������ Tp&& �Ļ� Tp&& && ���۵��� Tp&&
	// �� static_cast<T&&> ������۵�
	template <class T>
	T&& forward(typename std::remove_reference<T>::type& arg) noexcept {
		return static_cast<T&&> (arg);
	}

	// Ϊʲô���������Ҫ��һ��static_assert���������أ�
	// ��Ϊ���������۵������������п���ѡ�����ģ���
	template <class T>
	T&& forward(typename std::remove_reference<T>::type&& arg) noexcept {
		static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
		return static_cast<T&&> (arg);
	}

	template <class T>
	void swap(T& lhs, T& rhs) {
		// ������move������Ҫ�Ǵ����ƶ�������ƶ���ֵ����С�ɱ�
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
		

		// ģ���� std::enable_if<bool, T>::type
		// ���boolΪtrue����Է�������T,Ϊfalse�򲻴���type���ɱ������״�
		 
		// ģ����std::is_constructible<T>::value���ж�һ�������Ƿ�߱����캯��������߱���valueΪtrue

		// ģ����std::is_convertible<T1,T2>::value���ж�T1�Ƿ����ת��ΪT2�����������valueΪtrue


		// Ĭ�Ϲ���
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
