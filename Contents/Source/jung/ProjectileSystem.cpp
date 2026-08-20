#include "ProjectileSystem.h"
#include "ECS/Components/Components.h"
#include "ProjectileComponent.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "State/State.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::TickPhase::Update, tomato::RunMode::Game, ProjectileSystem)

using namespace tomato;

void ProjectileSystem::Update(SimContext& simCtx)
{
	auto& reg = simCtx.state->GetRegistry();
	auto view = reg.view<TransformComponent, MoveBetweenComponent, VelocityComponent, TargetComponent>();

	for (auto [e, transform, move, velocity, target] : view.each())
	{
		glm::vec3 toEnd = move.end - transform.GetWorldPosition();
		if (toEnd == glm::vec3{ 0 })
			continue;

		glm::vec3 moveDir = glm::normalize(velocity.velocity);

		if (glm::dot(moveDir, toEnd) <= 0.f)
		{
			transform.SetPosition(move.end);
			velocity.velocity = glm::vec3{ 0 };
		}
		else
			velocity.velocity = glm::normalize(toEnd) * velocity.horizontalSpeed;


		/*auto tEntity = GetEntityByUUID(reg, target.target);
		auto& targetTransform = reg.get<TransformComponent>(tEntity);

		glm::vec3 toTarget = targetTransform.GetWorldPosition() - transform.GetWorldPosition();
		toTarget.y = 0.f;

		if (!target.init)
		{
			target.init = true;
			target.dir = glm::normalize(toTarget);

			velocity.velocity.x = target.dir.x * velocity.horizontalSpeed;
			velocity.velocity.z = target.dir.z * velocity.horizontalSpeed;
			velocity.velocity.y = 0;
		}

		glm::vec3 moveDir = velocity.velocity;

		float dot = glm::dot(moveDir, toTarget);
		//std::cout << dot << '\n';

		if (dot > 0.f)
		{
			glm::vec3 dir = glm::normalize(toTarget);
			target.dir = dir;

			velocity.velocity.x = dir.x * velocity.horizontalSpeed;
			velocity.velocity.z = dir.z * velocity.horizontalSpeed;
		}
		else
		{
			//velocity.velocity = glm::vec3{ 0 };
			//std::cout << "pass\n";
		}
		//velocity.horizontalSpeed = 0.5f;*/
		
	}

}