#ifndef KTR_FNV_1A_STRING_F
#define KTR_FNV_1A_STRING_F

#include "KTR_FNV_1ADef.h"

namespace KTR
{
	namespace HASH
	{
		template<typename HashBits> requires (std::is_same_v<HashBits, std::uint32_t> || std::is_same_v<HashBits, std::uint64_t>)
			struct FNV_1A<std::string, HashBits>
		{
			using hash_type = std::string;
			using FNV_1A_INFO_type = FNV_1A_INFO<HashBits>;
			using return_hash_type = typename FNV_1A_INFO_type::return_hash_type;
			[[nodiscard]] static constexpr return_hash_type Hash(const hash_type& val);
			[[nodiscard]] constexpr return_hash_type operator()(const hash_type& val) const;
		};


		template <typename HashBits> requires (std::is_same_v<HashBits, std::uint32_t> || std::is_same_v<HashBits, std::uint64_t>)
			constexpr typename FNV_1A<std::string, HashBits>::return_hash_type FNV_1A<std::string, HashBits>::Hash(
				const hash_type& val)
		{
			return_hash_type hash = FNV_1A_INFO_type::Init();
			for (size_t i = 0; i < val.size(); ++i)
			{
				FNV_1A_INFO_type::ComputeByte(hash, static_cast<std::uint8_t>(val[i]));
			}
			return hash;
		}

		template <typename HashBits> requires (std::is_same_v<HashBits, std::uint32_t > || std::is_same_v<HashBits, std::uint64_t>)
			constexpr typename FNV_1A<std::string, HashBits>::return_hash_type FNV_1A<std::string, HashBits>::operator()(
				const hash_type& val) const
		{
			return FNV_1A::Hash(val);
		}

		template<typename HashBits> requires (std::is_same_v<HashBits, std::uint32_t> || std::is_same_v<HashBits, std::uint64_t>)
			struct FNV_1A<std::string_view, HashBits>
		{
			using hash_type = std::string_view;
			using FNV_1A_INFO_type = FNV_1A_INFO<HashBits>;
			using return_hash_type = typename FNV_1A_INFO_type::return_hash_type;
			[[nodiscard]] static constexpr return_hash_type Hash(const hash_type val);

			[[nodiscard]] constexpr return_hash_type operator()(const hash_type val) const;
		};

		template <typename HashBits> requires (std::is_same_v<HashBits, std::uint32_t> || std::is_same_v<HashBits, std::uint64_t>)
			constexpr typename FNV_1A<std::string_view, HashBits>::return_hash_type FNV_1A<std::string_view, HashBits>::Hash(
				const hash_type val)
		{
			return_hash_type hash = FNV_1A_INFO_type::Init();
			for (size_t i = 0; i < val.size(); ++i)
			{
				FNV_1A_INFO_type::ComputeByte(hash, static_cast<std::uint8_t>(val[i]));
			}
			return hash;
		}

		template <typename HashBits> requires (std::is_same_v<HashBits, std::uint32_t> || std::is_same_v<HashBits, std::uint64_t>)
			constexpr typename FNV_1A<std::string_view, HashBits>::return_hash_type FNV_1A<std::string_view, HashBits>::operator
			()(const hash_type val) const
		{
			return FNV_1A::Hash(val);
		}
	}
}

#endif
