#ifndef KTR_SIMD_F
#define KTR_SIMD_F

#include <cstdint>
#include <immintrin.h>
#include <type_traits>


namespace KTR
{
	struct SIMD_HELPER
	{
		using pack_type = __m128i;
		[[nodiscard]] static pack_type Copy(std::uint8_t data);

		[[nodiscard]] static  pack_type Fill(const std::uint8_t* data);

		[[nodiscard]] static  pack_type Versus(pack_type lhs, pack_type rhs);

		template<typename T> requires(std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::uint64_t>)
		[[nodiscard]] static T Mask(pack_type pack);

		template<typename T> requires(std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::uint64_t>)
			static constexpr void EraseLast1R(T& mask);
	};


	inline SIMD_HELPER::pack_type SIMD_HELPER::Copy(std::uint8_t data)
	{
		return _mm_set1_epi8(static_cast<char>(data));
	}

	inline SIMD_HELPER::pack_type SIMD_HELPER::Fill(const std::uint8_t* data)
	{
		return _mm_loadu_si128(reinterpret_cast<__m128i const*>(data));
	}

	inline SIMD_HELPER::pack_type SIMD_HELPER::Versus(pack_type lhs, pack_type rhs)
	{
		return _mm_cmpeq_epi8(lhs, rhs);
	}

	template <typename T> requires (std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::uint64_t>)
		T SIMD_HELPER::Mask(pack_type pack)
	{
		return static_cast<std::uint32_t>(_mm_movemask_epi8(pack));
	}

	template <typename T> requires (std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::uint64_t>)
		constexpr void SIMD_HELPER::EraseLast1R(T& mask)
	{
		mask &= mask - 1;
	}

}
#endif
