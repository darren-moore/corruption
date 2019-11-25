#pragma once
#include <Core/subengine/IOEngine.h>
#include <Core/subengine/ResourceEngine.h>
#include <Core/subengine/RenderEngine.h>
#include <Core/subengine/AudioEngine.h>
#include <Core/subengine/CollisionEngine.h>
#include <Core/Locator.h>
#include <Core/graphics/Animation.h>
#include <Core/graphics/Font.h>
#include <math.h>
#include "BulletCommand.h"
#include "BulletManager.h"
#include "LuaEmbedding.h"
#include <Core/Timer.h>
#include "Player.h"
#include <irrKlang.h>
#include "Game.h"


Game::Game(GLuint windowWidth, GLuint windowHeight, std::string assetsPath)
{
	m_ioEngine.Init(windowWidth, windowHeight, "Liminal");
	Locator::Provide(m_ioEngine);

	m_resourceEngine.Init(assetsPath);
	Locator::Provide(m_resourceEngine);

	m_renderEngine.Init();
	Locator::Provide(m_renderEngine);

	m_collisionEngine.Init();
	Locator::Provide(m_collisionEngine);

	m_audioEngine.Init();
	Locator::Provide(m_audioEngine);
	
	Locator::Provide(*this);
}

void Game::Start() {
	m_bulletManager = new BulletManager(m_gameSize);
	m_luaEmbedding = new LuaEmbedding(Locator::GetResourceEngine().GetAssetsPath() + "LuaScripts/main.lua");
	m_player = new Player(math::Vec2f(400.f, 300.f), 300);


	m_renderEngine.SetClearColor(.1f, .7f, .2f, 0.f);	// bright green
	m_renderEngine.SetCircleNumSides(16);

	m_resourceEngine.AddTexture("title.png", "title");
	m_resourceEngine.AddTexture("bullet.png", "bullet");
	m_resourceEngine.AddTexture("sheet.png", "anim");
	m_resourceEngine.AddTexture("enemy.png", "enemy");

	//m_audioEngine.Play(Locator::GetResourceEngine().GetAssetsPath() + "wind.mp3", "SND_WIND", true);


	m_ioEngine.BindInput("DEBUG_REBUILD", GLFW_KEY_R);
	m_ioEngine.BindInput("PAUSE", GLFW_KEY_P);
	m_ioEngine.BindInput("START", GLFW_KEY_ENTER);


	m_ioEngine.BindInput("PLAYER_UP", GLFW_KEY_UP);
	m_ioEngine.BindInput("PLAYER_DOWN", GLFW_KEY_DOWN);
	m_ioEngine.BindInput("PLAYER_LEFT", GLFW_KEY_LEFT);
	m_ioEngine.BindInput("PLAYER_RIGHT", GLFW_KEY_RIGHT);


	m_ioEngine.BindInput("PLAYER_FIRE", GLFW_KEY_C);
	m_ioEngine.BindInput("PLAYER_FIRE_FOCUS", GLFW_KEY_X);

	m_luaEmbedding->RunFile();


	float lastFrameTime = 0;
	
	int countedFrames = 0;
	fpsTimer.start();

	// Enter game loop
	while (true) {
		if (m_ioEngine.ShouldClose()) {
			Stop();
			break;
		}
		float currentFrameTime = m_ioEngine.GetCurrentWindowTime();
		m_dt = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;
		
		m_ioEngine.ProcessInput();
		m_renderEngine.Clear();
		// UI ratio is fixed at 4:3
		// Game ratio is fixed at 3:4
		// NOTE: we assume the game viewport can be embedded within the window size
		const GLuint windowAdjustedUIWidth = (m_ioEngine.GetWindowHeight() * 4) / 3;
		const GLuint windowUIBottomLeft = (m_ioEngine.GetWindowWidth() - windowAdjustedUIWidth) / 2;
		const GLuint windowAdjustedGameWidth = (m_ioEngine.GetWindowHeight() * 3) / 4;
		const GLuint windowGameBottomLeft = (m_ioEngine.GetWindowWidth() - windowAdjustedGameWidth) / 2;

		// Compute frame rate
		double avgFPS = countedFrames / (fpsTimer.getTicks() / 1.f);
		if (avgFPS > 2000000) avgFPS = 0;

		const GLuint gameBottomLeft = (m_UISize[0] - m_gameSize[0])/2;
		const GLuint adjustedGameWidth = m_gameSize[0];

		// draw and update UI
		m_renderEngine.DrawRectangle(math::Vec2f(windowUIBottomLeft, 0.f), math::Vec2f(windowUIBottomLeft + windowAdjustedUIWidth, m_ioEngine.GetWindowHeight()), math::Vec3f(.5f, .5f, .5f));
		m_renderEngine.SetViewport(windowUIBottomLeft, 0, windowAdjustedUIWidth, m_ioEngine.GetWindowHeight(), m_UISize[0], m_UISize[1]);
		
		// FPS counter
		m_renderEngine.SetFontAlign(FontAlignH::Right, FontAlignV::Bottom);
		m_renderEngine.DrawText("FPS: " + std::to_string((int)avgFPS), m_UISize, .5f);

		m_renderEngine.SetFontAlign(FontAlignH::Left, FontAlignV::Top);
		m_renderEngine.DrawText("Lives: " + std::to_string(m_lives), math::Vec2f(0.f, 0.f), .5f);
		m_renderEngine.DrawText("Bombs: " + std::to_string(m_bombs), math::Vec2f(0.f, 50.f), .5f);
    
		glFinish(); // not sure if this is needed here

		// draw and update game in centre of screen
		m_renderEngine.DrawLine(math::Vec2f(gameBottomLeft, 0.f), math::Vec2f(gameBottomLeft, m_gameSize[1]), math::Vec3f(1.f, 0.f, 0.f));
		m_renderEngine.DrawLine(math::Vec2f(gameBottomLeft + adjustedGameWidth, 0.f), math::Vec2f(gameBottomLeft + adjustedGameWidth, m_gameSize[1]), math::Vec3f(1.f, 0.f, 0.f));
		m_renderEngine.SetViewport(windowGameBottomLeft, 0, windowAdjustedGameWidth, m_ioEngine.GetWindowHeight(), m_gameSize[0], m_gameSize[1]);

		this->Update(m_dt);
	
		// finish, swap, and reset viewport
		glFinish();
		m_ioEngine.SwapBuffers();
		m_renderEngine.SetViewport(0, 0, m_ioEngine.GetWindowWidth(), m_ioEngine.GetWindowHeight(), m_ioEngine.GetWindowWidth(), m_ioEngine.GetWindowHeight());
	
		++countedFrames;
	}
	fpsTimer.stop();
}

