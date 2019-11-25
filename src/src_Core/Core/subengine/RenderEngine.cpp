#include "RenderEngine.h"
#include "ResourceEngine.h"
#include "IOEngine.h"
#include <Core/Locator.h>

#include <Core/math/Matrix.h>
#include <Core/math/Matrix.h>

#include <Core/graphics/Shader.h>
#include <Core/graphics/Texture.h>


#define _USE_MATH_DEFINES
#include <math.h>

using namespace math;


void RenderEngine::Init() {
	Locator::GetResourceEngine().AddShader("DefaultShaders/defaultPrimitiveVertShader.vert", "DefaultShaders/defaultPrimitiveFragShader.frag", "defaultPrimitiveShader");
	Locator::GetResourceEngine().AddShader("DefaultShaders/defaultVertShader.vert", "DefaultShaders/defaultFragShader.frag", "defaultSpriteShader");
	Locator::GetResourceEngine().AddShader("DefaultShaders/text_vertex.vert", "DefaultShaders/text_fragment.frag", "fontShader");

	m_primitiveShader = Locator::GetResourceEngine().GetShader("defaultPrimitiveShader");
	m_textureShader = Locator::GetResourceEngine().GetShader("defaultSpriteShader");
	m_fontShader = Locator::GetResourceEngine().GetShader("fontShader");

	InitTextureData();
	InitRectangleData();
	InitLineData();
	InitCircleData(m_circleNumSides);
	SetViewport(0,0, Locator::GetIOEngine().GetWindowWidth(), Locator::GetIOEngine().GetWindowHeight(), Locator::GetIOEngine().GetWindowWidth(), Locator::GetIOEngine().GetWindowHeight());
	m_font = new Font((Locator::GetResourceEngine().GetAssetsPath() + "arial.ttf").c_str());
	std::cout << "The font is " << m_font << std::endl;
	m_font->GenerateCharacters();
}

void RenderEngine::Stop() {

}

void RenderEngine::SetShader(Shader shader) {
	glUseProgram(shader.ID);
}

void RenderEngine::SetClearColor(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}

void RenderEngine::Clear() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderEngine::DrawTexture(const Texture* texture, const Vec2f& position, const Vec2f& size, float rotation, const Vec3f& colour) {
	DrawTexture(texture, Vec2f::Zero(), Vec2f(texture->width, texture->height), position, size, rotation);
}

