#version 330 core
in vec2 TexCoords;
in vec4 textColor;

out vec4 color;

uniform sampler2D text;

void main()
{   
	//color = vec4(1.0, 0.0, 0.0, 1.0);
    float sampled = texture(text, TexCoords).r;
    color = vec4(textColor.rgb, sampled * textColor.a);
}