#ifndef KTR_HASHMAP_F
#define KTR_HASHMAP_F

#include "Tools/Hash/KTR_Hasher.h"
#include "Tools/KTR_SIMD.h"
#include "KTR_HashMapIt.h"
#include <utility>
#include <vector>

#include <bit>

#include "KTR_Assert.h"


namespace KTR
{
	template<typename KeyT, typename ValT, template<typename >class Hash = HASH::default_hash_type> requires(ValidHashOpp<KeyT,Hash>)
		class HashMap
	{
	public:
		using key_type = KeyT;
		using value_type = ValT;
		using data_type = std::pair<key_type, value_type>;
		using data_container_type = std::vector<data_type>;
		using meta_type = std::uint8_t;
		using meta_container_type = std::vector<std::uint8_t>;
		using hash_type = Hash<key_type>;
		using hidden_key_type = typename hash_type::return_hash_type;
		using iterator_type = MapIterator<key_type, value_type, false>;
		using const_iterator_type = MapIterator<key_type, value_type, true>;
		static constexpr uint8_t EMPTY = 0x80;
		static constexpr uint8_t HASH_MASK = 0x7F;
		static constexpr uint8_t DELETED = 0xFE;
		static constexpr float maxLoadFactor = 0.65f;

	public:
		HashMap();

	public:
		[[nodiscard]] bool Has(const key_type& key) const;

		void Add(const key_type& key, const value_type& value);

		void Add(const key_type& key, value_type&& value);

		void AddOrSet(const key_type& key, value_type&& value);

		void AddOrSet(const key_type& key, const value_type& value);

		void Remove(const key_type& key);
		// TODO
		//iterator_type Remove(iterator_type iterator);
		[[nodiscard]] size_t Size() const;
		[[nodiscard]] bool Empty() const;

		void Clear();

		value_type& operator[](const key_type& key);
		const value_type& operator[](const key_type& key) const;
		value_type& At(const key_type& key);
		const value_type& At(const key_type& key) const;


		const_iterator_type begin() const;

		const_iterator_type end() const;

		iterator_type begin();

		iterator_type end();

		const_iterator_type CBegin() const;

		const_iterator_type CEnd() const;

	private:
		void Resize();

		[[nodiscard]] std::pair<bool, hidden_key_type> PrivateHas(const key_type& key) const;

		[[nodiscard]] static constexpr hidden_key_type HashFn(const key_type& key);

		[[nodiscard]] hidden_key_type GroupIndex(hidden_key_type hash) const;

		[[nodiscard]] static constexpr meta_type FingerPrint(hidden_key_type hash);

		void InsertWithHash(hidden_key_type hash, meta_type fp, data_type&& pair);

		void InsertWithHashIndex(hidden_key_type index, meta_type fp, data_type&& pair);

		void InsertWithHash(hidden_key_type hash, meta_type fp, const  data_type& pair);

		void InsertWithHashIndex(hidden_key_type index, meta_type fp, const  data_type& pair);

		[[nodiscard]] static constexpr bool IsUnoccupied(std::uint8_t fp);
		data_container_type m_data;
		meta_container_type m_metaData;