// Shutdown sub-engines in inverse start-order.
void Game::Stop() {
	delete m_player;

	m_audioEngine.Shutdown();
	m_collisionEngine.Stop();
	m_renderEngine.Stop();
	m_resourceEngine.Stop();
	m_ioEngine.Stop();
}

void Game::Update(const float dt) {


	if (m_ioEngine.IsDown(GLFW_KEY_LEFT_ALT) && m_ioEngine.IsTapped(GLFW_KEY_RIGHT_SHIFT)) {
		m_ioEngine.ToggleFullscreen();
	}

	if (m_ioEngine.IsTapped("START")) {
		if (m_state == GameState::TitleScreen) {
			m_state = GameState::Simulation;
		}
	}

	if (m_ioEngine.IsTapped("PAUSE")) {
		if (m_state == GameState::Pause) {
			m_state = GameState::Simulation;
		}
		else if (m_state == GameState::Simulation) {
			m_state = GameState::Pause;
		}
	}


	if (m_state == GameState::TitleScreen) {
		m_renderEngine.DrawTexture(m_resourceEngine.GetTexture("title"), math::Vec2f(0, 0), math::Vec2f(480, 640));
		m_renderEngine.SetFontAlign(FontAlignH::Centre, FontAlignV::Middle);
		m_renderEngine.DrawText("Liminal", math::Vec2f(240.f, 50.f));
		if (int((m_ioEngine.GetCurrentWindowTime()) * 1.5f) % 2 == 0) {
			m_renderEngine.DrawText("Press Enter", math::Vec2f(240.f, 100.f));
		}
		
	}

	if (m_state == GameState::Pause) {
		m_renderEngine.DrawRectangle(math::Vec2f(120.f, 100.f), math::Vec2f(360.f, 200.f), math::Vec3f(1.f, 1.f, 1.f));
		m_renderEngine.SetFontAlign(FontAlignH::Centre, FontAlignV::Middle);
		m_renderEngine.DrawText("text", math::Vec2f(240.f, 150.f));
	}


	if (m_state != GameState::Simulation) {
		return;
	}

	if (m_ioEngine.IsTapped("DEBUG_REBUILD" ) || m_lives < 0) {
		m_simulationTime = 0;
		m_lives = 3;
		m_bombs = 3;
		m_bulletManager->DestroyAll();
		m_luaEmbedding->ResetState();
		m_luaEmbedding->RunFile();
	}


	m_simulationTime += dt;

	m_player->Update(dt);

	m_luaEmbedding->UpdateLua(dt, m_simulationTime);
	m_bulletManager->UpdateBullets(1.0f, m_player->GetPosition());

	m_collisionEngine.RunCollisionDetection();

	// demo animation code
	static Animation anim2(m_resourceEngine.GetTexture("anim"), math::Vec2f(100, 100), 12 * 20 + 10, 2);
	anim2.TryAdvanceFrame();
	math::Vec3f m_playerColour = math::Vec3f::Ones();
	if (!(m_player->IsInvincible() && int((m_player->GetInvincibleEndTime() - m_simulationTime) * 100) % 2 == 0)) {
		m_renderEngine.DrawTexture(anim2.GetTexture(), anim2.GetTopLeftFrameCoord(), anim2.GetTopLeftFrameCoord() + anim2.GetFrameSize(), m_player->GetPosition() - (.5 * math::Vec2f(100, 100)), math::Vec2f(100, 100), 0, m_playerColour);
	}
	
	m_renderEngine.DrawCircle(m_player->GetPosition(), 10.f, math::Vec3f(.5f, .5f, .5f));
}

