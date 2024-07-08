#ifndef MYSTL_CONSTRUCT_H
#define MYSTL_CONSTRUCT_H

#include <new>
#include "type_traits.h"
#include "iterator.h"
#include "util.h"


#ifdef _MSC_VER
#pragma warning(push)
// VS下面哪些警告可以由这个生成
#pragma warning(disable : 4100)
#endif // 

namespace mystl {

	// placement new（定位放置new）
	// 在指定位置（地址）调用指定类型的构造函数
	template <class Tp,class T>
	void construct(Tp* p,const T& value) {
		::new ((void*)p) Tp(value);
	}

	template <class Tp, class... Args>
	void construct(Tp* p, Args&&... value) {
		::new ((void*)p) Tp(mystl::forward<Args>(value)...);
	}

	template <class Tp>
	void construct(Tp* p) {
		::new ((void*)p) Tp();
	}

	template <class Tp>
	void destroy_one(Tp*, std::true_type) {}

	// 析构和归还空间相分离
	template <class Tp>
	void destroy_one(Tp* p, std::false_type) {
		if (p != nullptr) {
			p->~Tp();
		}
	}


	template <class Iter>
	void destroy_cat(Iter first,Iter end, std::true_type) {}


	template <class Iter>
	void destroy_cat(Iter first, Iter end, std::false_type) {
		while (first != end) {
			mystl::destroy_one(&(*first));
			++first;
		}
	}
	

	template <class Tp>
	void destroy(Tp* pointer) {
		destroy_one(pointer, std::is_trivially_destructible<Tp>{});
	}


	template <class Iter>
	void destroy(Iter first, Iter end) {
		destroy_cat(first, end, std::is_trivially_destructible<typename iterator_traits<Iter>::value_type>{});
	}


}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER




#endif // !MYSTL_CONSTRUCT_H
