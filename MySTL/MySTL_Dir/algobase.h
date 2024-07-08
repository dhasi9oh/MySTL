#ifndef MYSTL_ALGOBASE_H
#define MYSTL_ALGOBASE_H

// 这个文件是一些填充和拷贝的算法
#include <cstring>

#include "iterator.h"
#include "util.h"

namespace mystl {

#ifdef max
#pragma message("#undefing marco max")
#undef
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef
#endif // min


	template <class T>
	const T& max(const T& a, const T& b) {
		return a < b ? b : a;
	}


	template <class T, class Compare>
	const T& max(const T& a, const T& b, Compare cmp) {
		return cmp(a, b) ? b : a;
	}

	template <class T>
	const T& min(const T& a, const T& b) {
		return a < b ? a : b;
	}

	template <class T, class Compare>
	const T& min(const T& a, const T& b, Compare cmp) {
		return cmp(a, b) ? a : b;
	}

	
	template <class Iter1, class Iter2>
	void iter_swap(Iter1 a, Iter2 b) {
		mystl::swap(*a, *b);
	}

	template <class InputIter, class OutIter>
	OutIter
		unchecked_copy_cat(InputIter first, InputIter last, OutIter begin,
			mystl::input_iterator_tag)
	{
		for (; first != last; first++, begin++) {
			*begin = *first;
		}
		return begin;
	}

	template <class InputIter, class OutIter>
	OutIter
		unchecked_copy_cat(InputIter first, InputIter last, OutIter begin,
			mystl::radom_access_iterator_tag)
	{
		// 这里用减法不像上面那样我觉得是为了更安全
		for (size_t len = last - first; len > 0; len--, first++, begin++) {
			*begin = *first;
		}
		return begin;
	}

	template <class InputIter, class OutIter>
	OutIter
		unchecked_copy(InputIter first, InputIter last, OutIter begin)
	{
		return unchecked_copy_cat(first, last, begin, mystl::iterator_category(first));
	}


	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
		std::is_trivially_copy_assignable<Up>::value,
		Up*>::type
		unchecked_copy(Tp* first, Tp* last, Up* result)
	{
		const auto n = static_cast<size_t> (last - first);
		if (n != 0) {
			// 这个函数和 memcpy() 一样，但比mencpy更安全
			// 考虑到了重叠的问题
			std::memmove(result, first, n * sizeof(Up));
		}
		return result + n;
	}

