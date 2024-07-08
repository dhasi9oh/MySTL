#ifndef MYSTL_ALGO_H
#define MYSTL_ALGO_H


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#endif


#include <ctime>
#include <cstddef>

#include "algobase.h"
#include "memory.h"
#include "heap_algo.h"
#include "functional.h"


namespace mystl {


	template <class InputIter, class UnaryPredicate>
	bool all_of(InputIter first, InputIter last, UnaryPredicate unary_pred)
	{
		for (; first != last; first++) {
			if (!unary_pred(*first)) {
				return false;
			}
		}
		return true;
	}

	template <class InputIter, class UnaryPredicate>
	bool any_of(InputIter first, InputIter last, UnaryPredicate unary_pred)
	{
		for (; first != last; first++) {
			if (unary_pred(*first)) {
				return true;
			}
		}
		return false;
	}

	template <class InputIter, class UnaryPredicate>
	bool none_of(InputIter first, InputIter last, UnaryPredicate unary_pred)
	{
		for (; first != last; first++) {
			if (unary_pred(*first)) {
				return false;
			}
		}
		return true;
	}

	template <class InputIter, class T>
	size_t count(InputIter first, InputIter last, const T& value)
	{
		size_t cnt = 0;
		for (; first != last; first++) {
			if (*first == value) {
				cnt++;
			}
		}
		return cnt;
	}

	template <class InputIter, class UnaryPredicate>
	size_t count_if(InputIter first, InputIter last, UnaryPredicate unary_pred)
	{
		size_t cnt = 0;
		for (; first != last; first++) {
			if (unary_pred(*first)) {
				cnt++;
			}
		}
		return cnt;
	}

	template <class InputIter, class T>
	InputIter find(InputIter first, InputIter last, const T& value)
	{
		while (first != last && *first != value) {
			first++;
		}
		return first;
	}

	template <class InputIter, class UnaryPredicate>
	InputIter find_if(InputIter first, InputIter last, UnaryPredicate unary_pred)
	{
		while (first != last && !unary_pred(*first)) {
			first++;
		}
		return first;
	}

	template <class InputIter, class UnaryPredicate>
	InputIter find_if_not(InputIter first, InputIter last, UnaryPredicate unary_pred)
	{
		while (first != last && unary_pred(*first)) {
			first++;
		}
		return first;
	}

