#ifndef KTR_FNV_1ADEF_F
#define KTR_FNV_1ADEF_F

#include <climits>
#include <cstdint>
#include <string>
#include <string_view> 
#include <type_traits>
#include "KTR_congif_compiler.h"
namespace KTR
{
	namespace HASH
	{


		template<typename T>
		struct FNV_1A_INFO_TRAITS;

		template<>
		struct FNV_1A_INFO_TRAITS<std::uint32_t>
		{
			using return_hash_type = std::uint32_t;
			// for documentation 
			// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
			static constexpr return_hash_type FNV_basis = 2166136261u;
			static constexpr return_hash_type FNV_prime = 16777619u;
		};

		template<>
		struct FNV_1A_INFO_TRAITS<std::uint64_t>
		{
			using return_hash_type = std::uint64_t;
			// for documentation 
			// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
			static constexpr return_hash_type FNV_basis = 14695981039346656037ull;
			static constexpr return_hash_type FNV_prime = 1099511628211ull;
		};


		template<typename T> requires(std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::uint64_t>)
			struct FNV_1A_INFO
		{
			using return_hash_type = typename FNV_1A_INFO_TRAITS<T>::return_hash_type;
			static constexpr return_hash_type FNV_basis = FNV_1A_INFO_TRAITS<T>::FNV_basis;
			static constexpr return_hash_type FNV_prime = FNV_1A_INFO_TRAITS<T>::FNV_prime;

			[[nodiscard]] static constexpr  return_hash_type Init();
			static constexpr  void ComputeByte(return_hash_type& hash, const std::uint8_t byte);
		};


		template <typename T> requires (std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::uint64_t>)
			constexpr typename FNV_1A_INFO<T>::return_hash_type FNV_1A_INFO<T>::Init()
		{
			return FNV_basis;
		}

		template <typename T> requires (std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::uint64_t>)
			constexpr void FNV_1A_INFO<T>::ComputeByte(return_hash_type& hash, const std::uint8_t byte)
		{
			// xor_eq : ^= 
			// static_cast<std::uint8_t> get the last byte (8 bits)
			hash xor_eq byte;
			hash *= FNV_prime;
		}

		template<typename T, typename HashBits>
		struct FNV_1A;
	}

}
#endif
