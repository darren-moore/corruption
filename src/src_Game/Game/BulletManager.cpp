
#include <Core/Locator.h>
#include <Core/subengine/RenderEngine.h>
#include <Core/subengine/CollisionEngine.h>
#include <Core/math/Constants.h>
#include <Core/math/Vector.h>

#include "BulletManager.h"
#include "BulletCommand.h"

#include <bulletmlparser.h>
#include <bulletmlparser-tinyxml.h>
#include <bulletmlrunner.h>

#include <iostream>


BulletManager::BulletManager(math::Vec2i fieldSize) {
	m_renderEngine = &Locator::GetRenderEngine();
	m_statefulBullets = new StatefulBullet[MAX_BULLETS];
	m_statelessBullets = new StatelessBullet[MAX_BULLETS];

	m_newStatelessBullets = new std::queue<StatelessBullet>;
	m_newStatefulBullets = new std::queue<StatefulBullet>;

	m_lowCull = math::Vec2f(0.f, 0.f);
	m_highCull = fieldSize;
}

BulletManager::~BulletManager() {
	delete[] m_statefulBullets;
	delete[] m_statelessBullets;
	for (auto& command : m_bulletCommands) {
		delete command;
	}
	delete m_newStatelessBullets;
	delete m_newStatefulBullets;
}

void BulletManager::DestroyAll() {
	{
		StatefulBullet* b = m_statefulBullets;
		for (size_t i = 0; i < m_countUpperBound; ++i, ++b) {
			if (b->collisionAgent != nullptr)
				delete b->collisionAgent;
			*b = StatefulBullet();
		}
	}
	{
		StatelessBullet* b = m_statelessBullets;
		for (size_t i = 0; i < m_countUpperBound; ++i, ++b) {
			if (b->collisionAgent != nullptr)
				delete b->collisionAgent;
			*b = StatelessBullet();
		}
	}

	for (auto& command : m_bulletCommands) {
		delete command;
	}
	m_bulletCommands.clear();

	for (auto& parser : m_bulletParsers) {
		delete parser.second;
	}
	m_bulletParsers.clear();

	while (!m_newStatelessBullets->empty()) {
		m_newStatelessBullets->pop();
	}
	while (!m_newStatefulBullets->empty()) {
		m_newStatefulBullets->pop();
	}
}

void BulletManager::DestroyCommand(const BulletCommand* bc) {
	auto it = std::find(m_bulletCommands.begin(), m_bulletCommands.end(), bc);
	if (it != m_bulletCommands.end()) {
		m_bulletCommands.erase(it);
	}
}

BulletCommand* BulletManager::AddBullet(std::string patternName, math::Vec2f pos) {
	BulletCommand* bc;
	if (m_bulletParsers.count(patternName) == 0)
	{
		BulletMLParser* parser = new BulletMLParserTinyXML(patternName);
		parser->build();
		m_bulletParsers[patternName] = parser;
		bc = new BulletCommand(m_bulletParsers[patternName], m_newStatelessBullets, m_newStatefulBullets, pos);
		m_bulletCommands.push_back(bc);
	}
	else {
		bc = new BulletCommand(m_bulletParsers[patternName], m_newStatelessBullets, m_newStatefulBullets, pos);
		m_bulletCommands.push_back(bc);
	}

	return bc;
}

// TODO: generalize. collision agent is very specific.
// Used for player bullets
void BulletManager::AddSimpleBullet(math::Vec2f pos, math::Vec2f velocity) {
	StatelessBullet b;
	b.position = pos;
	b.direction = acos(math::Vec2f(1.f, 0.f).dot(velocity) / velocity.norm()) * math::RADIANS_TO_DEGREES;
	b.speed = velocity.norm();
	b.enabled = true;

	b.vecDirection = velocity.normed();
	b.collisionAgent = new CollisionAgent(b.position, 10, (uint32_t)(1 << 1), 0);
	b.type = BulletType::Player;

	m_newStatelessBullets->push(b);
}

