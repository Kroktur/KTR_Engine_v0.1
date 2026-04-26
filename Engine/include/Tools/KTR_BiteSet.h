#pragma once
#include <type_traits>

namespace KTR
{
	template<typename T> requires(std::is_integral_v<T>)
	struct Bit
	{
		[[nodiscard]] static constexpr T ValToBit(T val);

		 template<typename Enum,bool isBit> requires(std::is_enum_v<Enum>&& std::is_same_v<T, std::underlying_type_t<Enum>>)
		[[nodiscard]] static constexpr T EnumToBit(Enum val);
	};


	template <typename T> requires (std::is_integral_v<T>)
	 constexpr T Bit<T>::ValToBit(T val)
	{
		return static_cast<T>(1) << val;
	}

	template <typename T> requires (std::is_integral_v<T>)
	template <typename Enum, bool isBit> requires (std::is_enum_v<Enum> && std::is_same_v<T, std::underlying_type_t<Enum
		>>)
	constexpr T Bit<T>::EnumToBit(Enum val)
	{
		if constexpr (isBit)
			return static_cast<T>(val);
		else
			return Bit::ValToBit(static_cast<T>(val));
	}

	template<typename T> requires(std::is_integral_v<T>)
	struct BitSet
	{
	public:
		using value_type = T;
		static constexpr size_t maxFlags = sizeof(value_type) * 8;
		static constexpr value_type defaultValue = static_cast<value_type>(0);
	public:
		BitSet();
		BitSet(const BitSet&) = default;
		BitSet(BitSet&&) noexcept = default;
		~BitSet() = default;
		BitSet& operator=(const BitSet&) = default;
		BitSet& operator=(BitSet&&) noexcept = default;
	public:
		void Add(value_type val);
		template<typename... Ts> requires(... && std::is_same_v<Ts, T>)
		void Adds(Ts... val);
		void Remove(value_type val);
		template<typename... Ts> requires(... && std::is_same_v<Ts, T>)
		void Removes(Ts... val);
		void Toggle(value_type val);
		template<typename... Ts> requires(... && std::is_same_v<Ts, T>)
		void Toggles(Ts... val);
		[[nodiscard]] bool Has(value_type val) const;
		template<typename... Ts> requires(... && std::is_same_v<Ts, T>)
		[[nodiscard]] bool HasAll(Ts... val);
		template<typename... Ts> requires(... && std::is_same_v<Ts, T>)
		[[nodiscard]] bool HasAny(Ts... val);
		void Clear();
		[[nodiscard]] bool operator==(const BitSet& other) const;
		[[nodiscard]] bool operator!=(const BitSet& other) const;
		[[nodiscard]] BitSet operator+(const BitSet& other) const;
		BitSet& operator+=(const BitSet& other);
		[[nodiscard]] BitSet operator-(const BitSet& other) const;
		BitSet& operator-=(const BitSet& other);

	private:
		value_type m_flag;
	};

	template <typename T> requires (std::is_integral_v<T>)
	BitSet<T>::BitSet(): m_flag(defaultValue)
	{}


	template <typename T> requires (std::is_integral_v<T>)
	void BitSet<T>::Add(value_type val)
	{
		m_flag |= val;
	}

	template <typename T> requires (std::is_integral_v<T>)
	template <typename ... Ts> requires (... && std::is_same_v<Ts, T>)
	void BitSet<T>::Adds(Ts... val)
	{
		(this->Add(val), ...);
	}

	template <typename T> requires (std::is_integral_v<T>)
	void BitSet<T>::Remove(value_type val)
	{
		m_flag &= ~val;
	}

	template <typename T> requires (std::is_integral_v<T>)
		template <typename ... Ts> requires (... && std::is_same_v<Ts, T>)
		void BitSet<T>::Removes(Ts... val)
	{
		(this->Remove(val), ...);
	}

	template <typename T> requires (std::is_integral_v<T>)
	void BitSet<T>::Toggle(value_type val)
	{
		m_flag ^= val;
	}

	template <typename T> requires (std::is_integral_v<T>)
	template <typename ... Ts> requires (... && std::is_same_v<Ts, T>)
	void BitSet<T>::Toggles(Ts... val)
	{
		(this->Toggle(val), ...);
	}

	template <typename T> requires (std::is_integral_v<T>)
	 bool BitSet<T>::Has(value_type val) const
	{
		return (m_flag & val) != 0;
	}

	template <typename T> requires (std::is_integral_v<T>)
	template <typename ... Ts> requires (... && std::is_same_v<Ts, T>)
	bool BitSet<T>::HasAll(Ts... val)
	{
		return (this->Has(val) &&  ...);
	}

	template <typename T> requires (std::is_integral_v<T>)
	template <typename ... Ts> requires (... && std::is_same_v<Ts, T>)
	 bool BitSet<T>::HasAny(Ts... val)
	{
		return (this->Has(val) ||  ...);
	}

	template <typename T> requires (std::is_integral_v<T>)
	void BitSet<T>::Clear()
	{
		m_flag = defaultValue;
	}

	template <typename T> requires (std::is_integral_v<T>)
	bool BitSet<T>::operator==(const BitSet& other) const
	{
		return m_flag == other.m_flag;
	}

	template <typename T> requires (std::is_integral_v<T>)
	bool BitSet<T>::operator!=(const BitSet& other) const
	{
		return !(*this == other);
	}

	template <typename T> requires (std::is_integral_v<T>)
	BitSet<T> BitSet<T>::operator+(const BitSet& other) const
	{
		auto tmp = *this;
		tmp.m_flag |= other.m_flag;
		return tmp;
	}

	template <typename T> requires (std::is_integral_v<T>)
	BitSet<T>& BitSet<T>::operator+=(const BitSet& other)
	{
		m_flag |= other.m_flag;
		return *this;
	}

	template <typename T> requires (std::is_integral_v<T>)
	BitSet<T> BitSet<T>::operator-(const BitSet& other) const
	{
		auto tmp = *this;
		tmp.m_flag &= ~other.m_flag;
		return tmp;
	}

	template <typename T> requires (std::is_integral_v<T>)
	BitSet<T>& BitSet<T>::operator-=(const BitSet& other)
	{
		m_flag &= ~other.m_flag;
		return *this;
	}
}
