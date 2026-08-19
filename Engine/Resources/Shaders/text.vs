#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec4 aColor;

out vec2 TexCoords;
out vec4 textColor;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos, 1.0);
    TexCoords = aTex;
	textColor = aColor;
}