void BulletManager::UpdateBullets(float stepSize, math::Vec2f playerPos) {
	
	const Texture* statelessTexture = Locator::GetResourceEngine().GetTexture("bullet");
	math::Vec2f texSize = math::Vec2f(128, 128*(3.0f/5));
	texSize /= (texSize[1]/10);		// assume radius 10 for now.

	// Stateful bullet logic
	for (BulletCommand* bc : m_bulletCommands) {
		bc->SetAim(playerPos);
		bc->Run();
	}

	{
		StatefulBullet* b = m_statefulBullets;
		for (size_t i = 0; i < m_countUpperBound; ++i, ++b) {
			if (!b->enabled) {
				if (!m_newStatefulBullets->empty()) {

					// Remove old bullet command
					BulletCommand* bc = b->command;
					if (bc != nullptr){
						auto it = std::find(m_bulletCommands.begin(), m_bulletCommands.end(), bc);
						if (it != m_bulletCommands.end()) {
							m_bulletCommands.erase(it);
						}
						delete bc;
					}
					if (b->collisionAgent != nullptr) {
						delete b->collisionAgent;
					}
					

					// Add new bullet
					*b = m_newStatefulBullets->front();
					m_newStatefulBullets->pop();
					b->enabled = true;

					// Append command
					m_bulletCommands.push_back(new BulletCommand(b, m_newStatelessBullets, m_newStatefulBullets));
					// fall through and compute.
				}
				else {
					continue;
				}
			}

			b->timeAlive += stepSize;
			b->position += b->vecDirection * b->speed * stepSize;
			b->collisionAgent->position = b->position;

			// remove if collided
			if (b->collisionAgent->isHit) {
				delete b->collisionAgent;
				b->collisionAgent = nullptr;
				b->enabled = false;
				continue;
			}

			// remove if offscreen
			if (b->position[0] < (m_lowCull)[0] || b->position[0] > (m_highCull)[0] ||
				b->position[1] < (m_lowCull)[1] || b->position[1] > (m_highCull)[1]) {
				b->enabled = false;
				continue;
			}

			// render bullets
			m_renderEngine->DrawTexture(statelessTexture, b->position, texSize, b->timeAlive / 5);
		}
	}

	{
		StatelessBullet* slb = m_statelessBullets;
		for (size_t i = 0; i < m_countUpperBound; ++i, ++slb) {
			if (!slb->enabled) {
				if (!m_newStatelessBullets->empty()) {
					if (slb->collisionAgent != nullptr) {
						delete slb->collisionAgent;
					}
					
					// add new bullet
					*slb = m_newStatelessBullets->front();
					m_newStatelessBullets->pop();

					slb->enabled = true;
					// fall through and compute.
				}
				else {
					continue;
				}
			}
			slb->timeAlive += stepSize;
			slb->position += slb->vecDirection * slb->speed * stepSize;
			slb->collisionAgent->position = slb->position;

			// remove if collided
			if (slb->collisionAgent->isHit) {
				delete slb->collisionAgent;
				slb->collisionAgent = nullptr;
				slb->enabled = false;
				continue;
			}

			// remove if offscreen
			if (slb->position[0] < (m_lowCull)[0] || slb->position[0] > (m_highCull)[0] ||
				slb->position[1] < (m_lowCull)[1] || slb->position[1] > (m_highCull)[1]) {
				slb->enabled = false;
				continue;
			}

			// render bullets
			if (slb->type == BulletType::Standard) {
				m_renderEngine->DrawTexture(statelessTexture, slb->position, texSize, slb->timeAlive / 5);
			}
			// note: player bullets can't have state
			else if (slb->type == BulletType::Player) {
				const math::Vec2f playerBulletRect(5.f, 30.f);
				float rotation = (90 - slb->direction) * math::DEGREES_TO_RADIANS;
				m_renderEngine->DrawRectangle(slb->position - playerBulletRect/2, slb->position+playerBulletRect/2, math::Vec3f(0.0f, 0.0f, 1.0f), rotation);
			}
			
		}
	}
}
