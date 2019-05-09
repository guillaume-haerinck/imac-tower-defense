#version 440 core

layout(location = 0) out vec4 color;

uniform vec4 u_color;
uniform vec2 normal;
uniform vec2 startPos;
uniform float halfWidth;

void main() {
	float dist = abs(normal.x*(gl_FragCoord.x-startPos.x) + normal.y*(gl_FragCoord.y-startPos.y));
	float attenuation = 1-dist/halfWidth ;
	float overflow = max( 1.15-1-dist/halfWidth ,0);
	color = u_color;
	color.a *= attenuation ;
	color += vec4( vec3(overflow) , 0 );
	//color = vec4(u_color.r*attenuation+overflow , u_color.g*attenuation+overflow , u_color.b*attenuation+overflow , attenuation );
}
