#ifndef KTR_HANDLER_F
#define KTR_HANDLER_F

#include <utility>
#include "Tools/Hash/KTR_Hasher.h"




namespace KTR
{


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
	Handle<Tag, DataLoad, Accessor>::Handle(const data_type& data): m_data(data)
	{}

	template <typename Tag, typename DataLoad, typename Accessor>
	Handle<Tag, DataLoad, Accessor>::Handle(data_type&& data) noexcept: m_data(std::move(data))
	{}

	namespace HASH
	{
		template<typename Tag, typename DataLoad, typename Accessor, typename HashBits> requires (std::is_same_v<HashBits, std::uint32_t> || std::is_same_v<HashBits, std::uint64_t>)
			struct FNV_1A < Handle < Tag, DataLoad, Accessor>, HashBits >
		{
			using hash_type = Handle <Tag, DataLoad, Accessor>;
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


}
#endif
