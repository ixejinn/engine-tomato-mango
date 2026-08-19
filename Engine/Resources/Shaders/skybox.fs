#version 450 core

out vec4 FragColor;

in vec3 vTexDir;

uniform samplerCube uCubemap;

void main()
{
	FragColor = texture(uCubemap, vTexDir);
}