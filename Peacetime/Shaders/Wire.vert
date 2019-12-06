#version 330
in vec2 offset;
in vec4 color;
in vec2 position;
in float rotationAngle;

uniform mat4 perspective;

out vec4 outputColor;

vec2 rotate(vec2 p, float angle, vec2 rp) {
    float s = sin(angle);
    float c = cos(angle);
    // translate point back to origin:
    p.x -= rp.x;
    p.y -= rp.y;
    // rotate point
    float xnew = p.x * c - p.y * s;
    float ynew = p.x * s + p.y * c;
    // translate point back:
    p.x = xnew + rp.x;
    p.y = ynew + rp.y;
    return p;
}

void main() {
	gl_Position = perspective * vec4(
		rotate(position + offset, rotationAngle, position),
		1.0,
		1.0
	);
	outputColor = color;
}