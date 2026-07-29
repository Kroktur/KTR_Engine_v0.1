#ifndef KTR_CACHEREGISTRY_F
#define KTR_CACHEREGISTRY_F

#include "Tools/Hash/KTR_Hasher.h"
#include "HashMap/KTR_HashMap.h"
#include "KTR_assert.h"
#include <utility>
#include "KTR_Congif_Compiler.h"
#include "KTR_IdPool.h"
#include <optional>
#include <memory>


namespace KTR
{
	namespace CACHE
	{
		struct DataHandler
		{
			using id_type = size_t;
			id_type index = 0;
			id_type gen = 0;
		};

		template<typename ValT>
		struct Slot
		{
			using raw_type = ValT;
			using data_type = std::unique_ptr<raw_type>;
			using gen_type = size_t;
			static constexpr gen_type invalidGen = std::numeric_limits<gen_type>::max();
			data_type data = nullptr;
			gen_type gen = invalidGen;
		};



		template<typename Tag, typename DataLoad, typename Accessor>
		class Handle
		{
		public:
			using tag_type = Tag;
			using data_type = DataLoad;
			using accessor_type = Accessor;
			friend accessor_type;
		public:
			Handle() = default;
			Handle(const Handle&) = default;
			Handle(Handle&&) noexcept = default;
			~Handle() = default;
			Handle& operator=(const Handle&) = default;
			Handle& operator=(Handle&&) noexcept = default;
			[[nodiscard]] bool operator==(const Handle& other) const;
			[[nodiscard]] data_type GetData() const;
		private:
			Handle(const data_type& data);
			Handle(data_type&& data) noexcept;
			data_type m_data = data_type{};
		};



		template <typename Tag, typename DataLoad, typename Accessor>
		bool Handle<Tag, DataLoad, Accessor>::operator==(const Handle& other) const
		{
			return m_data == other.m_data;
		}

		template <typename Tag, typename DataLoad, typename Accessor>
		typename Handle<Tag, DataLoad, Accessor>::data_type Handle<Tag, DataLoad, Accessor>::GetData() const
		{
			return m_data;
		}

		template <typename Tag, typename DataLoad, typename Accessor>
		Handle<Tag, DataLoad, Accessor>::Handle(const data_type& data) : m_data(data)
		{
		}

		template <typename Tag, typename DataLoad, typename Accessor>
		Handle<Tag, DataLoad, Accessor>::Handle(data_type&& data) noexcept : m_data(std::move(data))
		{
		}
	}
	namespace HASH
	{
		template<typename Tag, typename DataLoad, typename Accessor, typename HashBits> requires (std::is_same_v<HashBits, std::uint32_t> || std::is_same_v<HashBits, std::uint64_t>)
			struct FNV_1A < CACHE::Handle < Tag, DataLoad, Accessor>, HashBits >
		{
			using hash_type = CACHE::Handle  <Tag, DataLoad, Accessor>;
			using FNV_1A_INFO_type = FNV_1A_INFO<HashBits>;
			using return_hash_type = typename FNV_1A_INFO_type::return_hash_type;
			[[nodiscard]] static constexpr return_hash_type Hash(const hash_type& val)
			{
				return FNV_1A<DataLoad, HashBits>::Hash(val.GetData());
			}
			[[nodiscard]] constexpr return_hash_type operator()(const hash_type& val) const
			{
				return FNV_1A<DataLoad, HashBits>::Hash(val.GetData());
			}
		};
	}



	template<ValidHashOpp KeyT, typename ValT>
	class CacheRegistry
	{
	public:
		using key_type = KeyT;
		using raw_type = ValT;
		using raw_ptr_type = raw_type*;
		using ref_type = raw_type&;
		
		using data_type = std::unique_ptr<raw_type>;
		using handle_data_type = CACHE::DataHandler;
		using handle_type = CACHE::Handle <raw_type, handle_data_type, CacheRegistry>;
		using slot_type = CACHE::Slot<raw_type>;

		static constexpr size_t chuckSize = 64;
		using hash_container = HashMap<key_type, size_t>;
		using pool_type = IdPool<size_t>;
		using slot_container = std::vector<slot_type>;
	public:
		CacheRegistry();
		CacheRegistry(const CacheRegistry&) = delete;
		CacheRegistry(CacheRegistry&&) noexcept = default;
		~CacheRegistry() = default;
		CacheRegistry& operator=(const CacheRegistry&) = delete;
		CacheRegistry& operator=(CacheRegistry&&) = default;
	public:
		handle_type Insert(const key_type& key, data_type ptr);
		handle_type Swap(const key_type& key, data_type newPtr);
		[[nodiscard]] std::optional<handle_type> Find(const key_type& key) const;

		void Remove(const key_type& key);

		[[nodiscard]] bool IsValid(const handle_type& handle) const;
		[[nodiscard]] std::optional <handle_type> Update(const handle_type& handle) const;

