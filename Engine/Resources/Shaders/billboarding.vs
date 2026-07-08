#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;

uniform mat4 uModel;
uniform mat4 uViewProj;
uniform vec3 uCamRight;
uniform vec3 uCamUp;

void main()
{
    vec3 localPos = aPos.x * uCamRight + aPos.y * uCamUp;

	vec4 position = uModel * vec4(localPos, 1.0);
    gl_Position = uViewProj * position;

	vTexCoord = aTexCoord;
}