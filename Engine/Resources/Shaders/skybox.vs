#version 450 core
layout(location = 0) in vec3 aPos;

out vec3 vTexDir;

uniform mat4 uModel;
uniform mat4 uViewProj;

void main()
{
	vec4 position = uModel * vec4(aPos, 1.0);
    gl_Position = uViewProj * position;

	vTexDir = aPos;
}