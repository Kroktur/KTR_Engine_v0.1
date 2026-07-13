#ifndef KTR_RTTI_F
#define KTR_RTTI_F

#include <atomic>
#include <string_view>
#include "Hash/KTR_Hasher.h"
#include "META/KTR_MetaTools.h"

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



KTR_DEFINE_HAS_MEMBER_TYPE(has_rtti_tag_type, rtti_tag_type)
namespace KTR
{


	namespace RTTI
	{
		struct Counter
		{
			template<typename T>
			[[nodiscard]] static std::uint64_t GetId();

		private:
			static std::atomic_uint64_t m_id;
		};

		template<typename TagT> requires (has_rtti_tag_type_v<TagT>)
		struct DedicatedCounter
		{
			using tag_type = TagT;
			template<typename T>
			[[nodiscard]] static std::uint64_t GetId();

		private:
			static std::atomic_uint64_t m_id;
		};

		
		template <typename T>
		std::uint64_t Counter::GetId()
		{
			static std::uint64_t id = m_id++;
			return id;
		}

		template <typename TagT>requires (has_rtti_tag_type_v<TagT>)
		template <typename T>
		std::uint64_t DedicatedCounter<TagT>::GetId()
		{

			static std::uint64_t id = m_id++;
			return id;
		}

		struct Hasher
		{
			template<RTTIType T>
			[[nodiscard]] static std::uint64_t GetId();
		};

		template <RTTIType T>
		std::uint64_t Hasher::GetId()
		{
			static std::string_view name = KTR::TypeName<T>::Name();
			static std::uint64_t hash = HASH::default_hash_type<std::string_view>::Hash(name);
			return hash;
		}

		template<typename T>
		struct Instance
		{
			Instance();
			[[nodiscard]] std::uint64_t GetInstance() const;
			[[nodiscard]] static std::uint64_t GetInstanceCount();

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
		std::atomic_uint64_t Instance<T>::m_lastInstance = 0;
	}
	inline std::atomic_uint64_t RTTI::Counter::m_id = 0;

	

	template<typename TagT> requires (has_rtti_tag_type_v<TagT>)
	inline std::atomic_uint64_t RTTI::DedicatedCounter<TagT>::m_id = 0;
}




#endif
