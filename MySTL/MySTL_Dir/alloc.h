#ifndef MYSTL_ALLOC_H
#define MYSTL_ALLOC_H

#include <new>
#include <cstddef>
#include <cstdio>
#include <cstdlib>


// ���ͷ�ļ�Ŀǰ���ֵ����⣺
// 1.
// _m_chunk_alloc��char*ת��Ϊ_MemoryBlock*
// ָ���ǿ��������ת����ʹ��ָ��ָ����ڴ��С��ı�
// ���ڴ�ı����ʹ��ָ��ָ�ķ�Χ��������������ڴ�
// 2.
// ���ͷ�ļ����˺ܶ��my_free_list������������û����������ʲô
// 3.
// ����������֪��������ô���ڴ�̸�free_list�����
// 4.
// ����Ҫ�ع�

namespace mystl
{
	// ��������ڴ����
	union _MemoryBlock {
		// union������๫���ڴ棬ǰ��n-4�����ֽڼ�¼�ڴ棬���ĸ��ֽڼ�¼��ַ
		// ������Ҫsize��ָ������Ϊ��ͬ��С�ᱻ�ֵ���Ӧ��freelist[index]��
		// size_t			size = 0;
		// ��������
		char			data[1];				// ָ���ڴ�ص�λ��
		_MemoryBlock*	next = nullptr;			// ָ����һ���ڴ��
	};



	// �������Ҫ����Ŀռ��С��ö����
	enum {
		EAlign128	=	8,
		EAlign256	=	16,
		EAlign512	=	32,
		EAlign1024	=	64,
		EAlign2048	=	128,
		EAlign4096	=	256
	};

	// free_list�ĸ���
	enum {
		EFreeListsNumber = 56
	};

	// �������뵽�����λ��
	enum {
		EMaxObejectBytes = 4096
	};


	// һ��������
	// ʵ���̳߳�
	// ����֪��malloc��Ч�ʵͣ�����Ҫ����malloc�Ĵ���
	// �������Ҫ�����ڴ��С�Ķ���ʱȴ����malloc��
	// ��ô�ͻ��ó����������ڴ�����ɢ��ԭ������
	// ��������ı���Щ��ɢ�ڴ�ָ���ò���������
	// �������ǿ���һ��ʼ�ͷ���һ����ڴ�������Щ
	// С�ڴ�ʹ��

	class alloc {
	private:
		// ��̬��ԱҪ�������ʼ��
		static char*		 end_free;							// ����ǽ���λ��
		static size_t		 heap_size;							// ������ڴ�ش�С
		static char*		 start_free;						// ������ڴ����ʼλ��
		static _MemoryBlock* free_list[EFreeListsNumber];		// ������ͷŵ��ڴ���������

	public:

		static void* allocate(size_t);							// ����ռ�

		static void  deallocate(void*,size_t);					// �黹�ռ�

		static void* reallocate(void*,size_t,size_t);			// ��չ�ռ�

	private:

		static size_t _m_freelist_index(size_t);				// �ҳ��ÿռ�Ӧ�÷����ĸ���������

		static size_t _m_align(size_t);							// ��������������ҳ�������Сö��
		
		static size_t _m_round_up(size_t);						// �������������ʵ���ڴ�����

		static void* _m_refill(size_t);

		static void* _m_chunk_alloc(size_t,size_t&);
	};


	// ��̬��Ա���������ʼ��
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
		// Ҫ������ڴ�̫���˾���malloc
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
		// Ҫ�ͷ��ڴ�Ĵ�С��������ܷ���Ĵ�С˵���������ڴ�ط����
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

	// �ҳ����������Сö��
	// ���_m_round_upʵ���ڴ����
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

	// �ҵ��ܷ���ô��λ��freelist���±�
	// ��С��128λ�ǰ�ÿ��8λ������һ
	// ��С��256λ�ǰ�ÿ��16λ������һ
	// ��С��512λ�ǰ�ÿ��32λ������һ
	// ��������
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

	// ʵ���ڴ����
	inline size_t alloc::_m_round_up(size_t bytes) {
		return ((bytes + _m_align(bytes) - 1) & ~(_m_align(bytes) - 1));
	}
	
	// �ڴ�س�ʼ��Ҳ����
	inline void* alloc::_m_chunk_alloc(size_t bytes,size_t& count) {
		// bytes��һ������Ҫ����λ��count��Ҫ���ٸ�����
		// �ڴ�ػ��ж����ڴ�
		size_t pool_bytes = end_free - start_free;
		// ����Ҫ����λ
		size_t need_bytes = count * bytes;
		char* p = nullptr;
		// ����ҹ��ڴ����
		if (pool_bytes >= need_bytes) {
			p = start_free;
			start_free = start_free + pool_bytes;
			return reinterpret_cast<void*>(p);
		}
		// ����ҵ��ڴ湻����һ��
		else if (pool_bytes >= bytes) {
			count = pool_bytes / bytes;
			p = start_free;
			start_free = start_free + bytes * count;
			return reinterpret_cast<void*>(p);
		}
		else {
			// ����ڴ�����ڴ治Ϊ�㣬�����ǹҵ�freelist��freelist����
			if (pool_bytes > 0) {
				mystl::_MemoryBlock* my_free_list = free_list[_m_freelist_index(pool_bytes)];
				// ԭ��ָ��ֻȡһ���ֽڣ�ǿת��ָ��ͻ�ȡ��Ӧ���͵��ֽ���
				reinterpret_cast<mystl::_MemoryBlock*>(start_free)->next = my_free_list;
				// ��һ��˵ʵ���Ҹо�ûʲô��
				my_free_list = reinterpret_cast<mystl::_MemoryBlock*>(start_free);
			}
			size_t bytes_to_get = (need_bytes << 1) + _m_round_up(heap_size >> 4);
			start_free = reinterpret_cast<char*>(std::malloc(bytes_to_get));
			// ���û���뵽�ڴ�
			if (start_free == nullptr) {
				mystl::_MemoryBlock* my_free_list = nullptr, * p = nullptr;
				// ����freelist����û���㹻����ڴ��
				for (size_t i = bytes; i <= mystl::EMaxObejectBytes; i++) {
					my_free_list = free_list[_m_freelist_index(i)];
					p = my_free_list;
					// ��Ϊfreelistһ��ʼ��ʼ��Ϊnullptr
					// ���freelistΪnullptr˵��free_list[_m_freelist_index(i)]��û�п����ڴ�
					if (p != nullptr) {
						my_free_list = p->next;
						start_free = reinterpret_cast<char*>(p);
						end_free = start_free + i;
						return _m_chunk_alloc(bytes, count);
					}
				}
				// �����freelist�ﶼû�ڴ��Ǿ��״��~~~
				std::printf("out of memory");
				end_free = nullptr;
				throw std::bad_alloc();
			}
			end_free = start_free + bytes_to_get;
			heap_size += bytes_to_get;
			return _m_chunk_alloc(bytes, count);
		}

	}

	// ���ڴ�������ڴ�
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
