#ifndef KTR_HASHER_F
#define KTR_HASHER_F
#include <cstdint>
#include <type_traits>
#include "KTR_FNV_1A.h"

namespace KTR
{
	namespace HASH
	{
		template<typename T>
		using default_hash_type = FNV_1A_type<T>;
	}
}

template< typename T, template<typename >class Hash = KTR::HASH::default_hash_type>
concept ValidHashOpp = requires(Hash<T> hash, T t)
{
	typename Hash<T>::return_hash_type;
	typename Hash<T>::hash_type;
	{ Hash<T>::Hash(t) } -> std::convertible_to<typename Hash<T>::return_hash_type>;
	{ hash(t) }-> std::convertible_to<typename Hash<T>::return_hash_type>;
}
&& (std::is_same_v<std::uint32_t, typename Hash<T>::return_hash_type> || std::is_same_v<std::uint64_t, typename Hash<T>::return_hash_type>)
&& (std::is_same_v<T, typename Hash<T>::hash_type>);




#endif
