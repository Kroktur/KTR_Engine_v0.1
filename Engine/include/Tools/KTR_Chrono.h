#ifndef KTR_CHRONO_F
#define KTR_CHRONO_F

#include <chrono>
#include <utility>
#include "KTR_Assert.h"
namespace KTR
{
    namespace TIME
    {
        template<long long LhsT, long long RhsT = 1>
        using ratio_type = std::ratio<LhsT, RhsT>;

        using seconds_type = ratio_type<1>;
        using milli_seconds_type = std::milli;
        using micro_seconds_type = std::micro;
        using nano_seconds_type = std::nano;
        using minutes_type = ratio_type<60>;
        using hours_type = ratio_type<3600>;
    }
}

template<typename T>
concept RatioType = requires()
{
	typename T::type;
    { T::num };
    { T::den };
};

namespace KTR
{
    namespace TIME
    {
        using timer_type = std::chrono::high_resolution_clock;
        using time_point_type = std::chrono::time_point<timer_type>;
        using duration_type = timer_type::duration;

        static time_point_type Now()
        {
            return timer_type::now();
        }

        template<typename T> requires(std::is_arithmetic_v<T>)
            class Time
        {
        public:

            using value_type = T;
        public:
            Time() : m_duration(0) {}
            Time(const duration_type& duration);
            Time(duration_type&& duration) noexcept;
            Time(const time_point_type& start, const time_point_type& end);
            Time(const Time&) = default;
            Time(Time&&) noexcept = default;
            ~Time() = default;
            Time& operator=(const Time&) = default;
            Time& operator=(Time&&) noexcept = default;
            Time& operator=(const duration_type& other);
            Time& operator=(duration_type&& other) noexcept;;
        public:
            [[nodiscard]] bool operator==(const Time& other) const;
            [[nodiscard]] bool operator!=(const Time& other) const;
            [[nodiscard]] bool operator>=(const Time& other) const;
            [[nodiscard]] bool operator<=(const Time& other) const;
            [[nodiscard]] bool operator>(const Time& other) const;
            [[nodiscard]] bool operator<(const Time& other) const;

            [[nodiscard]] Time operator+(const Time& other) const;

            [[nodiscard]] Time operator-(const Time& other) const;

            Time& operator+=(const Time& other);

            Time& operator-=(const Time& other);

            [[nodiscard]] Time operator*(value_type factor) const;

            [[nodiscard]] Time operator/(value_type divider) const;

            Time& operator*=(value_type factor);

            Time& operator/=(value_type divider);

        public:
			template<RatioType Ratio>
            [[nodiscard]] static Time FromValue(value_type duration);
            [[nodiscard]] duration_type Duration() const;

            template<RatioType Ratio>
            [[nodiscard]] T AsRatio() const;

        private:
            Time(value_type nanoVal);
            duration_type m_duration;
        };


        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T>::Time(const duration_type& duration): m_duration(duration)
        {}

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T>::Time(duration_type&& duration) noexcept: m_duration(std::move(duration))
        {}

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T>::Time(const time_point_type& start, const time_point_type& end): m_duration((end - start))
        {}

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T>& Time<T>::operator=(const duration_type& other)
        { m_duration = other; return *this; }

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T>& Time<T>::operator=(duration_type&& other) noexcept
        { m_duration = std::move(other); return *this; }

        template <typename T> requires (std::is_arithmetic_v<T>)
        bool Time<T>::operator==(const Time& other) const
        { return m_duration == other.m_duration; }

        template <typename T> requires (std::is_arithmetic_v<T>)
        bool Time<T>::operator!=(const Time& other) const
        { return !(*this == other); }

        template <typename T> requires (std::is_arithmetic_v<T>)
        bool Time<T>::operator>=(const Time& other) const
        { return m_duration >= other.m_duration; }

        template <typename T> requires (std::is_arithmetic_v<T>)
        bool Time<T>::operator<=(const Time& other) const
        { return m_duration <= other.m_duration; }

        template <typename T> requires (std::is_arithmetic_v<T>)
        bool Time<T>::operator>(const Time& other) const
        { return m_duration > other.m_duration; }

