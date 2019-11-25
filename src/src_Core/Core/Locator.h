#pragma once

class ResourceEngine;
class RenderEngine;
class IOEngine;
class CollisionEngine;
class AudioEngine;
class Game;

// An implementation of the Service Locator pattern.
class Locator {
public:
	static IOEngine& GetIOEngine() { return *m_ioEngine; }
	static void Provide(IOEngine& ioEngine) { m_ioEngine = &ioEngine; }

	static RenderEngine& GetRenderEngine() { return *m_renderEngine; }
	static void Provide(RenderEngine& renderEngine) { m_renderEngine = &renderEngine; }

	static ResourceEngine& GetResourceEngine() { return *m_resourceEngine; }
	static void Provide(ResourceEngine& resourceEngine) { m_resourceEngine = &resourceEngine; }

	static CollisionEngine& GetCollisionEngine() { return *m_collisionEngine; }
	static void Provide(CollisionEngine& collisionEngine) { m_collisionEngine = &collisionEngine; }

	static AudioEngine& GetAudioEngine() { return *m_audioEngine; }
	static void Provide(AudioEngine& audioEngine) { m_audioEngine = &audioEngine; }

	static Game& GetGame() { return *m_game; }
	static void Provide(Game& game) { m_game = &game; }
	
private:
	static IOEngine* m_ioEngine;
	static RenderEngine* m_renderEngine;
	static ResourceEngine* m_resourceEngine;
	static CollisionEngine* m_collisionEngine;
	static AudioEngine* m_audioEngine;
	static Game* m_game;
};
