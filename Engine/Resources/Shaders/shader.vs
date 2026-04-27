#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

uniform mat4 uModel;
uniform mat4 uViewProj;
uniform mat3 uNormal;

void main()
{
	vec4 position = uModel * vec4(aPos, 1.0);
    gl_Position = uViewProj * position;

	vPosition = position.xyz;
	vNormal = normalize(uNormal * aNormal);
	vTexCoord = aTexCoord;
}