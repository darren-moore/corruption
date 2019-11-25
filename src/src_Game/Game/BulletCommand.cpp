#pragma once

#include <math.h>
#define _USE_MATH_DEFINES

#include <Core/subengine/CollisionEngine.h>

#include <vector>
#include <Core/math/Constants.h>
#include <Core/math/Vector.h>
#include <bulletmlrunner.h>


#include "BulletCommand.h"

int BulletCommand::rank_ = 1;

BulletCommand::BulletCommand(BulletMLParser* bp, std::queue<StatelessBullet>* toAddStateless, std::queue<StatefulBullet>* toAddStateful, math::Vec2f shipPosition) :
	BulletMLRunner(bp)
	, m_toAddStateless(toAddStateless)
	, m_toAddStateful(toAddStateful)
	, m_turn(0)
{
	m_bullet = new StatefulBullet;
	m_bullet->position = shipPosition;
	m_bullet->enabled = true;
}

BulletCommand::BulletCommand(StatefulBullet* b, std::queue<StatelessBullet>* toAddStateless, std::queue<StatefulBullet>* toAddStateful) :
	BulletMLRunner(b->state)
	, m_toAddStateless(toAddStateless)
	, m_toAddStateful(toAddStateful)
	, m_turn(0)
	, m_bullet(b)
{
	m_bullet->enabled = true;
	m_bullet->command = this;
}

void BulletCommand::Run() {
	run();
	++m_turn;
}

void BulletCommand::SetPosition(math::Vec2f position) {
	m_bullet->position = position;
}

void BulletCommand::SetAim(math::Vec2f playerPos) {
	math::Vec2f aimDirection = playerPos - m_bullet->position;
	m_bullet->aimDirection = atan2f(aimDirection[1], aimDirection[0]) * math::RADIANS_TO_DEGREES;
}

void BulletCommand::createSimpleBullet(double direction, double speed) {
	StatelessBullet b;
	b.direction = static_cast<float>(direction);
	b.speed = static_cast<float>(speed);
	b.position = m_bullet->position;
	b.enabled = true;

	b.vecDirection = math::Vec2f(cos(b.direction * math::DEGREES_TO_RADIANS), sin(b.direction * math::DEGREES_TO_RADIANS));
	b.collisionAgent = new CollisionAgent(b.position, 10, (uint32_t)(1 << 0), 0);

	m_toAddStateless->push(b);
}

void BulletCommand::createBullet(BulletMLState* state, double direction, double speed) {
	StatefulBullet b;
	b.direction = static_cast<float>(direction);
	b.speed = static_cast<float>(speed);
	b.position = m_bullet->position;
	b.state = state;
	b.enabled = true;

	b.vecDirection = math::Vec2f(cos(b.direction * math::DEGREES_TO_RADIANS), sin(b.direction * math::DEGREES_TO_RADIANS));
	b.collisionAgent = new CollisionAgent(b.position, 10, (uint32_t)(1 << 0), 0);

	m_toAddStateful->push(b);
}

void BulletCommand::doVanish() {
	m_bullet->enabled = false;
}

void BulletCommand::doChangeSpeed(double speed) {
	m_bullet->speed = static_cast<float>(speed);
}

void BulletCommand::doChangeDirection(double direction) {
	m_bullet->direction = static_cast<float>(direction);
	m_bullet->vecDirection = math::Vec2f(cos(direction * math::DEGREES_TO_RADIANS), sin(direction * math::DEGREES_TO_RADIANS));
}

double BulletCommand::getBulletDirection() {
	return static_cast<double>(m_bullet->direction);
}

double BulletCommand::getAimDirection() {
	return static_cast<double>(m_bullet->aimDirection);
}

double BulletCommand::getBulletSpeed() {
	return static_cast<double>(m_bullet->speed);
}

double BulletCommand::getDefaultSpeed() {
	return static_cast<double>(m_bullet->defaultSpeed);
}

double BulletCommand::getRank() {
	return rank_;
}

int BulletCommand::getTurn() {
	return m_turn;
}
