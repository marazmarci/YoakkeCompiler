#pragma once

#include <map>
#include <optional>
#include <functional>

namespace yk {
	template<
		class Key,
		class T,
		class Compare = std::less<Key>,
		class Allocator = std::allocator<std::pair<const Key, T>>
	> class opt_map {
	private:
		using map_t = std::map<Key, T, Compare, Allocator>;

	public:
		// Member types
		using key_type = typename map_t::key_type;
		using mapped_type = typename map_t::mapped_type;
		using value_type = typename map_t::value_type;
		using size_type = typename map_t::size_type;
		using difference_type = typename map_t::difference_type;
		using key_compare = typename map_t::key_compare;
		using allocator_type = typename map_t::allocator_type;
		using reference = typename map_t::reference;
		using const_reference = typename map_t::const_reference;
		using pointer = typename map_t::pointer;
		using const_pointer = typename map_t::const_pointer;
		using iterator = typename map_t::iterator;
		using const_iterator = typename map_t::const_iterator;
		using reverse_iterator = typename map_t::reverse_iterator;
		using const_reverse_iterator = typename map_t::const_reverse_iterator;

	public:
		// Member classes
		using value_compare = typename map_t::value_compare;

	private:
		map_t m_Map;

	public:
		// Constructors
		opt_map()
			: m_Map() {
		}

		explicit opt_map(const Compare& comp, const Allocator& alloc = Allocator())
			: m_Map(comp, alloc) {
		}

		explicit opt_map(const Allocator& alloc)
			: m_Map(alloc) {
		}

		template <typename InputIterator>
		opt_map(InputIterator first, InputIterator last,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: m_Map(first, last, comp, alloc) {
		}

		template <typename InputIterator>
		opt_map(InputIterator first, InputIterator last,
			const Allocator& alloc)
			: m_Map(first, last, alloc) {
		}

		opt_map(const map_t& other)
			: m_Map(other) {
		}

		opt_map(const map_t& other, const Allocator& alloc)
			: m_Map(other, alloc) {
		}

		opt_map(map_t&& other)
			: m_Map(other) {
		}

		opt_map(map_t&& other, const Allocator& alloc)
			: m_Map(other, alloc) {
		}

		opt_map(std::initializer_list<value_type> init,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: m_Map(init, comp, alloc) {
		}

		opt_map(std::initializer_list<value_type> init,
			const Allocator& alloc)
			: m_Map(init, alloc) {
		}

		// Constructors with opt_map type
		opt_map(const opt_map& other)
			: m_Map(other.m_Map) {
		}

		opt_map(const opt_map& other, const Allocator& alloc)
			: m_Map(other.m_Map, alloc) {
		}

		opt_map(opt_map&& other)
			: m_Map(std::move(other.m_Map)) {
		}

		opt_map(opt_map&& other, const Allocator& alloc)
			: m_Map(std::move(other.m_Map), alloc) {
		}

	public:
		// operator=
		opt_map& operator=(const map_t& other) {
			m_Map.operator=(other);
			return *this;
		}

		opt_map& operator=(const opt_map& other) {
			m_Map.operator=(other.m_Map);
			return *this;
		}

		opt_map& operator=(map_t&& other) {
			m_Map.operator=(other);
			return *this;
		}

		opt_map& operator=(opt_map&& other) {
			m_Map.operator=(other.m_Map);
			return *this;
		}

		opt_map& operator=(std::initializer_list<value_type> ilist) {
			m_Map.operator=(ilist);
			return *this;
		}

		// Allocator
		allocator_type get_allocator() const {
			return m_Map.get_allocator();
		}

		// Element access
		std::optional<std::reference_wrapper<T>> at(const Key& key) {
			auto it = find(key);
			if (it == end()) {
				return {};
			}
			return it->second;
		}

		std::optional<const std::reference_wrapper<T>> at(const Key& key) const {
			auto it = find(key);
			if (it == end()) {
				return {};
			}
			return it->second;
		}

		T& operator[](const Key& key) {
			return m_Map.operator[](key);
		}

		T& operator[](Key&& key) {
			return m_Map.operator[](key);
		}

		// Iterators
		iterator begin() {
			return m_Map.begin();
		}

		const_iterator begin() const {
			return m_Map.begin();
		}

		const_iterator cbegin() const {
			return m_Map.cbegin();
		}

		iterator end() {
			return m_Map.end();
		}

		const_iterator end() const {
			return m_Map.end();
		}

		const_iterator cend() const {
			return m_Map.cend();
		}

		reverse_iterator rbegin() {
			return m_Map.rbegin();
		}

		const_reverse_iterator rbegin() const {
			return m_Map.rbegin();
		}

		const_reverse_iterator crbegin() const {
			return m_Map.crbegin();
		}

		reverse_iterator rend() {
			return m_Map.rend();
		}

		const_reverse_iterator rend() const {
			return m_Map.rend();
		}

		const_reverse_iterator crend() const {
			return m_Map.crend();
		}

		// Capacity
		bool empty() const {
			return m_Map.empty();
		}

		size_type size() const {
			return m_Map.size();
		}

		size_type max_size() const {
			return m_Map.max_size();
		}

		// Modifiers
		void clear() {
			m_Map.clear();
		}

		std::pair<iterator, bool> insert(const value_type& value) {
			return m_Map.insert(value);
		}

		template <typename P>
		std::pair<iterator, bool> insert(P&& value) {
			return m_Map.insert(value);
		}

		std::pair<iterator, bool> insert(value_type&& value) {
			return m_Map.insert(value);
		}

		iterator insert(const_iterator hint, const value_type& value) {
			return m_Map.insert(hint, value);
		}

		template <typename P>
		iterator insert(const_iterator hint, P&& value) {
			return m_Map.insert(hint, value);
		}

		template <typename InputIt>
		void insert(InputIt first, InputIt last) {
			m_Map.insert(first, last);
		}

		void insert(std::initializer_list<value_type> ilist) {
			m_Map.insert(ilist);
		}

		template <typename... Args>
		std::pair<iterator, bool> emplace(Args&&... args) {
			return m_Map.insert(args...);
		}

		template <typename... Args>
		iterator emplace_hint(const_iterator hint, Args&&... args) {
			return m_Map.insert(hint, args...);
		}

		iterator erase(const_iterator pos) {
			return m_Map.erase(pos);
		}

		iterator erase(const_iterator first, const_iterator last) {
			return m_Map.erase(first, last);
		}

		size_type erase(const key_type& key) {
			return m_Map.erase(key);
		}

		void swap(map_t& other) {
			m_Map.swap(other);
		}

		void swap(opt_map& other) {
			m_Map.swap(other.m_Map);
		}

		// Lookup
		size_type count(const Key& key) const {
			return m_Map.count(key);
		}

		template< class K >
		size_type count(const K& x) const {
			return m_Map.count(x);
		}

		iterator find(const Key& key) {
			return m_Map.find(key);
		}

		const_iterator find(const Key& key) const {
			return m_Map.find(key);
		}

		template <typename K> 
		iterator find(const K& x) {
			return m_Map.find(x);
		}

		template <typename K>
		const_iterator find(const K& x) const {
			return m_Map.find(x);
		}

		std::pair<iterator, iterator> equal_range(const Key& key) {
			return m_Map.equal_range(key);
		}

		std::pair<const_iterator, const_iterator> equal_range(const Key& key) const {
			return m_Map.equal_range(key);
		}

		template <typename K>
		std::pair<iterator, iterator> equal_range(const K& x) {
			return m_Map.equal_range(x);
		}

		template <typename K>
		std::pair<const_iterator, const_iterator> equal_range(const K& x) const {
			return m_Map.equal_range(x);
		}

		iterator lower_bound(const Key& key) {
			return m_Map.lower_bound(key);
		}

		const_iterator lower_bound(const Key& key) const {
			return m_Map.lower_bound(key);
		}

		template <typename K>
		iterator lower_bound(const K& x) {
			return m_Map.lower_bound(x);
		}

		template <typename K>
		const_iterator lower_bound(const K& x) const {
			return m_Map.lower_bound(x);
		}

		iterator upper_bound(const Key& key) {
			return m_Map.upper_bound(key);
		}

		const_iterator upper_bound(const Key& key) const {
			return m_Map.upper_bound(key);
		}

		template <typename K>
		iterator upper_bound(const K& x) {
			return m_Map.upper_bound(x);
		}

		template <typename K>
		const_iterator upper_bound(const K& x) const {
			return m_Map.upper_bound(x);
		}

		// Observers
		key_compare key_comp() const {
			return m_Map.key_comp();
		}

		value_compare value_comp() const {
			return m_Map.value_comp();
		}
	};

	// opt_map with opt_map
	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator==(const opt_map<Key, T, Compare, Alloc>& lhs,
		const opt_map<Key, T, Compare, Alloc>& rhs) {
		return lhs.m_Map == rhs.m_Map;
	}
	
	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator!=(const opt_map<Key, T, Compare, Alloc>& lhs,
		const opt_map<Key, T, Compare, Alloc>& rhs) {
		return lhs.m_Map != rhs.m_Map;
	}
	
	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator<(const opt_map<Key, T, Compare, Alloc>& lhs,
		const opt_map<Key, T, Compare, Alloc>& rhs) {
		return lhs.m_Map < rhs.m_Map;
	}
	
	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator<=(const opt_map<Key, T, Compare, Alloc>& lhs,
		const opt_map<Key, T, Compare, Alloc>& rhs) {
		return lhs.m_Map <= rhs.m_Map;
	}
	
	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator>(const opt_map<Key, T, Compare, Alloc>& lhs,
		const opt_map<Key, T, Compare, Alloc>& rhs) {
		return lhs.m_Map > rhs.m_Map;
	}
	
	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator>=(const opt_map<Key, T, Compare, Alloc>& lhs,
		const opt_map<Key, T, Compare, Alloc>& rhs) {
		return lhs.m_Map >= rhs.m_Map;
	}

	// opt_map with map
	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator==(const opt_map<Key, T, Compare, Alloc>& lhs,
		const std::map<Key, T, Compare, Alloc>& rhs) {
		return lhs.m_Map == rhs;
	}

	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator!=(const opt_map<Key, T, Compare, Alloc>& lhs,
		const std::map<Key, T, Compare, Alloc>& rhs) {
		return lhs.m_Map != rhs;
	}

	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator<(const opt_map<Key, T, Compare, Alloc>& lhs,
		const std::map<Key, T, Compare, Alloc>& rhs) {
		return lhs.m_Map < rhs;
	}

	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator<=(const opt_map<Key, T, Compare, Alloc>& lhs,
		const std::map<Key, T, Compare, Alloc>& rhs) {
		return lhs.m_Map <= rhs;
	}

	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator>(const opt_map<Key, T, Compare, Alloc>& lhs,
		const std::map<Key, T, Compare, Alloc>& rhs) {
		return lhs.m_Map > rhs;
	}

	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator>=(const opt_map<Key, T, Compare, Alloc>& lhs,
		const std::map<Key, T, Compare, Alloc>& rhs) {
		return lhs.m_Map >= rhs;
	}

	// map with opt_map
	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator==(const std::map<Key, T, Compare, Alloc>& lhs,
		const opt_map<Key, T, Compare, Alloc>& rhs) {
		return lhs == rhs.m_Map;
	}

	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator!=(const std::map<Key, T, Compare, Alloc>& lhs,
		const opt_map<Key, T, Compare, Alloc>& rhs) {
		return lhs != rhs.m_Map;
	}

	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator<(const std::map<Key, T, Compare, Alloc>& lhs,
		const opt_map<Key, T, Compare, Alloc>& rhs) {
		return lhs < rhs.m_Map;
	}

	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator<=(const std::map<Key, T, Compare, Alloc>& lhs,
		const opt_map<Key, T, Compare, Alloc>& rhs) {
		return lhs <= rhs.m_Map;
	}

	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator>(const std::map<Key, T, Compare, Alloc>& lhs,
		const opt_map<Key, T, Compare, Alloc>& rhs) {
		return lhs > rhs.m_Map;
	}

	template <typename Key, typename T, typename Compare, typename Alloc>
	bool operator>=(const std::map<Key, T, Compare, Alloc>& lhs,
		const opt_map<Key, T, Compare, Alloc>& rhs) {
		return lhs >= rhs.m_Map;
	}

	// Swap
	template< class Key, class T, class Compare, class Alloc >
	void swap(opt_map<Key, T, Compare, Alloc>& lhs,
		opt_map<Key, T, Compare, Alloc>& rhs) {
		std::swap(lhs.m_Map, rhs.m_Map);
	}

	template< class Key, class T, class Compare, class Alloc >
	void swap(std::map<Key, T, Compare, Alloc>& lhs,
		opt_map<Key, T, Compare, Alloc>& rhs) {
		std::swap(lhs, rhs.m_Map);
	}

	template< class Key, class T, class Compare, class Alloc >
	void swap(opt_map<Key, T, Compare, Alloc>& lhs,
		std::map<Key, T, Compare, Alloc>& rhs) {
		std::swap(lhs.m_Map, rhs);
	}
}