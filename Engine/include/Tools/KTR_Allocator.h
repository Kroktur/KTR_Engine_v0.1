#ifndef KTR_ALLOCATOR_F
#define KTR_ALLOCATOR_F

#include <memory>
namespace KTR
{

	template<typename T>
	struct Allocator
	{
		using value_type = T;
		using ptr_type = T;
		using raw_ptr_type = T;
		template<typename... Args>
		[[nodiscard]] static ptr_type Allocate(Args&&... args);
		static void DeAllocate(ptr_type& val);
		static void Set(ptr_type& ptr, const value_type& val);
		static void Set(ptr_type& ptr, value_type&& val);
		[[nodiscard]] static raw_ptr_type ToPtr(ptr_type& ptr);
		static void Give(ptr_type& lhs, ptr_type& rhs);
		static void Swap(ptr_type& lhs, ptr_type& rhs);
		static void Free(ptr_type& ptr);
		[[nodiscard]] static bool IsValid(const ptr_type& ptr);
	};
	template <typename T>
	template <typename ... Args>
	typename Allocator<T>::ptr_type Allocator<T>::Allocate(Args&&... args)
	{
		return T(std::forward<Args>(args)...);
	}
	template <typename T>
	void Allocator<T>::DeAllocate(ptr_type& val)
	{
		val = ptr_type{};
	}
	template <typename T>
	void Allocator<T>::Set(ptr_type& ptr, const value_type& val)
	{
		ptr = val;
	}
	template <typename T>
	void Allocator<T>::Set(ptr_type& ptr, value_type&& val)
	{
		ptr = std::move(val);
	}
	template <typename T>
	typename Allocator<T>::raw_ptr_type Allocator<T>::ToPtr(ptr_type& ptr)
	{
		return ptr;
	}
	template <typename T>
	void Allocator<T>::Give(ptr_type& lhs, ptr_type& rhs)
	{
		lhs = rhs;
		rhs = ptr_type{};
	}
	template <typename T>
	void Allocator<T>::Swap(ptr_type& lhs, ptr_type& rhs)
	{
		auto tmp = lhs;
		lhs = rhs;
		rhs = tmp;
	}
	template <typename T>
	void Allocator<T>::Free(ptr_type& ptr)
	{
		ptr = ptr_type{};
	}
	template <typename T>
	 bool Allocator<T>::IsValid(const ptr_type& ptr)
	{
		return true;
	}
	template<typename T>
	using raw_type = T;
	template<typename T>
	using raw_ptr_type = T*;
	template<typename T>
	using unique_ptr_type = std::unique_ptr<T>;
	template<typename T>
	using shared_ptr_type = std::shared_ptr<T>;
	template<typename T>
	using weak_ptr_type = std::weak_ptr<T>;
}


template<typename T>
concept AllocatorType = requires()
{
	typename KTR::Allocator<T>::value_type;
	typename KTR::Allocator<T>::ptr_type;
	typename KTR::Allocator<T>::raw_ptr_type;
};

namespace KTR
{
	

	template<typename T>
	struct Allocator<T*>
	{
		using value_type = T;
		using ptr_type = T*;
		using raw_ptr_type = ptr_type;
		template<typename... Args>
		[[nodiscard]] static ptr_type Allocate(Args&&... args);
		static void DeAllocate(ptr_type& val);
		static void Set(ptr_type& ptr, const value_type& val);
		static void Set(ptr_type& ptr, value_type&& val);
		[[nodiscard]] static raw_ptr_type ToPtr(ptr_type& ptr);
		static void Give(ptr_type& lhs, ptr_type& rhs);
		static void Swap(ptr_type& lhs, ptr_type& rhs);
		static void Free(ptr_type& ptr);
		[[nodiscard]] static bool IsValid(const ptr_type& ptr);
	};


	template <typename T>
	template <typename ... Args>
	typename Allocator<T*>::ptr_type Allocator<T*>::Allocate(Args&&... args)
	{
		return new T(std::forward<Args>(args)...);
	}

	template <typename T>
	void Allocator<T*>::DeAllocate(ptr_type& val)
	{
		if (val)
		{
			delete val;
			val = nullptr;
		}
	}

	template <typename T>
	void Allocator<T*>::Set(ptr_type& ptr, const value_type& val)
	{
		*ptr = val;
	}

