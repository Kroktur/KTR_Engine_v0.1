#include <functional>

#include <iostream>
#include <map>
#include "Tools/META/KTR_MetaTools.h"
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


#include <set>

#include "Containers/KTR_EventBus.h"
#include "Containers/KTR_HashSet.h"

enum  test : std::uint32_t
{
	toto = 1 << 0,
	tata = 1 << 1,
	titi = 1 << 2
};

REGISTER_RTTI_TYPE(KTR::Sparse<std::uint32_t>);
REGISTER_RTTI_TYPE(int);

struct TestTag{};


KTR_DEFINE_HAS_MEMBER_VALUE(has_member_totorig, totorig);

using  TestData = std::uint32_t;
struct TestAccessor
{
	using handler = KTR::CACHE::Handle<TestTag, TestData, TestAccessor>;
	static void Test()
	{
		std::uint64_t toto = KTR::HASH::FNV_1A_type<handler>::Hash(handler{});
	}
};



struct TOTO
{
	static constexpr int totorig = 3;
};

struct FnPtr
{
	void testFunc(int)
	{
		std::cout << "function called succesful\n";
	}

};

namespace KTR
{
	namespace HASH
	{
		template<typename	 R,typename... Args, typename HashBits> requires((std::is_same_v<HashBits, std::uint32_t> || std::is_same_v<HashBits, std::uint64_t>))
			struct FNV_1A<R(*)(Args...), HashBits>
		{
			using hash_type = R(*)(Args...);
			using FNV_1A_INFO_type = FNV_1A_INFO<HashBits>;
			using return_hash_type = typename FNV_1A_INFO_type::return_hash_type;
			[[nodiscard]] static constexpr return_hash_type Hash(hash_type val) {
				return_hash_type hash = FNV_1A_INFO_type::Init();
				const auto addr = reinterpret_cast<std::uintptr_t>(val);

				for (size_t i = 0; i < sizeof(hash_type); ++i)
				{
					FNV_1A_INFO_type::ComputeByte(hash, static_cast<std::uint8_t>(addr >> (i * CHAR_BIT)));
				}
				return hash;
			}
			[[nodiscard]] constexpr return_hash_type operator()(hash_type val) const {
				return FNV_1A::Hash(val);
			}
		};


	}
}




struct TestEvent
{};



struct Listner1
{
	Listner1()
	{
		
	}
	~Listner1()
	{
	
	}
	static void TOTO(const TestEvent&)
	{
		std::cout << "event on staticTOTO" << std::endl;
	}
	void test(const TestEvent&)
	{
		std::cout << "event on Listner1" << std::endl;
	}
};



struct Listner2
{
	Listner2()
	{
		
	}
	~Listner2()
	{
		
	}
	void test(const TestEvent&)
	{
		std::cout << "event on Listner2" << std::endl;
	}
};

struct ObjListener
{};
struct StaticCallBack
{};

struct RunTimeCallBack
{};



struct EventBucket
{
	std::vector < ObjListener > list;
	std::vector<StaticCallBack> list2;
	std::vector<RunTimeCallBack> list3;

};

struct EventBus2p0
{
	template<typename EventType,typename T, void (T::* fn) (const EventType&)>
	void SubScribe(T* obj)
	{
		std::uint64_t typeId = KTR::RTTI::Counter::GetId<EventType>();
		if (!m_map.Has(typeId))
			m_map[typeId] = EventBucket{};
		EventBucket& bucket = m_map[typeId];

 	}


	template<typename EventType>
	void Notify(const EventType& event)
	{
		std::uint64_t typeId = KTR::RTTI::Counter::GetId<EventType>();
		if (!m_map.Has(typeId))
			return;
		EventBucket& bucket = m_map[typeId];
		// notify 
	}

	KTR::HashMap<std::uint64_t, EventBucket> m_map;
};

struct testalacon
{
	template<typename T>
	static void toto()
	{
		
	}
};


/// ECS
#include <type_traits>

template<typename T>
concept EcsEntityType = requires(T t)
{
	std::is_arithmetic_v<T>;
};

namespace KTR
{


