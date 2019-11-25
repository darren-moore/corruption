#pragma once

#include <queue>
#include <unordered_map>
#include <Core/math/Vector.h>

class RenderEngine;
class BulletCommand;
class BulletMLState;
class BulletMLParser;

struct StatelessBullet;
struct StatefulBullet;

#define BULLET_MANAGER_CLASS
class BulletManager final {
public:
	BulletManager(math::Vec2i fieldSize);
	~BulletManager();
	BulletCommand* AddBullet(std::string patternName, math::Vec2f pos);
	void AddSimpleBullet(math::Vec2f pos, math::Vec2f velocity);
	void DestroyCommand(const BulletCommand* bc);
	void DestroyAll();
	void UpdateBullets(float dt, math::Vec2f playerPos);

private:
	StatelessBullet* m_statelessBullets;
	StatefulBullet* m_statefulBullets;
	std::unordered_map<std::string, BulletMLParser*> m_bulletParsers;
	std::vector<BulletCommand*> m_bulletCommands;

	std::queue<StatelessBullet>* m_newStatelessBullets;
	std::queue<StatefulBullet>* m_newStatefulBullets;

	static const uint32_t MAX_BULLETS = 1000;
	size_t m_countUpperBound = MAX_BULLETS;

	RenderEngine* m_renderEngine;

	math::Vec2f m_lowCull;
	math::Vec2f m_highCull;
};
