#include "WaveSystem.h"
#include "ECS/Components/Components.h"
#include "WaveComponent.h"
#include "WavePool.h"
#include "WaveColliderPool.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "State/State.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::TickPhase::Update, tomato::RunMode::Game, WaveSystem)

using namespace tomato;

void WaveSystem::Update(SimContext& simCtx)
{
	auto& reg = simCtx.state->GetRegistry();
#if 0
	auto waveView = reg.view<TransformComponent, WaveComponent, HierarchyComponent>();

	for (auto [e, transform, wave, hierarchy] : waveView.each())
	{
		if (transform.GetLocalScale().x >= wave.radius * 2.f)
		{
			transform.SetScale({ 0.f, 0.1f, 0.f });
			wave.startTick = 0;
		}

		if (wave.startTick == 0)
			wave.startTick = simCtx.tick;

		float x = transform.GetLocalScale().x + wave.speed * wave.radius;
		float z = transform.GetLocalScale().z + wave.speed * wave.radius;
		float y = transform.GetLocalScale().y;
		transform.SetScale({ x, y, z });

		// 충돌체 위치 업데이트
		for (auto child : hierarchy.children)
		{
			auto* target = reg.try_get<TargetComponent>(child);
			if (!target) continue;

			auto tEntity = GetEntityByUUID(reg, target->target);
			auto& targetTransform = reg.get<TransformComponent>(tEntity);
			auto& childTransform = reg.get<TransformComponent>(child);

			if (transform.GetLocalScale().x >= wave.radius * 2.f)
				childTransform.SetPosition(wave.origin);

			int64_t elapsed = simCtx.tick - wave.startTick; //최초 생성 후 지난 틱

			glm::vec3 toTarget = targetTransform.GetWorldPosition() - wave.origin;
			toTarget.y = 0.f;

			// 진행 거리(현재 파동의 반지름) = 경과 시간 * 파동 속도
			float radius = elapsed * (wave.speed * wave.radius / 2.f);
			glm::vec3 wavePoint = wave.origin + glm::normalize(toTarget) * radius;

			// 부모 wave scale 역보정
			glm::vec3 newScale = glm::vec3{ 0.1f, 0.5f, 0.1f } / transform.GetLocalScale();
			childTransform.SetScale({ newScale.x, 0.5f, newScale.z });

			glm::vec3 newpoint = wavePoint / transform.GetLocalScale();
			childTransform.SetPosition({ newpoint.x, 0.f, newpoint.z });

			glm::vec3 newRot = targetTransform.GetWorldPosition() - childTransform.GetWorldPosition();
			childTransform.SetQuaternion(glm::vec3{ 0.f, glm::normalize(newRot).y, 0.f });
		}
	}
#elif 1
	auto waveView = reg.view<TransformComponent, WaveComponent>();

	for (auto [e, transform, wave] : waveView.each())
	{
		if (!wave.active) continue; // change active tag?
		if (transform.GetLocalScale().x >= wave.radius * 2.f)
			reg.ctx().get<WavePool>().Release(e);

		if (wave.startTick == 0)
			wave.startTick = simCtx.tick;

		float x = transform.GetLocalScale().x + wave.speed * wave.radius;
		float z = transform.GetLocalScale().z + wave.speed * wave.radius;
		float y = transform.GetLocalScale().y;
		transform.SetScale({ x, y, z });

		//// 충돌체 위치 업데이트
		//for (auto child : hierarchy.children)
		//{
		//	auto* target = reg.try_get<TargetComponent>(child);
		//	if (!target) continue;

		//	auto tEntity = GetEntityByUUID(reg, target->target);
		//	auto& targetTransform = reg.get<TransformComponent>(tEntity);
		//	auto& childTransform = reg.get<TransformComponent>(child);

		//	if (transform.GetLocalScale().x >= wave.radius * 2.f)
		//		childTransform.SetPosition(wave.origin);

		//	int64_t elapsed = simCtx.tick - wave.startTick; //최초 생성 후 지난 틱

		//	glm::vec3 toTarget = targetTransform.GetWorldPosition() - wave.origin;
		//	toTarget.y = 0.f;

		//	// 진행 거리(현재 파동의 반지름) = 경과 시간 * 파동 속도
		//	float radius = elapsed * (wave.speed * wave.radius / 2.f);
		//	glm::vec3 wavePoint = wave.origin + glm::normalize(toTarget) * radius;

		//	// 부모 wave scale 역보정
		//	glm::vec3 newScale = glm::vec3{ 0.1f, 0.5f, 0.1f } / transform.GetLocalScale();
		//	childTransform.SetScale({ newScale.x, 0.5f, newScale.z });

		//	glm::vec3 newpoint = wavePoint / transform.GetLocalScale();
		//	childTransform.SetPosition({ newpoint.x, 0.f, newpoint.z });

		//	glm::vec3 newRot = targetTransform.GetWorldPosition() - childTransform.GetWorldPosition();
		//	childTransform.SetQuaternion(glm::vec3{ 0.f, glm::normalize(newRot).y, 0.f });
		//}
		auto colView = reg.view<TransformComponent, TargetComponent, WaveColliderComponent, WaveColliderTag>();
		for (auto [col, colTransform, target, waveCollider] : colView.each())
		{
			if (waveCollider.wave != e)
				continue;

			auto* target = reg.try_get<TargetComponent>(col);
				if (!target) continue;

			auto tEntity = GetEntityByUUID(reg, target->target);
			auto& targetTransform = reg.get<TransformComponent>(tEntity);

			if (wave.active == false || transform.GetLocalScale().x >= wave.radius * 2.f)
				reg.ctx().get<WaveColliderPool>().Release(col);
				//colTransform.SetPosition(wave.origin);

			int64_t elapsed = simCtx.tick - wave.startTick; //최초 생성 후 지난 틱

			glm::vec3 toTarget = targetTransform.GetWorldPosition() - wave.origin;
			toTarget.y = 0.f;

			// 진행 거리(현재 파동의 반지름) = 경과 시간 * 파동 속도
			float radius = elapsed * (wave.speed * wave.radius / 2.f);
			glm::vec3 wavePoint = wave.origin + glm::normalize(toTarget) * radius;

			glm::vec3 newpoint = wavePoint;
			//colTransform.SetPosition({ newpoint.x, transform.GetLocalPosition().y, newpoint.z});
			colTransform.SetPosition({ newpoint.x, 0.f, newpoint.z});

			// target 방향으로 법선 회전
			glm::vec3 newRot = glm::normalize(targetTransform.GetWorldPosition() - colTransform.GetWorldPosition());
			glm::quat rotation = glm::quatLookAt(newRot, glm::vec3(0, 1, 0));
			colTransform.SetQuaternion(rotation);
		}
	}
#endif
}
