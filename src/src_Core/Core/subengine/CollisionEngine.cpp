#include "CollisionEngine.h"

#include <Core/Locator.h>

CollisionAgent::CollisionAgent() :
	position(math::Vec2f::Zero())
	, radius(0)
	, collisionLayer(0)
	, collidableLayers(0)
{
	Locator::GetCollisionEngine().RegisterAgent(this);
}

CollisionAgent::CollisionAgent(math::Vec2f position, float radius, uint32_t collisionLayer, uint32_t collidableLayers) :
	position(position)
	, radius(radius)
	, collisionLayer(collisionLayer)
	, collidableLayers(collidableLayers)
{
	Locator::GetCollisionEngine().RegisterAgent(this);
}
CollisionAgent::~CollisionAgent() {
	Locator::GetCollisionEngine().RemoveAgent(this);
}


void CollisionEngine::RegisterAgent(CollisionAgent* collisionAgent) {
	m_collisionAgents.push_back(collisionAgent);
}

void CollisionEngine::RemoveAgent(CollisionAgent* agent) {
	auto it = std::find(m_collisionAgents.begin(), m_collisionAgents.end(), agent);
	if (it != m_collisionAgents.end()) {
		m_collisionAgents.erase(it);
	}
}

void CollisionEngine::RunCollisionDetection() {

	for (CollisionAgent* agent : m_collisionAgents) {
		agent->isHit = false;
	}

	for (CollisionAgent* agent : m_collisionAgents) {
		if (agent->collidableLayers == 0) {
			continue;
		}

		for (CollisionAgent* otherAgent : m_collisionAgents) {
			if (agent == otherAgent) {
				continue;
			}

			if ((agent->collidableLayers & otherAgent->collisionLayer) == 0) {
				continue;	// mutually disjoint layers
			}

			float distSquared = (agent->position - otherAgent->position).norm2();
			// radius better not be negative...
			float minAllowed = agent->radius + otherAgent->radius;
			if (distSquared < minAllowed * minAllowed) {
				// collision detected!
				agent->isHit = true;
				otherAgent->isHit = true;
			}
		}
	}


}
