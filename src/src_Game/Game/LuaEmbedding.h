#pragma once

#include <string>
#include <iostream>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

class LuaEmbedding {
public:
	LuaEmbedding(std::string luafile);

	~LuaEmbedding() {
		lua_close(m_luaState);
	}
	lua_State* GetLuaState() { return m_luaState; }
	std::string GetLUAFile() { return m_luaFileName; }

	void UpdateLua(float dt, float time);
	void CleanupState();
	void ResetState();
	void RunFile();

    private:
        lua_State* m_luaState;
        std::string m_luaFileName;
};