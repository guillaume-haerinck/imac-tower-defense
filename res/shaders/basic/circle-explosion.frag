#version 440 core

layout(location = 0) out vec4 color;

uniform vec2 u_pos;
uniform float u_radius;
uniform float u_maxR;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	float midRadiusRatio = 0.25 ;
	float d = sqrt( (gl_FragCoord.x-u_pos.x)*(gl_FragCoord.x-u_pos.x) + (gl_FragCoord.y-u_pos.y)*(gl_FragCoord.y-u_pos.y) );
	float n_d = d/u_radius;
	if( n_d < (1-2*midRadiusRatio)){
		discard;
	}
	vec4 colorRed = vec4(0.8f, 0.2f, 0.1f, 1.0f) ;
	vec4 colorYellow = vec4(0.78f, 0.3f, 0.13f, 1.0f) ;
	float rand = rand(gl_FragCoord.xy) ;
	color = colorRed * rand + colorYellow *(1-rand);
	color.a = (1 - abs(n_d - (1-midRadiusRatio))/midRadiusRatio) * (1 - u_radius/u_maxR) ;
}
