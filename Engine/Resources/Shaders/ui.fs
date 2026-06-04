#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D tex;
uniform vec4 uColor;

void main()
{   
	//color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	color = texture(tex, TexCoords) * uColor;
}