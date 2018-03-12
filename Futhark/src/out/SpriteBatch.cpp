#include "SpriteBatch.h"
#include "../base/Error.h"
#include "GLM/gtx/vector_angle.hpp"
namespace fk {


SpriteBatch::SpriteBatch() {
	TRY_GL(glGenVertexArrays(1, &m_vertexArrayObjectID));
	TRY_GL(glBindVertexArray(m_vertexArrayObjectID));
	TRY_GL(glGenBuffers(1, &m_vertexBufferObjectID));
	TRY_GL(glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectID));
	TRY_GL(glEnableVertexAttribArray(0));
	TRY_GL(glEnableVertexAttribArray(1));
	TRY_GL(glEnableVertexAttribArray(2));
	TRY_GL(glEnableVertexAttribArray(3));
	TRY_GL(glEnableVertexAttribArray(4));
	TRY_GL(glEnableVertexAttribArray(5));
	TRY_GL(glEnableVertexAttribArray(6));
	TRY_GL(glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, sizeof(Canvas), (void*)offsetof(Canvas, position)
	));
	TRY_GL(glVertexAttribPointer(
		1, 2, GL_FLOAT, GL_FALSE, sizeof(Canvas), (void*)offsetof(Canvas, dimensions)
	));
	TRY_GL(glVertexAttribPointer(
		2, 2, GL_FLOAT, GL_FALSE, sizeof(Canvas), (void*)offsetof(Canvas, texturePosition)
	));
	TRY_GL(glVertexAttribPointer(
		3, 2, GL_FLOAT, GL_FALSE, sizeof(Canvas), (void*)offsetof(Canvas, textureDimensions)
	));
	TRY_GL(glVertexAttribPointer(
		4, 2, GL_FLOAT, GL_FALSE, sizeof(Canvas), (void*)offsetof(Canvas, rotationAxis)
	));
	TRY_GL(glVertexAttribPointer(
		5, 1, GL_FLOAT, GL_FALSE, sizeof(Canvas), (void*)offsetof(Canvas, rotationAngle)
	));
	TRY_GL(glVertexAttribPointer(
		6, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Canvas), (void*)offsetof(Canvas, color)
	));
	TRY_GL(glBindVertexArray(0));
	TRY_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
SpriteBatch::Sprite SpriteBatch::makeSprite(const Texture& texture, const int frames) {
	m_modified = true;
	m_buffered = false;
	auto&& buffer = m_bufferMap[texture];
	buffer.emplace_back();
	return Sprite(&buffer, buffer.size() - 1, texture, frames, this);
}
void SpriteBatch::killSprite(Sprite& sprite) {
	m_modified = true;
	m_buffered = false;
	sprite.m_bufferPtr->at(sprite.m_bufferIndex) = sprite.m_bufferPtr->back();
	sprite.m_bufferPtr->pop_back();
	sprite.m_bufferPtr = nullptr;
	sprite.m_bufferIndex = -1;
	sprite.m_batchPtr = nullptr;
}
void SpriteBatch::flagRebuffer() { m_buffered = false; }
void SpriteBatch::m_render() {
	TRY_GL(glBindVertexArray(m_vertexArrayObjectID));
	if (!m_buffered) {
		// Copy relevant sorted data to local buffer data.
		if (m_modified) {
			// Get new offsets.
			int offset = 0;
			for (auto&& buffer : m_bufferMap) {
				buffer.second.offset = offset;
				buffer.second.modified = false;
				offset += buffer.second.size();
				m_vertexBuffer.insert(m_vertexBuffer.end(), buffer.second.begin(), buffer.second.end());
			}
			m_modified = false;
		}
		else {
			int offset = 0;
			for (auto&& buffer : m_bufferMap) {
				if (buffer.second.modified) {
					for (int i = 0; i < buffer.second.size(); ++i) { m_vertexBuffer[offset + i] = buffer.second[i]; }
				}
				offset += buffer.second.size();
			}
		}
		// Buffer data in GPU.
		TRY_GL(glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectID));
		TRY_GL(
			glBufferData(
				GL_ARRAY_BUFFER,
				sizeof(Canvas) * m_vertexBuffer.size(),
				m_vertexBuffer.data(),
				GL_DYNAMIC_DRAW
			)
		);
		m_buffered = true;
		TRY_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
	// Draw data.
	for (auto&& buffer : m_bufferMap) {
		TRY_GL(glBindTexture(GL_TEXTURE_2D, buffer.first.id));
		TRY_GL(glDrawArrays(GL_POINTS, buffer.second.offset, buffer.second.size()));
	}
	TRY_GL(glBindTexture(GL_TEXTURE_2D, 0));
	TRY_GL(glBindVertexArray(0));
}