        template <typename T> requires (std::is_arithmetic_v<T>)
        bool Time<T>::operator<(const Time& other) const
        { return m_duration < other.m_duration; }

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T> Time<T>::operator+(const Time& other) const
        {
	        Time tmp = *this;
	        tmp.m_duration += other.m_duration;
	        return tmp;
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T> Time<T>::operator-(const Time& other) const
        {
	        Time tmp = *this;
	        tmp.m_duration -= other.m_duration;
	        return tmp;
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T>& Time<T>::operator+=(const Time& other)
        {
	        m_duration += other.m_duration;
	        return *this;
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T>& Time<T>::operator-=(const Time& other)
        {
	        m_duration -= other.m_duration;
	        return *this;
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T> Time<T>::operator*(value_type factor) const
        {
	        Time tmp = *this;
	        tmp.m_duration *= factor;
	        return tmp;
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T> Time<T>::operator/(value_type divider) const
        {
	        if (divider == static_cast<value_type>(0))
		        throw SpeExcept<std::out_of_range>("impossible to divide by 0");
	        Time tmp = *this;
	        tmp.m_duration /= divider;
	        return tmp;
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T>& Time<T>::operator*=(value_type factor)
        {
	        m_duration *= factor;
	        return *this;
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T>& Time<T>::operator/=(value_type divider)
        {
	        if (divider == static_cast<value_type>(0))
		        throw SpeExcept<std::out_of_range>("impossible to divide by 0");
	        m_duration /= divider;
	        return *this;
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        template <RatioType Ratio>
        Time<T> Time<T>::FromValue(value_type duration)
        {
	        auto durationNano = std::chrono::duration_cast<duration_type>(std::chrono::duration<value_type, Ratio>(duration));
	        return Time(durationNano);
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        duration_type Time<T>::Duration() const
        { return m_duration; }

        template <typename T> requires (std::is_arithmetic_v<T>)
        template <RatioType Ratio>
        T Time<T>::AsRatio() const
        { return std::chrono::duration<value_type, Ratio>(m_duration).count(); }

        template <typename T> requires (std::is_arithmetic_v<T>)
        Time<T>::Time(value_type nanoVal):m_duration(nanoVal)
        {}

        template<typename T> requires(std::is_arithmetic_v<T>)
        class Timer
        {
        public:
            using value_type = T;
            using time_type = Time<value_type>;
        public:
            Timer(const time_point_type& time = Now(), bool isActive = true);
            Timer(const Timer& other) = default;
            Timer(Timer&& other) noexcept = default;
            ~Timer() = default;
            Timer& operator=(const Timer&) = default;
            Timer& operator=(Timer&&) noexcept = default;
        public:
            time_type Pause();

            time_type Resume();

            time_type Reset();

            [[nodiscard]] time_type GetTime() const;

            [[nodiscard]] bool IsPaused() const;

        private:
            time_point_type m_start;
            bool m_isPaused;
            time_type m_stopTime;
        };

        template <typename T> requires (std::is_arithmetic_v<T>)
        Timer<T>::Timer(const time_point_type& time, bool isActive): m_start(time),m_isPaused(!isActive),m_stopTime(time_type{})
        {}

        template <typename T> requires (std::is_arithmetic_v<T>)
        typename Timer<T>::time_type Timer<T>::Pause()
        {
	        time_type time = GetTime();
	        if (!m_isPaused)
		        m_stopTime = time_type(m_start, Now());
	        m_isPaused = true;
	        return time;
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        typename Timer<T>::time_type Timer<T>::Resume()
        {
	        time_type time = GetTime();
	        m_isPaused = false;
	        return time;
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        typename Timer<T>::time_type Timer<T>::Reset()
        {
	        time_type time = GetTime();
	        m_start = Now();
	        return time;
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        typename Timer<T>::time_type Timer<T>::GetTime() const
        {
	        if (m_isPaused)
		        return m_stopTime;
	        return time_type(m_start, Now());
        }

        template <typename T> requires (std::is_arithmetic_v<T>)
        bool Timer<T>::IsPaused() const
        {
	        return m_isPaused;
        }
    }
}




#endif
