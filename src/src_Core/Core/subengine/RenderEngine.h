#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
class Shader;
class Texture;
class Font;

#include <Core/math/Vector.h>
#include <Core/math/Matrix.h>
#include <Core/graphics/Font.h>

enum class FontAlignH { Left, Centre, Right };
enum class FontAlignV { Top, Middle, Bottom };

// The Render Engine's job is to push pixels onto the buffer.
#define RENDER_ENGINE_CLASS
class RenderEngine {
public:
	RenderEngine() = default;
	~RenderEngine() = default;

	void Init();

	void Stop();

	void SetShader(Shader shader);

	void DrawText(const std::string& text, math::Vec2i location, float scale = 1.f);
	void DrawTexture(const Texture* texture, const math::Vec2f& topLeft, const math::Vec2f& botRight, const math::Vec2f& position, const math::Vec2f& size = math::Vec2f(1, 1), float rotation = 0.0f, const math::Vec3f & colour = math::Vec3f::Ones());
	void DrawTexture(const Texture* texture, const math::Vec2f& position, const math::Vec2f& size = math::Vec2f(1, 1), float rotation = 0.0f, const math::Vec3f& colour = math::Vec3f::Ones());
	void DrawLine(const math::Vec2f& a, const math::Vec2f& b, const math::Vec3f& colour);
	void DrawRectangle(const math::Vec2f& p1, const math::Vec2f& p2, const math::Vec3f& colour, const float rotation = 0);
	void DrawCircle(const math::Vec2f& c, float r, const math::Vec3f& colour);

	void SetCircleNumSides(size_t n);
	void Clear();
	void SetClearColor(float r, float g, float b, float a);
	void SetViewport(GLuint x, GLuint y, GLuint width, GLuint height, GLuint canvasWidth, GLuint canvasHeight);

	void SetFontAlign(FontAlignH alignH, FontAlignV alignV) { m_fontAlignH = alignH; m_fontAlignV = alignV; }

private:

	void InitTextureData();
	GLuint m_textureVAO;
	Shader* m_textureShader{ nullptr };
	uint32_t m_targetWidth;
	uint32_t m_targetHeight;

	Shader* m_primitiveShader{nullptr};
	GLuint m_rectangleVAO;
	GLuint m_circleVAO;
	GLuint m_lineVAO;
	void InitCircleData(size_t nSides);
	void InitRectangleData();
	void InitLineData();

	Font* m_font;	
	Shader* m_fontShader;
	FontAlignH m_fontAlignH{FontAlignH::Left};
	FontAlignV m_fontAlignV{FontAlignV::Bottom};

	size_t m_circleNumSides = 32;
	math::Mat4f m_projection;
};