	template<typename T, EcsEntityType Entity>
	struct EcsBucket
	{
		using component_type = T;
		using value_type = Entity;
		using sparse_type = KTR::Sparse<Entity>;
		using vector_type = std::vector<component_type>;
		using iterator_type = vector_type::iterator;
		using const_iterator_type = vector_type::const_iterator;
	public:
		EcsBucket() = default;
		EcsBucket(const EcsBucket&) = default;
		EcsBucket(EcsBucket&&) noexcept = default;
		~EcsBucket() = default;
		EcsBucket& operator=(const EcsBucket&) = default;
		EcsBucket& operator=(EcsBucket&&) noexcept = default;
		bool operator==(const EcsBucket&) const = default;
		bool operator!=(const EcsBucket&) const = default;
	public:
		void Add(const Entity e)
		{
			m_sparse.Add(e);
			m_data.push_back(component_type{});
		}
		void Add(const Entity e, component_type&& component)
		{
			m_sparse.Add(e);
			m_data.push_back(std::move(component));
		}

		[[nodiscard]] bool Has(const Entity e) const
		{
			return m_sparse.Has(e);
		}

		void Remove(const Entity e)
		{
			value_type index = static_cast<value_type>(m_sparse.template At<Storage::SPARSE>(e));
			m_sparse.Remove(e);
			m_data[index] = std::move(m_data.back());
			m_data.pop_back();
		}

		void Reserve(const value_type size)
		{
			m_data.reserve(size);
			m_sparse.template Reserve<KTR::Storage::DENSE>(size);
		}
		[[nodiscard]] component_type& Get(const value_type e)
		{
			return m_data[m_sparse.template At<Storage::SPARSE>(e)];
		}
		[[nodiscard]] const component_type& Get(const value_type e) const
		{
			return m_data[m_sparse.template At<Storage::SPARSE>(e)];
		}
		[[nodiscard]] size_t Size() const
		{
			return m_data.size(); 
		}

		[[nodiscard]] vector_type GetAllCopy() const
		{
			return m_data;
		}

		// dangerous for multithread
		[[nodiscard]] vector_type& GetAll() const
		{
			return m_data;
		}

		[[nodiscard]] iterator_type begin()
		{
			return m_data.begin();
		}
		[[nodiscard]] iterator_type end()
		{
			return m_data.end();
		}
		
		[[nodiscard]] const_iterator_type begin() const
		{
			return m_data.begin();
		}
		[[nodiscard]] const_iterator_type end() const
		{
			return m_data.end();
		}

		[[nodiscard]] const_iterator_type CBegin() const
		{
			return m_data.cbegin();
		}
		[[nodiscard]] const_iterator_type CEnd() const
		{
			return m_data.cend();
		}
	private:
		sparse_type m_sparse;
		vector_type m_data;
	};

	struct ECSTagRTTI
	{
		using rtti_tag_type = void;
	};

	template< EcsEntityType Entity>
	struct ECSBucketEntry
	{
		void* ptr;
		void(*destroy)(void*);
		void(*destroyEntity)(void*, const Entity e);
	};
	template<EcsEntityType Entity>
	struct ECSRegistry;

	template<EcsEntityType Entity>
	struct ECSRegistryCreate
	{
	public:
		ECSRegistryCreate() = default;
		~ECSRegistryCreate()
		{
			for (auto& it : m_buckets)
				if (it.second.ptr != nullptr)
					it.second.destroy(it.second.ptr);
		}
		ECSRegistryCreate(const ECSRegistryCreate&) = delete;
		ECSRegistryCreate(ECSRegistryCreate&&) noexcept = default;
		ECSRegistryCreate& operator=(const ECSRegistryCreate&) = delete;
		ECSRegistryCreate& operator=(ECSRegistryCreate&&) noexcept = default;
	public:
		using key_type = std::uint64_t;
		using value_type = ECSBucketEntry<Entity>;
		using rtti_flag_type = ECSTagRTTI;
		friend ECSRegistry<Entity>;
		using map_type = HashMap<key_type, value_type>;
		using rtti_type = RTTI::DedicatedCounter<rtti_flag_type>;
		template<typename List> requires(Meta::is_type_list_v<List>)
			void Register()
		{
			auto lb = [&]<typename EventType>() {

				key_type id = rtti_type::GetId<EventType>();
				if (m_buckets.Has(id))
					return;

				EcsBucket<EventType, Entity>* bucket = new EcsBucket<EventType,Entity>();
				m_buckets.Add(id, {});
				m_buckets[id].ptr = static_cast<void*>(bucket);
				m_buckets[id].destroy = [](void* _ptr)
					{
						delete static_cast <EcsBucket<EventType, Entity>*>(_ptr);
					};
				m_buckets[id].destroyEntity = [](void* _ptr, const Entity e)
				{
						auto bucket = static_cast <EcsBucket<EventType, Entity>*>(_ptr);
						if (bucket->Has(e))
							bucket->Remove(e);
				};
			};

			Meta::RunTime<List>::for_each(lb);
		}
	private:
		map_type m_buckets;
	};

