#pragma once

#include <Core/math/Vector.h>
#include <cstdint>
#include <array>
#include <vector>
#include <queue>

// Supports only circle-circle collision for now.

struct CollisionAgent final {
	CollisionAgent();
	CollisionAgent(math::Vec2f position, float radius, uint32_t collisionLayer, uint32_t collidableLayers);
	~CollisionAgent();

	bool isHit{ false };
	math::Vec2f position{ math::Vec2f::Zero() };
	float radius{ 0 };

	uint32_t collisionLayer{ 0 };		// a single bit:	what layer this agent occupies
	uint32_t collidableLayers{ 0 };	// bitmask:			which layers this agent collides with
};


// TODO Darren: store agents locally for the cache bonus
#define COLLISION_ENGINE_CLASS
class CollisionEngine {
	friend struct CollisionAgent;
public:
	void Init() {};
	void Stop() {};
	void RunCollisionDetection();
	
private:
	void RegisterAgent(CollisionAgent* agent);
	void RemoveAgent(CollisionAgent* agent);
	std::vector<CollisionAgent*> m_collisionAgents;
};
