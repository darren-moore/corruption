#include "Locator.h"
#include "subengine/RenderEngine.h"
#include "subengine/IOEngine.h"
#include "subengine/ResourceEngine.h"
#include "subengine/CollisionEngine.h"
#include "subengine/AudioEngine.h"

IOEngine* Locator::m_ioEngine;
RenderEngine* Locator::m_renderEngine;
ResourceEngine* Locator::m_resourceEngine;
CollisionEngine* Locator::m_collisionEngine;
AudioEngine* Locator::m_audioEngine;
Game* Locator::m_game;