	template <typename T>
	void Allocator<T*>::Set(ptr_type& ptr, value_type&& val)
	{
		*ptr = std::move(val);
	}

	template <typename T>
	typename Allocator<T*>::raw_ptr_type Allocator<T*>::ToPtr(ptr_type& ptr)
	{
		return ptr;
	}

	template <typename T>
	void Allocator<T*>::Give(ptr_type& lhs, ptr_type& rhs)
	{
		lhs = rhs;
		rhs = nullptr;
	}

	template <typename T>
	void Allocator<T*>::Swap(ptr_type& lhs, ptr_type& rhs)
	{
		ptr_type tmp = std::move(lhs);
		lhs = std::move(rhs);
		rhs = std::move(tmp);
	}

	template <typename T>
	void Allocator<T*>::Free(ptr_type& ptr)
	{
		ptr = nullptr;
	}

	template <typename T>
	bool Allocator<T*>::IsValid(const ptr_type& ptr)
	{
		return ptr != nullptr;
	}

	 template<typename T>
	 struct Allocator<std::unique_ptr<T>>
	 {
		 using value_type = T;
		 using ptr_type = std::unique_ptr<T>;
		 using raw_ptr_type = T*;
		 template< typename... Args>
		 [[nodiscard]] static ptr_type Allocate(Args&&... args);

		 static void DeAllocate(ptr_type& val);

		 [[nodiscard]] static raw_ptr_type ToPtr(ptr_type& ptr);

		 static void Set(ptr_type& ptr, const value_type& val);

		 static void Set(ptr_type& ptr, value_type&& val);

		 static void Give(ptr_type& lhs, ptr_type& rhs);

		 static void Swap(ptr_type& lhs, ptr_type& rhs);

		 static void Free(ptr_type& ptr);

		 [[nodiscard]] static bool IsValid(const ptr_type& ptr);
	 };


	 template <typename T>
	 template <typename ... Args>
	 typename Allocator<std::unique_ptr<T>>::ptr_type Allocator<std::unique_ptr<T>>::Allocate(Args&&... args)
	 {
		 return std::make_unique<T>(std::forward<Args>(args)...);
	 }

	 template <typename T>
	 void Allocator<std::unique_ptr<T>>::DeAllocate(ptr_type& val)
	 {
		 val.reset();
		 val = nullptr;
	 }

	 template <typename T>
	 typename Allocator<std::unique_ptr<T>>::raw_ptr_type Allocator<std::unique_ptr<T>>::ToPtr(ptr_type& ptr)
	 {
		 return ptr.get();
	 }

	 template <typename T>
	 void Allocator<std::unique_ptr<T>>::Set(ptr_type& ptr, const value_type& val)
	 {
		 *ptr = val;
	 }

	 template <typename T>
	 void Allocator<std::unique_ptr<T>>::Set(ptr_type& ptr, value_type&& val)
	 {
		 *ptr = std::move(val);
	 }

	 template <typename T>
	 void Allocator<std::unique_ptr<T>>::Give(ptr_type& lhs, ptr_type& rhs)
	 {
		 lhs = std::move(rhs);
		 rhs = nullptr;
	 }

	 template <typename T>
	 void Allocator<std::unique_ptr<T>>::Swap(ptr_type& lhs, ptr_type& rhs)
	 {
		 ptr_type tmp = std::move(lhs);
		 lhs = std::move(rhs);
		 rhs = std::move(tmp);
	 }

	 template <typename T>
	 void Allocator<std::unique_ptr<T>>::Free(ptr_type& ptr)
	 {
		 ptr.release();
	 }

	 template <typename T>
	 bool Allocator<std::unique_ptr<T>>::IsValid(const ptr_type& ptr)
	 {
		 return ptr != nullptr;
	 }

	 template<typename T>
	 struct Allocator<std::shared_ptr<T>>
	 {
		 using value_type = T;
		 using ptr_type = std::shared_ptr<T>;
		 using raw_ptr_type = T*;
		 template< typename... Args>
		 [[nodiscard]] static ptr_type Allocate(Args&&... args);

		 static void DeAllocate(ptr_type& val);

		 static void Set(ptr_type& ptr, const value_type& val);

		 static void Set(ptr_type& ptr, value_type&& val);

		 [[nodiscard]] static raw_ptr_type ToPtr(ptr_type& ptr);

		 static void Give(ptr_type& lhs, ptr_type& rhs);

		 static void Swap(ptr_type& lhs, ptr_type& rhs);

