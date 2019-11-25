#pragma once
#include <Core/subengine/IOEngine.h>
#include <Core/subengine/ResourceEngine.h>
#include <Core/subengine/RenderEngine.h>
#include <Core/subengine/CollisionEngine.h>
#include <Core/subengine/AudioEngine.h>
#include <Core/Assert.h>
#include "Game/BulletManager.h"
#include "Game/LuaEmbedding.h"

#include "Game/Game.h"

#include <iostream>

static const GLuint WINDOW_WIDTH = 640;
static const GLuint WINDOW_HEIGHT = 480;

int main(int argc, char* argv[]) {

	RELEASE_ASSERT_FATAL(argc == 2, "Please provide the assets filepath as an argument.");

	const char* assetsPath = argv[1];
	std::cout << "Assets file path is: " << assetsPath << std::endl;
	size_t pathLength = strlen(assetsPath);

	for (size_t i = 0; i < pathLength; ++i) {
		RELEASE_ASSERT_FATAL(assetsPath[i] != '\\', "Please use forward-slashes, not back-slashes in your input filepath.");;
	}

	std::cout << "Starting Liminal with resolution (" << WINDOW_WIDTH
		<< "," << WINDOW_HEIGHT << ")." << std::endl;

	Game liminal(WINDOW_WIDTH, WINDOW_HEIGHT, assetsPath);
	liminal.Start();
	return 0;
}
