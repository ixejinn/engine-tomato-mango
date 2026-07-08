#version 450 core

out vec4 FragColor;

in vec2 vTexCoord;
in vec4 vColor;

uniform sampler2D uTexture;
uniform vec4 uColor;

void main()
{
    FragColor = texture(uTexture, vTexCoord) * uColor;
}