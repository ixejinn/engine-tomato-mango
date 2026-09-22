#ifndef MANGO_ENTITYPOOL_H
#define MANGO_ENTITYPOOL_H

#include <entt/entt.hpp>
#include "Containers/EntityPoolTraits.h"
#include "Utils/PassKey.h"
#include "State/StateFwd.h"
#include "ECS/Components/ActiveTag.h"
#include "Prefab/Prefab.h"
#include "Utils/Logger.h"

namespace tomato
{
	namespace _entityPoolDetail
	{
		enum class EntryState
		{
			Free,
			Active
		};
	}

	template<EntityPoolTraits Traits>
	class EntityPool
	{
	private:
		/// Private component(ECS)
		struct EntryIndex { const uint32_t index; };

	public:
		EntityPool(const PassKey<State>& key, entt::registry& registry)
			:registry_(registry)
		{
			CreateEntities();
		}

		template<typename... Args>
		entt::entity Acquire(Args&&... args);

		bool Release(entt::entity entity);

		std::size_t GetActiveEntityCount() const { return Traits::N - freeEntityCount_; }

	private:
		void CreateEntities();

		entt::registry& registry_;

		struct Entry
		{
			entt::entity entity;
			_entityPoolDetail::EntryState state;
		};
		std::array<Entry, Traits::N> entries_;

		std::array<uint32_t, Traits::N> freeIndices_;
		uint32_t freeEntityCount_{Traits::N};
	};

	template<EntityPoolTraits Traits>
	template<typename... Args>
	entt::entity EntityPool<Traits>::Acquire(Args&&... args)
	{
		if (freeEntityCount_ == 0)
		{
			TMT_DEBUG << "Fulled entity pool - Return null";
			return entt::null;
		}

		const uint32_t idx = freeIndices_[--freeEntityCount_];

		Entry& entry = entries_[idx];
		entry.state = _entityPoolDetail::EntryState::Active;

		const entt::entity entity = entries_[idx].entity;
		registry_.emplace<ActiveTag>(entity);
		Traits::Reset(registry_, entity, std::forward<Args>(args)...);

		return entity;
	}

	template<EntityPoolTraits Traits>
	bool EntityPool<Traits>::Release(entt::entity entity)
	{
		auto* entryIdx = registry_.try_get<EntryIndex>(entity);
		if (!entryIdx)
		{
			TMT_DEBUG << "Attempt to return a different pool entity.";
			return false;
		}

		Entry& entry = entries_[entryIdx->index];
		entry.state = _entityPoolDetail::EntryState::Free;
		freeIndices_[freeEntityCount_++] = entryIdx->index;

		registry_.erase<ActiveTag>(entity);		// Remove ActiveTag fast.
		// registry_.remove<ActiveTag>(entity);	// Remove ActiveTag safely.
		if constexpr (HasDeactivate<Traits>)
		{
			if (!Traits::Deactivate(registry_, entity))
				return false;
		}

		return true;
	}

	template<EntityPoolTraits Traits>
	void EntityPool<Traits>::CreateEntities()
	{
		uint32_t idx = 0;
		for (auto& entry : entries_)
		{
			entry.state = _entityPoolDetail::EntryState::Free;

			entry.entity = Prefab::CreateBaseEntity(registry_, false, true, Traits::Name);
			Traits::Assemble(registry_, entry.entity);
			registry_.emplace<EntryIndex>(entry.entity, idx);

			freeIndices_[idx] = idx;
			++idx;
		}
	}
}

#endif // !MANGO_ENTITYPOOL_H
