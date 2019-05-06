#version 440 core

layout(location = 0) out vec4 color;

uniform vec4 u_color;
uniform vec2 normal;
uniform float halfWidth;

void main() {
	float dist = abs(normal.x*gl_FragCoord.x + normal.y*gl_FragCoord.y);
	float attenuation = 0.1f ;
	float r = u_color.r;
	r *= attenuation;
	color = vec4(u_color.r*attenuation , u_color.g*attenuation , u_color.b*attenuation , 1 );
}
