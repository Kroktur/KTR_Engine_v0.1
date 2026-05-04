#ifndef KTR_FNV_1A_INTEGRAL_F
#define KTR_FNV_1A_INTEGRAL_F

#include "KTR_FNV_1ADef.h"

namespace KTR
{
	namespace HASH
	{
		template<typename T, typename HashBits> requires(std::is_integral_v<T>) && (std::is_same_v<HashBits, std::uint32_t> || std::is_same_v<HashBits, std::uint64_t>)
			struct FNV_1A<T, HashBits>
		{
			using hash_type = T;
			using FNV_1A_INFO_type = FNV_1A_INFO<HashBits>;
			using return_hash_type = typename FNV_1A_INFO_type::return_hash_type;
			[[nodiscard]] static constexpr return_hash_type Hash(hash_type val);
			[[nodiscard]] constexpr return_hash_type operator()(hash_type val) const;
		};


		template <typename T, typename HashBits> requires (std::is_integral_v<T>) && (std::is_same_v<HashBits, std::uint32_t>
		|| std::is_same_v<HashBits, std::uint64_t>)
			constexpr typename FNV_1A<T, HashBits>::return_hash_type FNV_1A<T, HashBits>::Hash(hash_type val)
		{
			return_hash_type hash = FNV_1A_INFO_type::Init();

			for (size_t i = 0; i < sizeof(hash_type); ++i)
			{
				FNV_1A_INFO_type::ComputeByte(hash, static_cast<std::uint8_t>(val >> (i * CHAR_BIT)));
			}
			return hash;
		}

		template <typename T, typename HashBits> requires (std::is_integral_v<T>) && (std::is_same_v<HashBits, std::uint32_t>
		|| std::is_same_v<HashBits, std::uint64_t>)
			constexpr typename FNV_1A<T, HashBits>::return_hash_type FNV_1A<T, HashBits>::operator()(hash_type val) const
		{
			return FNV_1A::Hash(val);
		}
	}
}

#endif
