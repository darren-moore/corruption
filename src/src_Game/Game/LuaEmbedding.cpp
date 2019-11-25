#pragma once

#include "LuaEmbedding.h"
#include <Core/Locator.h>
#include <Core/Assert.h>

#include <Core/subengine/IOEngine.h>
#include <Core/subengine/ResourceEngine.h>
#include <Core/subengine/RenderEngine.h>
#include <Core/subengine/CollisionEngine.h>
#include <Core/subengine/AudioEngine.h>

#include "BulletManager.h"
#include "BulletCommand.h"
#include "Game.h"


namespace {
	// define for binding lua function
#define LUABIND(funcName) lua_register(GetLuaState(), #funcName, funcName);

	// Error checking util function
	bool CheckLua(lua_State* L, int r) {
		if (r != LUA_OK) {
			std::string errormsg = lua_tostring(L, -1);
			ASSERT_BREAK(errormsg);
			return false;
		}
		return true;
	}

	// string xmlFile, int pos_x, int pos_y
	int Game_SpawnPattern(lua_State* L)
	{
		std::string xmlFile = (std::string)lua_tostring(L, 1);
		float pos_x = static_cast<float>(lua_tonumber(L, 2));
		float pos_y = static_cast<float>(lua_tonumber(L, 3));
		std::string fullAsset = Locator::GetResourceEngine().GetAssetsPath() + "/Patterns/" + xmlFile;
		BulletCommand* bc = Locator::GetGame().GetBulletManager().AddBullet(fullAsset, math::Vec2f(pos_x, pos_y));
		lua_pushlightuserdata(L, bc);
		return 1;
	}

	// BulletCommand*, int pos_x, int pos_y
	int Game_UpdatePattern(lua_State* L)
	{
		BulletCommand* bc = reinterpret_cast<BulletCommand*>(lua_touserdata(L, 1));
		float pos_x = static_cast<float>(lua_tonumber(L, 2));
		float pos_y = static_cast<float>(lua_tonumber(L, 3));
		bc->SetPosition(math::Vec2f(pos_x, pos_y));
		return 0;
	}

	// BulletCommand*
	int Game_DestroyPattern(lua_State* L)
	{
		BulletCommand* bc = reinterpret_cast<BulletCommand*>(lua_touserdata(L, 1));
		Locator::GetGame().GetBulletManager().DestroyCommand(bc);
		return 0;
	}

	// int pos_x, int pos_y
	int Game_DrawDebugCircle(lua_State* L)
	{
		float pos_x = static_cast<float>(lua_tonumber(L, 1));
		float pos_y = static_cast<float>(lua_tonumber(L, 2));
		float colour_1 = static_cast<float>(lua_tonumber(L, 3));
		float colour_2 = static_cast<float>(lua_tonumber(L, 3));
		float colour_3 = static_cast<float>(lua_tonumber(L, 3));
		Locator::GetRenderEngine().DrawCircle(math::Vec2f(pos_x, pos_y), 10.f, math::Vec3f(colour_1, colour_2, colour_3));
		return 0;
	}

	// string relativeFilePath, string name
	int Game_AddTexture(lua_State* L)
	{
		std::string relativeFilePath = (std::string)lua_tostring(L, 1);
		std::string textureName = (std::string)lua_tostring(L, 2);
		Locator::GetResourceEngine().AddTexture(relativeFilePath.c_str(), textureName.c_str());
		return 0;
	}

	// int pos_x, int pos_y
	int Game_DrawTexture(lua_State* L)
	{
		std::string textureName = (std::string)lua_tostring(L, 1);
		float pos_x = static_cast<float>(lua_tonumber(L, 2));
		float pos_y = static_cast<float>(lua_tonumber(L, 3));
		float size_x = static_cast<float>(lua_tonumber(L, 4));
		float size_y = static_cast<float>(lua_tonumber(L, 5));
		float rotation = static_cast<float>(lua_tonumber(L, 6));
		const Texture* texture = Locator::GetResourceEngine().GetTexture(textureName.c_str());

		Locator::GetRenderEngine().DrawTexture(texture, math::Vec2f(pos_x, pos_y), math::Vec2f(size_x, size_y), rotation);
		return 0;
	}

	int Game_CreateCollisionAgent(lua_State* L)
	{
		CollisionAgent* agent = new CollisionAgent(math::Vec2f(0.f, 0.f), 30, (uint32_t)(0 << 0), (uint32_t)(1 << 1));
		lua_pushlightuserdata(L, agent);
		return 1;
	}

	int Game_UpdateCollisionAgent(lua_State* L)
	{
		CollisionAgent* collisionAgent = reinterpret_cast<CollisionAgent*>(lua_touserdata(L, 1));
		float pos_x = static_cast<float>(lua_tonumber(L, 2));
		float pos_y = static_cast<float>(lua_tonumber(L, 3));
		collisionAgent->position = math::Vec2f(pos_x, pos_y);
		return 0;
	}

	int Game_DestroyCollisionAgent(lua_State* L)
	{
		CollisionAgent* collisionAgent = reinterpret_cast<CollisionAgent*>(lua_touserdata(L, 1));
		delete collisionAgent;
		return 0;
	}

	int Game_QueryCollisionAgent(lua_State* L)
	{
		CollisionAgent* collisionAgent = reinterpret_cast<CollisionAgent*>(lua_touserdata(L, 1));
		lua_pushboolean(L, collisionAgent->isHit);
		return 1;
	}
}


LuaEmbedding::LuaEmbedding(std::string luafile) :
	m_luaState(luaL_newstate())
	, m_luaFileName(luafile)
{
	ResetState();
	RunFile();
}

void LuaEmbedding::ResetState() {

	CleanupState();
	lua_close(m_luaState);
	m_luaState = luaL_newstate();
	luaL_openlibs(m_luaState);

	LUABIND(Game_SpawnPattern);
	LUABIND(Game_UpdatePattern);
	LUABIND(Game_DestroyPattern);

	LUABIND(Game_DrawDebugCircle);
	LUABIND(Game_AddTexture);
	LUABIND(Game_DrawTexture);

	LUABIND(Game_CreateCollisionAgent);
	LUABIND(Game_UpdateCollisionAgent);
	LUABIND(Game_DestroyCollisionAgent);
	LUABIND(Game_QueryCollisionAgent);

	std::string myPath = Locator::GetResourceEngine().GetAssetsPath() + "LuaScripts/";
	CheckLua(m_luaState, luaL_dostring(m_luaState, ("package.path = package.path .. \";" + myPath + "?.lua\"").c_str()));
}

void LuaEmbedding::RunFile() {
	CheckLua(m_luaState, luaL_dofile(m_luaState, m_luaFileName.c_str()));
}

void LuaEmbedding::UpdateLua(float dt, float time) {
	lua_getglobal(m_luaState, "_Update");
	if (lua_isfunction(m_luaState, -1)) {
		lua_pushnumber(m_luaState, dt);
		lua_pushnumber(m_luaState, time);

		CheckLua(m_luaState, lua_pcall(m_luaState, 2, 0, 0));
	}
}

void LuaEmbedding::CleanupState() {
	lua_getglobal(m_luaState, "_Cleanup");
	if (lua_isfunction(m_luaState, -1)) {
		CheckLua(m_luaState, lua_pcall(m_luaState, 0, 0, 0));
	}
}
