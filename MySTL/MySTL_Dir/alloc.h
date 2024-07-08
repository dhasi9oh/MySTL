#ifndef MYSTL_ALLOC_H
#define MYSTL_ALLOC_H

#include <new>
#include <cstddef>
#include <cstdio>
#include <cstdlib>


// 这个头文件目前发现的问题：
// 1.
// _m_chunk_alloc里char*转换为_MemoryBlock*
// 指针的强制性类型转换会使得指针指向的内存大小会改变
// 而内存改变可能使得指针指的范围超出申请过来的内存
// 2.
// 这个头文件用了很多次my_free_list变量，但从来没有用它做过什么
// 3.
// 我甚至都不知道他是怎么把内存教给free_list管理的
// 4.
// 必须要重构

namespace mystl
{
	// 这个类是内存块类
	union _MemoryBlock {
		// union让这个类公用内存，前（n-4）个字节记录内存，后四个字节记录地址
		// 并不需要size来指定，因为不同大小会被分到对应的freelist[index]里
		// size_t			size = 0;
		// 柔性数组
		char			data[1];				// 指向内存池的位置
		_MemoryBlock*	next = nullptr;			// 指向下一块内存块
	};



	// 这个类是要申请的空间大小的枚举类
	enum {
		EAlign128	=	8,
		EAlign256	=	16,
		EAlign512	=	32,
		EAlign1024	=	64,
		EAlign2048	=	128,
		EAlign4096	=	256
	};

	// free_list的个数
	enum {
		EFreeListsNumber = 56
	};

	// 允许申请到的最大位数
	enum {
		EMaxObejectBytes = 4096
	};


	// 一级配置器
	// 实现线程池
	// 我们知道malloc的效率低，所以要减少malloc的次数
	// 如果我们要分配内存较小的对象时却用了malloc，
	// 那么就会让长且完整的内存变得零散，原本够空
	// 间来分配的被这些零散内存分割后变得不够分配了
	// 所以我们可以一开始就分配一大段内存来供这些
	// 小内存使用

	class alloc {
	private:
		// 静态成员要在类外初始化
		static char*		 end_free;							// 这个是结束位置
		static size_t		 heap_size;							// 这个是内存池大小
		static char*		 start_free;						// 这个是内存池起始位置
		static _MemoryBlock* free_list[EFreeListsNumber];		// 这个是释放的内存由他管理

	public:

		static void* allocate(size_t);							// 申请空间

		static void  deallocate(void*,size_t);					// 归还空间

		static void* reallocate(void*,size_t,size_t);			// 扩展空间

	private:

		static size_t _m_freelist_index(size_t);				// 找出该空间应该分配哪个索引给它

		static size_t _m_align(size_t);							// 这个函数是用来找出它的最小枚举
		
		static size_t _m_round_up(size_t);						// 这个函数是用来实现内存对其的

		static void* _m_refill(size_t);

		static void* _m_chunk_alloc(size_t,size_t&);
	};


	// 静态成员必须类外初始化
	_MemoryBlock* alloc::free_list[EFreeListsNumber] = {
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	};


	size_t alloc::heap_size = 0;
	char* alloc::end_free	= nullptr;
	char* alloc::start_free = nullptr;




	inline void* alloc::allocate(size_t bytes) {
		// 要分配的内存太大了就用malloc
		if (bytes > mystl::EMaxObejectBytes) {
			return std::malloc(bytes);
		}
		mystl::_MemoryBlock* my_free_list = free_list[_m_freelist_index(bytes)];
		mystl::_MemoryBlock* result = my_free_list;
		if (result == nullptr) {
			void* p = _m_refill(_m_round_up(bytes));
			return p;
		}
		my_free_list = result->next;
		return result;
	}

	inline void alloc::deallocate(void* p,size_t bytes) {
		// 要释放内存的大小大于最大能分配的大小说明不是由内存池分配的
		if (bytes > static_cast<size_t>(mystl::EMaxObejectBytes)) {
			std::free(p);
			return;
		}
		mystl::_MemoryBlock* q = reinterpret_cast<mystl::_MemoryBlock*>(p);
		mystl::_MemoryBlock* my_free_list = free_list[_m_freelist_index(bytes)];
		q->next = my_free_list;
		my_free_list = q;
	}

	inline void* alloc::reallocate(void* p, size_t old_size, size_t new_size) {
		deallocate(p, old_size);
		p = allocate(new_size);
		return p;
	}

	// 找出比它大的最小枚举
	// 配合_m_round_up实现内存对齐
	inline size_t alloc::_m_align(size_t bytes) {
		if (bytes <= mystl::EAlign2048) {
			if (bytes <= mystl::EAlign1024) {
				if (bytes <= mystl::EAlign512) {
					if (bytes <= mystl::EAlign256) {
						if (bytes <= mystl::EAlign128) {
							return mystl::EAlign128;
						}
						else {
							return mystl::EAlign256;
						}
					}
					else {
						return mystl::EAlign512;
					}
				}
				else {
					return mystl::EAlign1024;
				}
			}
			else {
				return mystl::EAlign2048;
			}
		}
		else {
			return mystl::EAlign4096;
		}
	}