SpriteBatch::Sprite::Sprite(
	SpriteBatch::TBuffer* const bufferPtr,
	const int bufferIndex,
	const Texture& texturePtr,
	const unsigned int frames,
	SpriteBatch* const batchPtr
)
	: m_bufferPtr(bufferPtr),
	m_bufferIndex(bufferIndex),
	m_texture(texturePtr),
	m_frames(frames),
	m_batchPtr(batchPtr) {
	if (frames < 1) { m_frames = 1; }
	(*m_bufferPtr)[m_bufferIndex].textureDimensions /= m_frames;
}
void SpriteBatch::Sprite::move(const glm::vec2& translation) {
	auto& canvas = getCanvasRef();
	canvas.position.x += translation.x;
	canvas.position.y += translation.y;
}
void SpriteBatch::Sprite::move(const float x, const float y) {
	auto& canvas = getCanvasRef();
	canvas.position.x += x;
	canvas.position.y += y;
}
glm::vec2 SpriteBatch::Sprite::getPosition() const {
	auto& canvas = getCanvasConstRef();
	return glm::vec2(canvas.position.x, canvas.position.y);
}
void SpriteBatch::Sprite::setPosition(const glm::vec2& position) {
	auto& canvas = getCanvasRef();
	canvas.position.x = position.x;
	canvas.position.y = position.y;
}
void SpriteBatch::Sprite::setPosition(const float x, const float y) {
	auto& canvas = getCanvasRef();
	canvas.position.x = x;
	canvas.position.x = x;
}
float SpriteBatch::Sprite::getDepth() const {
	return getCanvasConstRef().position.z;
}
void SpriteBatch::Sprite::setDepth(const float depth) {
	getCanvasRef().position.z = depth;
}
glm::vec2 SpriteBatch::Sprite::getDimensions() const {
	return getCanvasConstRef().dimensions;
}
void SpriteBatch::Sprite::setDimensions(const glm::vec2& dimensions) {
	getCanvasRef().dimensions = dimensions;
}
void SpriteBatch::Sprite::setDimensions(const float width, const float height) {
	auto& canvas = getCanvasRef();
	canvas.dimensions.x = width;
	canvas.dimensions.y = height;
}
float SpriteBatch::Sprite::getRotation() const {
	return getCanvasConstRef().rotationAngle;
}
void SpriteBatch::Sprite::setRotation(const float angle) {
	getCanvasRef().rotationAngle = angle;
}
glm::vec2 SpriteBatch::Sprite::getRotationAxis() const {
	return getCanvasConstRef().rotationAxis;
}
void SpriteBatch::Sprite::setRotationAxis(const glm::vec2& rotationAxis) {
	getCanvasRef().rotationAxis = rotationAxis;
}
void SpriteBatch::Sprite::setRotationAxis(const float x, const float y) {
	auto& canvas = getCanvasRef();
	canvas.rotationAxis.x = x;
	canvas.rotationAxis.y = y;
}
Color SpriteBatch::Sprite::getColor() const {
	return getCanvasConstRef().color;
}
void SpriteBatch::Sprite::setColor(const Color& color) {
	getCanvasRef().color = color;
}
void SpriteBatch::Sprite::setColor(const char r, const char g, const char b, const char a) {
	auto& canvas = getCanvasRef();
	canvas.color.r = r;
	canvas.color.g = g;
	canvas.color.b = b;
	canvas.color.a = a;
}
glm::vec2 SpriteBatch::Sprite::getTexturePosition() const {
	return getCanvasConstRef().texturePosition;
}
void SpriteBatch::Sprite::setTexturePosition(const glm::vec2& tPosition) {
	getCanvasRef().texturePosition = tPosition;
}
void SpriteBatch::Sprite::setTexturePosition(const float x, const float y) {
	auto& canvas = getCanvasRef();
	canvas.texturePosition.x = x;
	canvas.texturePosition.y = y;
}
glm::vec2 SpriteBatch::Sprite::getTextureDimensions() const {
	return getCanvasConstRef().textureDimensions;
}
void SpriteBatch::Sprite::setTextureDimensions(const glm::vec2& tDimensions) {
	getCanvasRef().textureDimensions = tDimensions;
}
void SpriteBatch::Sprite::setTextureDimensions(const float width, const float height) {
	auto& canvas = getCanvasRef();
	canvas.textureDimensions.x = width;
	canvas.textureDimensions.y = height;
}
void SpriteBatch::Sprite::setFrame(const int frame) {
	auto& canvas = getCanvasRef();
	canvas.texturePosition.x = canvas.textureDimensions.x * frame;
}
SpriteBatch::Canvas& SpriteBatch::Sprite::getCanvasRef() {
	m_bufferPtr->modified = true;
	m_batchPtr->flagRebuffer();
	return (*m_bufferPtr)[m_bufferIndex];
}

void SpriteBatch::Sprite::setTexture(const Texture& texture, const int frames) {
	m_bufferPtr->at(m_bufferIndex) = m_bufferPtr->back();
	m_bufferPtr->pop_back();
	*this = m_batchPtr->makeSprite(texture, frames);
}

const SpriteBatch::Canvas& SpriteBatch::Sprite::getCanvasConstRef() const {
	return (*m_bufferPtr)[m_bufferIndex];
}
Texture SpriteBatch::Sprite::getTextureCopy() const { return m_texture; }
SpriteBatch* SpriteBatch::Sprite::getBatchPtr() const { return m_batchPtr; }

bool SpriteBatch::Sprite::isAlive() const { return m_bufferPtr; }

void SpriteBatch::Sprite::kill() { m_batchPtr->killSprite(*this); }

void SpriteBatch::Sprite::resurrect() {
	*this = m_batchPtr->makeSprite(m_texture, m_frames);
}

void SpriteBatch::Sprite::makeLine(glm::vec2& b, glm::vec2& a, float thickness) {
	Canvas& canvas = getCanvasRef();
	canvas.dimensions.y = glm::distance(b, a);
	canvas.dimensions.x = thickness;
	glm::vec2 difVec{ a - b };
	canvas.position.x = b.x + difVec.x/2;
	canvas.position.y = b.y + difVec.y/2;
	canvas.rotationAxis.x = canvas.position.x;
	canvas.rotationAxis.y = canvas.position.y;
	canvas.rotationAngle = glm::orientedAngle(glm::vec2(0, 1), glm::normalize(difVec));
}

}