#ifndef MYSTL_UNORDERED_MAP_H
#define MYSTL_UNORDERED_MAP_H


#include "hashtable.h"


namespace mystl {

	template <class Key, class T, class Hash = mystl::hash<Key>, class KeyEqual = mystl::equal_to<Key>>
	class unordered_map
	{
	private:

		using base_type = hashtable<pair<const Key, T>, Hash, KeyEqual>;
		base_type ht;

	public:

		using allocator_type = typename base_type::allocator_type;
		using key_type = typename base_type::key_type;
		using mapped_type = typename base_type::mapped_type;
		using value_type = typename base_type::value_type;
		using hasher = typename base_type::hasher;
		using key_equal = typename base_type::key_equal;

		using pointer = typename base_type::pointer;
		using const_pointer = typename base_type::const_pointer;
		using reference = typename base_type::reference;
		using const_reference = typename base_type::const_reference;
		using size_type = typename base_type::size_type;
		using difference_type = typename base_type::difference_type;

		using iterator = typename base_type::iterator;
		using const_iterator = typename base_type::const_iterator;
		using local_iterator = typename base_type::local_iterator;
		using const_local_iterator = typename base_type::const_local_iterator;

		allocator_type get_allocator() const { return ht.get_allocate(); }

	public:

		unordered_map() : ht(100, Hash(), KeyEqual()) {}

		explicit unordered_map(size_type n, const Hash& hash = Hash(), const KeyEqual& key = KeyEqual())
			: ht(n, hash, key)
		{

		}

		template <class InputIter>
		unordered_map(InputIter first, InputIter last,
			const size_type n = 100,
			const Hash& hash = Hash(),
			const KeyEqual& key = KeyEqual())
			: ht(mystl::max(n, mystl::distance(first, last)), hash, key)
		{
			for (auto iter = first; iter != last; iter++) {
				ht.insert_unique_noresize(*iter);
			}
		}


		unordered_map(std::initializer_list<value_type> ilist,
			const size_type n = 100,
			const Hash& hash = Hash(),
			const KeyEqual& key = KeyEqual())
			: ht(mystl::max(n, static_cast<size_type>(ilist.size())), hash, key)
		{
			for (auto iter = ilist.begin(); iter != ilist.end(); iter++) {
				ht.insert_unique_noresize(*iter);
			}
		}


		unordered_map(const unordered_map& other) : ht(other.ht)
		{

		}

		unordered_map(unordered_map&& other) : ht(mystl::move(other.ht))
		{

		}


		unordered_map& operator=(const unordered_map& other)
		{
			ht = other.ht;
			return *this;
		}

		unordered_map& operator=(unordered_map&& other)
		{
			ht = mystl::move(other.ht);
			return *this;
		}

		unordered_map& operator=(std::initializer_list<value_type> ilist)
		{
			ht.clear();
			ht.reserve(ilist.size());
			for (auto iter = ilist.begin(); iter != ilist.end(); iter++) {
				ht.insert_unique_noresize(*iter);
			}
			return *this;
		}

		~unordered_map() = default;

		iterator begin() noexcept
		{
			return ht.begin();
		}

		const_iterator begin() const noexcept
		{
			return ht.cbegin();
		}

		iterator end() noexcept
		{
			return ht.end();
		}

		const_iterator end() const noexcept
		{
			return ht.cend();
		}

		const_iterator cbegin() const noexcept
		{
			return ht.cbegin();
		}

		const_iterator cend() const noexcept
		{
			return ht.cend();
		}

		bool empty() const noexcept
		{
			return ht.empty();
		}

		size_type size() const noexcept
		{
			return ht.size();
		}

		size_type max_size() const noexcept
		{
			return ht.max_size();
		}

		template <class ...Args>
		pair<iterator, bool> emplace(Args&& ...args)
		{
			return ht.emplace_unique(mystl::forward<Args>(args)...);
		}

		pair<iterator, bool> insert(const value_type& value)
		{
			return ht.insert_unique(value);
		}

		pair<iterator, bool> insert(value_type&& value)
		{
			return ht.emplace_unique(mystl::move(value));
		}

		template <class InputIter>
		void insert(InputIter first, InputIter last)
		{
			ht.insert_unique(first, last);
		}

		void swap(unordered_set& rhs)
		{
			ht.swap(rhs.ht);
		}

		mapped_type& at(const key_type& key)
		{
			iterator it = ht.find(key);
			THROW_LENGTH_ERROR_IF(it.node == nullptr, "");
			return it->second;
		}

		const mapped_type& at(const key_type& key) const
		{
			iterator it = ht.find(key);
			THROW_LENGTH_ERROR_IF(it.node == nullptr, "");
			return it->second;
		}

		mapped_type& operator[](const key_type& key)
		{
			iterator it = ht.find(key);
			if (it.node == nullptr) {
				it = ht.emplace_unique(key, T{}).first;
			}
			return it->second;
		}

		const mapped_type& operator[](const key_type& key) const
		{
			iterator it = ht.find(key);
			if (it.node == nullptr) {
				it = ht.emplace_unique(key, T{}).first;
			}
			return it->second;
		}

