#ifndef KTR_COUNTER_F	
#define KTR_COUNTER_F
#include <type_traits>
#include "KTR_Assert.h"

namespace KTR
{
	template<typename T> requires(std::is_arithmetic_v<T>)
	class Counter
	{
	public:
		using value_type = T;
		Counter();
		Counter(value_type min , value_type max, bool activeLoop = false);
		Counter(const Counter&) = default;
		Counter(Counter&&) noexcept= default;
		~Counter() = default;
		Counter& operator=(const Counter&) = default;
		Counter& operator=(Counter&&) noexcept = default;
	public: 
		Counter& operator++();
		[[nodiscard]] Counter operator++(int);
		Counter& operator--();
		[[nodiscard]] Counter operator--(int);
		[[nodiscard]] Counter operator+(value_type value ) const;
		Counter& operator+=(value_type value);
		[[nodiscard]] Counter operator-(value_type value) const;
		Counter& operator-=(value_type value);
		[[nodiscard]] bool operator==(value_type value) const;
		[[nodiscard]] bool operator!=(value_type value) const;
		[[nodiscard]] bool operator<(value_type value) const;
		[[nodiscard]] bool operator<=(value_type value) const;
		[[nodiscard]] bool operator>(value_type value) const;
		[[nodiscard]] bool operator>=(value_type value) const;
		[[nodiscard]] bool IsLooping() const;
		void Loop(bool active);
		Counter& operator=(value_type value);
		void Set(T value);
		void SetMinMax(value_type min , value_type max);
		[[nodiscard]] value_type Get() const;
		[[nodiscard]] value_type Max() const;
		[[nodiscard]] value_type Min() const;

	private:
		void Plus(value_type value);

		void Plus();

		void Minus(value_type value);

		void Minus();
		bool m_loopEnabled;
		value_type m_val;
		value_type m_min;
		value_type m_max;
	};

	template <typename T> requires (std::is_arithmetic_v<T>)
	Counter<T>::Counter(): m_val(0),m_min(0),m_max(0),m_loopEnabled(false)
	{}

	template <typename T> requires (std::is_arithmetic_v<T>)
	Counter<T>::Counter(value_type min, value_type max, bool activeLoop): m_min(min),m_max(max),m_val(0),m_loopEnabled(activeLoop)
	{
		if (min > max)
			throw SpeExcept<std::out_of_range>("max need to be bigger than min");
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	Counter<T>& Counter<T>::operator++()
	{
		Plus();
		return *this;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	Counter<T> Counter<T>::operator++(int)
	{
		Counter tmp = *this;
		Plus();
		return tmp;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	Counter<T>& Counter<T>::operator--()
	{
		Minus();
		return *this;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	Counter<T> Counter<T>::operator--(int)
	{
		Counter tmp = *this;
		Minus();
		return tmp;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	Counter<T> Counter<T>::operator+(value_type value) const
	{
		Counter tmp = *this;
		tmp.Plus(value);
		return tmp;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	Counter<T>& Counter<T>::operator+=(value_type value)
	{
		this->Plus(value);
		return *this;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	Counter<T> Counter<T>::operator-(value_type value) const
	{
		Counter tmp = *this;
		tmp.Minus(value);
		return tmp;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	Counter<T>& Counter<T>::operator-=(value_type value)
	{
		this->Minus(value);
		return *this;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	bool Counter<T>::operator==(value_type value) const
	{
		return m_val == value;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	bool Counter<T>::operator!=(value_type value) const
	{
		return !(*this == value);
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	bool Counter<T>::operator<(value_type value) const
	{
		return m_val < value;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	bool Counter<T>::operator<=(value_type value) const
	{
		return m_val <= value;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	bool Counter<T>::operator>(value_type value) const
	{
		return m_val > value;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	bool Counter<T>::operator>=(value_type value) const
	{
		return m_val >= value;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	bool Counter<T>::IsLooping() const
	{
		return m_loopEnabled;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	void Counter<T>::Loop(bool active)
	{
		m_loopEnabled = active;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	Counter<T>& Counter<T>::operator=(value_type value)
	{
		if (value == m_val)
			return *this;

		if (value > m_val)
			this->Plus(value - m_val);
		else
			this->Minus(m_val - value);
		return *this;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	void Counter<T>::Set(T value)
	{
		*this = value;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	void Counter<T>::SetMinMax(value_type min, value_type max)
	{
		if (min > max)
			throw SpeExcept<std::out_of_range>("max need to be bigger than min");

		m_min = min;
		m_max = max;

		if (m_val > m_max)
			m_val = m_max;
		else if (m_val < m_min)
			m_val = m_min;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	typename Counter<T>::value_type Counter<T>::Get() const
	{
		return m_val;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	typename Counter<T>::value_type Counter<T>::Max() const
	{
		return m_max;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	typename Counter<T>::value_type Counter<T>::Min() const
	{
		return m_min;
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	void Counter<T>::Plus(value_type value)
	{
		for (value_type i = 0 ; i < value; ++i)
		{
			Plus();
		}
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	void Counter<T>::Plus()
	{
		if (++m_val > m_max)
		{
			if (m_loopEnabled)
				m_val = m_min;
			else
				m_val = m_max;
		}
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	void Counter<T>::Minus(value_type value)
	{
		for (value_type i = 0; i < value; ++i)
		{
			Minus();
		}
	}

	template <typename T> requires (std::is_arithmetic_v<T>)
	void Counter<T>::Minus()
	{
		if (--m_val < m_max)
		{
			if (m_loopEnabled)
				m_val = m_max;
			else
				m_val = m_min;
		}
	}
}

#endif