	template <class InputIter, class OutIter>
	OutIter
		copy(InputIter first, InputIter last, OutIter begin) 
	{
		return unchecked_copy(first, last, begin);
	}

	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last, 
			BidirectionalIter2 result, mystl::bidirectional_iterator_tag)
	{
		while (first != last) {
			*(--result) = *(--last);
		}
		return result;
	}


	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result, mystl::radom_access_iterator_tag)
	{
		for (size_t i = 1; i <= last - first; i++) {
			*(result - i) = *(last - i);
		}
		return result;
	}


	template <class Iter1, class Iter2>
	Iter2
		unchecked_copy_backward(Iter1 first, Iter1 last, Iter2 begin) 
	{
		return unchecked_copy_backward_cat(first, last, begin, mystl::iterator_category(first));
	}


	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<Tp, Up>::value &&
		std::is_copy_assignable<Up>::value,
		Up*>::type
		unchecked_copy_backward(Tp* first, Tp* last, Up* begin)
	{
		const auto n = static_cast<size_t> (last - first);
		if (n != 0) {
			begin -= n;
			std::memmove(begin, first, n * sizeof(Up));
		}
		return begin;
	}


	template <class BidrectionalIter1, class BidrectionalIter2>
	BidrectionalIter2
		copy_backward(BidrectionalIter1 first, BidrectionalIter1 last, BidrectionalIter2 begin)
	{
		return unchecked_copy_backward(first, last, begin);
	}


	template <class InputIter, class OutIter, class UnaryPredicate>
	OutIter
		copy_if(InputIter first, InputIter last, OutIter begin, UnaryPredicate unary_pred)
	{
		for (; first != last; first++) {
			if (unary_pred(*first)) {
				*begin = *first;
				begin++;
			}
		}
		return begin;
	}


	template <class InputIter, class Size, class OutIter>
	mystl::pair<InputIter, OutIter>
		uncheck_copy_n(InputIter first, Size n, OutIter begin, mystl::input_iterator_tag)
	{
		for (; n > 0; n--, first++, begin++) {
			*begin = *first;
		}
		return mystl::make_pair<InputIter, OutIter>(first, begin);
	}

	template <class InputIter, class Size, class OutIter>
	mystl::pair<InputIter, OutIter>
		uncheck_copy_n(InputIter first, Size n, OutIter begin, mystl::radom_access_iterator_tag)
	{
		auto result = first + n;
		return mystl::make_pair<InputIter, OutIter>(result, mystl::copy(first, result, begin));
	}

	template <class InputIter, class Size, class OutIter>
	mystl::pair<InputIter, OutIter>
		copy_n(InputIter first, Size n, OutIter begin)
	{
		return unchecked_copy(first, n, begin, mystl::iterator_category(first));
	}


	template <class RadomIter, class OutputIter>
	OutputIter
		unchecked_move_cat(RadomIter first, RadomIter last, OutputIter begin,
			mystl::input_iterator_tag)
	{
		for (; first != last; first++, begin++) {
			*begin = mystl::move(*first);
		}
		return begin;
	}


	template <class InputIter, class OutIter>
	OutIter
		unchecked_move_cat(InputIter first, InputIter last, OutIter begin,
			mystl::radom_access_iterator_tag)
	{
		for (auto n = last - first; n > 0; n--, first++, begin++) {
			*begin = mystl::move(*first);
		}
		return begin;
	}


	template <class InputIter, class OutIter>
	OutIter
		unchecked_move(InputIter first, InputIter last, OutIter begin)
	{
		return unchecked_copy_cat(first, last, begin, mystl::iterator_category(first));
	}

	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
		std::is_trivially_copy_assignable<Up>::value,
		Up*>::type
		unchecked_move(Tp* first, Tp* last, Up* begin)
	{
		const size_t n = static_cast<size_t> (last - first);
		if (n != 0) {
			std::memmove(begin, first, n * sizeof(Up));
		}
		return begin + n;
	}


	template <class InputIter, class OutIter>
	OutIter move(InputIter first, InputIter last, OutIter begin)
	{
		return unchecked_move(first, last, begin);
	}


	template <class BidrectionalIter1, class BidrectionalIter2>
	BidrectionalIter2
		uncheck_move_backward_cat(BidrectionalIter1 first, BidrectionalIter1 last,
			BidrectionalIter2 begin, mystl::bidirectional_iterator_tag)
	{
		while (first != last) {
			*(--begin) = mystl::move(*(--last));
		}
		return begin;
	}


	template <class RandomIter1, class RandomIter2>
	RandomIter2
		uncheck_move_backward_cat(RandomIter1 first, RandomIter1 last,
			RandomIter2 being, mystl::radom_access_iterator_tag)
	{
		for (auto n = last - first; n > 0; n--, last--, begin--) {
			*(--begin) = mystl::move(*(--last));
		}
		return begin;
	}


	template <class Iter1, class Iter2>
	Iter2
		uncheck_move_backward(Iter1 first, Iter1 last, Iter2 begin)
	{
		return uncheck_move_backward_cat(first, last, being, mystl::iterator_category(first));
	}


	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
		std::is_trivially_move_assignable<Tp>::value,
		Up*>::type
		uncheck_move_backward(Tp* first, Tp* last, Up* begin)
	{
		const auto n = static_cast<size_t> last - first;
		if (n != 0) {
			std::memmove(being, first, n * sizeof(Up));
		}
		return begin;
	}

	template <class InputIter, class OutIter>
	OutIter move_backward(InputIter first, InputIter last, OutIter begin)
	{
		return uncheck_move_backward(first, last, begin);
	}


	template <class Iter1, class Iter2>
	bool equal(Iter1 first, Iter1 last, Iter2 begin)
	{
		for (; first != last; first++, begin++) {
			if (*first != *begin) {
				return false;
			}
		}
		return true;
	}

	template <class Iter1, class Iter2, class Cmpare>
	bool equal(Iter1 first, Iter1 last, Iter2 begin, Cmpare cmp)
	{
		for (; first != last; first++, begin++) {
			if (!cmp(*first, *begin)) {
				return false;
			}
		}
		return true;
	}


	template <class Iter, class Size, class T>
	Iter unchecked_fill_n(Iter first, Size n, const T& value)
	{
		for (; n > 0; n--, first++) {
			*first = value;
		}
		return first;
	}
	

	// 如果是一个空类
	template <class Tp, class Size, class Up>
	typename std::enable_if<
		std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
		!std::is_same<Tp, bool>::value &&
		std::is_integral<Up>::value && sizeof(Up) == 1,
		Tp*>::type
		unchecked_fill_n(Tp* first, Size n, Up value)
	{
		if (n > 0) {
			std::memset(first, (unsigned char)value, (size_t)n);
		}
		return first + n;
	}


	template <class Iter, class Size, class T>
	Iter fill_n(Iter first, Size n, const T& value)
	{
		return unchecked_fill_n(first, n, value);
	}


	template <class ForwardIter, class T>
	void fill_cat(ForwardIter first, ForwardIter last, const T& value, 
		mystl::input_iterator_tag)
	{
		for (; first != last; first++) {
			*first = value;
		}
	}


	template <class RadomIter, class T>
	void fill_cat(RadomIter first, RadomIter last, const T& value,
		mystl::radom_access_iterator_tag)
	{
		fill_n(first, last - first, value);
	}


	template <class Iter, class T>
	void fill(Iter first, Iter last, const T& value)
	{
		fill_cat(first, last, value, mystl::iterator_category(Iter));
	}


	template<class Iter1, class Iter2>
	bool lexicographical_compare(Iter1 first, Iter1 last, Iter2 begin, Iter2 end)
	{
		for (; first != last && begin != end; first++, begin++) {
			if (*first < *begin) {
				return true;
			}
			if (*first > *begin) {
				return false;
			}
		}
		return first == last && begin != end;
	}


	template<class Iter1, class Iter2, class Cmpare>
	bool lexicographical_compare(Iter1 first, Iter1 last, Iter2 begin, Iter2 end, Cmpare cmp)
	{
		for (; first != last && begin != end; first++, begin++) {
			if (cmp(*first, *begin)) {
				return true;
			}
			if (!cmp(*begin, *first)) {
				return false;
			}
		}
		return first == last && begin != end;
	}


	bool lexicographical_compare(const unsigned char* first1,
		const unsigned char* last1,
		const unsigned char* first2,
		const unsigned char* last2)
	{
		size_t n = last1 - first1;
		size_t m = last2 - first2;
		const auto result = std::memcmp(first1, first2, min(n, m));
		return result != 0 ? result < 0 : n < m;
	}


	template <class Iter1, class Iter2>
	mystl::pair<Iter1, Iter2>
		mismatch(Iter1 first, Iter1 last, Iter2 begin)
	{
		while (first != last && *begin == *first)
		{
			first++;
			begin++;
		}
		return mystl::make_pair<Iter1, Iter2>(first, begin);
	}


	template <class Iter1, class Iter2, class Compred>
	mystl::pair<Iter1, Iter2>
		mismatch(Iter1 first, Iter1 last, Iter2 begin, Compred cmp)
	{
		while (first != last && cmp(*begin, *first))
		{
			first++;
			begin++;
		}
		return mystl::make_pair<Iter1, Iter2>(first, begin);
	}



}




#endif // !MYSTL_ALGOBASE_H
