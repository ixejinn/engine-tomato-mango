#ifndef MANGO_TARGETCOMPONENT_H
#define MANGO_TARGETCOMPONENT_H

#include <glm/vec3.hpp>
#include <entt/entt.hpp>

namespace tomato
{
#define TMT_TARGET_TYPE_LIST(X)	\
	X(Object,   "Object")		\
	X(Position, "Position")

	enum class TargetType : uint8_t
	{
#define X(Enum, Display) Enum,
		TMT_TARGET_TYPE_LIST(X)
#undef X
	};

	struct TargetTypeMeta
	{
		TargetType type;
		const char* name;
	};

	inline constexpr TargetTypeMeta targetTypeMetas[]
	{
#define X(Enum, Display) {TargetType::Enum, Display},
		TMT_TARGET_TYPE_LIST(X)
#undef X
	};
#undef TMT_TARGET_TYPE_LIST

#define TMT_TARGET_MOTION_LIST(X)	\
	X(Chase, "Chase")				\
	X(Lock,  "Lock")

	enum class TargetMotion : uint8_t
	{
#define X(Enum, Display) Enum,
		TMT_TARGET_MOTION_LIST(X)
#undef X
	};

	struct TargetMotionMeta
	{
		TargetMotion motion;
		const char* name;
	};

	inline constexpr TargetMotionMeta targetMotionMetas[]
	{
#define X(Enum, Display) {TargetMotion::Enum, Display},
		TMT_TARGET_MOTION_LIST(X)
#undef X
	};
#undef TMT_TARGET_MOTION_LIST

#define TMT_TARGET_STATE_LIST(X)	\
	X(Idle,    "Idle")				\
	X(Waiting, "Waiting")			\
	X(Chasing, "Chasing")

	enum class TargetState : uint8_t
	{
#define X(Enum, Display) Enum,
		TMT_TARGET_STATE_LIST(X)
#undef X
	};

	struct TargetStateMeta
	{
		TargetState state;
		const char* name;
	};

	inline constexpr TargetStateMeta targetStateMetas[]
	{
#define X(Enum, Display) {TargetState::Enum, Display},
		TMT_TARGET_STATE_LIST(X)
#undef X
	};
#undef TMT_TARGET_STATE_LIST

	struct TargetComponent
	{
		TargetType type{TargetType::Object};
		TargetMotion motion{TargetMotion::Chase};

		entt::entity targetEntity{entt::null};
		glm::vec3 targetPosition{0};
		glm::vec3 offset{0};

		float stopDistance{0};
		float margin{0.1};
		float delay{0};
		float speed{5};
		float acceleration{20};

		// Runtime
		TargetState state{TargetState::Idle};
		float accumulator{0};
		glm::vec3 velocity{0};
	};
}
#endif // !MANGO_TARGETCOMPONENT_H
