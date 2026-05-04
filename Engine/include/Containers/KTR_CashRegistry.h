#ifndef KTR_CASHREGISTRY_F
#define KTR_CASHREGISTRY_F

#include "Tools/Hash/KTR_Hasher.h"
#include "HashMap/KTR_HashMap.h"
#include "KTR_assert.h"
#include <memory>

// TODO FIX BUG

namespace KTR
{


	template<ValidHashOpp KeyT, typename ValT>
	class CashRegistry
	{
	public:
		using key_type = KeyT;
		using value_type = ValT;
		using ptr_type = std::unique_ptr<value_type>;
		using map_type = HashMap<key_type, ptr_type>;
		using pair_type = map_type::pair_type;
		using raw_ptr_type = value_type*;
		using raw_ref_type = value_type&;
		using const_iterator_type = map_type::const_iterator_type;
	public:
		CashRegistry() = default;
		CashRegistry(const CashRegistry&) = default;
		CashRegistry(CashRegistry&&) noexcept = default;
		~CashRegistry() = default;
		CashRegistry& operator=(const CashRegistry&) = default;
		CashRegistry& operator=(CashRegistry&&) noexcept = default;
	public: 
		void Reg(const key_type& key,ptr_type ptr);
		void Set(const key_type& key, ptr_type ptr);
		void UnReg(const key_type& key);
		void RegOrSet(const key_type& key, ptr_type ptr);

		[[nodiscard]] bool Has(const key_type& key) const;
		size_t Size() const;
		
		void Clear();

		[[nodiscard]] raw_ptr_type GetPtr(const key_type& key) const;
		[[nodiscard]] raw_ref_type GetRef(const key_type& key) const;

		[[nodiscard]] const_iterator_type begin() const;
		[[nodiscard]] const_iterator_type end() const;
		[[nodiscard]] const_iterator_type CBegin() const;
		[[nodiscard]] const_iterator_type CEnd() const;
	private:
		map_type m_map;
	};

	template <ValidHashOpp KeyT, typename ValT>
	void CashRegistry<KeyT, ValT>::Reg(const key_type& key, ptr_type ptr)
	{
		if (this->Has(key))
			throw SpeExcept<std::out_of_range>("already store");
		m_map.Add(key,std::move(ptr));
	}

	template <ValidHashOpp KeyT, typename ValT>
	void CashRegistry<KeyT, ValT>::Set(const key_type& key, ptr_type ptr)
	{
		if (!this->Has(key))
			throw SpeExcept<std::out_of_range>("not store");
		m_map[key] = std::move(ptr);
	}

	template <ValidHashOpp KeyT, typename ValT>
	void CashRegistry<KeyT, ValT>::RegOrSet(const key_type& key, ptr_type ptr)
	{
		if (this->Has(key))
			m_map[key] = std::move(ptr);
		else
			m_map.Add(key, std::move(ptr));
	}

	template <ValidHashOpp KeyT, typename ValT>
	bool CashRegistry<KeyT, ValT>::Has(const key_type& key) const
	{
		return m_map.Has(key);
	}

	template <ValidHashOpp KeyT, typename ValT>
	size_t CashRegistry<KeyT, ValT>::Size() const
	{
		return m_map.Size();
	}

	template <ValidHashOpp KeyT, typename ValT>
	void CashRegistry<KeyT, ValT>::Clear()
	{
		m_map.Clear();
	}

	template <ValidHashOpp KeyT, typename ValT>
	typename CashRegistry<KeyT, ValT>::raw_ptr_type CashRegistry<KeyT, ValT>::GetPtr(const key_type& key) const
	{
		if (this->Has(key))
			return m_map[key].get();
		return nullptr;
	}

	template <ValidHashOpp KeyT, typename ValT>
	typename CashRegistry<KeyT, ValT>::raw_ref_type CashRegistry<KeyT, ValT>::GetRef(const key_type& key) const
	{
		if (this->Has(key))
			return *m_map[key];
		throw SpeExcept<std::out_of_range>("not store");
	}

	template <ValidHashOpp KeyT, typename ValT>
	void CashRegistry<KeyT, ValT>::UnReg(const key_type& key)
	{
		if (!this->Has(key))
			throw SpeExcept<std::out_of_range>("not store");

		m_map.Remove(key);
	}

	template <ValidHashOpp KeyT, typename ValT>
	typename CashRegistry<KeyT, ValT>::const_iterator_type CashRegistry<KeyT, ValT>::begin() const
	{
		return m_map.begin();
	}

	template <ValidHashOpp KeyT, typename ValT>
	typename CashRegistry<KeyT, ValT>::const_iterator_type CashRegistry<KeyT, ValT>::end() const
	{
		return m_map.end();
	}

	template <ValidHashOpp KeyT, typename ValT>
	typename CashRegistry<KeyT, ValT>::const_iterator_type CashRegistry<KeyT, ValT>::CBegin() const
	{
		return m_map.CBegin();
	}

	template <ValidHashOpp KeyT, typename ValT>
	typename CashRegistry<KeyT, ValT>::const_iterator_type CashRegistry<KeyT, ValT>::CEnd() const
	{
		return m_map.CEnd();
	}
}

#endif
