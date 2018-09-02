#include "WireRenderer.h"
#include "Error.h"
namespace fk {


void WireRenderer::setShaders(std::vector<Shader>& shaders) {
	if (m_id) { TRY_GL(glDeleteProgram(m_id)); }
	TRY_GL(m_id = glCreateProgram());
	TRY_GL(glBindAttribLocation(m_id, 0, "offset"));
	TRY_GL(glBindAttribLocation(m_id, 1, "color"));
	TRY_GL(glBindAttribLocation(m_id, 2, "position"));
	TRY_GL(glBindAttribLocation(m_id, 3, "rotationAngle"));
	// Get shaders
	for (auto&& shader : shaders) { TRY_GL(glAttachShader(m_id, shader.id)); }
	// Link the shaders
	TRY_GL(glLinkProgram(m_id));
	// Error checking
	int isLinked = 0;
	glGetProgramiv(m_id, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(m_id, maxLength, &maxLength, &errorLog[0]);
		glDeleteProgram(m_id);
		std::printf("%s\n", &(errorLog[0]));
		LOG_LINE("Failed to link shaders");
		m_id = 0;
	}
	for (auto&& shader : shaders) { glDetachShader(m_id, shader.id); }
	TRY_GL(m_camLocation = glGetUniformLocation(m_id, "perspective"));
}
void WireRenderer::render(World& world, glm::mat4& perspective, const float WIRE_WIDTH) {
	TRY_GL(glUseProgram(m_id));
	TRY_GL(glUniformMatrix4fv(m_camLocation, 1, GL_FALSE, &perspective[0][0]));
	TRY_GL(glLineWidth(WIRE_WIDTH));
	world.m_render();
	TRY_GL(glUseProgram(0));
}

}