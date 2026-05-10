#ifndef KTR_METATOOLS_F
#define KTR_METATOOLS_F

namespace KTR
{
	template<typename T>
	struct type_container
	{
		using value_type = T;

		constexpr operator value_type() const { return value_type{}; }
		constexpr value_type operator()() const { return value_type{}; }
	};

	template<typename T>
	using type_container_t = typename type_container<T>::value_type;


	template<typename T,T data>
	struct data_container
	{
		using value_type = T;
		static constexpr value_type value = data;
		constexpr operator value_type() const { return value; }
		constexpr value_type operator()() const { return value; }
	};

	template<typename T, T data>
	using data_container_t = typename data_container<T, data>::value_type;
	template<typename T, T data>
	static constexpr data_container_t<T,data> data_container_v = data_container<T, data>::value;


	using true_type = data_container<bool, true>;
	using false_type = data_container<bool, false>;


	template<typename T>
	using alias_void_t = void;


	template<bool cond,typename T,typename T2>
	struct conditional;
	template<typename T,typename T2>
	struct conditional<true, T, T2> : type_container<T> {};
	template<typename T, typename T2>
	struct conditional<false, T, T2> : type_container<T2> {};

	template<bool cond, typename T, typename T2>
	using conditional_t = typename conditional<cond, T, T2>::value_type;

	

	template<bool cond, typename T = void>
	struct enable_if {};
	template<typename T>
	struct enable_if<true,T> : type_container<T> {};

	template<bool cond, typename T>
	using enable_if_t = enable_if<true, T>::value_type;
}


#define KTR_DEFINE_HAS_MEMBER_TYPE(trait_name, member_name)           \
    template<typename T, typename T2 = void>                          \
    struct trait_name : KTR::false_type {};                           \
    template<typename T>                                              \
    struct  trait_name<T, KTR::alias_void_t<                          \
        typename T::member_name>> : KTR::true_type {};                \
    template<typename T>                                              \
    static constexpr bool trait_name##_v = trait_name<T>::value;      \
	template<typename T>                                              \
	using trait_name##_t = trait_name<T>;


#define KTR_DEFINE_HAS_MEMBER_VALUE(trait_name, member_name)          \
    template<typename T, typename T2 = void>                          \
    struct trait_name : KTR::false_type {};                           \
    template<typename T>                                              \
    struct  trait_name<T, KTR::alias_void_t<                          \
        decltype(T::member_name)>> : KTR::true_type {};               \
    template<typename T>                                              \
    static constexpr bool trait_name##_v = trait_name<T>::value;      \
	template<typename T>                                              \
	using trait_name##_t = trait_name<T>;

#endif 