	// 找到能放这么多位的freelist的下标
	// 在小于128位是按每多8位索引加一
	// 在小于256位是按每多16位索引加一
	// 在小于512位是按每多32位索引加一
	// 依次类推
	inline size_t alloc::_m_freelist_index(size_t bytes) {
		if (bytes <= 512) {
			return bytes <= 256
				? bytes <= 128
				? ((bytes + mystl::EAlign128 - 1) / mystl::EAlign128 - 1)
				: (15 + (bytes + mystl::EAlign256 - 129) / mystl::EAlign256)
				: (23 + (bytes + mystl::EAlign512 - 257) / mystl::EAlign512);
		}
		else {
			return bytes <= 2048
				? bytes <= 1024
				? (31 + (bytes + mystl::EAlign1024 - 513) / mystl::EAlign1024)
				: (39 + (bytes + mystl::EAlign2048 - 1025) / mystl::EAlign2048)
				: (47 + (bytes + mystl::EAlign4096 - 2049) / mystl::EAlign4096);
		}
	}

	// 实现内存对其
	inline size_t alloc::_m_round_up(size_t bytes) {
		return ((bytes + _m_align(bytes) - 1) & ~(_m_align(bytes) - 1));
	}
	
	// 内存池初始化也靠它
	inline void* alloc::_m_chunk_alloc(size_t bytes,size_t& count) {
		// bytes是一个对象要多少位，count是要多少个对象
		// 内存池还有多少内存
		size_t pool_bytes = end_free - start_free;
		// 我需要多少位
		size_t need_bytes = count * bytes;
		char* p = nullptr;
		// 如果我够内存分配
		if (pool_bytes >= need_bytes) {
			p = start_free;
			start_free = start_free + pool_bytes;
			return reinterpret_cast<void*>(p);
		}
		// 如果我的内存够至少一个
		else if (pool_bytes >= bytes) {
			count = pool_bytes / bytes;
			p = start_free;
			start_free = start_free + bytes * count;
			return reinterpret_cast<void*>(p);
		}
		else {
			// 如果内存池里内存不为零，把他们挂到freelist由freelist管理
			if (pool_bytes > 0) {
				mystl::_MemoryBlock* my_free_list = free_list[_m_freelist_index(pool_bytes)];
				// 原来指针只取一个字节，强转后指针就会取对应类型的字节数
				reinterpret_cast<mystl::_MemoryBlock*>(start_free)->next = my_free_list;
				// 这一句说实话我感觉没什么用
				my_free_list = reinterpret_cast<mystl::_MemoryBlock*>(start_free);
			}
			size_t bytes_to_get = (need_bytes << 1) + _m_round_up(heap_size >> 4);
			start_free = reinterpret_cast<char*>(std::malloc(bytes_to_get));
			// 如果没申请到内存
			if (start_free == nullptr) {
				mystl::_MemoryBlock* my_free_list = nullptr, * p = nullptr;
				// 看看freelist中有没有足够大的内存块
				for (size_t i = bytes; i <= mystl::EMaxObejectBytes; i++) {
					my_free_list = free_list[_m_freelist_index(i)];
					p = my_free_list;
					// 因为freelist一开始初始化为nullptr
					// 如果freelist为nullptr说明free_list[_m_freelist_index(i)]下没有可用内存
					if (p != nullptr) {
						my_free_list = p->next;
						start_free = reinterpret_cast<char*>(p);
						end_free = start_free + i;
						return _m_chunk_alloc(bytes, count);
					}
				}
				// 如果连freelist里都没内存那就抛错吧~~~
				std::printf("out of memory");
				end_free = nullptr;
				throw std::bad_alloc();
			}
			end_free = start_free + bytes_to_get;
			heap_size += bytes_to_get;
			return _m_chunk_alloc(bytes, count);
		}

	}

	// 从内存池中拿内存
	inline void* alloc::_m_refill(size_t bytes) {
		size_t count = 10;
		char* p = reinterpret_cast<char*>(_m_chunk_alloc(bytes, count));
		if (count == 1) {
			return p;
		}
		mystl::_MemoryBlock* my_free_list = nullptr;
		mystl::_MemoryBlock* result = nullptr,* cur = nullptr,* next = nullptr;
		my_free_list = free_list[_m_freelist_index(bytes)];
		result = reinterpret_cast<mystl::_MemoryBlock*> (p);
		my_free_list = next = reinterpret_cast<mystl::_MemoryBlock*>(p + bytes);
		for (size_t i = 1; ; i++) {
			cur = next;
			next = reinterpret_cast<mystl::_MemoryBlock*> (reinterpret_cast<char*>(next) + bytes);
			if (count - 1 == i) {
				cur->next = nullptr;
				break;
			}
			else {
				cur->next = next;
			}
		}
		return result;
	}

}

#endif // !MYSTL_ALLOC_H
