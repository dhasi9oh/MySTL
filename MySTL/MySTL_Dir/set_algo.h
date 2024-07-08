#ifndef MYSTL_SET_ALGO_H
#define MYSTL_SET_ALGO_H

// 这个头文件所有函数要求有序

#include "algobase.h"
#include "iterator.h"

namespace mystl {

	template <class ForwardIter1, class ForwardIter2, class OutIter>
	OutIter set_union(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2, OutIter result)
	{
		while (first1 != last1 && first2 != last2) {
			if (*first1 < *first2) {
				*result = *first1;
				first1++;
			}
			else if(*first2 < *first1) {
				*result = *first2;
				first2++;
			}
			else {
				*result = *first1;
				first1++, first2++;
			}
			++result;
		}
		return mystl::copy(first1, last1, mystl::copy(first2, last2, result));
	}


	template <class ForwardIter1, class ForwardIter2, class OutIter, class Compared>
	OutIter set_union(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2, OutIter result, Compared cmp)
	{
		while (first1 != last1 && first2 != last2) {
			if (cmp(*first1, *first2)) {
				*result = *first1;
				first1++;
			}
			else if (cmp(*first2, *first1)) {
				*result = *first2;
				first2++;
			}
			else {
				*result = *first1;
				first1++, first2++;
			}
			++result;
		}
		return mystl::copy(first1, last1, mystl::copy(first2, last2, result));
	}


	template <class ForwardIter1, class ForwardIter2, class OutIter>
	OutIter set_intersection(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2, OutIter result)
	{
		while (first1 != last1 && &first2 != last2) {
			if (*first1 < *first2) {
				++first1;
			}
			else if (*first2 < *first1) {
				++first2;
			}
			else {
				*result = *first1;
				first1++;
				first2++;
				result++;
			}
		}
		return result;
	}
	

	template <class ForwardIter1, class ForwardIter2, class OutIter, class Compared>
	OutIter set_intersection(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2, OutIter result, Compared cmp)
	{
		while (first1 != last1 && &first2 != last2) {
			if (cmp(*first1, *first2)) {
				++first1;
			}
			else if (cmp(*first2, *first1)) {
				++first2;
			}
			else {
				*result = *first1;
				first1++;
				first2++;
				result++;
			}
		}
		return result;
	}


	template <class ForwardIter1, class ForwardIter2, class OutIter>
	OutIter set_difference(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2, OutIter result)
	{
		while (first1 != last1 && first2 != last2) {
			if (*first1 < *first2) {
				*result = *first1;
				result++;
				first1++;
			}
			else if (*first2 < *first1) {
				++first2;
			}
			else {
				++first1, ++first2;
			}
		}
		return result;
	}


	template <class ForwardIter1, class ForwardIter2, class OutIter, class Compared>
	OutIter set_difference(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2, OutIter result, Compared cmp)
	{
		while (first1 != last1 && first2 != last2) {
			if (cmp(*first1, *first2)) {
				*result = *first1;
				result++;
				first1++;
			}
			else if (cmp(*first2, *first1)) {
				++first2;
			}
			else {
				++first1, ++first2;
			}
		}
		return result;
	}


	template <class ForwardIter1, class ForwardIter2, class OutIter>
	OutIter set_symmetric_difference(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2, OutIter result)
	{
		while (first1 != last1 && first2 != last2) {
			if (*first1 < *first2) {
				*result = *first1;
				first1++, result++;
			}
			else if (*first2 < *first1) {
				*result = *first2;
				first2++, result++;
			}
			else {
				first1++, first2++;
			}
		}
		return mystl::copy(first1, last1, mystl::copy(first2, last2, result));
	}


	template <class ForwardIter1, class ForwardIter2, class OutIter, class Compared>
	OutIter set_symmetric_difference(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2, OutIter result, Compared cmp)
	{
		while (first1 != last1 && first2 != last2) {
			if (cmp(*first1, *first2)) {
				*result = *first1;
				first1++, result++;
			}
			else if (cmp(*first2, *first1)) {
				*result = *first2;
				first2++, result++;
			}
			else {
				first1++, first2++;
			}
		}
		return mystl::copy(first1, last1, mystl::copy(first2, last2, result));
	}



}





#endif // !MYSTL_SET_ALGO_H