	template<EcsEntityType Entity>
	struct ECSRegistry
	{
	public:
		ECSRegistry() = default;
		ECSRegistry(ECSRegistryCreate<Entity>&& creator) noexcept : m_buckets(std::move(creator.m_buckets))
		{
		}
		~ECSRegistry()
		{
			
			for (auto& it : m_buckets)
				if (it.second.ptr != nullptr)
					it.second.destroy(it.second.ptr);
		}
		ECSRegistry(const ECSRegistry&) = delete;
		ECSRegistry(ECSRegistry&&) noexcept = default;
		ECSRegistry& operator=(const ECSRegistry&) = delete;
		ECSRegistry& operator=(ECSRegistry&&) noexcept = default;
	public:
		using entity_type = Entity;
		using key_type = std::uint64_t;
		using value_type = ECSBucketEntry<entity_type>;
		using rtti_flag_type = ECSTagRTTI;
		using rtti_type = RTTI::DedicatedCounter<rtti_flag_type>;
		using map_type = HashMap<key_type, value_type>;
		using pool_type = IdPool<entity_type>;
		using view_type = std::vector<entity_type>;
		using vector_type = std::vector<entity_type>;
	public:
		template<typename T>
		[[nodiscard]] EcsBucket<T,entity_type>& Acquire() const
		{
			std::cout << rtti_type::GetId<T>() << std::endl;
			return *static_cast<EcsBucket<T, entity_type>*>(m_buckets[rtti_type::GetId<T>()].ptr);
		}

		size_t Size() const
		{
			return m_pool.Size();
		}

		[[nodiscard]] entity_type AcquireEntity()
		{
			return m_pool.AcqId();
		}
		
		[[nodiscard]] vector_type AcquireEntities(entity_type count)
		{
			return m_pool.AcqIds(count);
		}

		void FreeEntity(const entity_type e)
		{
			m_pool.FreeId(e);
			for (auto it = m_buckets.begin() ; it != m_buckets.end() ; ++it)
				it->second.destroyEntity(it->second.ptr, e);
		}

		void FreeEntities(const vector_type& entities)
		{
			m_pool.FreeIds(entities);

			for (auto& it : m_buckets)
				for (const entity_type e : entities)
					it.second.destroyEntity(it.second.ptr, e);
		}

