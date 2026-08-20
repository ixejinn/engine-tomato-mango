#include "WaveSystem.h"
#include "ECS/Components/Components.h"
#include "WaveComponent.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "State/State.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::TickPhase::Update, tomato::RunMode::Game, WaveSystem)

using namespace tomato;

void WaveSystem::Update(SimContext& simCtx)
{
	auto& reg = simCtx.state->GetRegistry();
	auto waveView = reg.view<TransformComponent, WaveComponent>();
	entt::entity waveEntity = entt::null;
	float waveSpeed = 0.01f;
	for (auto [e, transform, wave] : waveView.each())
	{
		if (reg.all_of<TargetComponent>(e)) continue;

		waveEntity = e;
		if (transform.GetLocalScale().x >= wave.diameter)
			transform.SetScale({ 0.f, 0.1f, 0.f });

		float x = transform.GetLocalScale().x + waveSpeed * wave.diameter;
		float z = transform.GetLocalScale().z + waveSpeed * wave.diameter;
		float y = transform.GetLocalScale().y;
		transform.SetScale({ x, y, z });
	}

	auto view = reg.view<TransformComponent, WaveComponent, VelocityComponent, TargetComponent>();

	for (auto [e, transform, wave, velocity, target] : view.each())
	{
		auto tEntity = GetEntityByUUID(reg, target.target);
		auto& targetTransform = reg.get<TransformComponent>(tEntity);
		auto& waveTransform = reg.get<TransformComponent>(waveEntity);

		// startTick이 초기화되지 않았다면 지금 tick에서 처음 생성 -> 현재 틱으로 초기화
		if (waveTransform.GetLocalScale().x >= wave.diameter)
		{
			transform.SetPosition(wave.origin);
			wave.startTick = 0;
		}
		if(wave.startTick == 0)
			wave.startTick = simCtx.tick;

		int64_t elapsed = simCtx.tick - wave.startTick; //최초 생성 후 지난 틱

		// 타겟과 파동의 방향
		glm::vec3 toTarget = targetTransform.GetWorldPosition() - wave.origin;
		toTarget.y = 0.f;

		// 진행 거리(현재 파동의 반지름) = 경과 시간 * 파동 속도
		float radius = elapsed * (waveSpeed * wave.diameter / 2.f);
		glm::vec3 wavePoint = wave.origin + glm::normalize(toTarget) * radius;

		transform.SetPosition(wavePoint);
		
	}

}