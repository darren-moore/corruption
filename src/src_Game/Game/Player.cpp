
#include <Core/Locator.h>
#include <Core/subengine/IOEngine.h>
#include <Core/subengine/CollisionEngine.h>
#include <Core/math/Constants.h>
#include "BulletCommand.h"
#include "BulletManager.h"

#include <functional>
#include <algorithm>
#include "Player.h"

#include <Core/subengine/IOEngine.h>
#include <Core/subengine/ResourceEngine.h>
#include <Core/subengine/RenderEngine.h>
#include <Core/subengine/CollisionEngine.h>
#include <Core/subengine/AudioEngine.h>
#include "BulletManager.h"
#include "Game.h"

using namespace math;


Player::Player(math::Vec2f position, float speed) : m_position(position), m_baseSpeed(speed), m_ioEngine(Locator::GetIOEngine()), m_collisionAgent(position, 0, (uint32_t)(0 << 0), (uint32_t)(1 << 0)) {

}

void Player::Update(const float dt) {

	const bool isFocusFiring = m_ioEngine.IsDown("PLAYER_FIRE_FOCUS");
	const bool isFiring = m_ioEngine.IsDown("PLAYER_FIRE") || isFocusFiring;

	math::Vec2f movementDirection = math::Vec2f::Zero();
	if (m_ioEngine.IsDown("PLAYER_UP")) {
		movementDirection += Vec2f(0.f, 1.f);
	}
	if (m_ioEngine.IsDown("PLAYER_DOWN")) {
		movementDirection += Vec2f(0.f, -1.f);
	}
	if (m_ioEngine.IsDown("PLAYER_LEFT")) {
		movementDirection += Vec2f(-1.f, 0.f);
	}
	if (m_ioEngine.IsDown("PLAYER_RIGHT")) {
		movementDirection += Vec2f(1.f, 0.f);
	}

	if (isFocusFiring) {
		m_speed = m_baseSpeed * .65f;
	}
	else {
		m_speed = m_baseSpeed;
	}

	// Don't norm a zero vector. We don't have safe norm yet.
	if (movementDirection != math::Vec2f::Zero()) {
		movementDirection[1] *= -1.f;	// due to coord system
		m_position += m_speed * dt * movementDirection.normed();
	}
	// cast to vec2f
	math::Vec2f gameSize(Locator::GetGame().GetGameSize());

	// clamp position
	m_position[0] = std::min(gameSize[0], std::max(m_position[0], 0.f));
	m_position[1] = std::min(gameSize[1], std::max(m_position[1], 0.f));

	m_collisionAgent.position = m_position;

	if (IsInvincible()) {
		m_collisionAgent.collidableLayers = 0;
	}
	else {
		m_collisionAgent.collidableLayers = 1;
	}

	if (m_collisionAgent.isHit && !IsInvincible()) {
		Locator::GetGame().m_lives--;
		m_invincibleEndTime = Locator::GetGame().GetSimTime() + k_invincibleTime;	
	}

	static const uint32_t fireRate = 5;
	static const float bulletSpeed = 30.f;
	static uint32_t fireCtr = 0;
	if (isFiring) {
		if (fireCtr == fireRate) {
			BulletManager& bm = Locator::GetGame().GetBulletManager();

			if (isFocusFiring) {
				bm.AddSimpleBullet(m_position, math::Vec2f(0.f, -bulletSpeed));
				bm.AddSimpleBullet(m_position + math::Vec2f(15.f, 10.f), math::Vec2f(0.f, -bulletSpeed));
				bm.AddSimpleBullet(m_position + math::Vec2f(-15.f, 10.f), math::Vec2f(0.f, -bulletSpeed));
			}
			else {
				bm.AddSimpleBullet(m_position, math::Vec2f(0.f, -bulletSpeed));
				float angle1 = 85 * math::DEGREES_TO_RADIANS;
				float angle2 = 95 * math::DEGREES_TO_RADIANS;
				bm.AddSimpleBullet(m_position + math::Vec2f(15.f, 10.f), bulletSpeed * math::Vec2f(cos(angle1), -sin(angle1)));
				bm.AddSimpleBullet(m_position + math::Vec2f(-15.f, 10.f), bulletSpeed * math::Vec2f(cos(angle2), -sin(angle2)));
			}
			fireCtr = 0;
		}
		else {
			++fireCtr;
		}
	}

}


bool Player::IsInvincible()
{
	return m_invincibleEndTime > Locator::GetGame().GetSimTime();
}