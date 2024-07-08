#ifndef MYSTL_ALLOCATOR_H
#define MYSTL_ALLOCATOR_H

#include "util.h"
#include "construct.h"

namespace mystl {

	template <class T>
	class allocator {

	public:

		typedef T			value_type;
		typedef T*			pointer;
		typedef T&			reference;
		typedef const T*	const_pointer;
		typedef const T&	const_reference;
		typedef size_t		size_type;
		typedef ptrdiff_t	difference_type;

	public:

		static T* allocate();
		static T* allocate(size_type n);

		static void deallocate(T*);
		static void deallocate(T*, size_type);

		static void construct(T*);
		static void construct(T*, const T&);
		static void construct(T*, T&&);
		template <class... Args>
		static void construct(T* ptr, Args&& ...);


		static void destroy(T*);
		static void destroy(T*, T*);
	};





	template<class T>
	inline T* allocator<T>::allocate()
	{
		return static_cast<T*> (::operator new(sizeof(T)));
	}

	template<class T>
	inline T* allocator<T>::allocate(size_type n)
	{
		return static_cast<T*> (::operator new(n * sizeof(T)));
	}

	template<class T>
	inline void allocator<T>::deallocate(T* ptr)
	{
		if (ptr == nullptr) {
			return;
		}
		::operator delete(ptr);
	}

	template<class T>
	inline void allocator<T>::deallocate(T* ptr, size_type)
	{
		if (ptr == nullptr) {
			return;
		}
		::operator delete(ptr);
	}

	template<class T>
	inline void allocator<T>::construct(T* ptr)
	{
		mystl::construct(ptr);
	}

	template<class T>
	inline void allocator<T>::construct(T* ptr, const T& value)
	{
		mystl::construct(ptr, value);
	}

	template<class T>
	inline void allocator<T>::construct(T* ptr, T&& value)
	{
		mystl::construct(ptr, value);
	}

	template<class T>
	inline void allocator<T>::destroy(T* ptr)
	{
		mystl::destroy(ptr);
	}

	template<class T>
	inline void allocator<T>::destroy(T* first, T* last)
	{
		mystl::destroy(first, last);
	}

	template<class T>
	template<class ...Args>
	inline void allocator<T>::construct(T* ptr, Args && ...args)
	{
		mystl::construct(ptr, mystl::forward<Args>(args)...);
	}

}



#endif // !MYSTL_ALLOCATOR_H
