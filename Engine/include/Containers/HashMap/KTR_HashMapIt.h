#ifndef KTR_HASHMAPITERATOR_F
#define KTR_HASHMAPITERATOR_F

#include <iterator>

namespace KTR
{

	template<typename KeyT, typename ValT, bool IsConst>
	class MapIterator
	{
	public:
		using meta_type = std::uint8_t;
		static constexpr uint8_t EMPTY = 0x80;
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::pair<KeyT, ValT>;
		using difference_type = std::ptrdiff_t;
		using meta_pointer_type = const  meta_type*;
		using reference = std::conditional_t<IsConst, const value_type&, value_type&>;
		using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;
		using begin_flag = bool;
	public:


		MapIterator(const MapIterator&) = default;
		MapIterator(MapIterator&&) = default;
		~MapIterator() = default;
		MapIterator& operator=(const MapIterator&) = default;
		MapIterator& operator=(MapIterator&&) = default;
		MapIterator(pointer current, meta_pointer_type currentMeta, pointer end) : m_current(current), m_currentMeta(currentMeta), m_end(end) {}
		MapIterator(pointer current, meta_pointer_type currentMeta, pointer end, begin_flag) : m_current(current), m_currentMeta(currentMeta), m_end(end)
		{
			Advance();
		}

	public:
		[[nodiscard]] reference operator*() const
		{
			return *m_current;
		}
		[[nodiscard]] pointer   operator->() const
		{
			return m_current;
		}
		MapIterator& operator++()
		{
			Advance();
			return *this;
		}
		[[nodiscard]] MapIterator  operator++(int)
		{
			MapIterator tmp = *this;
			Advance();
			return tmp;
		}
		[[nodiscard]] bool operator==(const MapIterator& other) const
		{
			return m_current == other.m_current;
		}
		[[nodiscard]] bool operator!=(const MapIterator& other) const
		{
			return !(*this == other);
		}
	private:

		void Advance()
		{
			while (m_current < m_end)
			{
				++m_current;
				++m_currentMeta;
				if (m_current == m_end)
					break;
				if ((*m_currentMeta & EMPTY) == 0)
					break;
			}
		}
		pointer m_current;
		meta_pointer_type m_currentMeta;
		pointer m_end;
	};
}
#endif
