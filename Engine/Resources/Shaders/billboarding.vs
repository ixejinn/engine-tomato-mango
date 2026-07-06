#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;
out vec4 vColor;

uniform mat4 uModel;
uniform mat4 uViewProj;
uniform mat3 uNormal;

void main()
{
    vec3 r = normalize(vec3(uViewProj[0][0], uViewProj[1][0], uViewProj[2][0]));
    vec3 u = normalize(vec3(uViewProj[0][1], uViewProj[1][1], uViewProj[2][1]));
    vec3 localPos = aPos.x * r + aPos.y * u;
    vColor = vec4(r, 1.0);

	vec4 position = uModel * vec4(localPos, 1.0);
	//vec4 position = uModel * vec4(aPos, 1.0);
    gl_Position = uViewProj * position;

	vPosition = position.xyz;
	vNormal = normalize(uNormal * aNormal);
	vTexCoord = aTexCoord;
}