		 static void Free(ptr_type& ptr);

		 [[nodiscard]] static bool IsValid(const ptr_type& ptr);
	 };

	 template <typename T>
	 template <typename ... Args>
	 typename Allocator<std::shared_ptr<T>>::ptr_type Allocator<std::shared_ptr<T>>::Allocate(Args&&... args)
	 {
		 return std::make_shared<T>(std::forward<Args>(args)...);
	 }

	 template <typename T>
	 void Allocator<std::shared_ptr<T>>::DeAllocate(ptr_type& val)
	 {
		 val.reset();
		 val = nullptr;
	 }

	 template <typename T>
	 void Allocator<std::shared_ptr<T>>::Set(ptr_type& ptr, const value_type& val)
	 {
		 *ptr = val;
	 }

	 template <typename T>
	 void Allocator<std::shared_ptr<T>>::Set(ptr_type& ptr, value_type&& val)
	 {
		 *ptr = std::move(val);
	 }

	 template <typename T>
	 typename Allocator<std::shared_ptr<T>>::raw_ptr_type Allocator<std::shared_ptr<T>>::ToPtr(ptr_type& ptr)
	 {
		 return ptr.get();
	 }

	 template <typename T>
	 void Allocator<std::shared_ptr<T>>::Give(ptr_type& lhs, ptr_type& rhs)
	 {
		 lhs = std::move(rhs);
		 rhs = nullptr;
	 }

	 template <typename T>
	 void Allocator<std::shared_ptr<T>>::Swap(ptr_type& lhs, ptr_type& rhs)
	 {
		 ptr_type tmp = std::move(lhs);
		 lhs = std::move(rhs);
		 rhs = std::move(tmp);
	 }

	 template <typename T>
	 void Allocator<std::shared_ptr<T>>::Free(ptr_type& ptr)
	 {
		 ptr.reset();
	 }

	 template <typename T>
	 bool Allocator<std::shared_ptr<T>>::IsValid(const ptr_type& ptr)
	 {
		 return ptr != nullptr;
	 }

	 template<typename T>
	 struct Allocator<std::weak_ptr<T>>
	 {
		 using value_type = T;
		 using ptr_type = std::weak_ptr<T>;
		 using raw_ptr_type = T*;
		 template<typename  Arg>
		 [[nodiscard]] static ptr_type Allocate(Arg&& arg);

		 static void DeAllocate(ptr_type& val);

		 static void Give(ptr_type& lhs, ptr_type& rhs);

		 static void Swap(ptr_type& lhs, ptr_type& rhs);

		 static void Free(ptr_type& ptr);

		 [[nodiscard]] static bool IsValid(const ptr_type& ptr);
	 };


	 template <typename T>
	 template <typename Arg>
	 typename Allocator<std::weak_ptr<T>>::ptr_type Allocator<std::weak_ptr<T>>::Allocate(Arg&& arg)
	 {
		 return std::weak_ptr<T>(std::forward<Arg>(arg));
	 }

	 template <typename T>
	 void Allocator<std::weak_ptr<T>>::DeAllocate(ptr_type& val)
	 {
		 val.reset();
	 }

	 template <typename T>
	 void Allocator<std::weak_ptr<T>>::Give(ptr_type& lhs, ptr_type& rhs)
	 {
		 lhs = std::move(rhs);
		 rhs = ptr_type{};
	 }

	 template <typename T>
	 void Allocator<std::weak_ptr<T>>::Swap(ptr_type& lhs, ptr_type& rhs)
	 {
		 ptr_type tmp = std::move(lhs);
		 lhs = std::move(rhs);
		 rhs = std::move(tmp);
	 }

	 template <typename T>
	 void Allocator<std::weak_ptr<T>>::Free(ptr_type& ptr)
	 {
		 ptr.reset();
	 }

	 template <typename T>
	 bool Allocator<std::weak_ptr<T>>::IsValid(const ptr_type& ptr)
	 {
		 return !ptr.expired();
	 }

	template<AllocatorType T,typename ...Args>
	[[nodiscard]] static typename Allocator<T>::ptr_type Allocate(Args&&... args)
	{
		return Allocator<T>::Allocate(std::forward<Args>(args)...);
	}

	template<AllocatorType T>
	static void DeAllocate(T& value)
	{
		Allocator<T>::DeAllocate(value);
	}

	

}




#endif

