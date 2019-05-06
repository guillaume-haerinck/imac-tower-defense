#version 440 core

layout(location = 0) out vec4 color;

uniform vec4 u_color;
uniform vec2 normal;
uniform vec2 startPos;
uniform float halfWidth;

void main() {
	float dist = abs(normal.x*(gl_FragCoord.x-startPos.x) + normal.y*(gl_FragCoord.y-startPos.y));
	float attenuation = 1-dist/halfWidth ;
	color = vec4(u_color.r*attenuation , u_color.g*attenuation , u_color.b*attenuation , attenuation );
}
