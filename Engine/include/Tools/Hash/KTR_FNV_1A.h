#ifndef KTR_FNV_1A_F
#define KTR_FNV_1A_F

#include "KTR_FNV_1AIntegral.h"
#include "KTR_FNV_1AString.h"

namespace KTR
{
	namespace HASH
	{
		

		template<typename T>
		using FNV_1A_32 = FNV_1A<T, std::uint32_t>;
		template<typename T>
		using FNV_1A_64 = FNV_1A<T, std::uint64_t>;

#ifdef KTR_64B
		template<typename T>
		using FNV_1A_type = FNV_1A_64<T>;
#elifdef KTR_32B
		template<typename T>
		using FNV_1A_type = FNV_1A_32<T>;
#endif

	}
}
#endif
