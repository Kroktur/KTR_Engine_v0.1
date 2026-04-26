#ifndef KTR_SPARSE_F
#define KTR_SPARSE_F

#include <vector>
#include <type_traits>
#include "KTR_Assert.h"

//TODO doxygen doc

namespace KTR {

	enum class Storage : char
	{
		SPARSE,
		DENSE
	};

	template<typename T> requires(std::is_unsigned_v<T> && std::is_integral_v<T>)
	struct Sparse
	{
	public:
		using value_type = T;
		static constexpr value_type invalidValue = std::numeric_limits<value_type>::max();
		using vector_type = std::vector<value_type>;
		using const_iterator_type = vector_type::const_iterator;
	public:
		Sparse() = default;
		~Sparse() = default;
		Sparse(const Sparse&) = default;
		Sparse(Sparse&&) noexcept = default;
		Sparse& operator=(const Sparse&) = default;
		Sparse& operator=(Sparse&&) noexcept= default;
	public:
		[[nodiscard]] bool Has(value_type e) const;
		void Add(value_type e);
		void Remove(value_type e);

		template<Storage storage>
		[[nodiscard]] size_t Size() const;
		template<Storage storage>
		[[nodiscard]] size_t  Capacity() const;
		template<Storage storage>
		void Reserve(value_type size);

		template<Storage storage>
		[[nodiscard]] value_type At(value_type index) const;
		[[nodiscard]] value_type operator[](value_type index) const;

		[[nodiscard]] const vector_type& Dense() const;

		// std convention range loop
		[[nodiscard]] const_iterator_type begin() const;
		[[nodiscard]] const_iterator_type end() const;

		void Clear(); 
	private:
		void ResizeSparse(value_type e);
		vector_type m_sparse;
		vector_type m_dense;
	};

	template <typename T> requires (std::is_unsigned_v<T> && std::is_integral_v<T>)
	bool Sparse<T>::Has(value_type e) const
	{
		return m_sparse.size() > e && m_sparse[e] != invalidValue;
	}

	template <typename T> requires (std::is_unsigned_v<T> && std::is_integral_v<T>)
	void Sparse<T>::Add(value_type e)
	{
		if (m_sparse.size() <= e)
		{
			// may change the logic with smooth and smart resize
			ResizeSparse(e);
		}
		// check if the data is not invalid to avoid using has in inner function
		T& data = m_sparse[e];
		KTR_DEBUG_ASSERT(data == invalidValue && "entity already added")
		data = static_cast<value_type>(m_dense.size());
		m_dense.push_back(e);
	}

	template <typename T> requires (std::is_unsigned_v<T> && std::is_integral_v<T>)
	void Sparse<T>::Remove(value_type e)
	{
		// check if the data is invalid to avoid using has in inner function
		value_type& index = m_sparse[e];
		KTR_DEBUG_ASSERT(index != invalidValue && "entity not stored")

		value_type lastEntity = m_dense.back();

		m_dense[index] = lastEntity;
		m_dense.pop_back();

		m_sparse[lastEntity] = index;
		index = invalidValue;
	}

	template <typename T> requires (std::is_unsigned_v<T> && std::is_integral_v<T>)
	template <Storage storage>
	 size_t Sparse<T>::Size() const
	{
		if constexpr (storage == Storage::DENSE)
			return m_dense.size();
		else
			return m_sparse.size();
	}

	template <typename T> requires (std::is_unsigned_v<T> && std::is_integral_v<T>)
	template <Storage storage>
	size_t Sparse<T>::Capacity() const
	{
		if constexpr (storage == Storage::DENSE)
			return m_dense.capacity();
		else
			return m_sparse.capacity();
	}

	template <typename T> requires (std::is_unsigned_v<T> && std::is_integral_v<T>)
	template <Storage storage>
	void Sparse<T>::Reserve(value_type size)
	{
		if constexpr (storage == Storage::DENSE)
			 m_dense.reserve(size);
		else
			 m_sparse.reserve(size);
	}

	template <typename T> requires (std::is_unsigned_v<T> && std::is_integral_v<T>)
	template <Storage storage>
	typename Sparse<T>::value_type Sparse<T>::At(value_type index) const
	{
		if constexpr (storage == Storage::DENSE)
			return m_dense.at(index);
		else
			return m_sparse.at(index);
	}

	template <typename T> requires (std::is_unsigned_v<T> && std::is_integral_v<T>)
	typename Sparse<T>::value_type Sparse<T>::operator[](value_type index) const
	{
		return m_dense[index];
	}

	template <typename T> requires (std::is_unsigned_v<T> && std::is_integral_v<T>)
	const typename Sparse<T>::vector_type& Sparse<T>::Dense() const
	{
		return m_dense;
	}

	template <typename T> requires (std::is_unsigned_v<T> && std::is_integral_v<T>)
	typename Sparse<T>::const_iterator_type Sparse<T>::begin() const
	{
		return m_dense.begin();
	}

	template <typename T> requires (std::is_unsigned_v<T> && std::is_integral_v<T>)
	typename Sparse<T>::const_iterator_type Sparse<T>::end() const
	{
		return m_dense.end();
	}

	template <typename T> requires (std::is_unsigned_v<T>&& std::is_integral_v<T>)
		void Sparse<T>::Clear()
	{
		m_dense.clear();
		m_sparse.clear();
	}

	template <typename T> requires (std::is_unsigned_v<T> && std::is_integral_v<T>)
	void Sparse<T>::ResizeSparse(value_type e)
	{
		m_sparse.resize(static_cast<size_t>(e) + 1, invalidValue);
	}
}

#endif
