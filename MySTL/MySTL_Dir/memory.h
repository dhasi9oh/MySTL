#ifndef MYSTL_MEMORY_H
#define MYSTL_MEMORY_H

// 这个文件是关于缓冲区的

#include <cstddef>
#include <limits>
#include <cstdlib>

#include "algobase.h"
#include "allocator.h"
#include "uninitialized.h"
#include "construct.h"


namespace mystl {

	//这个函数是从一个迭代器中取出指向位置的地址
	template <class T>
	constexpr T* address_of(T& value) noexcept {
		return &value;
	}

	template <class T>
	pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T*) {
		if (len > INT_MAX / sizeof(T)) {
			len = INT_MAX / sizeof(T);

		}
		while (len != 0) {
			T* ptr = static_cast<T*>(malloc(len * sizeof(T)));
			if (ptr != nullptr) {
				return pair<T*, ptrdiff_t>(ptr, len);
			}
			len /= 2;
		}
		return pair<T*, ptrdiff_t>(nullptr, 0)
	}

	template <class T>
	pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T*) {
		return get_buffer_helper(len, static_cast<T*>(0));
	}

	template <class T>
	pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len) {
		return get_buffer_helper(len, static_cast<T*>(0))
	}

	template <class T>
	void release_temporary_buffer(T* p) {
		if (p != nullptr) {
			free(p);
		}
	}


	template <class ForwardIter, class T>
	class temporary_buffer {
	private:
		ptrdiff_t len;
		ptrdiff_t original_len;
		T* ptr;

	public:
		temporary_buffer(ForwardIter first, ForwardIter last);

		~temporary_buffer() { mystl::destroy(ptr, ptr + len); free(ptr); }


	public:
		ptrdiff_t size() { return len; }

		ptrdiff_t requested_size() { return original_len; }

		T* begin() { return ptr; }

		T* end() { return ptr + len; }

	private:
		void allocate_buffer();

		void initialize_buffer(const T&, std::true_type) {}

		void initialize_buffer(const T& value, std::false_type)
		{
			mystl::uninitialized_fill_n(ptr, len, value);
		}
		
	private:
		temporary_buffer(const temporary_buffer&) = default;

		temporary_buffer& operator=(const temporary_buffer&) = default;


	};


	template<class ForwardIter, class T>
	inline temporary_buffer<ForwardIter, T>::temporary_buffer(ForwardIter first, ForwardIter last)
	{
		try {
			len = mystl::distance(first, last);
			allocate_buffer();
			if (len > 0) {
				initialize_buffer(*first, std::is_trivially_default_constructible<T>::value);
			}
		}
		catch (...) {
			free(ptr);
			ptr = nullptr;
			len = 0;
		}
	}

	template<class ForwardIter, class T>
	inline void temporary_buffer<ForwardIter, T>::allocate_buffer()
	{
		original_len = len;
		if (len > INT_MAX / sizeof(T)) {
			original_len = INT_MAX / sizeof(T);
		}
		while (original_len != 0) {
			ptr = static_cast<T*>(malloc(len * sizeof(T)));
			if (ptr != nullptr) {
				break;
			}
			original_len /= 2;
		}
	}

	

	template <class T>
	class auto_ptr {
	public:
		typedef T elem_type;

	private:
		T* m_ptr;

	public:
		explicit auto_ptr(T* rhs = nullptr) : m_ptr(rhs) {}

		auto_ptr(const auto_ptr& rhs) : m_ptr(rhs.release()) {}

		template <class U>
		auto_ptr(auto_ptr<U>& rhs) : m_ptr(rhs.release()) {}

		auto_ptr& operator=(auto_ptr& rhs) {
			if (this != &rhs) {
				delete m_ptr;
				m_ptr = rhs.release();
			}
			return *this;
		}

		template <class U>
		auto_ptr& operator=(auto_ptr<U>& rhs) {
			if (this != &rhs) {
				delete m_ptr;
				m_ptr = rhs.release();
			}
			return *this;
		}

		~auto_ptr() { delete m_ptr; }

	private:
		
		T& operator*() const { return *m_ptr; }
		T* operator->() const { return m_ptr; }

		T* get() const { return m_ptr; }

		void reset(T* p = nullptr) {
			if (m_ptr != p) {
				delete m_ptr;
				m_ptr = p;
			}
		}
	};


}


#endif // !MYSTL_MEMORY_H
