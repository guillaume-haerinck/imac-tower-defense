#version 440 core

layout(location = 0) out vec4 color;

uniform vec4 u_color;
uniform vec2 u_pos;
uniform float u_radius;

void main() {
	float midRadiusRatio = 0.1 ;
	float d = sqrt( (gl_FragCoord.x-u_pos.x)*(gl_FragCoord.x-u_pos.x) + (gl_FragCoord.y-u_pos.y)*(gl_FragCoord.y-u_pos.y) );
	float n_d = d/u_radius;
	if( n_d < (1-2*midRadiusRatio)){
		discard;
	}
	color = u_color;
	color.a = 1 - abs(n_d - (1-midRadiusRatio))/midRadiusRatio ;
}