		[[nodiscard]] raw_ptr_type GetPtr(const handle_type& handle) const;
		[[nodiscard]] ref_type GetRef(const handle_type& handle) const;

		
		void Clear();
	private:
		void Resize();


		hash_container m_map;
		pool_type m_idPool;
		slot_container m_slot;
	};

	template <ValidHashOpp KeyT, typename ValT>
	CacheRegistry<KeyT, ValT>::CacheRegistry()
	{
		m_slot.resize(chuckSize);
	}

	template <ValidHashOpp KeyT, typename ValT>
	typename CacheRegistry<KeyT, ValT>::handle_type CacheRegistry<KeyT, ValT>::Insert(const key_type& key,
		data_type ptr)
	{ 
		if (m_map.Has(key))
			throw SpeExcept<std::out_of_range>("already store");;

		size_t id = m_idPool.AcqId();

		if (id >= m_slot.size())
			Resize();

		size_t gen = m_slot[id].gen == slot_type::invalidGen ? 0 : m_slot[id].gen + 1;

			

		handle_type handle;
		handle.m_data = handle_data_type{ id,gen};

		m_slot[id].data = std::move(ptr);
		m_slot[id].gen = gen;

		m_map.Add(key, id);
		return handle;
	}

	template <ValidHashOpp KeyT, typename ValT>
	bool CacheRegistry<KeyT, ValT>::IsValid(const handle_type& handle) const
	{
		return m_slot.size() > handle.m_data.index && m_slot[handle.m_data.index].gen == handle.m_data.gen && m_slot[handle.m_data.index].data;
	}

	template <ValidHashOpp KeyT, typename ValT>
	std::optional<typename CacheRegistry<KeyT, ValT>::handle_type> CacheRegistry<KeyT, ValT>::Find(
		const key_type& key) const
	{
		const size_t* val = m_map.Find(key);
		if (val)
		{
			handle_type handle;
			handle.m_data = handle_data_type{*val,m_slot[*val].gen};
			return std::optional<handle_type>(handle);
		}
		return std::optional<handle_type>();
	}

	template <ValidHashOpp KeyT, typename ValT>
	std::optional<typename CacheRegistry<KeyT, ValT>::handle_type> CacheRegistry<KeyT, ValT>::Update(const handle_type& handle) const
	{
		if (this->IsValid(handle))
			return std::optional<handle_type>(handle);

		size_t index = handle.m_data.index;
		if (m_slot.size() <= index || !m_slot[index].data)
			return std::optional<handle_type>();
	

		handle_type newHandle;
		newHandle.m_data = handle_data_type{ index,m_slot[index].gen };
		return std::optional<handle_type>(newHandle);
	}

	template <ValidHashOpp KeyT, typename ValT>
	typename CacheRegistry<KeyT, ValT>::handle_type CacheRegistry<KeyT, ValT>::Swap(const key_type& key,
		data_type newPtr)
	{
		size_t* val = m_map.Find(key);
		if (!val)
			throw SpeExcept<std::out_of_range>("not store");;

		size_t index = *val;
		handle_type newHandle;
		newHandle.m_data.index = index;
		newHandle.m_data.gen = ++m_slot[index].gen;
		m_slot[index].data = std::move(newPtr);
		return newHandle;
	}

	template <ValidHashOpp KeyT, typename ValT>
	typename CacheRegistry<KeyT, ValT>::raw_ptr_type CacheRegistry<KeyT, ValT>::GetPtr(const handle_type& handle) const
	{
		if (this->IsValid(handle))
			return m_slot[handle.m_data.index].data.get();
		else
			return nullptr;
	}

	template <ValidHashOpp KeyT, typename ValT>
	typename CacheRegistry<KeyT, ValT>::ref_type CacheRegistry<KeyT, ValT>::GetRef(const handle_type& handle) const
	{
		if (!this->IsValid(handle))
			throw SpeExcept<std::out_of_range>("not store");;
		return *m_slot[handle.m_data.index].data;
	}

	template <ValidHashOpp KeyT, typename ValT>
	void CacheRegistry<KeyT, ValT>::Remove(const key_type& key)
	{
		size_t* val = m_map.Find(key);
		if (!val)
			throw SpeExcept<std::out_of_range>("not store");;
		size_t index = *val;
		m_slot[index].data = nullptr;
		m_map.Remove(key);
		m_idPool.FreeId(index);
	}

	template <ValidHashOpp KeyT, typename ValT>
	void CacheRegistry<KeyT, ValT>::Clear()
	{
		m_slot.clear();
		m_map.Clear();
		m_idPool.Clear();
	}

	template <ValidHashOpp KeyT, typename ValT>
	void CacheRegistry<KeyT, ValT>::Resize()
	{
		m_slot.resize(m_slot.size() + chuckSize);
	}
}

#endif
