#ifndef MANGO_ENTITYPOOL_H
#define MANGO_ENTITYPOOL_H

#include <entt/entt.hpp>
#include "Utils/PassKey.h"
#include "ECS/Components/ActiveTag.h"
#include "Prefab/Prefab.h"
#include "Utils/Logger.h"
#include "GameNetwork/Rollback/RollbackFwd.h"

namespace tomato
{
	namespace _entityPoolDetail
	{
		enum class EntryState
		{
			Free,
			Active
		};

		struct Entry
		{
			entt::entity entity;
			EntryState state;
		};
	}

	template<typename Traits>
	concept EntityPoolTraits = requires(entt::registry& registry, entt::entity entity)
	{
		{ Traits::Assemble(registry, entity) } -> std::same_as<void>;
		{ Traits::N } -> std::convertible_to<uint32_t>;
	};

	template<typename Traits, typename... Args>
	concept HasReset = requires(entt::registry& registry, entt::entity entity, Args&&... args)
	{
		{ Traits::Reset(registry, entity, std::forward<Args>(args)...) } -> std::same_as<void>;
	};

	template<typename Traits>
	concept HasDeactivate = requires(entt::registry& registry, entt::entity entity)
	{
		{ Traits::Deactivate(registry, entity) } -> std::same_as<bool>;
	};

	/**
	 * @brief Fixed-size, rollback-ready entity pool.
	 *
	 * @warning 롤백은 해당 풀의 SnapshotTimeline을 등록해야 가능하다.
	 *
	 * @tparam Traits 엔티티의 구성과 초기화 방식을 정의하는 타입. 아래의 static 멤버를 제공해야 한다.
	 * 엔티티에 기본으로 부착해야 하는 컴포넌트와 ActiveTag는 풀에서 관리한다.@n
	 * 필수:
	 * @code
	 * static constexpr uint32_t N;
	 * static void Assemble(entt::registry&, entt::entity);
	 * static void Reset(entt::registry&, entt::entity, Args...);
	 * @endcode
	 * 선택:
	 * @code
	 * static bool Deactivate(entt::registry&, entt::entity);
	 * @endcode
	 * - N: 풀의 크기. 생성 이후에는 변하지 않음
	 * - Assemble: 엔티티 생성 시 엔티티마다 한 번 호출하여 컴포넌트를 부착
	 * - Reset: Acquire 호출 시 얻은 엔티티 설정
	 * - Deactivate: Release 호출 시 반납할 엔티티 설정, false를 반환하면 엔티티를 파괴 후 재생성
	 */
	template<EntityPoolTraits Traits>
	class EntityPool
	{
	private:
		/// Private component(ECS)
		struct EntryIndex { const uint32_t index; };

	public:
		static EntityPool& EmplaceInContext(entt::registry& registry)
		{
			if (auto* pool = registry.ctx().find<EntityPool>())
			{
				TMT_DEBUG << "Already contains entity pool.";
				return *pool;
			}

			return registry.ctx().emplace<EntityPool>(PassKey<EntityPool>(), registry);
		}

		EntityPool(const PassKey<EntityPool>& key, entt::registry& registry)
		{
			CreateEntities(registry);
		}

		EntityPool(const EntityPool&) = delete;
		EntityPool& operator=(const EntityPool&) = delete;
		EntityPool(EntityPool&&) = delete;
		EntityPool& operator=(EntityPool&&) = delete;

		template<typename... Args>
		requires HasReset<Traits, Args...>
		entt::entity Acquire(entt::registry& registry, Args&&... args);

		bool Release(entt::registry& registry, entt::entity entity);

		std::size_t GetActiveEntityCount() const { return Traits::N - freeEntityCount_; }

	private:
		void CreateEntities(entt::registry& registry);

		std::array<_entityPoolDetail::Entry, Traits::N> entries_;

		std::array<uint32_t, Traits::N> freeIndices_;
		uint32_t freeEntityCount_{Traits::N};

		friend class SnapshotTimeline<EntityPool>;
	};

	template<EntityPoolTraits Traits>
	template<typename... Args>
	requires HasReset<Traits, Args...>
	entt::entity EntityPool<Traits>::Acquire(entt::registry& registry, Args&&... args)
	{
		if (freeEntityCount_ == 0)
		{
			TMT_DEBUG << "Fulled entity pool - Return null";
			return entt::null;
		}

		const uint32_t idx = freeIndices_[--freeEntityCount_];

		_entityPoolDetail::Entry& entry = entries_[idx];
		entry.state = _entityPoolDetail::EntryState::Active;

		const entt::entity entity = entries_[idx].entity;
		registry.emplace<ActiveTag>(entity);
		Traits::Reset(registry, entity, std::forward<Args>(args)...);

		return entity;
	}

	template<EntityPoolTraits Traits>
	bool EntityPool<Traits>::Release(entt::registry& registry, entt::entity entity)
	{
		auto* entryIdx = registry.try_get<EntryIndex>(entity);
		if (!entryIdx)
		{
			TMT_DEBUG << "Attempt to return a different pool entity.";
			return false;
		}

		_entityPoolDetail::Entry& entry = entries_[entryIdx->index];
		entry.state = _entityPoolDetail::EntryState::Free;
		freeIndices_[freeEntityCount_++] = entryIdx->index;

		registry.erase<ActiveTag>(entity);		// Remove ActiveTag fast.
		// registry_.remove<ActiveTag>(entity);	// Remove ActiveTag safely.
		if constexpr (HasDeactivate<Traits>)
		{
			if (!Traits::Deactivate(registry, entity))
			{
				uint32_t idx = registry.get<EntryIndex>(entity).index;
				registry.destroy(entry.entity);

				entry.entity = Prefab::CreateBaseEntity(registry, false, true, Traits::Name);
				Traits::Assemble(registry, entry.entity);
				registry.emplace<EntryIndex>(entry.entity, idx);
			}
		}

		return true;
	}

	template<EntityPoolTraits Traits>
	void EntityPool<Traits>::CreateEntities(entt::registry& registry)
	{
		uint32_t idx = 0;
		for (auto& entry : entries_)
		{
			entry.state = _entityPoolDetail::EntryState::Free;

			entry.entity = Prefab::CreateBaseEntity(registry, false, true, Traits::Name);
			Traits::Assemble(registry, entry.entity);
			registry.emplace<EntryIndex>(entry.entity, idx);

			freeIndices_[idx] = idx;
			++idx;
		}
	}
}

#endif // !MANGO_ENTITYPOOL_H