void RenderEngine::DrawTexture(const Texture* texture, const Vec2f& topLeft, const Vec2f& botRight, const Vec2f& position, const Vec2f& size, float rotation, const Vec3f& colour) {
	m_textureShader->Use();
	Mat4f model = Mat4f::Identity();

	model.TranslateSelf(Vec3f(position[0], position[1], 0.0f));

	// Rotate about centre.
	model.TranslateSelf(Vec3f(0.5f*size[0], 0.5f*size[1], 0.0f));
	model.RotateSelf(rotation);
	model.TranslateSelf(Vec3f(-0.5f*size[0], -0.5f*size[1], 0.0f));

	model.ScaleSelf(Vec3f(size[0], size[1], 1.0));

	m_textureShader->SetMat4("projection", m_projection);
	m_textureShader->SetMat4("model", model);
	m_textureShader->SetVec4("spriteColor", Vec4f(colour[0], colour[1], colour[2], 1.f));

	Matrix4f texMap = Matrix4f::Identity();
	Vec2f subRectSize = botRight - topLeft;
	texMap.TranslateSelf(Vec3f(topLeft[0] / texture->width, topLeft[1] / texture->height, 0.0f));
	texMap.ScaleSelf(Vec3f(subRectSize[0] / texture->width, subRectSize[1] / texture->height, 1.0f));
	m_textureShader->SetMat4("texMap", texMap);

	glActiveTexture(GL_TEXTURE0);
	texture->Bind();

	glBindVertexArray(m_textureVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void RenderEngine::DrawText(const std::string& text, math::Vec2i location, float scale) {
	m_fontShader->Use();
	m_fontShader->SetMat4("projection", m_projection);

	math::Vec2f drawLocation;
	math::Vec2f sSize = math::Vec2f(m_font->ComputeStringSize(text));

	switch (m_fontAlignH) {
	case FontAlignH::Left:
		drawLocation[0] = static_cast<float>(location[0]);
		break;
	case FontAlignH::Centre:
		drawLocation[0] = static_cast<float>(location[0] - scale * sSize[0] / 2.f);
		break;
	case FontAlignH::Right:
		drawLocation[0] = static_cast<float>(location[0] - scale * sSize[0]);
		break;
	}

	switch (m_fontAlignV) {
	case FontAlignV::Bottom:
		drawLocation[1] = static_cast<float>(location[1]);
		break;
	case FontAlignV::Middle:
		drawLocation[1] = static_cast<float>(location[1] + scale * sSize[1] / 2.f);
		break;
	case FontAlignV::Top:
		drawLocation[1] = static_cast<float>(location[1] + scale * sSize[1]);
		break;
	}



	if (!m_font->RenderString(text, drawLocation[0], drawLocation[1], scale)) {
		std::cerr << "Failed to draw text: " << location << std::endl;
	}
}



void RenderEngine::DrawLine(const Vec2f& a, const Vec2f& b, const Vec3f& colour) {
	m_primitiveShader->Use();
	Matrix4f model = Matrix4f::Identity();
	model.TranslateSelf(Vec3f(a[0], a[1], 0.0f));
	model.ScaleSelf(Vec3f(b[0] - a[0], b[1] - a[1], 1.0f));

	m_primitiveShader->SetMat4("projection", m_projection);
	m_primitiveShader->SetMat4("model", model);
	Vec4f colourVec4(colour[0], colour[1], colour[2], 1);
	m_primitiveShader->SetVec4("inColor", colourVec4);

	glBindVertexArray(m_lineVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void RenderEngine::DrawRectangle(const Vec2f& p1, const Vec2f& p2, const Vec3f& colour, const float rotation) {
	m_primitiveShader->Use();
	Matrix4f model = Matrix4f::Identity();

	Vec2f size = p2 - p1;

	model.TranslateSelf(Vec3f(p1[0], p1[1], 0.0f));

	// Rotate about centre.
	model.TranslateSelf(Vec3f(0.5f * size[0], 0.5f * size[1], 0.0f));
	model.RotateSelf(rotation);
	model.TranslateSelf(Vec3f(-0.5f * size[0], -0.5f * size[1], 0.0f));

	model.ScaleSelf(Vec3f(p2[0] - p1[0], p2[1] - p1[1], 1.0f));

	m_primitiveShader->SetMat4("projection", m_projection);
	m_primitiveShader->SetMat4("model", model);
	Vec4f colourVec4(colour[0], colour[1], colour[2], 1);
	m_primitiveShader->SetVec4("inColor", colourVec4);

	glBindVertexArray(m_rectangleVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void RenderEngine::DrawCircle(const Vec2f& c, float r, const Vec3f& colour) {
	m_primitiveShader->Use();
	Matrix4f model = Matrix4f::Identity();
	model.TranslateSelf(Vec3f(c[0], c[1], 0.0f));
	model.ScaleSelf(Vec3f(r, r, 1.0f));

	m_primitiveShader->SetMat4("projection", m_projection);
	m_primitiveShader->SetMat4("model", model);
	Vec4f colourVec4(colour[0], colour[1], colour[2], 1);
	m_primitiveShader->SetVec4("inColor", colourVec4);

	glBindVertexArray(m_circleVAO);
	glDrawElements(GL_TRIANGLES, 3 * (m_circleNumSides + 1), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void RenderEngine::SetCircleNumSides(size_t n) {
	m_circleNumSides = n;
	InitCircleData(n);
}

void RenderEngine::InitTextureData() {
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &m_textureVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(m_textureVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void RenderEngine::InitCircleData(size_t nSides) {
	glGenVertexArrays(1, &m_circleVAO);
	glBindVertexArray(m_circleVAO);

	// +1 for centre vertex
	GLfloat* circleVertices = (GLfloat*)malloc(sizeof(GLfloat) * 2 * (nSides + 2));
	circleVertices[0] = 0;
	circleVertices[1] = 0;

	for (size_t i = 0; i <= nSides; i++) {
		size_t x_i = (i + 1) * 2;
		circleVertices[x_i] = cos(2 * (float)M_PI*i / nSides);
		circleVertices[x_i + 1] = sin(2 * (float)M_PI*i / nSides);
	}

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * (nSides + 2), circleVertices, GL_STATIC_DRAW);

	GLuint* indices = (GLuint*)malloc(sizeof(GLuint) * 3 * (nSides + 1));
	for (size_t i = 0; i <= nSides; i++) {
		indices[i * 3] = 0;
		indices[i * 3 + 1] = i;
		indices[i * 3 + 2] = i + 1;
	}
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 3 * (nSides + 1), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);

	free(circleVertices);
	free(indices);
}

void RenderEngine::InitRectangleData() {
	glGenVertexArrays(1, &m_rectangleVAO);
	glBindVertexArray(m_rectangleVAO);

	GLfloat rectVertices[] = {
		// Pos
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void RenderEngine::InitLineData() {
	glGenVertexArrays(1, &m_lineVAO);
	glBindVertexArray(m_lineVAO);

	GLfloat lineVertices[] = {
		// Pos
		0.0f, 0.0f,
		1.0f, 1.0f,
	};

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void RenderEngine::SetViewport(GLuint x, GLuint y, GLuint width, GLuint height, GLuint canvasWidth, GLuint canvasHeight) {
	glViewport(x,y,width, height);
	m_projection = MyOrtho(0.0f, static_cast<float>(canvasWidth), static_cast<float>(canvasHeight), 0.0f, -1.0f, 1.0f);
	//m_projection = MyOrtho(0.0f, 640, 480, 0.0f, -1.0f, 1.0f);
}
