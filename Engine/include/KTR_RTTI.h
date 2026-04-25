#ifndef KTR_RTTI_F
#define KTR_RTTI_F

#include <atomic>
#include <string_view>
#include "KTR_Hasher.h"


namespace KTR
{
	
	template<typename T>
	struct TypeName;
	
}

#define REGISTER_RTTI_TYPE(T)						\
namespace KTR										\
{													\
	template<>										\
	struct TypeName<T>								\
	{												\
		static constexpr std::string_view Name()    \
		{											\
			return #T;								\
		}											\
	};												\
}											     		

template<typename T>
concept RTTIType  = requires()
{
	{ KTR::TypeName<T>::Name() };
};



namespace KTR
{
	namespace RTTI
	{
		struct Counter
		{
			template<typename T>
			static std::uint64_t GetId();

		private:
			static std::atomic_uint64_t m_id;
		};

		template <typename T>
		std::uint64_t Counter::GetId()
		{
			static std::uint64_t id = m_id++;
			return id;
		}

		struct Hasher
		{
			template<RTTIType T>
			static std::uint64_t GetId();
		};

		template <RTTIType T>
		std::uint64_t Hasher::GetId()
		{
			static std::string_view name = KTR::TypeName<T>::Name();
			static std::uint64_t hash = HASH::FNV1aHash(name.data(), name.size());
			return hash;
		}

		template<typename T>
		struct Instance
		{
			Instance();
			std::uint64_t GetInstance() const;
			static std::uint64_t GetInstanceCount();

		private:
			std::uint64_t m_instanceID;
			static std::atomic_uint64_t m_lastInstance;
		};

		template <typename T>
		Instance<T>::Instance(): m_instanceID(m_lastInstance++)
		{}

		template <typename T>
		std::uint64_t Instance<T>::GetInstance() const
		{
			return m_instanceID;
		}

		template <typename T>
		std::uint64_t Instance<T>::GetInstanceCount()
		{
			return m_lastInstance;
		}

		template<typename T>
		std::atomic_uint64_t KTR::RTTI::Instance<T>::m_lastInstance = 0;
	}

}




#endif
