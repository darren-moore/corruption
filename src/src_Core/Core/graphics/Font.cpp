#pragma once
#include "Font.h"

Font::Font(const char* fontFilepath) : m_textHeight(0) {

	int iResult;

	if ((iResult = FT_Init_FreeType(&m_fontLibrary))) {

		std::cerr << "Error could not initialise freetype." << std::endl;

	}

	if ((iResult = FT_New_Face(m_fontLibrary, fontFilepath, 0, &m_fontFace))) {

		std::cout << "Could not load font face " << iResult << std::endl;
	}

	FT_Set_Pixel_Sizes(m_fontFace, 0, 48);
	m_textHeight = 48;

	std::cout << "Initialised freetype" << std::endl;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
}


Font::~Font() {

	FT_Done_Face(m_fontFace);

	FT_Done_FreeType(m_fontLibrary);

}

bool Font::GenerateCharacters() {

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c < this->k_defaultLength; c++) {
		if (FT_Load_Char(m_fontFace, c, FT_LOAD_RENDER)) {
			std::cerr << "Failed to load glyph at code " << c << std::endl;
			return false;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		const FT_Bitmap& glyphBitmap = m_fontFace->glyph->bitmap;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyphBitmap.width, glyphBitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, glyphBitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		Character chars = {
			c,
			texture,
			math::Vec2f(glyphBitmap.width, glyphBitmap.rows),
			math::Vec2f(m_fontFace->glyph->bitmap_left, m_fontFace->glyph->bitmap_top), m_fontFace->glyph->advance.x
		};

		m_charMap[c] = chars;

	}

	return true;

}

bool Font::RenderString(const std::string& string, GLfloat x, GLfloat y, GLfloat scale) {

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glBindVertexArray(m_VAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));


	// Iterate through all characters
	for (const auto& c : string) {
		Character ch = m_charMap[c];
		GLfloat xpos = x + ch.bearing[0] * scale;
		GLfloat ypos = y - ch.bearing[1] * scale;
		GLfloat w = ch.size[0] * scale;
		GLfloat h = ch.size[1] * scale;

		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,			ypos + h,	0.0, 1.0 },
			{ xpos + w,		ypos,		1.0, 0.0 },
			{ xpos,			ypos,		0.0, 0.0 },

			{ xpos,			ypos + h,	0.0, 1.0 },
			{ xpos + w,		ypos + h,   1.0, 1.0 },
			{ xpos + w,		ypos,		1.0, 0.0 }
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);

		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

math::Vec2i Font::ComputeStringSize(const std::string& s) {

	int x = 0;
	for (const auto& c : s) {
		Character ch = m_charMap[c];
		x += (ch.advance >> 6);	// Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	return math::Vec2i(x, m_textHeight);
}

