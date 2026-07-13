#ifndef KTR_HASHSET_F
#define KTR_HASHSET_F

#include "HashMap/KTR_HashMap.h"

namespace KTR
{
	template<typename KeyT, bool IsConst>
	class SetIterator
	{
	public:
		
		using iterator_category = std::forward_iterator_tag;
		using value_type = KeyT;
		using difference_type = std::ptrdiff_t;
		using reference = std::conditional_t<IsConst, const value_type&, value_type&>;
		using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;
		using internalIt_type = MapIterator<KeyT, std::monostate, IsConst>;
	public:
		SetIterator(const SetIterator&) = default;
		SetIterator(SetIterator&&)noexcept = default;
		~SetIterator() = default;
		SetIterator& operator=(const SetIterator&) = default;
		SetIterator& operator=(SetIterator&&)noexcept = default;
		SetIterator(const internalIt_type& it);
		SetIterator(internalIt_type&& it) noexcept;

		SetIterator& operator=(const internalIt_type& other);

		SetIterator& operator=(internalIt_type&& other)noexcept;

	public:
		[[nodiscard]] reference operator*() const;

		[[nodiscard]] pointer   operator->() const;

		SetIterator& operator++();

		[[nodiscard]] SetIterator  operator++(int);

		[[nodiscard]] bool operator==(const SetIterator& other) const;

		[[nodiscard]] bool operator!=(const SetIterator& other) const;

	private:
		internalIt_type m_current;
	};

	template <typename KeyT, bool IsConst>
	SetIterator<KeyT, IsConst>::SetIterator(const internalIt_type& it): m_current(it)
	{}

	template <typename KeyT, bool IsConst>
	SetIterator<KeyT, IsConst>::SetIterator(internalIt_type&& it) noexcept: m_current(std::move(it))
	{}

	template <typename KeyT, bool IsConst>
	SetIterator<KeyT, IsConst>& SetIterator<KeyT, IsConst>::operator=(const internalIt_type& other)
	{
		m_current = other;
		return *this;
	}

	template <typename KeyT, bool IsConst>
	SetIterator<KeyT, IsConst>& SetIterator<KeyT, IsConst>::operator=(internalIt_type&& other) noexcept
	{
		m_current = std::move(other);
		return *this;
	}

	template <typename KeyT, bool IsConst>
	typename SetIterator<KeyT, IsConst>::reference SetIterator<KeyT, IsConst>::operator*() const
	{
		return m_current->first;
	}

	template <typename KeyT, bool IsConst>
	typename SetIterator<KeyT, IsConst>::pointer SetIterator<KeyT, IsConst>::operator->() const
	{
		return &m_current->first;
	}

	template <typename KeyT, bool IsConst>
	SetIterator<KeyT, IsConst>& SetIterator<KeyT, IsConst>::operator++()
	{
		++m_current;
		return *this;
	}

	template <typename KeyT, bool IsConst>
	SetIterator<KeyT, IsConst> SetIterator<KeyT, IsConst>::operator++(int)
	{
		SetIterator tmp = *this;
		++m_current;
		return tmp;
	}

	template <typename KeyT, bool IsConst>
	bool SetIterator<KeyT, IsConst>::operator==(const SetIterator& other) const
	{
		return m_current == other.m_current;
	}

	template <typename KeyT, bool IsConst>
	bool SetIterator<KeyT, IsConst>::operator!=(const SetIterator& other) const
	{
		return !(*this == other);
	}

	template<typename KeyT, template<typename >class Hash = HASH::default_hash_type> requires(ValidHashOpp<KeyT, Hash>)
		class HashSet
	{
	public:
		using key_type = KeyT;
		using hash_type = Hash<key_type>;
		using hidden_key_type = typename hash_type::return_hash_type;
		using iterator_type = SetIterator<key_type, false>;
		using const_iterator_type = SetIterator<key_type, true>;
		using container_type = HashMap<key_type, std::monostate, Hash>;
	public:
		HashSet() = default;
		~HashSet() = default;
		HashSet(const HashSet&) = default;
		HashSet(HashSet&&) noexcept = default;
		HashSet& operator=(const HashSet&) = default;
		HashSet& operator=(HashSet&&) noexcept =default;
	public:
		[[nodiscard]] bool Has(const key_type& key) const;
		// if already has return false
		bool Add(const key_type& key);

		void Remove(const key_type& key);

		[[nodiscard]] size_t Size() const;

		[[nodiscard]] bool Empty() const;

		void Clear();

		[[nodiscard]] const_iterator_type begin() const;

		[[nodiscard]] const_iterator_type end() const;

		[[nodiscard]] iterator_type begin();

		[[nodiscard]] iterator_type end();

		[[nodiscard]] const_iterator_type CBegin() const;

		[[nodiscard]] const_iterator_type CEnd() const;

	private:
		container_type m_map;
	};

	template <typename KeyT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
	bool HashSet<KeyT, Hash>::Has(const key_type& key) const
	{
		return m_map.Has(key);
	}

	template <typename KeyT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
	bool HashSet<KeyT, Hash>::Add(const key_type& key)
	{
		if (this->Has(key))
			return false;

		m_map.Add(key, std::monostate{});
		return true;
	}

	template <typename KeyT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
	void HashSet<KeyT, Hash>::Remove(const key_type& key)
	{
		if (!this->Has(key))
			return;
		m_map.Remove(key);
	}

	template <typename KeyT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
	size_t HashSet<KeyT, Hash>::Size() const
	{
		return m_map.Size();
	}

	template <typename KeyT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
	bool HashSet<KeyT, Hash>::Empty() const
	{
		return m_map.Empty();
	}

	template <typename KeyT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
	void HashSet<KeyT, Hash>::Clear()
	{
		m_map.Clear();
	}

	template <typename KeyT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
	typename HashSet<KeyT, Hash>::const_iterator_type HashSet<KeyT, Hash>::begin() const
	{
		return m_map.begin();
	}

	template <typename KeyT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
	typename HashSet<KeyT, Hash>::const_iterator_type HashSet<KeyT, Hash>::end() const
	{
		return m_map.end();
	}

	template <typename KeyT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
	typename HashSet<KeyT, Hash>::iterator_type HashSet<KeyT, Hash>::begin()
	{
		return m_map.begin();
	}

	template <typename KeyT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
	typename HashSet<KeyT, Hash>::iterator_type HashSet<KeyT, Hash>::end()
	{
		return m_map.end();
	}

	template <typename KeyT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
	typename HashSet<KeyT, Hash>::const_iterator_type HashSet<KeyT, Hash>::CBegin() const
	{
		return m_map.CBegin();
	}

	template <typename KeyT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
	typename HashSet<KeyT, Hash>::const_iterator_type HashSet<KeyT, Hash>::CEnd() const
	{
		return m_map.Cend();
	}
}
#endif