		void erase(iterator it)
		{
			ht.erase(it);
		}

		void erase(iterator first, iterator last)
		{
			ht.erase(first, last);
		}

		size_type erase(const key_type& key)
		{
			return ht.erase_unique(key);
		}

		void clear()
		{
			ht.clear();
		}

		size_type count(const key_type& key) const
		{
			return ht.count(key);
		}

		iterator find(const key_type& key)
		{
			return ht.find(key);
		}

		const_iterator find(const key_type& key) const
		{
			return ht.find(key);
		}

		pair<iterator, iterator> equal_range(const key_type& key)
		{
			return ht.equal_range_unique(key);
		}

		pair<const_iterator, const_iterator> equal_range(const key_type& key) const
		{
			return ht.equal_range_unique(key);
		}

		local_iterator begin(size_type n) noexcept
		{
			return ht.begin(n);
		}

		const_local_iterator begin(size_type n) const noexcept
		{
			return ht.begin(n);
		}

		const_local_iterator cbegin(size_type n) const noexcept
		{
			return ht.cbegin(n);
		}

		local_iterator end(size_type n) noexcept
		{
			return ht.end(n);
		}

		const_local_iterator end(size_type n) const noexcept
		{
			return ht.end(n);
		}

		const_local_iterator cend(size_type n) const noexcept
		{
			return ht.cend(n);
		}

		size_type bucket_count() const noexcept
		{
			return ht.bucket_count();
		}

		size_type max_bucket_count() const noexcept
		{
			return ht.max_bucket_count();
		}

		size_type bucket_size() const noexcept
		{
			return ht.bucket_size();
		}

		size_type bucket(const key_type& key) const
		{
			return hash(key);
		}

		float load_factor() const noexcept
		{
			return ht.load_factor();
		}

		float max_load_factor() const noexcept
		{
			return ht.max_load_factor();
		}

		void reserve(size_type cnt)
		{
			ht.reserve(cnt);
		}

		void rehash(size_type cnt)
		{
			ht.rehash(cnt);
		}

		key_equal key_eq() const
		{
			return ht.key_eq();
		}

		hasher hash_fcn() const
		{
			return ht.hash_fcn();
		}

	public:


		friend bool operator==(const unordered_map& lhs, const unordered_map& rhs)
		{
			return lhs.ht.equal_to_unique(rhs.ht);
		}

		friend bool operator!=(const unordered_map& lhs, const unordered_map& rhs)
		{
			return !(lhs == rhs);
		}


	};


	template <class Key, class T, class Hash = mystl::hash<Key>, class KeyEqual = mystl::equal_to<Key>>
	class unordered_multimap
	{
	private:

		using base_type = hashtable<pair<const Key, T>, Hash, KeyEqual>;
		base_type ht;

	public:

		using allocator_type = typename base_type::allocator_type;
		using key_type = typename base_type::key_type;
		using mapped_type = typename base_type::mapped_type;
		using value_type = typename base_type::value_type;
		using hasher = typename base_type::hasher;
		using key_equal = typename base_type::key_equal;

		using pointer = typename base_type::pointer;
		using const_pointer = typename base_type::const_pointer;
		using reference = typename base_type::reference;
		using const_reference = typename base_type::const_reference;
		using size_type = typename base_type::size_type;
		using difference_type = typename base_type::difference_type;

		using iterator = typename base_type::iterator;
		using const_iterator = typename base_type::const_iterator;
		using local_iterator = typename base_type::local_iterator;
		using const_local_iterator = typename base_type::const_local_iterator;

		allocator_type get_allocator() const { return ht.get_allocate(); }

	public:

		unordered_multimap() : ht(100, Hash(), KeyEqual()) {}

		explicit unordered_multimap(size_type n, const Hash& hash = Hash(), const KeyEqual& key = KeyEqual())
			: ht(n, hash, key)
		{

		}

		template <class InputIter>
		unordered_multimap(InputIter first, InputIter last,
			const size_type n = 100,
			const Hash& hash = Hash(),
			const KeyEqual& key = KeyEqual())
			: ht(mystl::max(n, mystl::distance(first, last)), hash, key)
		{
			for (auto iter = first; iter != last; iter++) {
				ht.insert_multi_noresize(*iter);
			}
		}


		unordered_multimap(std::initializer_list<value_type> ilist,
			const size_type n = 100,
			const Hash& hash = Hash(),
			const KeyEqual& key = KeyEqual())
			: ht(mystl::max(n, static_cast<size_type>(ilist.size())), hash, key)
		{
			for (auto iter = ilist.begin(); iter != ilist.end(); iter++) {
				ht.insert_multi_noresize(*iter);
			}
		}


		unordered_multimap(const unordered_multimap& other) : ht(other.ht)
		{

		}

		unordered_multimap(unordered_multimap&& other) : ht(mystl::move(other.ht))
		{

		}


		unordered_multimap& operator=(const unordered_multimap& other)
		{
			ht = other.ht;
			return *this;
		}

