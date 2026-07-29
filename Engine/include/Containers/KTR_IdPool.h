#ifndef KTR_IDPOOL_F
#define KTR_IDPOOL_F

#include <cstdint>
#include <type_traits>
#include <stdexcept>
#include <limits>
#include "KTR_Sparse.h"
namespace KTR
{
	template<typename T> requires(std::is_integral_v<T>)
	class IdPool
	{
	public:
		using id_type = T;
		using container_type = KTR::Sparse<id_type>;
		using vector_type = std::vector<id_type>;
		static constexpr id_type invalidValue = std::numeric_limits<id_type>::max();

	public:
		IdPool();
		IdPool(const IdPool&) = default;
		IdPool(IdPool&&) noexcept= default;
		~IdPool() = default;
		IdPool& operator=(const IdPool&) = default;
		IdPool& operator=(IdPool&&) noexcept= default;
	public:
		[[nodiscard]] bool IsAlive(id_type id) const;
		[[nodiscard]] vector_type AcqIds(const id_type count);
		[[nodiscard]] id_type AcqId();
		[[nodiscard]] size_t Size() const;
		[[nodiscard]] size_t MaxSize() const;

		void FreeId(id_type id);
		void FreeIds(const vector_type& vec);
		void Clear();

	private:
		container_type m_sparse;
		vector_type m_recycling;
		id_type m_nextId;
	};


	template <typename T> requires (std::is_integral_v<T>)
	IdPool<T>::IdPool(): m_nextId(static_cast<id_type>(0))
	{
			
	}

	template <typename T> requires (std::is_integral_v<T>)
	bool IdPool<T>::IsAlive(id_type id) const
	{
		return id < m_nextId && !m_sparse.Has(id);
	}

	template <typename T> requires (std::is_integral_v<T>)
	typename IdPool<T>::vector_type IdPool<T>::AcqIds(const id_type count)
	{
		vector_type ids;
		for (id_type i = 0; i < count; ++i)
			ids.push_back(this->AcqId());
		return ids;
	}

	template <typename T> requires (std::is_integral_v<T>)
	typename IdPool<T>::id_type IdPool<T>::AcqId()
	{
		id_type e;
		if (!m_recycling.empty())
		{
			m_sparse.Remove(m_recycling.back());
			e = m_recycling.back();
			m_recycling.pop_back();
		}
		else
		{
			if (m_nextId == invalidValue)
				throw SpeExcept<std::out_of_range>("IdPool exhausted");

			e = m_nextId++;
		}
		return e;
	}

	template <typename T> requires (std::is_integral_v<T>)
	size_t IdPool<T>::Size() const
	{
		return m_nextId - m_recycling.size();
	}

	template <typename T> requires (std::is_integral_v<T>)
	size_t IdPool<T>::MaxSize() const
	{
		return m_nextId;
	}

	template <typename T> requires (std::is_integral_v<T>)
	void IdPool<T>::FreeId(id_type id)
	{
		m_recycling.push_back(id);
		m_sparse.Add(id);
	}

	template <typename T> requires (std::is_integral_v<T>)
	void IdPool<T>::FreeIds(const vector_type& vec)
	{
		for (const id_type e : vec)
			this->FreeId(e);
	}

	template <typename T> requires (std::is_integral_v<T>)
	void IdPool<T>::Clear()
	{
		m_sparse.Clear();
		m_recycling.clear();
		m_nextId = static_cast<id_type>(0);
	}
}

#endif