	// 第一个范围中是否包含的第二个范围所有数并且是连续的
	template <class ForwardIter1, class ForwardIter2>
	ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2)
	{
		auto len1 = mystl::distance(first1, last1);
		auto len2 = mystl::distance(first2, last2);
		if (len1 < len2) {
			return first1;
		}
		auto cur1 = first1;
		auto cur2 = first2;
		while (cur2 != last2) {
			if (*cur1 == *cur2) {
				++cur1, ++cur2;
			}
			else {
				if (len1 == len2) {
					return last1;
				}
				else {
					cur1 = ++first1;
					cur2 = first2;
					--len1;
				}
			}
		}
		return first1;
	}

	template <class ForwardIter1, class ForwardIter2, class Compared>
	ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2, Compared cmp)
	{
		auto len1 = mystl::distance(first1, last1);
		auto len2 = mystl::distance(first2, last2);
		if (len1 < len2) {
			return first1;
		}
		auto cur1 = first1;
		auto cur2 = first2;
		while (cur2 != last2) {
			if (cmp(*cur1, *cur2)) {
				++cur1, ++cur2;
			}
			else {
				if (len1 == len2) {
					return last1;
				}
				else {
					cur1 = ++first1;
					cur2 = first2;
					--len1;
				}
			}
		}
		return first1;
	}

	//第一个范围中是否连续包含n个value
	template <class ForwardIter, class Size, class T>
	ForwardIter search_n(ForwardIter first, ForwardIter last, Size n, const T& value)
	{
		if (n <= 0) {
			return first;
		}
		else {
			first = mystl::find(first, last, value);
			while (first != last) {
				auto m = n - 1;
				auto i = first;
				++i;
				while (i != last && m != 0 && *i == value) {
					++i;
					--m;
				}
				if (m == 0) {
					return first;
				}
				else {
					first = mystl::find(i, last, value);
				}
			}
		}
		return first;
	}


	template <class ForwardIter, class Size, class T, class Compare>
	ForwardIter search_n(ForwardIter first, ForwardIter last, Size n, const T& value, Compare cmp)
	{
		if (n <= 0) {
			return first;
		}
		else {
			while (first != last) {
				if (cmp(*first, value)) {
					break;
				}
				first++;
			}
			while (first != last) {
				auto m = n - 1;
				auto i = first;
				++i;
				while (i != last && m != 0 && cmp(*i, value)) {
					++i;
					--m;
				}
				if (m == 0) {
					return first;
				}
				else {
					while (i != last) {
						if (cmp(*i, value)) {
							break;
						}
						i++;
					}
					first = i;
				}
			}
		}
		return first;
	}

	// 范围一中最后一组包含范围二的位置
	template <class ForwardIter1, class ForwardIter2>
	ForwardIter1 find_end_dispatch(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2,
		forward_iterator_tag, forward_iterator_tag)
	{
		if (first2 == last2) {
			return last1;
		}
		else {
			auto result = last1;
			while (true) {
				auto new_result = mystl::search(first1, last1, first2, last2);
				if (new_result == last1) {
					return result;
				}
				else {
					first1 = new_result;
					result = new_result;
					++first1;
				}
			}
		}
	}


	template <class BidirectionalIter1, class BidrectionalIter2>
	BidirectionalIter1 find_end_dispatch(BidirectionalIter1 first1, BidirectionalIter1 last1,
		BidrectionalIter2 first2, BidrectionalIter2 last2,
		bidirectional_iterator_tag, bidirectional_iterator_tag)
	{
		if (first2 == last2) {
			return last1;
		}
		mystl::reverse_iterator<BidirectionalIter1> rlast1 = first1;
		mystl::reverse_iterator<BidirectionalIter1> rlast2 = first2;
		auto rresult = search(mystl::reverse_iterator<BidirectionalIter1>(last1), rlast1,
			mystl::reverse_iterator<BidirectionalIter1>(last2), rlast2);
		if (rresult == rlast1) {
			return last1;
		}
		else {
			auto begin = rresult.base();
			mystl::advance(begin, -mystl::distance(first2, last2));
			return begin;
		}
	}


	template <class Iter1, class Iter2>
	Iter1 find_end(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2)
	{
		return mystl::find_end_dispatch(first1, last1, 
			first2, last2, iterator_category(first1), iterator_category(first2));
	}


	template <class ForwardIter1, class ForwardIter2, class Compared>
	ForwardIter1 find_end_dispatch(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2,
		forward_iterator_tag, forward_iterator_tag,
		Compared cmp)
	{
		if (first2 == last2) {
			return last1;
		}
		else {
			auto result = last1;
			while (true) {
				auto new_result = mystl::search(first1, last1, first2, last2, cmp);
				if (new_result == last1) {
					return result;
				}
				else {
					result = new_result;
					first1 = new_result;
					++first1;
				}
			}
		}
	}


	template <class BidirectionalIter1, class BidrectionalIter2, class Compared>
	BidirectionalIter1 find_end_dispatch(BidirectionalIter1 first1, BidirectionalIter1 last1,
		BidrectionalIter2 first2, BidrectionalIter2 last2,
		bidirectional_iterator_tag, bidirectional_iterator_tag,
		Compared cmp)
	{
		if (first2 == last2) {
			return last1;
		}
		mystl::reverse_iterator<BidirectionalIter1> rlast1 = first1;
		mystl::reverse_iterator<BidirectionalIter1> rlast2 = first2;
		auto rresult = search(mystl::reverse_iterator<BidirectionalIter1>(last1), rlast1,
			mystl::reverse_iterator<BidirectionalIter1>(last2), rlast2, cmp);
		if (rresult == rlast1) {
			return last1;
		}
		else {
			auto begin = rresult.base();
			mystl::advance(begin, -mystl::distance(first2, last2));
			return begin;
		}
	}


	template <class Iter1, class Iter2, class Compared>
	Iter1 find_end(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2, Compared cmp)
	{
		return mystl::find_end_dispatch(first1, last1,
			first2, last2, iterator_category(first1), iterator_category(first2), cmp);
	}

	//输出范围一中第一个在范围二中的数
	template <class InputIter, class ForwardIter>
	InputIter find_first_of(InputIter first1, InputIter last1,
		ForwardIter first2, ForwardIter last2)
	{
		for (; first1 != last1; first1++) {
			for (auto iter = first2; iter != last2; iter++) {
				if (*iter == *first1) {
					return first1;
				}
			}
		}
		return last1;
	}


	template <class InputIter, class ForwardIter, class Compared>
	InputIter find_first_of(InputIter first1, InputIter last1,
		ForwardIter first2, ForwardIter last2, Compared cmp)
	{
		for (; first1 != last1; first1++) {
			for (auto iter = first2; iter != last2; iter++) {
				if (cmp(*iter, *first1)) {
					return first1;
				}
			}
		}
		return last1;
	}


	template <class Iter, class Function>
	Function for_each(Iter first, Iter last, Function f)
	{
		for (; first != last; first++) {
			f(*first);
		}
		return f;
	}

	//找相邻并返回位置
	template <class ForwardIter>
	ForwardIter adjacent_find(ForwardIter first, ForwardIter last)
	{
		if (first != last) {
			return last;
		}
		auto next = first;
		next++;
		while (next != last) {
			if (*first == *next) {
				return first;
			}
			first = next++;
		}
		return last;
	}


	template <class ForwardIter, class Compared>
	ForwardIter adjacent_find(ForwardIter first, ForwardIter last, Compared cmp)
	{
		if (first != last) {
			return last;
		}
		auto next = first;
		next++;
		while (next != last) {
			if (cmp(*first, *next)) {
				return first;
			}
			first = next++;
		}
		return last;
	}

	// 找一个大于等于value的数的位置，该算法要求已经排序
	template <class ForwardIter, class T>
	ForwardIter lbound_dispatch(ForwardIter first, ForwardIter last,
		const T& value, forward_iterator_tag)
	{
		auto len = mystl::distance(first, last);
		auto half = len;
		ForwardIter middle;
		while (len > 0) {
			half = len >> 1;
			middle = first;
			mystl::advance(middle, half);
			if (*middle < value) {
				first = middle;
				first++;
				len = len - half - 1;
			}
			else {
				len = half;
			}
		}
		return first;
	}


	template <class ForwardIter, class T>
	ForwardIter lbound_dispatch(ForwardIter first, ForwardIter last,
		const T& value, radom_access_iterator_tag)
	{
		auto len = last - first;
		auto half = len;
		ForwardIter middle;
		while (len > 0) {
			half = len >> 1;
			middle = first + half;
			if (*middle < value) {
				first = middle + 1;
				len = len - half - 1;
			}
			else {
				len = half;
			}
		}
		return first;
	}


	template <class Iter, class T>
	Iter lower_bound(Iter first, Iter last, const T& value)
	{
		return mystl::lbound_dispatch(first, last, value, mystl::iterator_category(first));
	}


	template <class ForwardIter, class T, class Compared>
	ForwardIter lbound_dispatch(ForwardIter first, ForwardIter last,
		const T& value, forward_iterator_tag, Compared cmp)
	{
		auto len = mystl::distance(first, last);
		auto half = len;
		ForwardIter middle;
		while (len > 0) {
			half = len >> 1;
			middle = first;
			mystl::advance(middle, half);
			if (cmp(*middle, value)) {
				first = middle;
				first++;
				len = len - half - 1;
			}
			else {
				len = half;
			}
		}
		return first;
	}


	template <class ForwardIter, class T, class Compared>
	ForwardIter lbound_dispatch(ForwardIter first, ForwardIter last,
		const T& value, radom_access_iterator_tag, Compared cmp)
	{
		auto len = last - first;
		auto half = len;
		ForwardIter middle;
		while (len > 0) {
			half = len >> 1;
			middle = first + half;
			if (cmp(*middle, value)) {
				first = middle + 1;
				len = len - half - 1;
			}
			else {
				len = half;
			}
		}
		return first;
	}


	template <class Iter, class T, class Compared>
	Iter lower_bound(Iter first, Iter last, const T& value, Compared cmp)
	{
		return mystl::lbound_dispatch(first, last, value, mystl::iterator_category(first), cmp);
	}


	template <class ForwardIter, class T>
	ForwardIter ubound_dispatch(ForwardIter first, ForwardIter last,
		const T& value, forward_iterator_tag)
	{
		auto len = mystl::distance(first, last);
		auto half = len;
		ForwardIter middle;
		while (len > 0) {
			half = len >> 1;
			middle = first;
			mystl::advance(middle, half);
			if (value < *middle) {
				len = half;
			}
			else {
				first = middle;
				first++;
				len = len - half - 1;
			}
		}
		return first;
	}

	// 找到第一个大于value的数的位置，要求排序
	template <class ForwardIter, class T>
	ForwardIter ubound_dispatch(ForwardIter first, ForwardIter last,
		const T& value, radom_access_iterator_tag)
	{
		auto len = last - first;
		auto half = len;
		ForwardIter middle;
		while (len > 0) {
			half = len >> 1;
			middle = first + half;
			if (value < *middle) {
				len = half;
			}
			else {
				first = middle + 1;
				len = len - half - 1;
			}
		}
		return first;
	}


	template <class Iter, class T>
	Iter upper_bound(Iter first, Iter last, const T& value)
	{
		return mystl::ubound_dispatch(first, last, value, iterator_category(first));
	}


	template <class ForwardIter, class T, class Compared>
	ForwardIter ubound_dispatch(ForwardIter first, ForwardIter last,
		const T& value, forward_iterator_tag, Compared cmp)
	{
		auto len = mystl::distance(first, last);
		auto half = len;
		ForwardIter middle;
		while (len > 0) {
			half = len >> 1;
			middle = first;
			mystl::advance(middle, half);
			if (cmp(value, *middle)) {
				len = half;
			}
			else {
				first = middle;
				first++;
				len = len - half - 1;
			}
		}
		return first;
	}


	template <class ForwardIter, class T, class Compared>
	ForwardIter ubound_dispatch(ForwardIter first, ForwardIter last,
		const T& value, radom_access_iterator_tag, Compared cmp)
	{
		auto len = last - first;
		auto half = len;
		ForwardIter middle;
		while (len > 0) {
			half = len >> 1;
			middle = first + half;
			if (cmp(value, *middle)) {
				len = half;
			}
			else {
				first = middle + 1;
				len = len - half - 1;
			}
		}
		return first;
	}


	template <class Iter, class T, class Compared>
	Iter upper_bound(Iter first, Iter last, const T& value, Compared cmp)
	{
		return mystl::ubound_dispatch(first, last, value, iterator_category(first), cmp);
	}

	// 二分查找看范围中是否存在value
	template <class Iter, class T>
	bool binaty_search(Iter first, Iter last, const T& value)
	{
		auto iter = mystl::lower_bound(first, last, value);
		return iter != last && !(value < *iter);
	}


	template <class Iter, class T, class Compared>
	bool binaty_search(Iter first, Iter last, const T& value, Compared cmp)
	{
		auto iter = mystl::lower_bound(first, last, value);
		return iter != last && !cmp(value, *iter);
	}

	// 返回一个指向大于等于value的值的位置和一个大于value值的位置，要求已经排序
	template <class ForwardIter, class T>
	mystl::pair<ForwardIter, ForwardIter>
		erange_dispatch(ForwardIter first, ForwardIter last,
			const T& value, forward_iterator_tag)
	{
		auto len = mystl::distance(first, last);
		ForwardIter middle;
		while (len > 0) {
			auto size = len >> 1;
			middle = first;
			mystl::advance(middle, size);
			if (value < *middle) {
				len = size;
			}
			else if (value > *middle) {
				first = middle;
				first++;
				len = len - size - 1;
			}
			else {
				ForwardIter left = mystl::lower_bound(first, last, value);
				mystl::advance(first, size);
				ForwardIter right = mystl::upper_bound(++middle, first, value);
				return mystl::pair<ForwardIter, ForwardIter>(left, right);
			}
		}
		return mystl::pair<ForwardIter, ForwardIter>(last, last);
	}


	template <class ForwardIter, class T>
	mystl::pair<ForwardIter, ForwardIter>
		erange_dispatch(ForwardIter first, ForwardIter last,
			const T& value, radom_access_iterator_tag)
	{
		auto len = mystl::distance(first, last);
		ForwardIter middle;
		while (len > 0) {
			auto size = len >> 1;
			middle = first + size;
			if (value < *middle) {
				len = size;
			}
			else if (value > *middle) {
				first = middle + 1;
				len = len - size - 1;
			}
			else {
				ForwardIter left = mystl::lower_bound(first, last, value);
				ForwardIter right = mystl::upper_bound(++middle, first + len, value);
				return mystl::pair<ForwardIter, ForwardIter>(left, right);
			}
		}
		return mystl::pair<ForwardIter, ForwardIter>(last, last);
	}


	template <class ForwardIter, class T>
	mystl::pair<ForwardIter, ForwardIter>
		equal_erange(ForwardIter first, ForwardIter last, const T& value)
	{
		return mystl::erange_dispatch(first, last, value, iterator_category(first));
	}


	template <class ForwardIter, class T, class Compared>
	mystl::pair<ForwardIter, ForwardIter>
		erange_dispatch(ForwardIter first, ForwardIter last,
			const T& value, forward_iterator_tag, Compared cmp)
	{
		auto len = mystl::distance(first, last);
		ForwardIter middle;
		while (len > 0) {
			auto size = len >> 1;
			middle = first;
			mystl::advance(middle, size);
			if (cmp(value, *middle)) {
				len = size;
			}
			else if (cmp(*middle, value)) {
				first = middle;
				first++;
				len = len - size - 1;
			}
			else {
				ForwardIter left = mystl::lower_bound(first, last, value);
				mystl::advance(first, size);
				ForwardIter right = mystl::upper_bound(++middle, first, value);
				return mystl::pair<ForwardIter, ForwardIter>(left, right);
			}
		}
		return mystl::pair<ForwardIter, ForwardIter>(last, last);
	}


	template <class ForwardIter, class T, class Compared>
	mystl::pair<ForwardIter, ForwardIter>
		erange_dispatch(ForwardIter first, ForwardIter last,
			const T& value, radom_access_iterator_tag, Compared cmp)
	{
		auto len = mystl::distance(first, last);
		ForwardIter middle;
		while (len > 0) {
			auto size = len >> 1;
			middle = first + size;
			if (cmp(value, *middle)) {
				len = size;
			}
			else if (cmp(*middle, value)) {
				first = middle + 1;
				len = len - size - 1;
			}
			else {
				ForwardIter left = mystl::lower_bound(first, last, value);
				ForwardIter right = mystl::upper_bound(++middle, first + len, value);
				return mystl::pair<ForwardIter, ForwardIter>(left, right);
			}
		}
		return mystl::pair<ForwardIter, ForwardIter>(last, last);
	}


	template <class ForwardIter, class T, class Compared>
	mystl::pair<ForwardIter, ForwardIter>
		equal_erange(ForwardIter first, ForwardIter last, const T& value, Compared cmp)
	{
		return mystl::erange_dispatch(first, last, value, mystl::iterator_category(first), cmp);
	}

	
	template <class ForwardIter, class Generate>
	void generate(ForwardIter first, ForwardIter last, Generate g)
	{
		for (; first != last; first++) {
			*first = g();
		}
	}


	template <class ForwardIter, class Size, class Generate>
	void generate_n(ForwardIter first, Size n, Generate g)
	{
		for (; n != 0; n--, first++) {
			*first = g();
		}
	}

	// 查找范围一中是否存在范围二且要求已经排序
	template <class ForwardIter1, class ForwardIter2>
	bool includes(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2)
	{
		if (first1 == last1) {
			return false;
		}
		if (first2 == last2) {
			return false;
		}
		while (first1 != last1) {
			if (*first2 < *first1) {
				return false;
			}
			else if (*first1 < *first2){
				++first1;
			}
			else {
				++first1, ++first2;
			}
		}
		return first2 == last2;
	}


	template <class ForwardIter1, class ForwardIter2, class Compared>
	bool includes(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2, Compared cmp)
	{
		if (first1 == last1) {
			return false;
		}
		if (first2 == last2) {
			return false;
		}
		while (first1 != last1) {
			if (cmp(*first2, *first1)) {
				return false;
			}
			else if (cmp(*first1, *first2)) {
				++first1;
			}
			else {
				++first1, ++first2;
			}
		}
		return first2 == last2;
	}

	// 堆的特点是父节点大于子节点
	template <class RadomIter>
	bool is_heap(RadomIter first, RadomIter last)
	{
		auto len = mystl::advance(first, last);
		size_t parent = 0;
		for (size_t chile = 1; chile < len; chile++) {
			if (first[parent] < first[chile]) {
				return false;
			}
			if (chile % 2 == 0) {
				parent++;
			}
		}
		return true;
	}


	template <class RadomIter, class Compared>
	bool is_heap(RadomIter first, RadomIter last, Compared cmp)
	{
		auto len = mystl::advance(first, last);
		size_t parent = 0;
		for (size_t chile = 1; chile < len; chile++) {
			if (cmp(first[parent], first[chile])) {
				return false;
			}
			if (chile % 2 == 0) {
				parent++;
			}
		}
		return true;
	}


	template <class Iter>
	bool is_sorted(Iter first, Iter last)
	{
		if (first == last) {
			return true;
		}
		auto next = first;
		next++;
		for (; next != last; first=next, next++) {
			if (*next < *first) {
				return false;
			}
		}
		return true;
	}


	template <class Iter, class Compared>
	bool is_sorted(Iter first, Iter last, Compared cmp)
	{
		if (first == last) {
			return true;
		}
		auto next = first;
		next++;
		for (; next != last; first = next, next++) {
			if (cmp(*next, *first)) {
				return false;
			}
		}
		return true;
	}


	template <class T>
	const T& median(const T& a, const T& b, const T& c)
	{
		if (a < b) {
			return a < c ? (c < b ? c : b) : a;
		}
		else {
			return b < c ? (c < a ? c : a) : b;
		}
	}


	template <class T, class Compared>
	const T& median(const T& a, const T& b, const T& c, Compared cmp)
	{
		if (cmp(a, b)) {
			return cmp(a, b) ? (cmp(c, b) ? c : b) : a;
		}
		else {
			return cmp(b, c) ? (cmp(c, a) ? c : a) : b;
		}
	}


	template <class ForwardIter>
	ForwardIter max_element(ForwardIter first, ForwardIter last)
	{
		if (first == last) {
			return last;
		}
		auto result = first;
		first++;
		for (; first != last; first++) {
			if (*result < *first) {
				result = first;
			}
		}
		return result;
	}


	template <class ForwardIter, class Compared>
	ForwardIter max_element(ForwardIter first, ForwardIter last, Compared cmp)
	{
		if (first == last) {
			return last;
		}
		auto result = first;
		first++;
		for (; first != last; first++) {
			if (cmp(*result, *first)) {
				result = first;
			}
		}
		return result;
	}


	template <class ForwardIter>
	ForwardIter min_element(ForwardIter first, ForwardIter last)
	{
		if (first == last) {
			return last;
		}
		auto result = first;
		first++;
		for (; first != last; first++) {
			if (*first < *result) {
				result = first;
			}
		}
		return result;
	}


	template <class ForwardIter, class Compared>
	ForwardIter min_element(ForwardIter first, ForwardIter last, Compared cmp)
	{
		if (first == last) {
			return last;
		}
		auto result = first;
		first++;
		for (; first != last; first++) {
			if (cmp(*first, *result)) {
				result = first;
			}
		}
		return result;
	}


	template <class ForwardIter1, class ForwardIter2>
	void swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2)
	{
		for (; first1 != last1; first1++, first2++) {
			mystl::iter_swap(first1, first2);
		}
	}


	template <class InputIter, class OutIter, class UnaryOperation>
	OutIter transform(InputIter first, InputIter last,
		OutIter result, UnaryOperation unary)
	{
		for (; first != last; first++, result++) {
			*result = unary(*first);
		}
		return result;
	}


	template <class InputIter1, class InputIter2, class OutIter, class BinaryOperation>
	OutIter transform(InputIter1 first, InputIter1 last, InputIter2 first1,
		OutIter result, BinaryOperation binary)
	{
		for (; first != last; first++, first1++, result++) {
			*result = binary(*first, *first1);
		}
		return result;
	}


	template <class InputIter, class OutIter, class T>
	OutIter remove_copy(InputIter first, InputIter last, OutIter result, const T& value)
	{
		for (; first != last; first++) {
			if (*first != value) {
				*result = *first;
				result++;
			}
		}
		return result;
	}


	template <class InputIter, class OutIter, class T>
	OutIter remove(InputIter first, InputIter last, OutIter result, const T& value)
	{
		first = mystl::find(first, last, value); 
		return first == last ? last : mystl::remove_copy(++first, last, result, value);
	}


	template <class InputIter, class OutIter, class T, class UnaryPredicate>
	OutIter remove_copy_if(InputIter first, InputIter last, OutIter result, const T& value, UnaryPredicate unary)
	{
		for (; first != last; first++) {
			if (!unary(*first, value)) {
				*result = *first;
				result++;
			}
		}
		return result;
	}


	template <class InputIter, class OutIter, class T, class UnaryPredicate>
	OutIter remove_if(InputIter first, InputIter last, OutIter result, const T& value, UnaryPredicate unary)
	{
		first = mystl::find_if(first, last, cmp);
		return first == last ? last : mystl::remove_copy(++first, last, result, value, unary);
	}


	template <class ForwardIter, class T>
	void replase(ForwardIter first, ForwardIter last, const T& oldValue, const T& newValue)
	{
		for (; first != last; first++) {
			if (*first == oldValue) {
				*first = newValue;
			}
		}
	}


	template <class ForwardIter, class T, class UnaryPredicate>
	void replase_if(ForwardIter first, ForwardIter last, UnaryPredicate unary, const T& newValue)
	{
		for (; first != last; first++) {
			if (unary(*first)) {
				*first = newValue;
			}
		}
	}


	template <class InputIter, class OutIter, class T>
	OutIter replace_copy(InputIter first, InputIter last,
		OutIter result, const T& oldValue, const T& newValue)
	{
		for (; first != last; first++, result++) {
			if (*first == oldValue) {
				*result = newValue;
			}
			else {
				*result = *first;
			}
		}
		return result;
	}


	template <class InputIter, class OutIter, class T, class BidnaryPredicate>
	OutIter replace_copy_if(InputIter first, InputIter last,
		OutIter result, BidnaryPredicate bidnary, const T& newValue)
	{
		for (; first != last; first++, result++) {
			if (bidnary(*first)) {
				*result = newValue;
			}
			else {
				*result = *first;
			}
		}
		return result;
	}


	template <class BidirectionalIter>
	void reverse_dispatch(BidirectionalIter first, BidirectionalIter last,
		bidirectional_iterator_tag)
	{
		while (true) {
			if (first == last || first == --last) {
				return;
			}
			mystl::iter_swap(++first, last);
		}
	}


	template <class RamdonIter>
	void reverse_dispatch(RamdonIter first, RamdonIter last,
		radom_access_iterator_tag)
	{
		while (first < last) {
			mystl::iter_swap(first++, --last);
		}
	}


	template <class ForwardIter>
	void reverse(ForwardIter first, ForwardIter last)
	{
		mystl::reverse_dispatch(first, last, mystl::iterator_category(first));
	}


	template <class BidrectionalIter, class OutIter>
	OutIter reverse_copy(BidrectionalIter first, BidrectionalIter last,
		OutIter result)
	{
		while (first != last) {
			last--;
			*result = *last;
			result++;
		}
		return result;
	}


	template <class RadomIter>
	void radom_shuffle(RadomIter first, RadomIter last)
	{
		if (first == last) {
			return;
		}
		srand((unsigned)time(0));
		for (auto iter = first + 1; iter != last; iter++) {
			mystl::iter_swap(iter, first + rand() % (iter - first + 1));
		}
	}


	template <class RadomIter, class RadomNumberGenerator>
	void radom_shuffle(RadomIter first, RadomIter last,
		RadomNumberGenerator& rand)
	{
		if (first == last) {
			return;
		}
		size_t len = mystl::distance(first, last);
		for (auto iter = first + 1; iter != last; iter++) {
			mystl::iter_swap(iter, first + (rand(iter - first + 1) % len));
		}
	}

	// 这个算法只会在一个容器中使用，用法如下
	// 1 2 3 4 5 6 7
	// 以4为中心翻转
	// 4 5 6 7 1 2 3
	template <class BidirectionalIter>
	BidirectionalIter rotate_dispatch(BidirectionalIter first, BidirectionalIter middle,
		BidirectionalIter last, bidirectional_iterator_tag)
	{
		mystl::reverse_dispatch(first, middle, bidirectional_iterator_tag);
		mystl::reverse_dispatch(middle, last, bidirectional_iterator_tag);
		while (first != middle && middle != last) {
			mystl::iter_swap(first++, --last);
		}
		if (first == middle) {
			mystl::reverse_dispatch(middle, last, bidirectional_iterator_tag);
			return last;
		}
		else {
			mystl::reverse_dispatch(first, middle, bidirectional_iterator_tag);
			return first;
		}
	}


	template <class ForwardIter>
	ForwardIter rotate_dispatch(ForwardIter first, ForwardIter middle,
		ForwardIter last, forward_iterator_tag)
	{
		auto first1 = middle;
		do {
			mystl::iter_swap(first++, first1++);
			if (first == middle) {
				middle = first1;
			}
		} while (first1 != last);
		auto new_result = first;
		first1 = middle;
		while (first1 != last) {
			mystl::iter_swap(first++, first1++);
			if (first == middle) {
				middle = first1;
			}
			else if (first1 == last) {
				first1 = middle;
			}
		}
		return new_result;
	}


	template <class T>
	const T gcd(T a, T b)
	{
		while (b != 0) {
			auto tmp = a % b;
			a = b;
			b = tmp;
		}
		return a;
	}


	template <class RadomIter>
	RadomIter rotate_dispatch(RadomIter first, RadomIter middle,
		RadomIter last, radom_access_iterator_tag)
	{
		auto n = last - first;
		auto l = middle - first;
		auto r = n - l;
		auto result = first + (last - middle);
		if (l == r) {
			mystl::swap_range(first, middle, middle);
			return middle;
		}
		auto num = mystl::gcd(l, n);
		for (int i = 0; i < num; i++) {
			auto tmp = *first;
			auto p = first;
			if (l < r) {
				for (int j = 0; j < r / num; j++) {
					if (p > first + r) {
						*p = *(p - r);
						p -= r;
					}
					*p = *(p + l);
					p += l;
				}
			}
			else {
				for (int j = 0; j < l / num; j++) {
					if (p < last - l) {
						*p = *(p + l);
						p += l;
					}
					*p = *(p - r);
					p -= r;
				}
			}
			*p = tmp;
			first++;
		}
		return result;
	}

	// 在一个容器中将两段范围交换，可大小不同
	template <class ForwardIter>
	ForwardIter rotate(ForwardIter first, ForwardIter middle, ForwardIter last)
	{
		if (first == middle) {
			return last;
		}
		if (middle == last) {
			return first;
		}
		return mystl::rotate_dispatch(first, middle, last, iterator_category(first));
	}


	template <class ForwardIter, class OutIter>
	ForwardIter rotate_copy(ForwardIter first, ForwardIter middle, ForwardIter last, OutIter result)
	{
		return mystl::copy(first, middle, mystl::copy(middle, last, result));
	}

	
	template <class ForwardIter1, class ForwardIter2, class BinaryPred>
	bool is_permutation_aux(ForwardIter1 first, ForwardIter1 last,
		ForwardIter2 begin, ForwardIter2 end, BinaryPred pred)
	{
		constexpr bool is_ra_it = mystl::is_radom_access_iterator<ForwardIter1>::value
			&& mystl::is_radom_access_iterator<ForwardIter2>::value;

		if (is_ra_it) {
			auto len1 = last - first;
			auto len2 = end - begin;
			if (len1 != len2) {
				return false;
			}
		}

		for (; first != last && begin != end; ++first, ++begin) {
			if (!pred(*first, *begin)) {
				break;
			}
		}
		if (is_ra_it) {
			if (first == last) {
				return true;
			}
		}
		else {
			auto len1 = mystl::distance(first, last);
			auto len2 = mystl::distance(begin, end);
			if (len1 == 0 && len2 == 0) {
				return true;
			}
			if (len1 != len2) {
				return false;
			}
		}

		for (auto i = first; i != last; i++) {
			bool flag = false;
			for (auto j = first; j != i; ++j) {
				if (pred(*j, *i)) {
					flag = true;
					break;
				}
			}
			if (!flag) {
				auto c2 = 0;
				for (auto j = begin; j != end; j++) {
					if (pred(*i, *j)) {
						++c2;
					}
				}
				if (c2 == 0) {
					return false;
				}
				auto c1 = 1;
				auto j = i;
				for (++j; j != last; j++) {
					if (pred(*i, *j)) {
						++c1;
					}
				}
				if (c1 != c2) {
					return false;
				}
			}
		}
		return  true;
	}


	template <class ForwardIter1, class ForwardIter2, class BinaryPred>
	bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred)
	{
		return mystl::is_permutation_aux(first1, last1, first2, last2, pred);
	}


	template <class ForwardIter1, class ForwardIter2>
	bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2)
	{
		typedef typename iterator_traits<ForwardIter1>::value v1;
		typedef typename iterator_traits<ForwardIter2>::value v2;
		static_assert(std::is_same<v1, v2>::value, "the type should be same in mystl::is_permutation");
		return mystl::is_permutation_aux(first1, last1, first2, last2, mystl::equal_to<v1>());
	}

	//将从小到大的排序逐渐排列成由大到小的排序
	template <class BidirectionalIter>
	bool next_permutation(BidirectionalIter first, BidirectionalIter last)
	{
		auto i = last;
		if (first == last || first == --i) {
			return false;
		}
		while (true) {
			auto iter = i;
			if (*(--i) < *(iter)) {
				auto j = last;
				while (!(*i < *(--j))) {}
				mystl::iter_swap(i, j);
				mystl::reverse(iter, last);
				return true;
			}
			if (i == first) {
				mystl::reverse(first, last);
				return false;
			}
		}
	}


	template <class BidirectionalIter, class Compared>
	bool next_permutation(BidirectionalIter first, BidirectionalIter last, Compared cmp)
	{
		auto i = last;
		if (first == last || first == --i) {
			return false;
		}
		while (true) {
			auto iter = i;
			if (cmp(*(--i), *(iter))) {
				auto j = last;
				while (!cmp(*i, *(--j))) {}
				mystl::iter_swap(i, j);
				mystl::reverse(iter, last);
				return true;
			}
			if (i == first) {
				mystl::reverse(first, last);
				return false;
			}
		}
	}


	template <class BidirectionalIter>
	bool prev_permutation(BidirectionalIter first, BidirectionalIter last)
	{
		auto i = last;
		if (first == last || first == --i) {
			return false;
		}
		while (true) {
			auto iter = i;
			if (*(--i) > *iter) {
				auto j = last;
				while (!(*i > *(--j))) {}
				mystl::iter_swap(i, j);
				mystl::reverse(iter, last);
				return true;
			}
			if (i == first) {
				mystl::reverse(first, last);
				return false;
			}
		}
	}


	template <class BidirectionalIter, class Compared>
	bool prev_permutation(BidirectionalIter first, BidirectionalIter last, Compared cmp)
	{
		auto i = last;
		if (first == last || first == --i) {
			return false;
		}
		while (true) {
			auto iter = i;
			if (cmp(*(iter), *(--i))) {
				auto j = last;
				while (!cmp(*(--j), *i)) {}
				mystl::iter_swap(i, j);
				mystl::reverse(iter, last);
				return true;
			}
			if (i == first) {
				mystl::reverse(first, last);
				return false;
			}
		}
	}

	// 将两个容器的数进行归并排序
	template <class InputIter1, class InputIter2, class OutIter>
	OutIter merge(InputIter1 first, InputIter1 last,
		InputIter2 begin, InputIter2 end, OutIter result)
	{
		while (first != last && begin != end) {
			if (*first < *begin) {
				*result = *first;
				++first;
			}
			else {
				*result = *begin;
				++begin;
			}
			++result;
		}
		return mystl::copy(first, last, mystl::copy(begin, end, result));
	}


	template <class InputIter1, class InputIter2, class OutIter, class Compared>
	OutIter merge(InputIter1 first, InputIter1 last,
		InputIter2 begin, InputIter2 end, OutIter result, Compared cmp)
	{
		while (first != last && begin != end) {
			if (cmp(*first, *begin)) {
				*result = *first;
				++first;
			}
			else {
				*result = *begin;
				++begin;
			}
			++result;
		}
		return mystl::copy(first, last, mystl::copy(begin, end, result));
	}

	// 在一个容器中将两段排序了的范围进行合并并排序
	template <class BidirectionalIter, class Distance>
	void merge_without_buffer(BidirectionalIter first, BidirectionalIter middle,
		BidirectionalIter last, Distance len1, Distance len2)
	{
		if (len1 == 0 || len2 == 0) {
			return;
		}
		if (len1 + len2 == 2) {
			if (*first < *middle) {
				mystl::iter_swap(first, middle);
			}
			return;
		}
		auto first_cut = first;
		auto second_cut = middle;
		Distance len11 = 0, len22 = 0;
		if (len1 > len2) {
			len11 = len1 >> 1;
			mystl::advance(first_cut, len11);
			second_cut = mystl::lower_bound(middle, last, *first_cut);
			len22 = mystl::distance(middle, second_cut);
		}
		else {
			len22 = len2 >> 1;
			mystl::advance(second_cut, len22);
			first_cut = mystl::upper_bound(first, middle, *second_cut);
			len11 = mystl::distance(first, first_cut);
		}
		auto new_result = mystl::rotate(first_cut, middle, second_cut);
		merge_without_buffer(first, first_cut, new_result, len11, len22);
		merge_without_buffer(new_result, second_cut, last, len1 - len11, len2 - len22);
	}


	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter1 merge_backward(BidirectionalIter1 first1, BidirectionalIter1 last1,
		BidirectionalIter2 first2, BidirectionalIter2 last2, BidirectionalIter2 result)
	{
		if (first1 == last1) {
			return mystl::copy_backward(first2, last2, result);
		}
		if (first2 == last2) {
			return mystl::copy_backward(first1, last1, result);
		}
		--last1, --last2;
		while (true) {
			if (*last2 < *last1) {
				*--result = *last1;
				if (first1 == last1) {
					return mystl::copy_backward(first2, last2, result);
				}
				--last1;
			}
			else {
				*--result = *last2;
				if (first2 == last2) {
					return mystl::copy_backward(first1, last1, result);
				}
				--last2;
			}
		}
	}

	// 用缓存区的范围翻转
	template <class BidirectionalIter1, class BidirectionalIter2, class Distance>
	BidirectionalIter1 rotate_adaptive(BidirectionalIter1 first, BidirectionalIter1 middle,
		BidirectionalIter1 last, Distance len1, Distance len2, BidirectionalIter2 buffer, Distance buf_size)
	{
		if (len1 > len2 && len2 <= buf_size) {
			auto buffer_end = mystl::copy(middle, last, buffer);
			mystl::copy_backward(first, middle, last);
			return mystl::copy_backward(buffer, buffer_end, first);
		}
		else if (len1 <= buf_size) {
			auto buffer_end = mystl::copy(first, middle, buffer);
			mystl::copy(middle, last, first);
			return mystl::copy_backward(buffer, buffer_end, last);
		}
		else {
			return rotate(first, middle, last);
		}
	}

	// 有缓冲区的将一个容器中的两段排了序的范围排成一段排好序的范围
	template <class BidirectionalIter, class Distance, class Pointer>
	void merge_adaptive(BidirectionalIter first, BidirectionalIter middle,
		BidirectionalIter last, Distance len1, Distance len2,
		Pointer buffer, Distance buff_size)
	{
		if (len1 <= len2 && len1 <= buff_size) {
			Pointer buff_end = mystl::copy(first, middle, buffer);
			mystl::merge(buffer, buff_end, middle, last, first);
		}
		else if (len2 <= buff_size) {
			Pointer buff_end = mystl::copy(middle, last, buffer);
			mystl::merge_backward(buffer, buff_end, first, middle, last);
		}
		else {
			auto first_cur = first;
			auto second_cur = middle;
			Distance len11 = 0, len22 = 0;
			if (len1 > len2) {
				len11 = len1 >> 1;
				mystl::advance(first_cur, len11);
				second_cur = mystl::lower_bound(middle, last, *first_cur);
				len22 = mystl::distance(middle, second_cur);
			}
			else {
				len22 = len2 >> 1;
				mystl::advance(second_cur, len22);
				first_cur = mystl::upper_bound(first, middle, *second_cur);
				len11 = mystl::distance(first, first_cur);
			}
			auto new_result = mystl::rotate_adaptive(first_cur, middle, second_cur,
				len1-len11, len22, buffer, buff_size);
			mystl::merge_adaptive(first, first_cur, new_result, len11, len22, buffer, buff_size);
			mystl::merge_adaptive(new_result, second_cur, last, len1 - len11, len2 - len22, buffer, buff_size);
		}
	}

	//看看是否能申请缓冲区，借用缓冲区来完成范围的换位
	template <class BidirectionalIter, class T>
	void inplace_merge_aux(BidirectionalIter first, BidirectionalIter middle,
		BidirectionalIter last, T*)
	{
		auto len1 = mystl::distance(first, middle);
		auto len2 = mystl::distance(middle, last);
		temporary_buffer<BidirectionalIter, T> buf(first, last);
		if (buf.begin() == nullptr) {
			mystl::merge_without_buffer(first, middle, last, len1, len2);
		}
		else {
			mystl::merge_adaptive(first, middle, last, len1, len2, buf, buf.size());
		}
	}


	template <class BidirectionalIter>
	void inplace_merge(BidirectionalIter first, BidirectionalIter middle, BidirectionalIter last)
	{
		if (first == middle || middle == last) {
			return;
		}
		mystl::inplace_merge_aux(first, middle, last, value_type(first));
	}


	template <class BidirectionalIter, class Distance, class Compared>
	void merge_without_buffer(BidirectionalIter first, BidirectionalIter middle,
		BidirectionalIter last, Distance len1, Distance len2, Compared cmp)
	{
		if (len1 == 0 || len2 == 0) {
			return;
		}
		if (len1 + len2 == 2) {
			if (cmp(*first, *middle)) {
				mystl::iter_swap(first, middle);
			}
			return;
		}
		auto first_cut = first;
		auto second_cut = middle;
		Distance len11 = 0, len22 = 0;
		if (len1 > len2) {
			len11 = len1 >> 1;
			mystl::advance(first_cut, len11);
			second_cut = mystl::lower_bound(middle, last, *first_cut, cmp);
			len22 = mystl::distance(middle, second_cut);
		}
		else {
			len22 = len2 >> 1;
			mystl::advance(second_cut, len22);
			first_cut = mystl::upper_bound(first, middle, *second_cut, cmp);
			len11 = mystl::distance(first, first_cut);
		}
		auto new_result = mystl::rotate(first_cut, middle, second_cut);
		merge_without_buffer(first, first_cut, new_result, len11, len22, cmp);
		merge_without_buffer(new_result, second_cut, last, len1 - len11, len2 - len22, cmp);
	}


	template <class BidirectionalIter1, class BidirectionalIter2, class Compared>
	BidirectionalIter1 merge_backward(BidirectionalIter1 first1, BidirectionalIter1 last1,
		BidirectionalIter2 first2, BidirectionalIter2 last2, BidirectionalIter2 result, Compared cmp)
	{
		if (first1 == last1) {
			return mystl::copy_backward(first2, last2, result);
		}
		if (first2 == last2) {
			return mystl::copy_backward(first1, last1, result);
		}
		--last1, --last2;
		while (true) {
			if (cmp(*last2, *last1)) {
				*--result = *last1;
				if (first1 == last1) {
					return mystl::copy_backward(first2, last2, result);
				}
				--last1;
			}
			else {
				*--result = *last2;
				if (first2 == last2) {
					return mystl::copy_backward(first1, last1, result);
				}
				--last2;
			}
		}
	}


	template <class BidirectionalIter, class Distance, class Pointer, class Compared>
	void merge_adaptive(BidirectionalIter first, BidirectionalIter middle,
		BidirectionalIter last, Distance len1, Distance len2,
		Pointer buffer, Distance buff_size, Compared cmp)
	{
		if (len1 <= len2 && len1 <= buff_size) {
			Pointer buff_end = mystl::copy(first, middle, buffer);
			mystl::merge(buffer, buff_end, middle, last, first, cmp);
		}
		else if (len2 <= buff_size) {
			Pointer buff_end = mystl::copy(middle, last, buffer);
			mystl::merge_backward(buffer, buff_end, first, middle, last, cmp);
		}
		else {
			auto first_cur = first;
			auto second_cur = middle;
			Distance len11 = 0, len22 = 0;
			if (len1 > len2) {
				len11 = len1 >> 1;
				mystl::advance(first_cur, len11);
				second_cur = mystl::lower_bound(middle, last, *first_cur, cmp);
				len22 = mystl::distance(middle, second_cur);
			}
			else {
				len22 = len2 >> 1;
				mystl::advance(second_cur, len22);
				first_cur = mystl::upper_bound(first, middle, *second_cur, cmp);
				len11 = mystl::distance(first, first_cur);
			}
			auto new_result = mystl::rotate_adaptive(first_cur, middle, second_cur,
				len1 - len11, len22, buffer, buff_size);
			mystl::merge_adaptive(first, first_cur, new_result, len11, len22, buffer, buff_size, cmp);
			mystl::merge_adaptive(new_result, second_cur, last, len1 - len11, len2 - len22, buffer, buff_size, cmp);
		}
	}


	template <class BidirectionalIter, class T, class Compared>
	void inplace_merge_aux(BidirectionalIter first, BidirectionalIter middle,
		BidirectionalIter last, T*, Compared cmp)
	{
		auto len1 = mystl::distance(first, middle);
		auto len2 = mystl::distance(middle, last);
		temporary_buffer<BidirectionalIter, T> buf(first, last);
		if (buf.begin() == nullptr) {
			mystl::merge_without_buffer(first, middle, last, len1, len2, cmp);
		}
		else {
			mystl::merge_adaptive(first, middle, last, len1, len2, buf, buf.size(), cmp);
		}
	}


	template <class BidirectionalIter, class Compared>
	void inplace_merge(BidirectionalIter first, BidirectionalIter middle, BidirectionalIter last, Compared cmp)
	{
		if (first == middle || middle == last) {
			return;
		}
		mystl::inplace_merge_aux(first, middle, last, value_type(first), cmp);
	}

	// 部分排序
	// 对[first， middle) 进行堆排序
	template <class RadomIter>
	void partial_sort(RadomIter first, RadomIter middle, RadomIter last)
	{
		mystl::make_heap(first, middle);
		for (auto i = middle; i < last; i++) {
			if (*i < *first) {
				mystl::pop_heap_aux(first, middle, i, *i, distance_type(first));
			}
		}
		mystl::sort_heap(first, middle);
	}


	template <class RadomIter, class Compared>
	void partial_sort(RadomIter first, RadomIter middle, RadomIter last, Compared cmp)
	{
		mystl::make_heap(first, middle, cmp);
		for (auto i = middle; i < last; i++) {
			if (cmp(*i, *first)) {
				mystl::pop_heap_aux(first, middle, i, *i, distance_type(first), cmp);
			}
		}
		mystl::sort_heap(first, middle, cmp);
	}


	template <class InputIter, class RadomIter, class Distance>
	RadomIter psort_copy_aux(InputIter first, InputIter last,
		RadomIter result_first, RadomIter result_last, Distance*)
	{
		if (result_first == result_last) {
			return result_last;
		}
		auto iter = result_first;
		while (first != last && iter != result_last) {
			*iter = *first;
			++iter, ++first;
		}
		mystl::make_heap(result_first, result_last);
		while (first != last) {
			if (*first < *result_first) {
				mystl::adjust_heap(result_first, static_cast<Distance>(0),
					result_last - result_first, *first);
			}
			++first;
		}
		mystl::sort_heap(result_first, result_last);
		return iter;
	}


	template <class InputIter, class RadomIter>
	RadomIter partial_sort_copy(InputIter first, InputIter last,
		RadomIter result_first, RadomIter result_last)
	{
		return mystl::psort_copy_aux(first, last, result_first, result_last, distance_type(result_first));
	}


	template <class InputIter, class RadomIter, class Distance, class Compared>
	RadomIter psort_copy_aux(InputIter first, InputIter last,
		RadomIter result_first, RadomIter result_last, Distance*, Compared cmp)
	{
		if (result_first == result_last) {
			return result_last;
		}
		auto iter = result_first;
		while (first != last && iter != result_last) {
			*iter = *first;
			++iter, ++first;
		}
		mystl::make_heap(result_first, result_last, cmp);
		while (first != last) {
			if (cmp(*first, *result_first)) {
				mystl::adjust_heap(result_first, static_cast<Distance>(0),
					result_last - result_first, *first, cmp);
			}
			++first;
		}
		mystl::sort_heap(result_first, result_last, cmp);
		return iter;
	}


	template <class InputIter, class RadomIter, class Compared>
	RadomIter partial_sort_copy(InputIter first, InputIter last,
		RadomIter result_first, RadomIter result_last, Compared cmp)
	{
		return mystl::psort_copy_aux(first, last, result_first, result_last, distance_type(result_first), cmp);
	}


	template <class BidirectinoalIter, class UnaryPredicate>
	BidirectinoalIter partition(BidirectinoalIter first,
		BidirectinoalIter last, UnaryPredicate unary)
	{
		while (true) {
			while (first != last) {
				if (!unary(*first)) {
					break;
				}
				++first;
			}
			if (first == last) {
				break;
			}
			--last;
			while (first != last) {
				if (unary(*last)) {
					break;
				}
				--last;
			}
			mystl::iter_swap(first, last);
			++first;
		}
		return first;
	}


	template <class InputIter, class OutIter1, class OutIter2, class UnaryPredicate>
	mystl::pair<OutIter1, OutIter2>
		partition_copy(InputIter first, InputIter last,
			OutIter1 result_true, OutIter2 result_false, UnaryPredicate unary)
	{
		while (first != last) {
			if (unary(*first)) {
				*result_true = *first;
				++result_true;
			}
			else {
				*result_false = *first;
				++result_false;
			}
			++first;
		}
		return mystl::pair<OutIter1, OutIter2>(result_true, result_false);
	}


	constexpr static size_t kSmallSectionSize = 128;

	template <class Size>
	Size slg2(Size n)
	{
		Size cnt = 0;
		for (; n > 1; n >>= 1) {
			++cnt;
		}
		return cnt;
	}


	template <class RadomIter, class T>
	RadomIter unchecked_partition(RadomIter first, RadomIter last, const T& value)
	{
		while (true) {
			while (*first < value) {
				++first;
			}
			--last;
			while (value < *last) {
				--last;
			}
			if (!(first < last)) {
				return first;
			}
			mystl::iter_swap(first, last);
			++first;
		}
	}

	// 快排
	template <class RadomIter, class Size>
	void intro_sort(RadomIter first, RadomIter last, Size depth_limit)
	{
		while (static_cast<Size>(last - first) > kSmallSectionSize) {
			if (depth_limit == 0) {
				mystl::partial_sort(first, last, last);
				return;
			}
			--depth_limit;
			auto mid = mystl::median(*(first), *(first + (last - first) / 2), *(last - 1));
			auto cut = mystl::unchecked_partition(first, last, mid);
			mystl::intro_sort(cut, last, depth_limit);
			last = cut;
		}
	}

	// 从后插入
	template <class RadomIter, class T>
	void unchecked_linear_insert(RadomIter last, const T& value)
	{
		auto pre = last - 1;
		while (*pre > value) {
			*last = *pre;
			pre--;
			last--;
		}
		*last = value;
	}

	//插入排序
	template <class RandomIter>
	void unchecked_insertion_sort(RandomIter first, RandomIter last)
	{
		for (auto iter = first + 1; iter != last; iter++) {
			unchecked_linear_insert(iter, *iter);
		}
	}

	// 将[first, last)进行插入排序
	template <class RandomIter>
	void insertion_sort(RandomIter first, RandomIter last)
	{
		for (auto iter = first + 1; iter != last; iter++) {
			auto value = *iter;
			if (value < *first) {
				mystl::copy_backward(first, iter, iter + 1);
				*first = value;
			}
			else {
				mystl::unchecked_linear_insert(iter, value);
			}
		}
	}

	// 对插入排序的优化
	template <class RandomIter>
	void final_insertion_sort(RandomIter first, RandomIter last)
	{
		if (static_cast<size_t>(last - first) > kSmallSectionSize) {
			mystl::insertion_sort(first, first + kSmallSectionSize);
			mystl::unchecked_insertion_sort(first + kSmallSectionSize, last);
		}
		else {
			mystl::insertion_sort(first, last);
		}
	}


	template <class RandomIter>
	void sort(RandomIter first, RandomIter last)
	{
		if (first != last) {
			mystl::intro_sort(first, last, slg2(last - first) * 2);
			mystl::final_insertion_sort(first, last);
		}
	}


	template <class RadomIter, class T, class Compared>
	RadomIter unchecked_partition(RadomIter first, RadomIter last, const T& value, Compared cmp)
	{
		while (true) {
			while (cmp(*first, value)) {
				++first;
			}
			--last;
			while (cmp(value, *last)) {
				--last;
			}
			if (!(cmp(*first, *last))) {
				return first;
			}
			mystl::iter_swap(first, last);
			++first;
		}
	}

	// 快排
	template <class RadomIter, class Size, class Compared>
	void intro_sort(RadomIter first, RadomIter last, Size depth_limit, Compared cmp)
	{
		while (static_cast<Size>(last - first) > kSmallSectionSize) {
			if (depth_limit == 0) {
				mystl::partial_sort(first, last, last,cmp);
				return;
			}
			--depth_limit;
			auto mid = mystl::median(*(first), *(first + (last - first) / 2), *(last - 1), cmp);
			auto cut = mystl::unchecked_partition(first, last, mid, cmp);
			mystl::intro_sort(cut, last, depth_limit, cmp);
			last = cut;
		}
	}

	// 从后插入
	template <class RadomIter, class T, class Compared>
	void unchecked_linear_insert(RadomIter last, const T& value, Compared cmp)
	{
		auto pre = last - 1;
		while (cmp(*pre, value)) {
			*last = *pre;
			pre--;
			last--;
		}
		*last = value;
	}

	//插入排序
	template <class RandomIter, class Compared>
	void unchecked_insertion_sort(RandomIter first, RandomIter last, Compared cmp)
	{
		for (auto iter = first + 1; iter != last; iter++) {
			unchecked_linear_insert(iter, *iter, cmp);
		}
	}


	template <class RandomIter, class Compared>
	void insertion_sort(RandomIter first, RandomIter last, Compared cmp)
	{
		for (auto iter = first + 1; iter != last; iter++) {
			auto value = *iter;
			if (cmp(value, *first)) {
				mystl::copy_backward(first, iter, iter + 1);
				*first = value;
			}
			else {
				mystl::unchecked_linear_insert(iter, value, cmp);
			}
		}
	}


	template <class RandomIter, class Compared>
	void final_insertion_sort(RandomIter first, RandomIter last, Compared cmp)
	{
		if (static_cast<size_t>(last - first) > kSmallSectionSize) {
			mystl::insertion_sort(first, first + kSmallSectionSize, cmp);
			mystl::unchecked_insertion_sort(first + kSmallSectionSize, last, cmp);
		}
		else {
			mystl::insertion_sort(first, last, cmp);
		}
	}


	template <class RandomIter, class Compared>
	void sort(RandomIter first, RandomIter last, Compared cmp)
	{
		if (first != last) {
			mystl::intro_sort(first, last, slg2(last - first) * 2, cmp);
			mystl::final_insertion_sort(first, last, cmp);
		}
	}


	//将所有大于 *nth 的元素放到他后面，小于的放在前面
	template <class RandomIter>
	void ntl_element(RandomIter first, RandomIter nth, RandomIter last)
	{
		if (nth == last) {
			return;
		}
		while (last - first > 3) {
			auto cut = mystl::unchecked_partition(first, last,
				mystl::median(*first, *(first + (lasat - first) / 2), *last));

			if (cut <= nth) {
				first = cut;
			}
			else {
				last = cut;
			}
		}
	}


	template <class RandomIter, class Compared>
	void ntl_element(RandomIter first, RandomIter nth, RandomIter last, Compared cmp)
	{
		if (nth == last) {
			return;
		}
		while (last - first > 3) {
			auto cut = mystl::unchecked_partition(first, last,
				mystl::median(*first, *(first + (lasat - first) / 2), *last, cmp), cmp);

			if (cut <= nth) {
				first = cut;
			}
			else {
				last = cut;
			}
		}
	}

	// [first, last)要求已经排序
	template <class InputIter, class ForwardIter>
	ForwardIter unique_copy_dispatch(InputIter first, InputIter last,
		ForwardIter result, forward_iterator_tag)
	{
		*result = *first;
		while ((++first) != last) {
			if (*result != *first) {
				*(++result) = *first;
			}
		}
		return ++result;
	}


	template <class InputIter, class OutIter>
	OutIter unique_copy_dispatch(InputIter first, InputIter last,
		OutIter result, output_iterator_tag)
	{
		auto value = *first;
		*(result++) = value;
		while ((++first) != last) {
			if (value != *first) {
				value = *first;
				*(result++) = value;
			}
		}
	}


	template <class InputIter, class Iter>
	Iter unique_copy(InputIter first, InputIter last, Iter result)
	{
		if (first == last) {
			return result;
		}
		return unique_copy_dispatch(first, last, result, iterator_category(first));
	}


	template <class InputIter, class ForwardIter, class Compared>
	ForwardIter unique_copy_dispatch(InputIter first, InputIter last,
		ForwardIter result, forward_iterator_tag, Compared cmp)
	{
		*result = *first;
		while ((++first) != last) {
			if (cmp(*result, *first)) {
				*(++result) = *first;
			}
		}
		return ++result;
	}


	template <class InputIter, class OutIter, class Compared>
	OutIter unique_copy_dispatch(InputIter first, InputIter last,
		OutIter result, output_iterator_tag, Compared cmp)
	{
		auto value = *first;
		*(result++) = value;
		while ((++first) != last) {
			if (cmp(value, *first)) {
				value = *first;
				*(result++) = value;
			}
		}
	}


	template <class InputIter, class Iter, class Compared>
	Iter unique_copy(InputIter first, InputIter last, Iter result, Compared cmp)
	{
		if (first == last) {
			return result;
		}
		return unique_copy_dispatch(first, last, result, iterator_category(first), cmp);
	}


	template <class ForwardIter>
	ForwardIter unique(ForwardIter first, ForwardIter last)
	{
		first = mystl::adjacent_find(first, last);
		return mystl::unique_copy(first, last, first);
	}


	template <class ForwardIter, class Compared>
	ForwardIter unique(ForwardIter first, ForwardIter last, Compared cmp)
	{
		first = mystl::adjacent_find(first, last, cmp);
		return mystl::unique_copy(first, last, first, cmp);
	}



}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // 


#endif // !MYSTL_ALGO_H
