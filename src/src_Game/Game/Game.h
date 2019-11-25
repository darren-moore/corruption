#pragma once

#include <glad/glad.h>
#include <Core/math/Vector.h>
#include <Core/Timer.h>

class BulletManager;
class ResourceEngine;
class RenderEngine;
class IOEngine;
class CollisionEngine;
class AudioEngine;
class LuaEmbedding;
class Player;

#if !defined(RESOURCE_ENGINE_CLASS) || !defined(RENDER_ENGINE_CLASS) || !defined(IO_ENGINE_CLASS) || !defined(BULLET_MANAGER_CLASS) || !defined(COLLISION_ENGINE_CLASS)
	#error "You must include these classes before Game.h"
#endif

enum class GameState {TitleScreen, Simulation, Pause};

class Game {

public:
	Game(GLuint windowWidth, GLuint windowHeight, std::string assetsPath);
	~Game() {}
	void Stop();
	void Start();
	void Update(float const dt);
	float GetDt() { return m_dt; }
	float GetSimTime() { return m_simulationTime; }
	BulletManager& GetBulletManager() { return *m_bulletManager; }
	math::Vec2i GetGameSize() { return m_gameSize; }

	// this data should be moved out?
	int32_t m_bombs{ 3 };
	int32_t m_lives{ 3 };

private:

	BulletManager* m_bulletManager;

	ResourceEngine m_resourceEngine;
	RenderEngine m_renderEngine;
	IOEngine m_ioEngine;
	CollisionEngine m_collisionEngine;
	AudioEngine m_audioEngine;

	LuaEmbedding* m_luaEmbedding;
	float m_dt{ 0 };
	float m_simulationTime{ 0 };
	GameState m_state{ GameState::TitleScreen };

	Player* m_player;

	// FPS counter
	Timer fpsTimer;

	math::Vec2f m_UISize{ 640 * (4. / 3), 640 };
	math::Vec2i m_gameSize{ 480, 640 };

};