		unordered_multimap& operator=(unordered_multimap&& other)
		{
			ht = mystl::move(other.ht);
			return *this;
		}

		unordered_multimap& operator=(std::initializer_list<value_type> ilist)
		{
			ht.clear();
			ht.reserve(ilist.size());
			for (auto iter = ilist.begin(); iter != ilist.end(); iter++) {
				ht.insert_multi_noresize(*iter);
			}
			return *this;
		}

		~unordered_multimap() = default;

		iterator begin() noexcept
		{
			return ht.begin();
		}

		const_iterator begin() const noexcept
		{
			return ht.cbegin();
		}

		iterator end() noexcept
		{
			return ht.end();
		}

		const_iterator end() const noexcept
		{
			return ht.cend();
		}

		const_iterator cbegin() const noexcept
		{
			return ht.cbegin();
		}

		const_iterator cend() const noexcept
		{
			return ht.cend();
		}

		bool empty() const noexcept
		{
			return ht.empty();
		}

		size_type size() const noexcept
		{
			return ht.size();
		}

		size_type max_size() const noexcept
		{
			return ht.max_size();
		}

		template <class ...Args>
		pair<iterator, bool> emplace(Args&& ...args)
		{
			return ht.emplace_multi(mystl::forward<Args>(args)...);
		}

		pair<iterator, bool> insert(const value_type& value)
		{
			return ht.insert_multi(value);
		}

		pair<iterator, bool> insert(value_type&& value)
		{
			return ht.emplace_multi(mystl::move(value));
		}

		template <class InputIter>
		void insert(InputIter first, InputIter last)
		{
			ht.insert_multi(first, last);
		}

		void swap(unordered_set& rhs)
		{
			ht.swap(rhs.ht);
		}

		mapped_type& at(const key_type& key)
		{
			iterator it = ht.find(key);
			THROW_LENGTH_ERROR_IF(it.node == nullptr, "");
			return it->second;
		}

		const mapped_type& at(const key_type& key) const
		{
			iterator it = ht.find(key);
			THROW_LENGTH_ERROR_IF(it.node == nullptr, "");
			return it->second;
		}

		mapped_type& operator[](const key_type& key)
		{
			iterator it = ht.find(key);
			if (it.node == nullptr) {
				it = ht.emplace_multi(key, T{}).first;
			}
			return it->second;
		}

		const mapped_type& operator[](const key_type& key) const
		{
			iterator it = ht.find(key);
			if (it.node == nullptr) {
				it = ht.emplace_multi(key, T{}).first;
			}
			return it->second;
		}

		void erase(iterator it)
		{
			ht.erase(it);
		}

		void erase(iterator first, iterator last)
		{
			ht.erase(first, last);
		}

		size_type erase(const key_type& key)
		{
			return ht.erase_multi(key);
		}

		void clear()
		{
			ht.clear();
		}

		size_type count(const key_type& key) const
		{
			return ht.count(key);
		}

		iterator find(const key_type& key)
		{
			return ht.find(key);
		}

		const_iterator find(const key_type& key) const
		{
			return ht.find(key);
		}

		pair<iterator, iterator> equal_range(const key_type& key)
		{
			return ht.equal_range_multi(key);
		}

		pair<const_iterator, const_iterator> equal_range(const key_type& key) const
		{
			return ht.equal_range_multi(key);
		}

		local_iterator begin(size_type n) noexcept
		{
			return ht.begin(n);
		}

		const_local_iterator begin(size_type n) const noexcept
		{
			return ht.begin(n);
		}

		const_local_iterator cbegin(size_type n) const noexcept
		{
			return ht.cbegin(n);
		}

		local_iterator end(size_type n) noexcept
		{
			return ht.end(n);
		}

		const_local_iterator end(size_type n) const noexcept
		{
			return ht.end(n);
		}

		const_local_iterator cend(size_type n) const noexcept
		{
			return ht.cend(n);
		}

		size_type bucket_count() const noexcept
		{
			return ht.bucket_count();
		}

		size_type max_bucket_count() const noexcept
		{
			return ht.max_bucket_count();
		}

		size_type bucket_size() const noexcept
		{
			return ht.bucket_size();
		}

		size_type bucket(const key_type& key) const
		{
			return hash(key);
		}

		float load_factor() const noexcept
		{
			return ht.load_factor();
		}

		float max_load_factor() const noexcept
		{
			return ht.max_load_factor();
		}

		void reserve(size_type cnt)
		{
			ht.reserve(cnt);
		}

		void rehash(size_type cnt)
		{
			ht.rehash(cnt);
		}

		key_equal key_eq() const
		{
			return ht.key_eq();
		}

		hasher hash_fcn() const
		{
			return ht.hash_fcn();
		}

	public:


		friend bool operator==(const unordered_multimap& lhs, const unordered_multimap& rhs)
		{
			return lhs.ht.equal_to_unique(rhs.ht);
		}

		friend bool operator!=(const unordered_multimap& lhs, const unordered_multimap& rhs)
		{
			return !(lhs == rhs);
		}


	};



}



#endif // !MYSTL_UNORDERED_MAP_H

