#include "Animation.h"

#include <Core/graphics/Texture.h>

Animation::Animation(const Texture* texture, const math::Vec2f frameSize, const uint32_t numFrames, const uint32_t speed) :
	m_texture(texture)
	, m_frameSize(frameSize)
	, m_numFrames(numFrames)
	, m_speed(speed)
	, m_currentTick(0)
	, m_currentFrame(0)
{
}

void Animation::TryAdvanceFrame() {

	++m_currentTick;
	if (m_currentTick % m_speed != 0) {
		return;
	}

	++m_currentFrame;
	if (m_currentFrame >= m_numFrames) {
		m_currentFrame = 0;
		m_topLeftFrameCoord = math::Vec2f::Zero();
		return;
	}

	m_topLeftFrameCoord[0] += m_frameSize[0];
	if (m_topLeftFrameCoord[0] >= m_texture->width) {
		m_topLeftFrameCoord[0] = 0;
		m_topLeftFrameCoord[1] += m_frameSize[1];
	}
}

const Texture* Animation::GetTexture() const {
	return m_texture;
}

const math::Vec2f Animation::GetTopLeftFrameCoord() const {
	return m_topLeftFrameCoord;
}

const math::Vec2f Animation::GetFrameSize() const {
	return m_frameSize;
}
