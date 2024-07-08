#ifndef MYSTL_NUMERIC_H

#include "iterator.h"


namespace mystl {
	
	template <class T,class Iter>
	T accumulate(Iter first, Iter end, T init) {
		for (; first != end; first++) {
			init += *first;
		}
		return init;
	}

	template <class T,class Iter,class BinaryOp>
	T accumulate(Iter first, Iter end, T init, BinaryOp binaryop) {
		for (; first != end; first++) {
			init = binaryop(init, *first);
		}
		return init;
	}

	template <class InputIter,class OutIter>
	OutIter adjacent_difference(InputIter first, InputIter end, OutIter result) {
		if (first == result) {
			return result;
		}
		*result = *first;
		auto value = *first;
		while (++first != end) {
			auto tmp = *first;
			*(++result) = tmp - value;
			value = tmp;
		}
		return ++result;
	}

	template <class InputIter,class OutIter,class BinaryOp>
	OutIter adjacent_difference(InputIter first, InputIter end, OutIter result, BinaryOp binaryop) {
		if (first == result) {
			return result;
		}
		*result = *first;
		auto value = *first;
		while (++first != end) {
			auto tmp = *first;
			*(++result) = binaryop(tmp, value);
			value = tmp;
		}
		return ++result;
	}

	template <class Iter1,class Iter2,class T>
	T inner_product(Iter1 first, Iter1 last, Iter2 begin, T init) {
		for (; first != last; first++, begin++) {
			init = init + (*first * *begin);
		}
		return init;
	}


	template <class Iter1, class Iter2, class T,
		class BinaryOp1,class BinaryOp2>
	T inner_product(Iter1 first, Iter1 last, Iter2 begin, T init, BinaryOp1 binaryop1, BinaryOp2 binaryop2) {
		for (; first != last; first++, begin++) {
			init = binaryop1(init, binaryop2(*begin, *first));
		}
		return init;
	}


	template <class Iter,class T>
	void iota(Iter first, Iter end, T value) {
		for (; first != end; ++first) {
			*first = value;
			++value;
		}
	}


	template <class Iter, class T>
	Iter partial_sum(Iter first, Iter end, Iter begin) {
		if (first != begin) {
			return begin;
		}
		*begin = *first;
		auto value = *first;
		while (++first != end) {
			value = *first + value;
			*(++begin) = value;
		}
		return ++begin;
	}


	template <class Iter, class T, class BinaryOp>
	Iter partial_sum(Iter first, Iter end, Iter begin, BinaryOp binaryop) {
		if (first != begin) {
			return begin;
		}
		*begin = *first;
		auto value = *first;
		while (++first != end) {
			value = binaryop(*first, value);
			*(++begin) = value;
		}
		return ++begin;
	}

}




#endif // !MYSTL_NUMERIC_H
