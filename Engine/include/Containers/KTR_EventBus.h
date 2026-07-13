#ifndef KTR_EVENTBUS_F
#define KTR_EVENTBUS_F
#include <cstdint>
#include <vector>

#include "Tools/KTR_RTTI.h"
#include "Tools/META/KTR_TypeList.h"

namespace KTR
{


	template<typename EventType>
	struct BusBucket
	{
	public:
		using event_type = EventType;
	public:
		struct ObjSubScriber
		{
			using callback_t = void(*)(void*, const event_type&);
			void* objInstance;
			std::uint64_t typeId;
			callback_t callback;
		};
		template<typename T, void (T::* fn) (const event_type&)>
		void Subscribe(T* obj) 
		{
			ObjSubScriber oSb;
			oSb.typeId = RTTI::Counter::GetId<T>();
			oSb.objInstance = static_cast<void*>(obj);
			oSb.callback = [](void* obj, const event_type& event)
				{
					(static_cast<T*>(obj)->*fn)(event);
				};
			m_objSub.push_back(oSb);
		}
		template<typename T>
		void UnSubscribe(T* obj)
		{
			typename std::vector<ObjSubScriber>::iterator removeIt = std::remove_if(
				m_objSub.begin(),
				m_objSub.end(),
				[obj](const ObjSubScriber& s)
				{
					return s.objInstance == static_cast<void*>(obj) && s.typeId == RTTI::Counter::GetId<T>();
				});
			m_objSub.erase(removeIt, m_objSub.end());
		}
	private:
		std::vector<ObjSubScriber> m_objSub;
	public:
		struct StaticSubscriber
		{
			using callback_t = void(*)(const event_type&);
			std::uintptr_t fnId;
			callback_t callback;
		};
		template<void (*fn)(const event_type&)>
		void Subscribe()
		{
			StaticSubscriber sSb;
			sSb.fnId = reinterpret_cast<std::uintptr_t>(fn);
			sSb.callback = [](const event_type& event)
				{
					fn(event);
				};
			m_objStatic.push_back(sSb);
		}
		template<void (*fn)(const event_type&)>
		void UnSubscribe()
		{
			typename std::vector<StaticSubscriber>::iterator removeIt = std::remove_if(
				m_objStatic.begin(),
				m_objStatic.end(),
				[](const StaticSubscriber& s)
				{
					return s.fnId == reinterpret_cast<std::uintptr_t>(fn);
				});
			m_objStatic.erase(removeIt, m_objStatic.end());
		}
	private:
		std::vector<StaticSubscriber> m_objStatic;
	public:
		struct RunTimeSubscriber
		{
			using callback_t = void(*)(void*, const event_type&);
			void* instance;
			callback_t callback;
		};

		template<typename T> requires(std::is_invocable_v<T, const event_type&>)
		void Subscribe(T* lambdaPtr) 
		{
			RunTimeSubscriber rSb;
			rSb.instance = static_cast<void*>(lambdaPtr);
			rSb.callback = [](void* instance, const event_type& event)
				{
					(*static_cast<T*>(instance))(event);
				};
			m_objRunTime.push_back(rSb);
		}
		template<typename T> requires(std::is_invocable_v<T,const event_type&>)
		void UnSubscribe(T* lambdaPtr)
		{
			typename std::vector<RunTimeSubscriber>::iterator removeIt = std::remove_if(
				m_objRunTime.begin(),
				m_objRunTime.end(),
				[lambdaPtr](const RunTimeSubscriber& s)
				{
					return s.instance == static_cast<void*>(lambdaPtr);
				});
			m_objRunTime.erase(removeIt, m_objRunTime.end());
		}
	private:
		std::vector<RunTimeSubscriber> m_objRunTime;

	public:
		void Notify(const event_type& event) const 
		{
			for (auto& it : m_objSub)
				it.callback(it.objInstance, event);
			for (auto& it : m_objStatic)
				it.callback(event);
			for (auto& it : m_objRunTime)
				it.callback(it.instance, event);
		}
	};

	struct EventBusTagRTTI
	{
		using rtti_tag_type = void;
	};
	
	struct BucketEntry
	{
		void* ptr = nullptr;
		void(*destroy)(void*) = nullptr;
	};

	struct EventBus;

