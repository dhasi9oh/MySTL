#ifndef MYSTL_UNINITIALIZED_H
#define MYSTL_UNINITIALIZED_H


#include "type_traits.h"
#include "construct.h"
#include "algobase.h"
#include "iterator.h"
#include "util.h"

namespace mystl {

	template <class InputIter, class ForwardIter>
	ForwardIter uncheck_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::true_type)
	{
		return mystl::copy(first, last, result);
	}

	template <class InputIter, class ForwardIter>
	ForwardIter uncheck_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::false_type)
	{
		auto begin = result;
		try {
			for (; first != last; begin++, first++) {
				mystl::construct(&*begin, *first);
			}
		}
		catch {
			while (result != begin) {
				mystl::destroy(&*result);
				--begin;
			}
		}
		return begin;
	}

	template <class InputIter, class ForwardIter>
	ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result)
	{
		return mystl::uncheck_uninit_copy(first, last, result,
			std::is_trivially_copy_assignable<typename iterator_traits<ForwardIter>::value_type>{});
	}

	template <class InputIter, class Size, class ForwardIter>
	ForwardIter uncheck_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::true_type)
	{
		return mystl::copy_n(first, n, result).second;
	}

	template <class InputIter, class Size, class ForwardIter>
	ForwardIter uncheck_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::false_type)
	{
		auto begin = result;
		try {
			for (; n > 0; n--, first++, begin++) {
				mystl::construct(&*result, *first);
			}
		}
		catch {
			while (begin != result) {
				mystl::destroy(&*begin);
				--begin;
			}
		}
		return begin;
	}

	template <class InputIter, class Size, class ForwardIter>
	ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result)
	{
		return mystl::uncheck_uninit_copy_n(first, n, result,
			std::is_trivially_copy_assignable<typename mystl::iterator_traits<ForwardIter>::value_type>{});
	}

	template <class InputIter, class T>
	InputIter uncheck_uninit_fill(InputIter first, InputIter last, const T& value, std::true_type)
	{
		return mystl::fill(first, last, value);
	}

	template <class InputIter, class T>
	InputIter uncheck_uninit_fill(InputIter first, InputIter last, const T& value, std::false_type)
	{
		auto result = first;
		try {
			for (; first != last; first++) {
				mystl::construct(&*first, value);
			}
		}
		catch {
			while (result != first) {
				mystl::destroy(&*first);
				first--;
			}
		}
		return first;
	}

	template <class InputIter, class T>
	InputIter uninitialized_fill(InputIter first, InputIter last, const T& value)
	{
		return mystl::uncheck_uninit_fill(first, last, value,
			std::is_trivially_copy_assignable<typename mystl::iterator_traits<T>::value_type>{});
	}


	template <class InputIter, class T, class Size>
	InputIter uncheck_uninit_fill_n(InputIter first, Size n, const T& value, std::true_type)
	{
		return mystl::fill_n(first, n, value);
	}


	template <class InputIter, class T, class Size>
	InputIter uncheck_uninit_fill_n(InputIter first, Size n, const T& value, std::false_type)
	{
		auto begin = first;
		try {
			for (; n > 0; n--, first++) {
				mystl::construct(&*first, value);
			}
		}
		catch {
			while (first != begin) {
				mystl::destroy(&*first);
				--first;
			}
		}
		return first;
	}


	template <class InputIter, class T, class Size>
	InputIter uninitialized_fill_n(InputIter first, Size n, const T& value)
	{
		return mystl::uncheck_uninit_fill_n(first, n, value,
			std::is_trivially_copy_assignable<typename mystl::iterator_traits<InputIter>::value_type>{});
	}


	template <class InputIter, class ForwardIter>
	InputIter uncheck_uninit_move(InputIter first, InputIter last, ForwardIter result, std::true_type)
	{
		return mystl::move(first, last, result);
	}


	template <class InputIter, class ForwardIter>
	InputIter uncheck_uninit_move(InputIter first, InputIter last, ForwardIter result, std::false_type)
	{
		auto begin = first;
		try {
			for (; first != last; first++) {
				mystl::construct(&*result, mystl::move(*first));
			}
		}
		catch {
			mystl::destroy(begin, first);
		}
		return first;
	}


	template <class InputIter, class ForwardIter>
	InputIter uninitialized_move(InputIter first, InputIter last, ForwardIter result)
	{
		return mystl::uncheck_uninit_move(first, last, result,
			std::is_trivially_move_assignable<typename mystl::iterator_traits<InputIter>::value_type> {});
	}


	template <class InputIter, class ForwardIter, class Size>
	InputIter uncheck_uninit_move_n(InputIter first, Size n, ForwardIter result, std::true_type)
	{
		return mystl::move(first, first + n, result);
	}


	template <class InputIter, class ForwardIter, class Size>
	InputIter uncheck_uninit_move_n(InputIter first, Size n, ForwardIter result, std::false_type)
	{
		auto begin = first;
		try {
			for (; n > 0; n--, first++) {
				mystl::construct(&*result, mystl::move(*first));
			}
		}
		catch {
			mystl::destroy(begin, first);
		}
		return first;
	}


	template <class InputIter, class ForwardIter, class Size>
	InputIter uninitialized_move_n(InputIter first, Size n, ForwardIter result)
	{
		return uncheck_uninit_move_n(first, n, result,
			std::is_trivially_move_assignable<typename mystl::iterator_traits<InputIter>::value_type>{});
	}



}




#endif