#ifndef KTR_HASHMAP_F
#define KTR_HASHMAP_F

#include "Tools/KTR_Hasher.h"
#include "KTR_Sparse.h"
#include "KTR_Assert.h"
#include <vector>
#include <stdexcept>
namespace KTR
{
	template<HashType KeyT, typename ValT>
	class HashMap
	{
	public:
		using key_type = KeyT;
		using value_type = ValT;
		using hidden_key_type = std::uint64_t;
		using hasher_type = Hash<key_type>;
		using vector_type = std::vector<value_type>;
		using iterator_type = vector_type::iterator;
		using const_iterator_type = vector_type::const_iterator;
		using sparse_type = Sparse<hidden_key_type>;
	public:
		HashMap() = default;
		HashMap(const HashMap&) = default;
		HashMap(HashMap&&) = default;
		~HashMap() = default;
		HashMap& operator=(const HashMap&) = default;
		HashMap& operator=(HashMap&&) = default;
	public:
		[[nodiscard]] bool Has(const key_type& key) const;

		void Add(const key_type& key, const value_type& value);

		void Add(const key_type& key, value_type&& value);

		void AddOrSet(const key_type& key, const value_type& value);

		void AddOrSet(const key_type& key,value_type&& value);

		void Remove(const key_type& key);

		[[nodiscard]] value_type& At(const key_type& key);

		[[nodiscard]] value_type& operator[](const key_type& key);

		[[nodiscard]] const value_type& At(const key_type& key) const;

		[[nodiscard]] const value_type& operator[](const key_type& key) const;

		[[nodiscard]] iterator_type begin();

		[[nodiscard]] iterator_type end();

		[[nodiscard]] const_iterator_type begin() const;

		[[nodiscard]] const_iterator_type end() const;

		[[nodiscard]] size_t Size() const;

		[[nodiscard]] bool Empty() const;

		void Clear();

	private:
		sparse_type m_sparse;
		vector_type m_dense;
	};

	template <HashType KeyT, typename ValT>
	bool HashMap<KeyT, ValT>::Has(const key_type& key) const
	{
		return m_sparse.Has(hasher_type::Compute(key));
	}

	template <HashType KeyT, typename ValT>
	void HashMap<KeyT, ValT>::Add(const key_type& key, const value_type& value)
	{
		hidden_key_type e = hasher_type::Compute(key);
		if (m_sparse.Has(e))
			throw SpeExcept<std::out_of_range>("already store");
		m_sparse.Add(e);
		m_dense.push_back(value);
	}

	template <HashType KeyT, typename ValT>
	void HashMap<KeyT, ValT>::Add(const key_type& key, value_type&& value )
	{
		hidden_key_type e = hasher_type::Compute(key);
		if (m_sparse.Has(e))
			throw SpeExcept<std::out_of_range>("already store");
		m_sparse.Add(e);
		m_dense.push_back(std::move(value));
	}

	template <HashType KeyT, typename ValT>
	void HashMap<KeyT, ValT>::AddOrSet(const key_type& key, const value_type& value)
	{
		hidden_key_type e = hasher_type::Compute(key);
		if (!m_sparse.Has(e))
		{
			m_sparse.Add(e);
			m_dense.push_back(value);
		}
		else
		{
			m_dense[m_sparse.At<Storage::SPARSE>(e)] = value;
		}
	}

	template <HashType KeyT, typename ValT>
	void HashMap<KeyT, ValT>::AddOrSet(const key_type& key, value_type&& value)
	{
		hidden_key_type e = hasher_type::Compute(key);
		if (!m_sparse.Has(e))
		{
			m_sparse.Add(e);
			m_dense.push_back(std::move(value));
		}
		else
		{
			m_dense[m_sparse.At<Storage::SPARSE>(e)] = std::move(value);
		}
	}

	template <HashType KeyT, typename ValT>
	void HashMap<KeyT, ValT>::Remove(const key_type& key)
	{
		hidden_key_type e = hasher_type::Compute(key);
		if (!m_sparse.Has(e))
			throw SpeExcept<std::out_of_range>("not store");

		hidden_key_type index = m_sparse.At<Storage::SPARSE>(e);
		m_sparse.Remove(e);
		m_dense[index] = std::move(m_dense.back());
		m_dense.pop_back();
	}

	template <HashType KeyT, typename ValT>
	typename HashMap<KeyT, ValT>::value_type& HashMap<KeyT, ValT>::At(const key_type& key)
	{
		hidden_key_type e = hasher_type::Compute(key);
		return m_dense.at(m_sparse.At<Storage::SPARSE>(e));
	}

	template <HashType KeyT, typename ValT>
	typename HashMap<KeyT, ValT>::value_type& HashMap<KeyT, ValT>::operator[](const key_type& key)
	{
		hidden_key_type e = hasher_type::Compute(key);
		return m_dense[m_sparse.At<Storage::SPARSE>(e)];
	}

	template <HashType KeyT, typename ValT>
	const typename HashMap<KeyT, ValT>::value_type& HashMap<KeyT, ValT>::At(const key_type& key) const
	{
		hidden_key_type e = hasher_type::Compute(key);
		return m_dense.at(m_sparse.At<Storage::SPARSE>(e));
	}

	template <HashType KeyT, typename ValT>
	const typename HashMap<KeyT, ValT>::value_type& HashMap<KeyT, ValT>::operator[](const key_type& key) const
	{
		hidden_key_type e = hasher_type::Compute(key);
		return m_dense[m_sparse.At<Storage::SPARSE>(e)];
	}

	template <HashType KeyT, typename ValT>
	typename HashMap<KeyT, ValT>::iterator_type HashMap<KeyT, ValT>::begin()
	{
		return m_dense.begin();
	}

	template <HashType KeyT, typename ValT>
	typename HashMap<KeyT, ValT>::iterator_type HashMap<KeyT, ValT>::end()
	{
		return m_dense.end();
	}

	template <HashType KeyT, typename ValT>
	typename HashMap<KeyT, ValT>::const_iterator_type HashMap<KeyT, ValT>::begin() const
	{
		return m_dense.begin();
	}

	template <HashType KeyT, typename ValT>
	typename HashMap<KeyT, ValT>::const_iterator_type HashMap<KeyT, ValT>::end() const
	{
		return m_dense.end();
	}

	template <HashType KeyT, typename ValT>
	size_t HashMap<KeyT, ValT>::Size() const
	{
		return m_dense.size();
	}

	template <HashType KeyT, typename ValT>
	bool HashMap<KeyT, ValT>::Empty() const
	{
		return m_dense.empty();
	}

	template <HashType KeyT, typename ValT>
	void HashMap<KeyT, ValT>::Clear()
	{
		m_sparse.Clear();
		m_dense.clear();
	}
}

#endif
