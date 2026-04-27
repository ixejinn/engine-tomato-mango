#version 450 core

out vec4 FragColor;

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

uniform sampler2D uTexture;
uniform vec3 uLightPos;
uniform vec4 uColor;

void main()
{
	float amb = 0.2;

	vec3 n = normalize(vNormal);
	vec3 l = normalize(uLightPos - vPosition);
	//float diff = max(dot(n, l), 0.0);						// Lambert
	float diff = clamp(dot(n, l) * 0.5 + 0.5, 0.0, 1.0);	// Half Lambert

	vec4 texColor = texture(uTexture, vTexCoord) * uColor;
	vec3 ambient = amb * texColor.rgb;
	vec3 diffuse = diff * texColor.rgb;
	
	FragColor = vec4(ambient + diffuse, texColor.a);
}