	struct EventBusCreate
	{
		friend EventBus;
		using rtti_type = RTTI::DedicatedCounter<EventBusTagRTTI>;
		template<typename List> requires(Meta::is_type_list_v<List>)
			void Register()
		{
			auto lb = [&]<typename EventType>() {
			
				std::uint64_t id = rtti_type::GetId<EventType>();
				if (m_buckets.Has(id))
					return;
				
				BusBucket<EventType>* bucket = new BusBucket<EventType>();
				m_buckets.Add(id, {});
				m_buckets[id].ptr = static_cast<void*>(bucket);
				m_buckets[id].destroy = [](void* _ptr)
					{
						delete static_cast <BusBucket<EventType>*>(_ptr);
					};
			};

			Meta::RunTime<List>::for_each(lb);
		}
		private:
			HashMap<std::uint64_t, BucketEntry> m_buckets;
	};

	struct EventBus
	{
	public:
		EventBus() = default;
		EventBus(EventBusCreate&& creator) noexcept : m_buckets(std::move(creator.m_buckets)){}
		~EventBus()
		{
			for (auto& it : m_buckets)
				if (it.second.ptr != nullptr)
					it.second.destroy(it.second.ptr);
		}
		EventBus(const EventBus&) = delete;
		EventBus(EventBus&&) noexcept = default;
		EventBus& operator=(const EventBus&) = delete;
		EventBus& operator=(EventBus&&) noexcept = default;
	public:
		using rtti_type = RTTI::DedicatedCounter<EventBusTagRTTI>;
	public:

		template<typename EventType, typename T, void (T::* fn) (const EventType&)>
		void SubScribe(T* obj)
		{
			std::uint64_t id = GetID<EventType>();
			if (id == std::numeric_limits < std::uint64_t>::max())
			{
				//TODO log
				return;
			}
			static_cast<BusBucket<EventType>*>(m_buckets[id].ptr)-> template Subscribe<T, fn>(obj);
		}

		template<typename EventType, void (*fn)(const EventType&)>
		void SubScribe()
		{
			std::uint64_t id = GetID<EventType>();
			if (id == std::numeric_limits < std::uint64_t>::max())
			{
				//TODO log
				return;
			}
			static_cast<BusBucket<EventType>*>(m_buckets[id].ptr)->template Subscribe<fn>();
		}

		template<typename EventType, typename T> requires(std::is_invocable_v<T, const EventType&>)
			void Subscribe(T* lambdaPtr)
		{
			std::uint64_t id = GetID<EventType>();
			if (id == std::numeric_limits < std::uint64_t>::max())
			{
				//TODO log
				return;
			}

			static_cast<BusBucket<EventType>*>(m_buckets[id].ptr)->template Subscribe<T>(lambdaPtr);
		}

		template<typename EventType, typename T>
		void UnSubscribe(T* obj)
		{
			std::uint64_t id = GetID<EventType>();
			if (id == std::numeric_limits < std::uint64_t>::max())
			{
				//TODO log
				return;
			}
			static_cast<BusBucket<EventType>*>(m_buckets[id].ptr)->template  UnSubscribe<T>(obj);
		}
		template<typename EventType, typename T> requires(std::is_invocable_v<T, const EventType&>)
			void UnSubscribe(T* lambdaPtr)
		{
			std::uint64_t id = GetID<EventType>();
			if (id == std::numeric_limits < std::uint64_t>::max())
			{
				//TODO log
				return;
			}
			static_cast<BusBucket<EventType>*>(m_buckets[id].ptr)->template  UnSubscribe<T>(lambdaPtr);
		}
		template<typename EventType, void (*fn)(const EventType&)>
		void UnSubscribe()
		{
			std::uint64_t id = GetID<EventType>();
			if (id == std::numeric_limits < std::uint64_t>::max())
			{
				//TODO log
				return;
			}
			static_cast<BusBucket<EventType>*>(m_buckets[id].ptr)->template  UnSubscribe<fn>();
		}
		template<typename EventType>
		void Notify(const EventType& event) const
		{
			std::uint64_t id = GetID<EventType>();
			if (id == std::numeric_limits < std::uint64_t>::max())
			{
				//TODO log
				return;
			}
			static_cast<BusBucket<EventType>*>(m_buckets[id].ptr)->Notify(event);
		}
	private:
		template<typename EventType>
		std::uint64_t GetID() const 
		{
			std::uint64_t id = rtti_type::GetId<EventType>();
			if (!m_buckets.Has(id))
				return std::numeric_limits < std::uint64_t >::max();
			return id;
		}

	private:
		HashMap<std::uint64_t, BucketEntry> m_buckets;
	};

}

#endif
//TODO passer avec une hash map et un register différé 