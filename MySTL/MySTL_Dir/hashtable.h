#ifndef MYSTL_HASHTABLE_H
#define MYSTL_HASHTABLE_H

#include <initializer_list>

#include "algo.h"
#include "functional.h"
#include "memory.h"
#include "vector.h"
#include "util.h"
#include "exceptdef.h"

namespace mystl {

	template <class T>
	struct  hashtable_node
	{
		hashtable_node* next;
		T value;

		hashtable_node() = default;

		hashtable_node(const T& data): next(nullptr), value(data) {}

		hashtable_node(const hashtable_node& rhs): next(rhs.next), value(rhs.value) {}

		hashtable_node(hashtable_node&& rhs) : next(rhs.next), value(mystl::move(rhs.value))
		{
			rhs.next = nullptr;
		}
	};

	// 和 iterator 中一样的设计，特例化 pair
	template <class T, bool>
	struct ht_value_traits_imp
	{
		typedef T	key_type;
		typedef T	mapped_type;
		typedef T	value_type;

		template <class Ty>
		static const key_type& get_key(const Ty& value)
		{
			return value;
		}

		template <class Ty>
		static const value_type& get_value(const Ty& value)
		{
			return value;
		}

	};


	template <class T>
	struct ht_value_traits_imp<T, true>
	{
		// remove_cv 去除 type 中的 const 和 volatile 关键字
		typedef typename std::remove_cv<typename T::first_type>::type	key_type;
		typedef typename T::second_type	mapped_type;
		typedef T	value_type;

		template <class Ty>
		static const key_type& get_key(const Ty& value)
		{
			return value.first;
		}

		template <class Ty>
		static const value_type& get_value(const Ty& value)
		{
			return value;
		}
	};

	
	template <class T>
	struct ht_value_traits
	{
		static constexpr bool is_map = mystl::is_pair<T>::value;

		typedef  ht_value_traits_imp<T, is_map>				value_traits_type;
		typedef  typename value_traits_type::key_type		key_type;
		typedef  typename value_traits_type::mapped_type	mapped_type;
		typedef  typename value_traits_type::value_type		value_type;

		template <class Ty>
		static const key_type& get_key(const Ty& value)
		{
			return value_traits_type::get_key(value);
		}

		template <class Ty>
		static const value_type& get_value(const Ty& value)
		{
			return value_traits_type::get_value(value);
		}

	};

	// 声明
	template <class T, class HashFun, class KeyEqual>
	class hashtable;

	template <class T, class HashFun, class KeyEqual>
	struct ht_iterator;

	template <class T, class HashFun, class KeyEqual>
	struct ht_const_iterator;

	template <class T>
	struct ht_local_iterator;

	template <class T>
	struct ht_const_local_iterator;

	// 这个是 hash 表上数组的迭代器
	template <class T, class Hash, class KeyEqual>
	struct ht_iterator_base: public mystl::iterator<mystl::forward_iterator_tag, T>
	{
		typedef mystl::hashtable<T, Hash, KeyEqual>				hashtable;
		typedef ht_iterator_base<T, Hash, KeyEqual>				base;
		typedef mystl::ht_iterator<T, Hash, KeyEqual>			iterator;
		typedef mystl::ht_const_iterator<T, Hash, KeyEqual>		const_iterator;
		typedef hashtable_node<T>*								node_ptr;
		typedef hashtable*										contain_ptr;
		typedef const node_ptr									const_node_ptr;
		typedef const contain_ptr								const_contain_ptr;

		typedef size_t		size_type;
		typedef ptrdiff_t	difference_type;

		node_ptr node;
		contain_ptr ht;
		
		ht_iterator_base() = default;

		bool operator==(const base& rhs) 
		{
			return node == rhs.node;
		}

		bool operator!=(const base& rhs)
		{
			return node != rhs.node;
		}

	};


	template <class T, class Hash, class KeyEqual>
	struct ht_iterator: public ht_iterator_base<T, Hash, KeyEqual>
	{
		typedef ht_iterator_base<T, Hash, KeyEqual> base;
		typedef typename base::hashtable			hashtable;
		typedef typename base::iterator				iterator;
		typedef typename base::const_iterator		const_iterator;
		typedef typename base::node_ptr				node_ptr;
		typedef typename base::contain_ptr			contain_ptr;

		typedef ht_value_traits<T>					value_traits;
		typedef T									value_type;
		typedef value_type*							pointer;
		typedef value_type&							reference;

		using base::node;
		using base::ht;

		ht_iterator() = default;

		ht_iterator(node_ptr n, contain_ptr t) : node(n), ht(t) {}

		ht_iterator(const iterator& rhs)
		{
			node = rhs.node;
			ht = rhs.ht;
		}

		ht_iterator(const const_iterator& rhs)
		{
			node = rhs.node;
			ht = rhs.ht;
		}

