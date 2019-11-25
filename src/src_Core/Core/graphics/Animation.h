#pragma once
#include <glad/glad.h>
#include <Core/math/Vector.h>
#include <cstdint>

class Texture;

class Animation final {
public:
	Animation(const Texture* texture, const math::Vec2f frameSize, const uint32_t numFrames, const uint32_t speed);
	~Animation() = default;

	void TryAdvanceFrame();
	const Texture* GetTexture() const;
	const math::Vec2f GetTopLeftFrameCoord() const;
	const math::Vec2f GetFrameSize() const;

	
private:
	const Texture* m_texture;
	const math::Vec2f m_frameSize;
	const uint32_t m_numFrames;
	const uint32_t m_speed;

	uint32_t m_currentTick;
	uint32_t m_currentFrame;
	math::Vec2f m_topLeftFrameCoord;
};