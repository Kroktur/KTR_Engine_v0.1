#ifndef KTR_HASHER_F
#define KTR_HASHER_F
#include <cstdint>
#include <string>

namespace KTR
{
	struct HASH
	{
        [[nodiscard]] static constexpr std::uint64_t FNV1aHash(const char* data, size_t len);
	};

    constexpr std::uint64_t HASH::FNV1aHash(const char* data, size_t len)
	{
		std::uint64_t hash = 14695981039346656037ull; 
		for (size_t i = 0; i < len; ++i)
		{
			hash xor_eq static_cast<std::uint8_t>(data[i]);
            hash *= 1099511628211ull;
		}
		return hash;
	}

	template<typename T>
    struct Hash;

    template<>
    struct Hash<std::string>
    {
        [[nodiscard]] static std::uint64_t Compute(const std::string& val);
    };

    inline std::uint64_t Hash<std::string>::Compute(const std::string& val)
    {
	    return KTR::HASH::FNV1aHash(val.c_str(), val.size());
    }

    template<>
    struct Hash<std::string_view>
    {
        [[nodiscard]] static constexpr std::uint64_t Compute(const std::string_view& val);
    };

    constexpr std::uint64_t Hash<std::string_view>::Compute(const std::string_view& val)
    {
	    return KTR::HASH::FNV1aHash(val.data(), val.size());
    }


    template<typename T> requires(std::is_integral_v<T>)
    struct Hash<T>
    {
        [[nodiscard]] static constexpr std::uint64_t Compute(T val);
    };

    template <typename T> requires (std::is_integral_v<T>)
    constexpr std::uint64_t Hash<T>::Compute(T val)
    {
	    return static_cast<std::uint64_t>(val);
    }
}

template<typename T>
concept HashType = requires(T val)
{
    { KTR::Hash<T>::Compute(val) } -> std::same_as<std::uint64_t>;
};




#endif