		iterator& operator=(const iterator& rhs)
		{
			if (&rhs != this) {
				node = rhs.node;
				ht = rhs.ht;
			}
			return *this;
		}

		iterator& operator=(const const_iterator& rhs)
		{
			if (&rhs != this) {
				node = rhs.node;
				ht = rhs.ht;
			}
			return *this;
		}

		reference operator*() const
		{
			return node->value;
		}

		pointer operator->() const
		{
			return &(operator*());
		}

		iterator& operator++()
		{
			MYSTL_DEBUG(node != nullptr);
			const node_ptr old = node;
			node = node->next;
			if (node == nullptr) {
				auto idx = ht->hash(value_traits::get_key(old->value));
				while (node == nullptr && ++idx < ht->mBucketSize) {
					node = ht->mBucket[idx];
				}
			}
			return *this;
		}

		iterator operator++(int)
		{
			iterator tmp = * this;
			++* this;
			return tmp;
		}

	};


	template <class T, class Hash, class KeyEqual>
	struct ht_cosnt_iterator : public ht_iterator_base<T, Hash, KeyEqual>
	{
		typedef ht_iterator_base<T, Hash, KeyEqual> base;
		typedef typename base::hashtable			hashtable;
		typedef typename base::iterator				iterator;
		typedef typename base::const_iterator		const_iterator;
		typedef typename base::const_node_ptr		node_ptr;
		typedef typename base::const_contain_ptr	contain_ptr;

		typedef ht_value_traits<T>					value_traits;
		typedef T									value_type;
		typedef const value_type*					pointer;
		typedef const value_type&					reference;

		using base::node;
		using base::ht;

		ht_cosnt_iterator() = default;

		ht_cosnt_iterator(node_ptr n, contain_ptr t) : node(n), ht(t) {}

		ht_cosnt_iterator(const iterator& rhs)
		{
			node = rhs.node;
			ht = rhs.ht;
		}

		ht_cosnt_iterator(const const_iterator& rhs)
		{
			node = rhs.node;
			ht = rhs.ht;
		}

		const_iterator& operator=(const iterator& rhs)
		{
			if (&rhs != this) {
				node = rhs.node;
				ht = rhs.ht;
			}
			return *this;
		}

		const_iterator& operator=(const const_iterator& rhs)
		{
			if (&rhs != this) {
				node = rhs.node;
				ht = rhs.ht;
			}
			return *this;
		}

		reference operator*() const
		{
			return node->value;
		}

		pointer operator->() const
		{
			return &(operator*());
		}

		iterator& operator++()
		{
			MYSTL_DEBUG(node != nullptr);
			const node_ptr old = node;
			node = node->next;
			if (node == nullptr) {
				auto idx = ht->hash(value_traits::get_key(old->value));
				while (node == nullptr && ++idx < ht->mBucketSize) {
					node = ht->mBucket[idx];
				}
			}
			return *this;
		}

		iterator operator++(int)
		{
			iterator tmp = *this;
			++* this;
			return tmp;
		}

	};

	// 这个是链表的迭代器
	template <class T>
	struct ht_local_iterator : public mystl::iterator<mystl::forward_iterator_tag, T>
	{
		using value_type	= T;
		using pointer = value_type*;
		using reference = value_type&;
		using size_type = size_t;
		using different_type = ptrdiff_t;
		using node_ptr = hashtable_node<T>*;

		using self = ht_local_iterator<T>;
		using local_iterator = ht_local_iterator<T>;
		using const_local_iterator = ht_const_local_iterator<T>;

		node_ptr node;

		ht_local_iterator(node_ptr n): node(n) {}

		ht_local_iterator(const local_iterator& rhs): node(rhs.node) {}

		ht_local_iterator(const const_local_iterator& rhs) : node(rhs.node) {}

		reference operator*()
		{
			return node->value;	
		}

		pointer operator->()
		{
			return &node->value;
		}

		self& operator++()
		{
			MYSTL_DEBUG(node != nullptr);
			node = node->next;
			return *this;
		}

		self operator++(int)
		{
			self tmp = *this;
			++* this;
			return tmp;
		}

		bool operator==(const self& rhs)
		{
			return node == rhs.node;
		}

		bool operator!=(const self& rhs)
		{
			return node != rhs.node;
		}

	};


	template <class T>
	struct ht_const_local_iterator : public mystl::iterator<mystl::forward_iterator_tag, T>
	{
		using value_type = T;
		using pointer = const value_type*;
		using reference = const value_type&;
		using size_type = size_t;
		using different_type = ptrdiff_t;
		using node_ptr = const hashtable_node<T>*;

		using self = ht_const_local_iterator<T>;
		using local_iterator = ht_local_iterator<T>;
		using const_local_iterator = ht_const_local_iterator<T>;

