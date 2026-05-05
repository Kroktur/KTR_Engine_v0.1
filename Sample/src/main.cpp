

#include <iostream>
#include <map>

#include "KTR_Application.h"
#include "Containers/KTR_CacheRegistry.h"
#include "Containers/HashMap/KTR_HashMap.h"
#include "Containers/KTR_Sparse.h"
#include "Tools/KTR_RTTI.h"

#include "Tools/KTR_BiteSet.h"
#include "Tools/KTR_Random.h"
#include "Tools/KTR_Chrono.h"
#include "Tools/KTR_Counter.h"
#include "Tools/KTR_Allocator.h"

#include "Tools/KTR_Handler.h"

enum  test : std::uint32_t
{
	toto = 1 << 0,
	tata = 1 << 1,
	titi = 1 << 2
};

REGISTER_RTTI_TYPE(KTR::Sparse<std::uint32_t>);
REGISTER_RTTI_TYPE(int);

struct TestTag{};


using  TestData = std::uint32_t;
struct TestAccessor
{
	using handler = KTR::Handle<TestTag, TestData, TestAccessor>;
	static void Test()
	{
		std::uint64_t toto = KTR::HASH::FNV_1A_type<handler>::Hash(handler{});
	}
};



KTR_ARGV_APPLICATION
{
	KTR::TIME::Timer<float> timer(KTR::TIME::Now(),false);
	
	KTR::BitSet<std::uint32_t> bite;
	KTR::Random rd(100);
	std::cout << rd.getRandom(1, 4) << std::endl;;
	std::cout << rd.getRandom(1, 4) << std::endl;;
	KTR::Random rd2(100);
	std::cout << rd2.getRandom(1, 4) << std::endl;
	std::cout << rd2.getRandom(1, 4) << std::endl;
	KTR::TIME::Time<float> time = KTR::TIME::Time<float>::FromValue<KTR::TIME::seconds_type>(10);

	std::cout << time.AsRatio<KTR::TIME::seconds_type>() << std::endl;;
	std::cout << time.AsRatio<KTR::TIME::milli_seconds_type>() << std::endl;;
	std::cout << time.AsRatio<KTR::TIME::micro_seconds_type>() << std::endl;;
	std::cout << time.AsRatio<KTR::TIME::nano_seconds_type>() << std::endl;;


	KTR::TIME::Time<float> time2;
	std::cout << time2.AsRatio<KTR::TIME::nano_seconds_type>() << std::endl;;
	std::cout << timer.GetTime().AsRatio<KTR::TIME::seconds_type>() << std::endl;

	KTR::Counter<float> counter(0,9);
	counter.Loop(true);
	counter += 20;
	KTR::HashMap<int, std::string> map;
	map.Add(0,"toto");
	map.Add(1, "toto");
	if (!map.Find(0) || map.Find(1000))
		throw;
	map[0] = "toto3";
	map.Remove(1);

	auto before = KTR::Allocate<std::shared_ptr<int>>(1);

	auto test = KTR::Allocate<std::weak_ptr<int>>(before);
	KTR::DeAllocate(test);
	test = KTR::Allocate<std::weak_ptr<int>>(before);
	int testptr = 5;
	auto& ptr  = KTR::Allocator<int>::ToRef(testptr);
	ptr = 10;

	std::cout << testptr << std::endl;
	std::cout << counter.Get() << std::endl;



	KTR::CacheRegistry<std::string, int> reg;
	auto handle = reg.Insert("toto", std::make_unique<int>(5));
	std::cout << *reg.GetPtr(handle) << std::endl;;
	auto handletmp = reg.Swap("toto", std::make_unique<int>(5));
	handle = reg.Update(handle).value();
	if (!reg.IsValid(handle))
		throw;
	
	reg.Remove("toto");
	auto handle2 = reg.Insert("toto", std::make_unique<int>(5));
	if (reg.IsValid(handle) || !reg.IsValid(handle2))
		throw;
	std::cout << handle2.GetData().gen << std::endl;;


	//KTR::CashRegistry<std::string, int> reg;
	//reg.Reg("toto", std::make_unique<int>(5));
	//std::cout << *reg.GetPtr("toto");
	

	KTR_APPLICATION_END;

}
