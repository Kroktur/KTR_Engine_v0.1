#ifndef KTR_HASHER_F
#define KTR_HASHER_F
#include <cstdint>
#include <string>

namespace KTR
{
	struct HASH
	{
        static constexpr std::uint64_t FNV1aHash(const char* data, size_t len);
	};

	constexpr std::uint64_t HASH::FNV1aHash(const char* data, size_t len)
	{
		std::uint64_t hash = 14695981039346656037ull; // FNV offset basis
		for (size_t i = 0; i < len; ++i)
		{
			hash *= 1099511628211ull;                 // FNV prime
			hash xor_eq static_cast<std::uint8_t>(data[i]);
		}
		return hash;
	}

	template<typename T>
    struct Hash;

    template<>
    struct Hash<std::string>
    {
        static constexpr std::uint64_t Compute(const std::string& val);
    };

    constexpr std::uint64_t Hash<std::string>::Compute(const std::string& val)
    {
	    return KTR::HASH::FNV1aHash(val.c_str(), val.size());
    }

    template<>
    struct Hash<std::string_view>
    {
        static constexpr std::uint64_t Compute(const std::string_view& val);
    };

    constexpr std::uint64_t Hash<std::string_view>::Compute(const std::string_view& val)
    {
	    return KTR::HASH::FNV1aHash(val.data(), val.size());
    }
}

template<typename T>
concept HashType = requires(const T & val)
{
    { KTR::Hash<T>::Compute(val) };
};




#endif