		node_ptr node;

		ht_const_local_iterator(node_ptr n) : node(n) {}

		ht_const_local_iterator(const ht_local_iterator& rhs) : node(rhs.node) {}

		ht_const_local_iterator(const ht_const_local_iterator& rhs) : node(rhs.node) {}

		reference operator*()
		{
			return node->value;
		}

		pointer operator->()
		{
			return &node->value;
		}

		self& operator++()
		{
			MYSTL_DEBUG(node != nullptr);
			node = node->next;
			return *this;
		}

		self operator++(int)
		{
			self tmp = *this;
			++* this;
			return tmp;
		}

		bool operator==(const self& rhs)
		{
			return node == rhs.node;
		}

		bool operator!=(const self& rhs)
		{
			return node != rhs.node;
		}
	};

	// 用来确定是哪个编译器来编译这段程序
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
#define SYSTEM_64 1
#else
#define SYSTEM_32 1
#endif

#ifdef SYSTEM_64

#define PRIME_NUM 99

	static constexpr size_t ht_prime_list[] = {
  101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
  4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
  119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
  2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
  23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
  264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
  2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
  15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
  115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull,
  585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull,
  2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
  15013704261371ull, 22520556392057ull, 33780834588157ull, 50671251882247ull,
  76006877823377ull, 114010316735089ull, 171015475102649ull, 256523212653977ull,
  384784818980971ull, 577177228471507ull, 865765842707309ull, 1298648764060979ull,
  1947973146091477ull, 2921959719137273ull, 4382939578705967ull, 6574409368058969ull,
  9861614052088471ull, 14792421078132871ull, 22188631617199337ull, 33282947425799017ull,
  49924421138698549ull, 74886631708047827ull, 112329947562071807ull, 168494921343107851ull,
  252742382014661767ull, 379113573021992729ull, 568670359532989111ull, 853005539299483657ull,
  1279508308949225477ull, 1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull,
  6477510814055453699ull, 9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
	};

#else

#define PRIME_NUM 44
	static constexpr size_t ht_prime_list[] = {
  101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u,
  10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
  402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
  10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
  176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
  2006689337u, 3010034021u, 4294967291u,
	};

#endif // SYSTEM_64

	inline size_t ht_next_prime(size_t n)
	{
		const size_t* first = ht_prime_list;
		const size_t* last = ht_prime_list + PRIME_NUM;
		const size_t* pos = mystl::lower_bound(first, last, n);
		return pos == last ? *(last - 1) : *pos;
	}

	template <class T, class Hash, class KeyEqual>
	class hashtable
	{
		friend struct mystl::ht_iterator<T, Hash, KeyEqual>;
		friend struct mystl::ht_const_iterator<T, Hash, KeyEqual>;

	public:

		using value_traits	= ht_value_traits<T>;
		using key_type		= typename value_traits::key_type;
		using mapped_type	= typename value_traits::mapped_type;
		using value_type	= typename value_traits::value_type;
		using hasher		= Hash;
		using key_equal		= KeyEqual;

		using node_type		= hashtable_node<T>;
		using node_ptr		= hashtable_node<T>*;
		using bucket_type	= mystl::vector<node_ptr>;

		using allocator_type = mystl::allocator<T>;
		using data_allocator = mystl::allocator<T>;
		using node_allocator = mystl::allocator<node_type>;

		using pointer			= typename allocator_type::pointer;
		using const_pointer		= typename allocator_type::const_pointer;
		using reference			= typename allocator_type::reference;
		using const_reference	= typename allocator_type::const_reference;
		using size_type			= typename allocator_type::size_type;
		using difference_type	= typename allocator_type::difference_type;

		using iterator				= mystl::ht_iterator<T, Hash, KeyEqual>;
		using const_iterator		= mystl::ht_const_iterator<T, Hash, KeyEqual>;
		using local_iterator		= mystl::ht_local_iterator<T>;
		using const_local_iterator	= mystl::ht_const_local_iterator<T>;

		allocator_type get_allocate() const { return allocator_type(); }

	private:

		bucket_type m_Bucket;
		size_type	m_Bucket_Size;
		size_type	m_Size;
		float		m_Mlf;
		hasher		m_Hash;
		key_equal	m_Equal;

		
	private:

		bool is_equal(const key_type& lhs, const key_type& rhs)
		{
			return m_Equal(lhs, rhs);
		}

		bool is_equal(const key_type& lhs, const key_type& rhs) const
		{
			return m_Equal(lhs, rhs);
		}

		const_iterator M_cit(node_ptr node) const noexcept
		{
			// 这里要用 const_cast 是因为如果是 const 变量调用它的话
			// 那么 this 会是一个 const hashtalbe*
			return const_iterator(node, const_cast<hashtable*>(this));
		}

