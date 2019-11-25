#pragma once
#include <core/subengine/RenderEngine.h>
#include <Core/subengine/ResourceEngine.h>
#include <core/graphics/Shader.h>
#include <ft2build.h>
#include <iostream>
#include FT_FREETYPE_H
#include <Core/math/Vector.h>
#include <map>
#include <string>

struct Character {
	char character;
	GLuint textureID;
	math::Vec2f size;
	math::Vec2f bearing;	
	GLuint advance;
};


class Font
{
public:

	Font(const char* fontFilepath);
	~Font();

	bool GenerateCharacters();
	bool RenderString(const std::string& string, GLfloat x, GLfloat y, GLfloat scale);
	math::Vec2i ComputeStringSize(const std::string& s);

private:	

	int m_textHeight;

	static const int k_defaultLength = 128;

	FT_Library m_fontLibrary;
	FT_Face m_fontFace;
	Character m_charMap[k_defaultLength];
	GLuint m_VAO, m_VBO;

};

