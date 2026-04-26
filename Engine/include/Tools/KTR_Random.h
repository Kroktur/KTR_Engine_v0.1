#ifndef KTR_RANDOM_F
#define KTR_RANDOM_F
#include "KTR_Congif_Compiler.h"

#include <random>
#include <vector>
#include <type_traits>




namespace KTR
{
	class Random
	{
	public:

#ifdef KTR_64B
		using seed_type = std::uint64_t;
		using gen_type = std::mt19937_64;
#elifdef KTR_32B
		using seed_type = std::uint32_t;
		using gen_type = std::mt19937;
#else
#error "KTR_64B ou KTR_32B doit être défini"
#endif
	public:
        Random();
        Random(seed_type val);
		Random(const Random&) = default;
		Random(Random&&) noexcept= default;
		~Random() = default;
		Random& operator=(const Random&) = default;
		Random& operator=(Random&&) noexcept = default;
	public:
        template<typename T> requires(std::is_arithmetic_v<T>)
		[[nodiscard]] T getRandom(T min , T max);
        template<typename T> requires(std::is_arithmetic_v<T>)
		[[nodiscard]] std::vector<T> getRandoms(T min ,T max , size_t count);


		void SetSeed(seed_type val);
		[[nodiscard]] seed_type GetSeed() const;

    private:
        static std::random_device m_rd;
        seed_type m_seed;
        gen_type m_gen;
	};

	inline Random::Random(): m_seed(m_rd()),m_gen(m_seed)
	{}

	inline Random::Random(seed_type val):m_seed(val),m_gen(m_seed)
	{}

	template <typename T> requires (std::is_arithmetic_v<T>)
	 T Random::getRandom(T min, T max)
	{
		if constexpr(std::is_integral_v<T>)
		{
			std::uniform_int_distribution<T> dis(min, max);
			return static_cast<T>(dis(m_gen));
		}
		else
		{
			std::uniform_real_distribution<T> dis(min, max);
			return  static_cast<T>(dis(m_gen));
		}
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	std::vector<T> Random::getRandoms(T min, T max, size_t count)
	{
		std::vector<T> result;
		result.reserve(count);
		for (size_t i = 0; i < count; ++i)
		{
			result.push_back( this->getRandom<T>(min, max));
		}
		return result;
	}

	inline void Random::SetSeed(seed_type val)
	{
		m_seed = val;
		m_gen.seed(val);
	}

	inline Random::seed_type Random::GetSeed() const
	{
		return m_seed;
	}

    inline std::random_device Random::m_rd = std::random_device{};
}

#endif
