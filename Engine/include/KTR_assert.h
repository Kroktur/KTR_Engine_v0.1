#ifndef KTR_ASSERT_F
#define KTR_ASSERT_F

#include "KTR_Congif_Compiler.h"
#include <cassert>
#include <type_traits>


template<typename T>
concept ExceptCrtpType =  requires(T t)
{
	{ t.WhatImpl() }-> std::same_as<typename T::msg_type>;
};

namespace KTR
{
	#define KTR_STATIC_ASSERT(cond,msg) static_assert(cond,msg);
	#define KTR_ASSERT(cond) assert(cond)
	#define KTR_DEBUG_ASSERT(cond)  \
	if constexpr (KTR::debugEnable) \
		{						    \
			KTR_ASSERT(cond);       \
		}                           \
	else						    \
		((void)0);


	struct BaseExcept
	{
		using msg_type = const char*;
		static constexpr msg_type defaultValue = "Unknown exception";
		
		static [[nodiscard]] msg_type What();
	protected:
		BaseExcept() = default;
	};

	inline BaseExcept::msg_type BaseExcept::What()
	{
		return defaultValue;
	}

	template<typename Except> 
	struct SpeExcept : public BaseExcept
	{
		using msg_type = typename BaseExcept::msg_type;
		SpeExcept(msg_type msg);

		[[nodiscard]] msg_type What() const;
	private:
		msg_type m_msg;
	};

	template <typename Except>
	SpeExcept<Except>::SpeExcept(msg_type msg): BaseExcept(),m_msg(msg)
	{}

	template <typename Except>
	typename SpeExcept<Except>::msg_type SpeExcept<Except>::What() const
	{
		return m_msg;
	}


	template<typename Derived>
	struct CrtpExcept : public BaseExcept
	{
		using msg_type = typename BaseExcept::msg_type;

		[[nodiscard]] msg_type What() const;
	protected:
		CrtpExcept();
	};

	template <typename Derived>
	typename CrtpExcept<Derived>::msg_type CrtpExcept<Derived>::What() const
	{
		return static_cast<const Derived*>(this)->WhatImpl();
	}

	template <typename Derived>
	CrtpExcept<Derived>::CrtpExcept(): BaseExcept()
	{
		KTR_STATIC_ASSERT(std::is_base_of_v<CrtpExcept<Derived>, Derived>, "The type must inherit from crtp");
		KTR_STATIC_ASSERT(ExceptCrtpType<Derived>, "Must implement the function WhatImpl()");
	}
}

#endif
