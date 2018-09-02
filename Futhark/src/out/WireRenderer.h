#pragma once
#include <GLEW/glew.h>
#include <vector>
#include "../up/World.h"
#include "../in/FileCache.h"
#include "Camera.h"
namespace fk {


class WireRenderer {
  public:
	  WireRenderer() = default;
	~WireRenderer() = default;
	/* Sets the shaders for this renderer.
	(shaders) The shaders to use. DO NOT USE MORE THAN ONE OF EACH TYPE!
	[t3chma] */
	void setShaders(std::vector<Shader>& shaders);
	/* Renders a sprite batch from the perspective of the given camera.
	(batch) The sprite batch to render.
	(cam) The perspective to use.
	[t3chma] */
	void render(World& world, glm::mat4& perspective, const float WIRE_WIDTH = 1);
  private:
	GLuint m_id{ 0 };
	GLuint m_camLocation{ 0 };
};

}