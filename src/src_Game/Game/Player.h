#pragma once

#include <Core/math/Vector.h>
#include <Core/subengine/CollisionEngine.h>

class IOEngine;
struct CollisionAgent;

class Player final {

public:
	Player(math::Vec2f position, float speed);
	~Player() = default;

	void Update(const float dt);
	math::Vec2f GetPosition() { return m_position; }
	bool IsInvincible();
	float GetInvincibleEndTime() { return m_invincibleEndTime; }
	static constexpr float k_invincibleTime = 3.f;

private:

	math::Vec2f m_position;
	float m_baseSpeed;
	float m_speed{ 0 };
	float m_invincibleEndTime{ -1 };
	IOEngine& m_ioEngine;
	CollisionAgent m_collisionAgent;
};
