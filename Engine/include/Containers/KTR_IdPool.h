#ifndef KTR_IDPOOL_F
#define KTR_IDPOOL_F

#include <cstdint>
#include <type_traits>
#include <stdexcept>
#include <limits>
#include "KTR_Sparse.h"
namespace KTR
{
	template<typename T,size_t allocCount> requires(std::is_integral_v<T> && allocCount > 0)
	class IdPool
	{
	public:
		using id_type = T;
		using container_type = KTR::Sparse<id_type>;
		using vector_type = std::vector<id_type>;
	public:
		IdPool();
		IdPool(const IdPool&) = default;
		IdPool(IdPool&&) noexcept= default;
		~IdPool() = default;
		IdPool& operator=(const IdPool&) = default;
		IdPool& operator=(IdPool&&) noexcept= default;
	public:
		[[nodiscard]] bool HasId(id_type id) const;
		[[nodiscard]] id_type AcqId();
		[[nodiscard]] size_t Size() const;
		[[nodiscard]] size_t MaxSize() const;
		void FreeId(id_type id);
		void Clear();
	private:
		void FillWithId();
		container_type m_sparse;
		vector_type m_freeId;
		id_type m_nextId;
	};

	template <typename T, size_t allocCount> requires(std::is_integral_v<T>&& allocCount > 0)
	IdPool<T, allocCount>::IdPool() : m_nextId(0)
	{ FillWithId(); }

	

	template <typename T, size_t allocCount> requires (std::is_integral_v<T> && allocCount > 0)
	bool IdPool<T, allocCount>::HasId(id_type id) const
	{
		return m_sparse.Has(id);
	}

	template <typename T, size_t allocCount> requires (std::is_integral_v<T> && allocCount > 0)
	typename IdPool<T, allocCount>::id_type IdPool<T, allocCount>::AcqId()
	{
		if (m_freeId.empty())
			FillWithId();
			
		id_type id = m_freeId.back();
		m_freeId.pop_back();
		m_sparse.Add(id);
		return id;
	}

	template <typename T, size_t allocCount> requires (std::is_integral_v<T> && allocCount > 0)
	size_t IdPool<T, allocCount>::Size() const 
	{
		return m_sparse.template Size<Storage::DENSE>();
	}

	template <typename T, size_t allocCount> requires (std::is_integral_v<T> && allocCount > 0)
	size_t IdPool<T, allocCount>::MaxSize() const
	{
		return m_nextId;
	}

	template <typename T, size_t allocCount> requires (std::is_integral_v<T> && allocCount > 0)
	void IdPool<T, allocCount>::FreeId(id_type id)
	{
		if (!m_sparse.Has(id))
			throw SpeExcept<std::out_of_range>("id not register");
		m_sparse.Remove(id);
		m_freeId.push_back(id);
	}

	template <typename T, size_t allocCount> requires (std::is_integral_v<T> && allocCount > 0)
	void IdPool<T, allocCount>::Clear()
	{
		m_nextId = 0;
		m_sparse.Clear();
		m_freeId.clear();
	}

	template <typename T, size_t allocCount> requires (std::is_integral_v<T> && allocCount > 0)
	void IdPool<T, allocCount>::FillWithId()
	{
		if (m_nextId > std::numeric_limits<id_type>::max() - (allocCount - 1))
			throw SpeExcept<std::out_of_range>("IdPool exhausted");

		size_t oldSize = m_freeId.size();
		m_freeId.resize(oldSize + allocCount);

		// if 0 start at 0 
		id_type start = m_nextId;
		for (size_t i = 0; i < allocCount; ++i)
			m_freeId[oldSize + i] = start +(allocCount - 1 - i);
		m_nextId += allocCount ;
	}
}

#endif
