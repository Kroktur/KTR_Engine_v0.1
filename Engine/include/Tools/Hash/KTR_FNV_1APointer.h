#ifndef KTR_FNV_1A_POINTER_F
#define KTR_FNV_1A_POINTER_F

#include "KTR_FNV_1ADef.h"
namespace KTR
{
	namespace hash
	{

		/**
		 * @brief FNV-1A hash specialization for pointer types.
		 *
		 * This specialization provides hashing support for pointer values by
		 * converting the memory address into an integer representation and
		 * processing its individual bytes through the FNV-1A algorithm.
		 *
		 * The pointed data is not accessed or hashed. Only the address value
		 * stored inside the pointer is used as the input.
		 *
		 * @tparam T Pointer type to hash.
		 * @tparam HashBits Hash size type used by the algorithm
		 *                  (std::uint32_t or std::uint64_t).
		 */
		template<typename T, KTR_ValidHashBits HashBits> requires(std::is_pointer_v<T>)
			struct FNV_1A<T, HashBits>
		{
			using hash_type = T;
			using FNV_1A_INFO_type = FNV_1A_INFO<HashBits>;
			using return_hash_type = typename FNV_1A_INFO_type::return_hash_type;
			/**
			 * @brief Computes the FNV-1A hash of a pointer value.
			 *
			 * The pointer address is converted into an integer representation
			 * and processed byte by byte. The memory content referenced by the
			 * pointer is intentionally ignored.
			 *
			 * @param val Pointer value to hash.
			 * @return Computed FNV-1A hash value.
			 */
			[[nodiscard]] static constexpr return_hash_type Hash(hash_type val)
			{
				return_hash_type hash = FNV_1A_INFO_type::Init();
				const auto addr = reinterpret_cast<std::uintptr_t>(val);

				for (size_t i = 0; i < sizeof(std::uintptr_t); ++i)
				{
					FNV_1A_INFO_type::ComputeByte(hash, static_cast<std::uint8_t>(addr >> (i * KTR_CHAR_BIT)));
				}
				return hash;
			}

			/**
			 * @brief Functor interface for hashing a pointer value.
			 *
			 * Provides a callable wrapper around the static Hash() function.
			 *
			 * @param val Pointer value to hash.
			 * @return Computed FNV-1A hash value.
			 */
			[[nodiscard]] constexpr return_hash_type operator()(hash_type val) const
			{
				return FNV_1A::Hash(val);
			}
		};
	}
}

#endif
