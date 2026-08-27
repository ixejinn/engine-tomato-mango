#ifndef MANGO_ENTITYPOOL_H
#define MANGO_ENTITYPOOL_H

#include <entt/entt.hpp>
#include <vector>
#include <optional>
#include <cassert>
#include <type_traits>

#include "Utils/PassKey.h"
#include "State/StateFwd.h"
namespace tomato
{
	template<typename Traits>
	class EntityPool
	{
	public:
		EntityPool(const PassKey<State>& key, entt::registry& registry, std::size_t poolSize = 32)
			:registry_(registry), poolSize_(poolSize)
		{
			entities_.reserve(poolSize);
			Create(poolSize);
		}

		template<typename... Args>
		std::optional<entt::entity> Acquire(Args&&... args)
		{
			if (entities_.empty())
				return std::nullopt;

			const entt::entity entity = entities_.back();
			entities_.pop_back();

			Traits::Reset(registry_, entity, std::forward<Args>(args)...);

			return entity;
		}

		bool Release(entt::entity entity)
		{
			if (!Traits::Deactivate(registry_, entity))
				return false;

			entities_.push_back(entity);

			return true;
		}

		std::size_t GetActiveEmitterNum() const { return poolSize_ - entities_.size(); }

	private:
		void Create(std::size_t count)
		{
			for (std::size_t i = 0; i < count; ++i)
			{
				entt::entity entity = registry_.create();
				Traits::Assemble(registry_, entity);
				//Traits Tag?

				entities_.push_back(entity);
			}
		}

		entt::registry& registry_;

		std::size_t poolSize_;
		std::vector<entt::entity> entities_;
	};
}

#endif // !MANGO_ENTITYPOOL_H