		iterator M_begin() noexcept
		{
			for (size_type i = 0; i < m_Bucket_Size; i++) {
				if (m_Bucket[i] != nullptr) {
					return iterator(m_Bucket[i], this);
				}
			}
			return iterator(nullptr, this);
		}

		const_iterator M_begin() const noexcept
		{
			for (size_type i = 0; i < m_Bucket_Size; i++) {
				if (m_Bucket[i] != nullptr) {
					return M_cit(m_Bucket[i]);
				}
			}
			return M_cit(nullptr);
		}

		
	public:

		explicit hashtable(size_type bucket_cnt,
			const Hash& hash = Hash(),
			const KeyEqual& equal = KeyEqual())
			: m_Size(0), m_Mlf(1.0f), m_Hash(hash), m_Equal(equal)
		{
			init(bucket_cnt);
		}

		template <class Iter, typename std::enable_if<
			mystl::is_input_iterator<Iter>::value, int>::type = 0>
		hashtable(Iter first, Iter last, size_type bucket_cnt,
			const Hash& hash = Hash(),
			const KeyEqual& equal = KeyEqual())
			: m_Size(mystl::distance(first, last)), m_Mlf(1.0f), m_Hash(hash), m_Equal(equal)
		{
			init(mystl::max(bucket_cnt, static_cast<size_type>(mystl::distance(first, last))));
		}

		hashtable(const hashtable& rhs)
		{
			copy_init(rhs);
		}

		hashtable(hashtable&& rhs)	noexcept
			: m_Bucket_Size(rhs.m_Bucket_Size)
			, m_Size(rhs.m_Size)
			, m_Mlf(rhs.m_Mlf)
			, m_Hash(rhs.m_Hash)
			, m_Equal(rhs.m_Equal)
		{
			m_Bucket = mystl::move(rhs.m_Bucket);
			rhs.m_Bucket_Size = 0;
			rhs.m_Size = 0;
			rhs.m_Mlf = 0.0f;
		}

		~hashtable()
		{
			clear();
		}

		hashtable& operator=(const hashtable& rhs)
		{
			if (this != &rhs) {
				hashtable tmp(rhs);
				swap(rhs);
			}
			return *this;
		}

		hashtable& operator=(hashtable&& rhs) noexcept
		{
			hashtable tmp(mystl::move(rhs));
			swap(rhs);
			return *this;
		}

		iterator		begin() noexcept
		{
			return M_begin();
		}

		const_iterator	begin() const noexcept
		{
			return M_begin();
		}

		iterator		end() noexcept
		{
			return iterator(nullptr, this);
		}

		const_iterator	end() const noexcept
		{
			return iterator(nullptr, this);
		}

		// const 函数只能调用 const 函数
		const_iterator	cbegin() const noexcept
		{
			return begin();
		}

		const_iterator	cend() const noexcept
		{
			return end();
		}

		bool empty() const noexcept
		{
			return m_Size == 0;
		}

		size_type size() const noexcept
		{
			return m_Size;
		}

		size_type max_size() const noexcept
		{
			return static_cast<size_type>(-1);
		}

		template <class ...Args>
		iterator emplace_multi(Args&& ...args)
		{
			node_ptr np = create_node(mystl::forward<Args>(args)...);
			try {
				// 如果元素个数 +1 大于桶个数 * 哈希因子，重新设置哈希因子减小碰撞
				if ((float)(m_Size + 1) > (float)m_Bucket_Size * max_load_factor()) {
					rehash(m_Size + 1);
				}
			}
			catch (...) {
				destroy_node(np);
				throw;
			}
			return insert_node_multi(np);
		}

		template <class ...Args>
		pair<iterator, bool> emplace_unique(Args&& ...args)
		{
			node_ptr np = create_node(mystl::forward<Args>(args)...);
			try {
				// 如果元素个数 +1 大于桶个数 * 哈希因子，重新设置哈希因子减小碰撞
				if ((float)(m_Size + 1) > (float)m_Bucket_Size * max_load_factor()) {
					rehash(m_Size + 1);
				}
			}
			catch (...) {
				destroy_node(np);
				throw;
			}
			return insert_node_unique(np);
		}