	private:
		map_type m_buckets;
		pool_type m_pool;
	};

}
KTR_ARGV_APPLICATION

	//KTR::TIME::Timer<float> timer(KTR::TIME::Now(),false);
	//
	//KTR::BitSet<std::uint32_t> bite;
	//KTR::Random rd(100);
	//std::cout << rd.getRandom(1, 4) << std::endl;
	//std::cout << rd.getRandom(1, 4) << std::endl;
	//KTR::Random rd2(100);
	//std::cout << rd2.getRandom(1, 4) << std::endl;
	//std::cout << rd2.getRandom(1, 4) << std::endl;
	//KTR::TIME::Time<float> time = KTR::TIME::Time<float>::FromValue<KTR::TIME::seconds_type>(10);

	//std::cout << time.AsRatio<KTR::TIME::seconds_type>() << std::endl;;
	//std::cout << time.AsRatio<KTR::TIME::milli_seconds_type>() << std::endl;;
	//std::cout << time.AsRatio<KTR::TIME::micro_seconds_type>() << std::endl;;
	//std::cout << time.AsRatio<KTR::TIME::nano_seconds_type>() << std::endl;;

	//using ptr_type = void(FnPtr::*) (int);

	//ptr_type fn = &FnPtr::testFunc;
	//
 //   

	//FnPtr ptrF;
	//(ptrF.*fn)(3);
	//KTR::TIME::Time<float> time2;
	//std::cout << time2.AsRatio<KTR::TIME::nano_seconds_type>() << std::endl;;
	//std::cout << timer.GetTime().AsRatio<KTR::TIME::seconds_type>() << std::endl;

	//KTR::Counter<float> counter(0,9);
	//counter.Loop(true);
	//counter += 20;
	//KTR::HashMap<int, std::string> map;
	//map.Add(0,"toto");
	//map.Add(1, "toto");
	//if (!map.Find(0) || map.Find(1000))
	//	throw;
	//map[0] = "toto3";
	//map.Remove(1);

	//auto before = KTR::Allocate<std::shared_ptr<int>>(1);

	//auto test = KTR::Allocate<std::weak_ptr<int>>(before);
	//KTR::DeAllocate(test);
	//test = KTR::Allocate<std::weak_ptr<int>>(before);
	//int testptr = 5;
	//auto& ptr  = KTR::Allocator<int>::ToRef(testptr);
	//ptr = 10;

	//std::cout << testptr << std::endl;
	//std::cout << counter.Get() << std::endl;

	//

	//KTR::CacheRegistry<std::string, int> reg;
	//auto handle = reg.Insert("toto", std::make_unique<int>(5));
	//std::cout << *reg.GetPtr(handle) << std::endl;
	//auto handletmp = reg.Swap("toto", std::make_unique<int>(5));
	//handle = reg.Update(handle).value();
	//if (!reg.IsValid(handle))
	//	throw;
	//
	//reg.Remove("toto");
	//auto handle2 = reg.Insert("toto", std::make_unique<int>(5));
	//if (reg.IsValid(handle) || !reg.IsValid(handle2))
	//	throw;
	//std::cout << handle2.GetData().gen << std::endl;;


	//int tototestMap = 3;


	//KTR::HashMap<int*, std::string> maptest;
	//maptest.Add(&tototestMap, "tototo");
	//int tototestmapres = KTR::HASH::FNV_1A<int*, std::uint32_t>::Hash(&tototestMap);
	//
	//KTR_STATIC_ASSERT(has_member_totorig_v<TOTO>, "toto")

	//	Listner1 l1;
	//	Listner1 l1_2;
	//	Listner2 l2;

	//	auto lambda = [&](const TestEvent&)
	//		{
	//			std::cout << "lambda called \n";
	//		};

	//std::set<int > intset;
	//intset.insert(3);
	//intset.contains(3);
	//std::cout << std::endl;

	using tp = KTR::Meta::typelist<size_t,int,float,double,std::string >;

	//KTR::EventBusCreate evb{};
	//evb.Register<tp>();
	//KTR::EventBus testBucket(std::move(evb));
	//testBucket.SubScribe< TestEvent , Listner1, &Listner1::test>(&l1);
	//testBucket.SubScribe<TestEvent,&Listner1::TOTO>();
	//testBucket.Subscribe<TestEvent>(&lambda);
	//testBucket.Notify<TestEvent>({  });
	//std::cout << std::endl;
	////testBucket.UnSubscribe(&l1);
	//testBucket.UnSubscribe<TestEvent,&Listner1::TOTO>();
	//testBucket.UnSubscribe<TestEvent>(&lambda);
	//testBucket.Notify<TestEvent>({  });


	KTR::ECSRegistryCreate<std::uint64_t> ecsvb;
	ecsvb.Register<tp>();
	KTR::ECSRegistry<std::uint64_t> regEcs(std::move(ecsvb));

	auto e = regEcs.AcquireEntity();
	


	KTR::EcsBucket<float, std::uint64_t>& buckint = regEcs.Acquire<float>();
	buckint.Add(e);
	KTR_DEBUG_ASSERT(buckint.Has(e))
		buckint.Get(e) = 5;
	std::cout << "value :" << buckint.Get(e) << std::endl;

	regEcs.FreeEntities({ e });

	std::vector<int> ids;

	KTR_APPLICATION_END