		size_t m_size;
		static hash_type m_hash;
	};

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		HashMap<KeyT, ValT, Hash>::HashMap() : m_data(), m_size(0)
	{
		m_data.resize(64, data_type{});
		m_metaData.resize(64, EMPTY);
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		bool HashMap<KeyT, ValT, Hash>::Has(const key_type& key) const
	{
		hidden_key_type hash = this->HashFn(key);
		hidden_key_type index = this->GroupIndex(hash);
		meta_type       fingerPrint = HashMap::FingerPrint(hash);

		SIMD_HELPER::pack_type fingerPack = SIMD_HELPER::Copy(fingerPrint);
		SIMD_HELPER::pack_type emptyPack = SIMD_HELPER::Copy(EMPTY);

		while (true)
		{
			SIMD_HELPER::pack_type groupPack = SIMD_HELPER::Fill(&m_metaData[index]);
			SIMD_HELPER::pack_type compPack = SIMD_HELPER::Versus(fingerPack, groupPack);
			hidden_key_type mask = SIMD_HELPER::Mask<hidden_key_type>(compPack);
			while (mask != 0)
			{
				hidden_key_type slot = static_cast<hidden_key_type>(std::countr_zero(mask));
				if (m_data[index + slot].first == key)
					return true;
				SIMD_HELPER::EraseLast1R<hidden_key_type>(mask);
			}


			SIMD_HELPER::pack_type emptyCompPack = SIMD_HELPER::Versus(emptyPack, groupPack);
			hidden_key_type emptyMask = SIMD_HELPER::Mask<hidden_key_type>(emptyCompPack);
			if (emptyMask != 0)
				return false;

			index = (index + 16) % m_data.size();
		}
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		void HashMap<KeyT, ValT, Hash>::Add(const key_type& key, const value_type& value)
	{
			KTR_DEBUG_ASSERT(this->Has(key) != true && "entity already added")

		float loadFactor = static_cast<float>(m_size) / static_cast<float>(m_data.size());
		while (loadFactor > maxLoadFactor)
			Resize();

		hidden_key_type hash = this->HashFn(key);
		meta_type fingerPrint = HashMap::FingerPrint(hash);
		this->InsertWithHash(hash, fingerPrint, { key,value });
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		void HashMap<KeyT, ValT, Hash>::Add(const key_type& key, value_type&& value)
	{
		KTR_DEBUG_ASSERT(this->Has(key) != true && "entity already added")

		float loadFactor = static_cast<float>(m_size) / static_cast<float>(m_data.size());
		while (loadFactor > maxLoadFactor)
			Resize();

		hidden_key_type hash = this->HashFn(key);
		meta_type fingerPrint = HashMap::FingerPrint(hash);
		this->InsertWithHash(hash, fingerPrint, { key, std::move(value) });
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		void HashMap<KeyT, ValT, Hash>::AddOrSet(const key_type& key, value_type&& value)
	{
		std::pair<bool, hidden_key_type> result = this->PrivateHas(key);
		if (result.first)
		{
			m_data[result.second].second = std::move(value);
		}
		else
			this->Add(key, std::move(value));
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		void HashMap<KeyT, ValT, Hash>::AddOrSet(const key_type& key, const value_type& value)
	{
		std::pair<bool, hidden_key_type> result = this->PrivateHas(key);
		if (result.first)
		{
			m_data[result.second].second = value;
		}
		else
			this->Add(key, value);
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		void HashMap<KeyT, ValT, Hash>::Remove(const key_type& key)
	{
		std::pair<bool, hidden_key_type> result = this->PrivateHas(key);

		if (result.first)
		{
			m_data[result.second] = data_type{};
			--m_size;
			m_metaData[result.second] = DELETED;
		}
		KTR_DEBUG_ASSERT(result.first == true && "entity not added")
	}

	

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		size_t HashMap<KeyT, ValT, Hash>::Size() const
	{
		return m_size;
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		bool HashMap<KeyT, ValT, Hash>::Empty() const
	{
		return m_size == 0;
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		void HashMap<KeyT, ValT, Hash>::Clear()
	{
		m_size = 0;
		m_data.clear();
		m_data.resize(64, data_type{});
		m_metaData.clear();
		m_metaData.resize(64, EMPTY);
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		typename HashMap<KeyT, ValT, Hash>::value_type& HashMap<KeyT, ValT, Hash>::operator[](const key_type& key)
	{
		std::pair<bool, hidden_key_type> result = this->PrivateHas(key);
		KTR_DEBUG_ASSERT(result.first == true && "entity not added")
			return m_data[result.second].second;
		
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		const typename HashMap<KeyT, ValT, Hash>::value_type& HashMap<KeyT, ValT, Hash>::operator[](
			const key_type& key) const
	{
		std::pair<bool, hidden_key_type> result = this->PrivateHas(key);
		KTR_DEBUG_ASSERT(result.first == true && "entity not added")
			return m_data[result.second].second;
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		typename HashMap<KeyT, ValT, Hash>::value_type& HashMap<KeyT, ValT, Hash>::At(const key_type& key)
	{
		std::pair<bool, hidden_key_type> result = this->PrivateHas(key);
		KTR_DEBUG_ASSERT(result.first == true && "entity not added")
			return m_data[result.second].second;
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		const typename HashMap<KeyT, ValT, Hash>::value_type& HashMap<KeyT, ValT, Hash>::At(const key_type& key) const
	{
		std::pair<bool, hidden_key_type> result = this->PrivateHas(key);
		KTR_DEBUG_ASSERT(result.first == true && "entity not added")
			return m_data[result.second].second;
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		typename HashMap<KeyT, ValT, Hash>::const_iterator_type HashMap<KeyT, ValT, Hash>::begin() const
	{
		return const_iterator_type(m_data.data(), m_metaData.data(), m_data.data() + m_data.size(), const_iterator_type::begin_flag());
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		typename HashMap<KeyT, ValT, Hash>::const_iterator_type HashMap<KeyT, ValT, Hash>::end() const
	{
		return const_iterator_type(m_data.data() + m_data.size(), m_metaData.data() + m_metaData.size(), m_data.data() + m_data.size());
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		typename HashMap<KeyT, ValT, Hash>::iterator_type HashMap<KeyT, ValT, Hash>::begin()
	{
		return iterator_type(m_data.data(), m_metaData.data(), m_data.data() + m_data.size(), const_iterator_type::begin_flag());
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		typename HashMap<KeyT, ValT, Hash>::iterator_type HashMap<KeyT, ValT, Hash>::end()
	{
		return iterator_type(m_data.data() + m_data.size(), m_metaData.data() + m_metaData.size(), m_data.data() + m_data.size());
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		typename HashMap<KeyT, ValT, Hash>::const_iterator_type HashMap<KeyT, ValT, Hash>::CBegin() const
	{
		return const_iterator_type(m_data.data(), m_metaData.data(), m_data.data() + m_data.size(), const_iterator_type::begin_flag());
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		typename HashMap<KeyT, ValT, Hash>::const_iterator_type HashMap<KeyT, ValT, Hash>::CEnd() const
	{
		return const_iterator_type(m_data.data() + m_data.size(), m_metaData.data() + m_metaData.size(), m_data.data() + m_data.size());
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		void HashMap<KeyT, ValT, Hash>::Resize()
	{
		data_container_type oldData = std::move(m_data);
		meta_container_type oldMeta = std::move(m_metaData);
		m_data.clear();
		m_data.resize(oldData.size() * 2, data_type{});
		m_metaData.clear();
		m_metaData.resize(oldMeta.size() * 2, EMPTY);
		m_size = 0;
		for (size_t i = 0; i < oldData.size(); ++i)
		{
			if (!HashMap::IsUnoccupied(oldMeta[i]))
			{
				this->InsertWithHash(HashFn(oldData[i].first), oldMeta[i], std::move(oldData[i]));
			}
		}
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		std::pair<bool, typename HashMap<KeyT, ValT, Hash>::hidden_key_type> HashMap<KeyT, ValT, Hash>::PrivateHas(
			const key_type& key) const
	{
		hidden_key_type hash = this->HashFn(key);
		hidden_key_type index = this->GroupIndex(hash);
		meta_type       fingerPrint = HashMap::FingerPrint(hash);

		SIMD_HELPER::pack_type fingerPack = SIMD_HELPER::Copy(fingerPrint);
		SIMD_HELPER::pack_type emptyPack = SIMD_HELPER::Copy(EMPTY);

		while (true)
		{
			SIMD_HELPER::pack_type groupPack = SIMD_HELPER::Fill(&m_metaData[index]);
			SIMD_HELPER::pack_type compPack = SIMD_HELPER::Versus(fingerPack, groupPack);
			hidden_key_type mask = SIMD_HELPER::Mask<hidden_key_type>(compPack);
			while (mask != 0)
			{
				hidden_key_type slot = static_cast<hidden_key_type>(std::countr_zero(mask));
				if (m_data[index + slot].first == key)
					return { true, index + slot };
				SIMD_HELPER::EraseLast1R<hidden_key_type>(mask);
			}


			SIMD_HELPER::pack_type emptyCompPack = SIMD_HELPER::Versus(emptyPack, groupPack);
			hidden_key_type emptyMask = SIMD_HELPER::Mask<hidden_key_type>(emptyCompPack);
			if (emptyMask != 0)
				return { false, -1 };

			index = (index + 16) % m_data.size();
		}
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		constexpr typename HashMap<KeyT, ValT, Hash>::hidden_key_type HashMap<KeyT, ValT, Hash>::HashFn(const key_type& key)
	{
		return m_hash(key);
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		typename HashMap<KeyT, ValT, Hash>::hidden_key_type HashMap<KeyT, ValT, Hash>::GroupIndex(
			hidden_key_type hash) const
	{
		return (hash % (m_data.size() / 16)) * 16;
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		constexpr typename HashMap<KeyT, ValT, Hash>::meta_type HashMap<KeyT, ValT, Hash>::FingerPrint(hidden_key_type hash)
	{
		return  hash & HASH_MASK;
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		void HashMap<KeyT, ValT, Hash>::InsertWithHash(hidden_key_type hash, meta_type fp, data_type&& pair)
	{
		hidden_key_type index = this->GroupIndex(hash);
		this->InsertWithHashIndex(index, fp, std::move(pair));
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		void HashMap<KeyT, ValT, Hash>::InsertWithHashIndex(hidden_key_type index, meta_type fp, data_type&& pair)
	{
		while (true)
		{

			SIMD_HELPER::pack_type groupPack = SIMD_HELPER::Fill(&m_metaData[index]);
			SIMD_HELPER::pack_type deletedVec = SIMD_HELPER::Copy(DELETED);
			SIMD_HELPER::pack_type deletedCompPack = SIMD_HELPER::Versus(groupPack, deletedVec);

			hidden_key_type mask = SIMD_HELPER::Mask<hidden_key_type>(deletedCompPack);
			if (mask == 0)
			{
				SIMD_HELPER::pack_type emptyVec = SIMD_HELPER::Copy(EMPTY);
				SIMD_HELPER::pack_type emptyCompPack = SIMD_HELPER::Versus(groupPack, emptyVec);
				mask = SIMD_HELPER::Mask<hidden_key_type>(emptyCompPack);
			}

			if (mask != 0)
			{
				hidden_key_type slot = static_cast<hidden_key_type>(std::countr_zero(mask));
				m_metaData[index + slot] = fp;
				m_data[index + slot] = std::move(pair);
				++m_size;
				return;
			}

			index = (index + 16) % m_data.size();
		}
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		void HashMap<KeyT, ValT, Hash>::InsertWithHash(hidden_key_type hash, meta_type fp, const data_type& pair)
	{
		hidden_key_type index = this->GroupIndex(hash);
		this->InsertWithHashIndex(index, fp, pair);
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		void HashMap<KeyT, ValT, Hash>::InsertWithHashIndex(hidden_key_type index, meta_type fp, const data_type& pair)
	{
		while (true)
		{

			SIMD_HELPER::pack_type groupPack = SIMD_HELPER::Fill(&m_metaData[index]);
			SIMD_HELPER::pack_type deletedVec = SIMD_HELPER::Copy(DELETED);
			SIMD_HELPER::pack_type deletedCompPack = SIMD_HELPER::Versus(groupPack, deletedVec);

			hidden_key_type mask = SIMD_HELPER::Mask<hidden_key_type>(deletedCompPack);
			if (mask == 0)
			{
				SIMD_HELPER::pack_type emptyVec = SIMD_HELPER::Copy(EMPTY);
				SIMD_HELPER::pack_type emptyCompPack = SIMD_HELPER::Versus(groupPack, emptyVec);
				mask = SIMD_HELPER::Mask<hidden_key_type>(emptyCompPack);
			}

			if (mask != 0)
			{

				hidden_key_type slot = static_cast<hidden_key_type>(std::countr_zero(mask));
				m_metaData[index + slot] = fp;
				m_data[index + slot] = pair;
				++m_size;
				return;
			}

			index = (index + 16) % m_data.size();
		}
	}

	template <typename KeyT, typename ValT, template <typename> class Hash> requires (ValidHashOpp<KeyT, Hash>)
		constexpr bool HashMap<KeyT, ValT, Hash>::IsUnoccupied(std::uint8_t fp)
	{
		return (fp & EMPTY) != 0;
	}

	template<typename KeyT, typename ValT, template<typename >class Hash> requires(ValidHashOpp<KeyT, Hash>)
		typename HashMap<KeyT, ValT, Hash>::hash_type  HashMap<KeyT, ValT, Hash>::m_hash = {};
}

#endif