		iterator insert_multi_noresize(const value_type& value)
		{
			const size_type n = hash(value_traits::get_key(value));
			node_ptr first = m_Bucket[n];
			node_ptr np = create_node(value);
			for (node_ptr cur = first; cur != nullptr; cur = cur->next) {
				// 如果存在相等的就插入让它们在一起,不然丢到链表头
				if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(value))) {
					np->next = cur->next;
					cur->next = np;
					++m_Size;
					return iterator(np, this);
				}
			}
			np->next = m_Bucket[n];
			m_Bucket = np;
			++m_Size;
			return iterator(np, this);
		}
		
		pair<iterator, bool> insert_unique_noresize(const value_type& value)
		{
			// 计算哈希找出它的下标
			const size_type n = hash(value_traits::get_key(value));
			node_ptr first = m_Bucket[n];
			// 桶的默认排序是大的在下小的在上
			for (node_ptr cur = first; cur != nullptr; cur = cur->next) {
				// 如果有和 value 相等的就输出 false 并返回当前位置
				if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(value))) {
					return mystl::make_pair(iterator(cur, this), false);
				}
			}
			node_ptr np = create_node(value);
			np->next = first;
			m_Bucket[n] = np;
			++m_Size;
			// true 和 false 哈希表中是否已经有该元素
			return mystl::make_pair<iterator, bool>(iterator(np, this), true);
		}

		iterator insert_multi(const value_type& value)
		{
			rehash_if_need(1);
			return insert_multi_noresize(value);
		}

		iterator insert_multi(value_type&& value)
		{
			rehash_if_need(1);
			return emplace_multi(mystl::move(value));
		}

		pair<iterator, bool> insert_unique(const value_type& value)
		{
			rehash_if_need(1);
			return insert_unique_noresize(value);
		}

		pair<iterator, bool> insert_unique(value_type&& value)
		{
			rehash_if_need(1);
			return emplace_unique(mystl::move(value));
		}

		template <class Iter>
		void insert_multi(Iter first, Iter last)
		{
			copy_insert_multi(first, last, mystl::iterator_category(first));
		}

		template <class Iter>
		void insert_unique(Iter first, Iter last)
		{
			copy_insert_unique(first, last, mystl::iterator_category(first));
		}

		void erase(const_iterator pos)
		{
			node_ptr p = pos.node;
			if (p != nullptr) {
				const size_type n = hash(value_traits::get_key(p->value));
				node_ptr cur = m_Bucket[n];
				if (cur == p) {
					m_Bucket[n] = cur->next;
					destroy_node(cur);
					--m_Size;
				}
				else {
					node_ptr next = cur->next;
					while (next != nullptr) {
						if (next == p) {
							cur->next = p->next;
							destroy_node(p);
							--m_Size;
							break;
						}
						else {
							cur = next;
							next = cur->next;
						}
					}
				}
			}
		}

		void erase(const_iterator first, const_iterator last)
		{
			if (first == last) {
				return;
			}
			size_type first_bucket = (first.node == nullptr)
				? hash(value_traits::get_key(first.node->value))
				: m_Bucket_Size;
			size_type last_bucket = (first.node == nullptr)
				? hash(value_traits::get_key(last.node->value))
				: m_Bucket_Size;
			if (first_bucket == last_bucket) {
				erase_bucket(first_bucket, first.node, last.node);
			}
			else {
				erase_bucket(first_bucket, first.node, nullptr);
				for (size_type i = first_bucket + 1; i < last_bucket; ++i) {
					if (m_Bucket[i] != nullptr) {
						erase_bucket(i, nullptr);
					}
				}
				if (last_bucket != m_Bucket_Size) {
					erase_bucket(last_bucket, last.node);
				}
			}

		}

		size_type erase_multi(const key_type& key)
		{
			// 删除所有的和 key 相同的 node ,并返回删除了多少个
			auto p = equal_range_multi(key);
			if (p.first.node != nullptr) {
				erase(p.first, p.second);
				return mystl::distance(p.first, p.second);
			}
			return 0;
		}

		size_type erase_unique(const key_type& key)
		{
			const size_type n = hash(key);
			node_ptr first = m_Bucket[n];
			if (first != nullptr) {
				if (is_equal(value_traits::get_key(first->value), key)) {
					m_Bucket[n] = first->next;
					destroy_node(first);
					--m_Size;
					return 1;
				}
				else {
					node_ptr next = first->next;
					while (next != nullptr) {
						if (is_equal(value_traits::get_key(next->value), key)) {
							m_Bucket[n] = first->next;
							destroy_node(first);
							--m_Size;
							return 1;
						}
						else {
							first = next;
							next = first->next;
						}
					}
				}
			}
			return 0;
		}

		void clear()
		{
			if (m_Size != 0) {
				for (size_type i = 0; i < m_Bucket_Size; i++) {
					node_ptr np = m_Bucket[i];
					while (np != nullptr) {
						node_ptr next = np->next;
						destroy_node(np);
						np = next;
					}
					m_Bucket[i] = nullptr;
				}
				m_Size = 0;
			}
		}

		void swap(hashtable& rhs) noexcept
		{
			if (this != &rhs) {
				m_Bucket.swap(rhs.m_Bucket);
				mystl::swap(m_Size, rhs.m_Size);
				mystl::swap(m_Bucket_Size, rhs.m_Bucket_Size);
				mystl::swap(m_Mlf, rhs.m_Mlf);
				mystl::swap(m_Hash, rhs.m_Hash);
				mystl::swap(m_Equal, rhs.m_Equal);
			}
		}

		size_type count(const key_type& key) const
		{
			const size_type idx = hash(key);
			size_type result = 0;
			for (node_ptr cur = m_Bucket[idx]; cur != nullptr; cur = cur->next) {
				if (is_equal(key, value_traits::get_key(cur->value))) {
					++result;
				}
			}
			return result;
		}

		iterator find(const key_type& key)
		{
			const size_type n = hash(key);
			node_ptr first = m_Bucket[n];
			for(; first != nullptr && !is_equal(key, value_traits::get_key(first->value)); first = first->next) {}
			return iterator(first, this);
		}

		// const 函数无法调用到非 const 函数
		const_iterator find(const key_type& key) const
		{
			const size_type n = hash(key);
			node_ptr first = m_Bucket[n];
			for (; first != nullptr && !is_equal(key, value_traits::get_key(first->value)); first = first->next) {}
			return M_cit(first);
		}

		pair<iterator, iterator> equal_range_multi(const key_type& key)
		{
			const size_type n = hash(key);
			for (node_ptr first = m_Bucket[n]; first != nullptr; first = first->next) {
				if (is_equal(key, value_traits::get_key(first->value))) {
					for (node_ptr second = first->next; second != nullptr; second = second->next) {
						if (!is_equal(key, value_traits::get_key(second->value))) {
							return make_pair(iterator(first, this), iterator(second, this));
						}
					}
					for (size_type m = n + 1; m < m_Bucket_Size; m++) {
						if (m_Bucket[m] != nullptr) {
							return make_pair(iterator(first, this), iterator(m_Bucket[m], this));
						}
					}
					return make_pair(iterator(first, this), end());
				}
			}
			return make_pair(end(), end());
		}

		pair<const_iterator, const_iterator> equal_range_multi(const key_type& key) const
		{
			const size_type n = hash(key);
			for (node_ptr first = m_Bucket[n]; first != nullptr; first = first->next) {
				if (is_equal(key, value_traits::get_key(first->value))) {
					for (node_ptr second = first->next; second != nullptr; second = second->next) {
						if (!is_equal(key, value_traits::get_key(second->value))) {
							return make_pair(M_cit(first), M_cit(second));
						}
					}
					for (size_type m = n + 1; m < m_Bucket_Size; m++) {
						if (m_Bucket[m] != nullptr) {
							return make_pair(M_cit(first), M_cit(m_Bucket[m]));
						}
					}
					return make_pair(M_cit(first), cend());
				}
			}
			return make_pair(cend(), cend());
		}

		pair<iterator, iterator> equal_range_unique(const key_type& key)
		{
			const size_type n = hash(key);
			for (node_ptr first = m_Bucket[n]; first != nullptr; first = first->next) {
				if (is_equal(key, value_traits::get_key(first->value))) {
					if (first->next != nullptr) {
						return make_pair(iterator(first, this), iterator(first->next, this));
					}
					for (size_type m = n + 1; m < m_Bucket_Size; ++m) {
						if (m_Bucket[m] != nullptr) {
							return make_pair(iterator(first, this), iterator(m_Bucket[m], this));
						}
					}
					return make_pair(iterator(first, this), end());
				}
			}
			return make_pair(end(), end());
		}

		pair<const_iterator, const_iterator> equal_range_unique(const key_type& key) const
		{
			const size_type n = hash(key);
			for (node_ptr first = m_Bucket[n]; first != nullptr; first = first->next) {
				if (is_equal(key, value_traits::get_key(first->value))) {
					if (first->next != nullptr) {
						return make_pair(M_cit(first), M_cit(first->next));
					}
					for (size_type m = n + 1; m < m_Bucket_Size; ++m) {
						if (m_Bucket[m] != nullptr) {
							return make_pair(M_cit(first), M_cit(m_Bucket[m]));
						}
					}
					return make_pair(M_cit(first), cend());
				}
			}
			return make_pair(cend(), cend());
		}

		local_iterator begin(size_type n) noexcept
		{
			MYSTL_DEBUG(n < m_Size);
			return m_Bucket[n];
		}

		const_local_iterator begin(size_type n) const noexcept
		{
			MYSTL_DEBUG(n < m_Size);
			return m_Bucket[n];
		}

		const_local_iterator cbegin(size_type n) const noexcept
		{
			MYSTL_DEBUG(n < m_Size);
			return m_Bucket[n];
		}

		local_iterator end(size_type n) noexcept
		{
			MYSTL_DEBUG(n < m_Size);
			return nullptr;
		}

		const_local_iterator end(size_type n) noexcept
		{
			MYSTL_DEBUG(n < m_Size);
			return nullptr;
		}

		const_local_iterator cend(size_type n) noexcept
		{
			MYSTL_DEBUG(n < m_Size);
			return nullptr;
		}

		size_type bucket_count() const noexcept
		{
			return m_Bucket_Size;
		}

		size_type max_bucket_count() const noexcept
		{
			return ht_prime_list[PRIME_NUM - 1];
		}

		// 返回下标为 idx 下的元素数量
		size_type bucket_size(size_type idx) const noexcept
		{
			size_type result = 0;
			for (node_ptr cur = m_Bucket[idx]; cur != nullptr; cur = cur->next) {
				++result;
			}
			return result;
		}

		size_type bucket(const key_type& key) const
		{
			return hash(key);
		}

		// 碰撞几率 如果 m_Bucket_Size 小于 m_Size 则一定会碰撞
		float load_factor() const noexcept
		{
			return m_Bucket_Size != 0 ? (float)m_Size / m_Bucket_Size : 0.0f;
		}

		// 最大碰撞几率
		float max_load_factor() const noexcept
		{
			return m_Mlf;
		}

		void rehash(size_type cnt)
		{
			// rehash 可以增大桶个数也可以减小桶个数
			size_type n = ht_next_prime(cnt);
			// 如果新桶大于旧桶的个数,重新调整桶
			if (n > m_Bucket_Size) {
				replace_bucket(n);
			}
			else {
				if (((float)m_Size / (float)n < max_load_factor() - 0.25f) &&
					((float)n < (float)m_Bucket_Size * 0.75f)) {
					replace_bucket(n);
				}
			}
		}

		void reserve(size_type count)
		{
			rehash(static_cast<size_type>((float)count / max_load_factor() + 0.5f));
		}

		hasher hash_fcn() const
		{
			return m_Hash;
		}

		key_equal key_eq() const
		{
			return m_Equal;
		}

	private:

		void	init(size_type n)
		{
			const size_type bucket_cnt = next_size(n);
			try {
				m_Bucket.reserve(bucket_cnt);
				m_Bucket.assign(bucket_cnt, nullptr);
			}
			catch (...) {
				m_Bucket_Size = 0;
				m_Size = 0;
				throw;
			}
			m_Bucket_Size = m_Bucket.size();
		}

		void	copy_init(const hashtable& ht)
		{
			m_Bucket_Size = 0;
			m_Bucket.reserve(ht.m_Bucket_Size);
			m_Bucket.assign(ht.m_Bucket_Size, nullptr);
			try {
				for (size_type i = 0; i < ht.m_Bucket_Size; i++) {
					node_ptr cur = ht.m_Bucket[i];
					if (cur != nullptr) {
						node_ptr np = create_node(cur->value);
						m_Bucket[i] = np;
						for (node_ptr next = cur->next; next != nullptr; cur = next, next = cur->next) {
							np->next = create_node(next->value);
							np = np->next;
						}
						np->next = nullptr;
					}
				}
			}
			catch (...) {
				clear();
			}
		}

		template <class ...Args>
		node_ptr	create_node(Args&& ...args)
		{
			node_ptr tmp = node_allocator::allocate(1);
			try {
				data_allocator::construct(mystl::address_of(tmp->value), mystl::forward<Args>(args)...);
				tmp->next = nullptr;
			}
			catch (...) {
				node_allocator::deallocate(tmp);
				throw;
			}
			return tmp;
		}

		void	destroy_node(node_ptr np)
		{
			data_allocator::destroy(np->value);
			node_allocator::deallocate(np);
			np = nullptr;
		}

		size_type	next_size(size_type n) const
		{
			return ht_next_prime(n);
		}

		size_type	hash(const key_type& key, size_type n) const
		{
			return m_Hash(key) % n;
		}

		size_type	hash(const key_type& key) const
		{
			return m_Hash(key) % m_Bucket_Size;
		}

		void	rehash_if_need(size_type n)
		{
			// 这里的 n 表示要插入数据个数
			if (static_cast<float>(m_Size + n) > static_cast<float>(m_Bucket_Size) * max_load_factor()) {
				// 最少要 m_Size + n 个桶
				rehash(m_Size + n);
			}
		}

		template <class InputIter>
		void	copy_insert_multi(InputIter first, InputIter last, mystl::input_iterator_tag)
		{
			rehash_if_need(mystl::distance(first, last));
			for (; first != last; first++) {
				insert_multi_noresize(*first);
			}
		}

		template <class ForwardIter>
		void	copy_insert_multi(ForwardIter first, ForwardIter last, mystl::forward_iterator_tag)
		{
			size_type n = mystl::distance(first, last);
			rehash_if_need(n);
			for (; n > 0; n--, first++) {
				insert_multi_noresize(*first);
			}
		}

		template <class InputIter>
		void	copy_insert_unique(InputIter first, InputIter last, mystl::input_iterator_tag)
		{
			rehash_if_need(mystl::distance(first, last));
			for (; first != last; first++) {
				insert_unique_noresize(*first);
			}
		}

		template <class ForwardIter>
		void	copy_insert_unique(ForwardIter first, ForwardIter last, mystl::forward_iterator_tag)
		{
			size_type n = mystl::distance(first, last);
			rehash_if_need(n);
			for (; n > 0; n--, first++) {
				insert_unique_noresize(*first);
			}
		}

		pair<iterator, bool> insert_node_unique(node_ptr node)
		{
			const size_type n = hash(value_traits::get_key(node->value));
			node_ptr np = m_Bucket[n];
			if (np == nullptr) {
				m_Bucket[n] = node;
				++m_Size;
				return mystl::make_pair(iterator(node, this), true);
			}
			for (; np != nullptr; np = np->next) {
				if (is_equal(value_traits::get_key(node->value), value_traits::get_key(np->value))) {
					return mystl::make_pair(iterator(np, this), false);
				}
			}
			node->next = m_Bucket[n];
			m_Bucket[n] = node;
			++m_Size;
			return mystl::make_pair(iterator(node, this), true);
		}

		iterator	insert_node_multi(node_ptr node)
		{
			const size_type n = hash(value_traits::get_key(node->value));
			node_ptr np = m_Bucket[n];
			if (np == nullptr) {
				m_Bucket[n] = node;
				++m_Size;
				return iterator(node, this);
			}
			for (; np != nullptr; np = np->next) {
				if (is_equal(value_traits::get_key(node->value), value_traits::get_key(np->value))) {
					node->next = m_Bucket[n];
					m_Bucket[n] = node;
					++m_Size;
					return iterator(np, this);
				}
			}
			node->next = m_Bucket[n];
			m_Bucket[n] = node;
			++m_Size;
			return iterator(node, this);
		}

		void replace_bucket(size_type bucket_cnt)
		{
			bucket_type bucket(bucket_cnt, nullptr);
			if (m_Size != 0) {
				for (size_type i = 0; i < m_Bucket_Size; i++) {
					for (node_ptr first = m_Bucket[i]; first != nullptr; first = first->next) {
						node_ptr tmp = create_node(first->value);
						const size_type n = hash(value_traits::get_key(first->value), bucket_cnt);
						node_ptr f = bucket[n];
						bool is_inserted = false;
						for (node_ptr cur = f; cur != nullptr; cur = cur->next) {
							if (is_equal(value_traits::get_key(first->value), value_traits::get_key(cur->value))) {
								tmp->next = cur->next;
								cur->next = tmp;
								is_inserted = true;
								break;
							}
						}
						if (is_inserted == false) {
							tmp->next = f;
							bucket[n] = tmp;
						}
					}
				}
			}
			bucket.swap(m_Bucket);
			m_Bucket_Size = bucket.size();
		}

		void erase_bucket(size_type n, node_ptr first, node_ptr last)
		{
			node_ptr cur = m_Bucket[n];
			if (cur == first) {
				erase_bucket(n, last);
			}
			else {
				node_ptr next = cur->next;
				for (; next != first; cur = next, next = next->next) {}
				while (next != last) {
					cur->next = next->next;
					destroy_node(next);
					next = cur->next;
					--m_Size;
				}
			}
		}

		void erase_bucket(size_type n, node_ptr last)
		{
			node_ptr cur = m_Bucket[n];
			while (cur != last) {
				node_ptr next = cur->next;
				destroy_node(cur);
				cur = next;
				--m_Size;
			}
			m_Bucket[n] = last;
		}

		bool equal_to_multi(const hashtable& other)
		{
			if (m_Size != other.m_Size) {
				return false;
			}
			for (iterator left = begin(), right = end(); left != right; ) {
				auto p1 = equal_range_multi(value_traits::get_key(*left));
				auto p2 = other.equal_range_multi(value_traits::get_key(*left));
				if (mystl::distance(p1.first, p1.second) != mystl::distance(p2.first, p2.second) ||
					!mystl::is_permutation(p1.first, p1.second, p2.first, p2.second)) {
					return false;
				}
				left = p1.second;
			}
			return true;
		}

		bool equal_to_unique(const hashtable& other)
		{
			if (m_Size != other.m_Size) {
				return false;
			}
			for (iterator left = begin(), right = end(); left != right; ++left) {
				auto ptr = other.find(value_traits::get_key(*left));
				if (ptr.node == nullptr) {
					return false;
				}
			}
			return true;
		}


	};

	template <class T, class Hash, class KeyEqual>
	void swap(hashtable<T, Hash, KeyEqual>& lhs, hashtable<T, Hash, KeyEqual>& rhs) noexcept
	{
		lhs.swap(rhs);
	}


}



#endif // !MYSTL_HASHTABLE_H
