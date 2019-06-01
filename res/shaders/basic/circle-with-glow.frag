#version 440 core

layout(location = 0) out vec4 color;

uniform vec4 u_color;
uniform vec2 u_pos;
uniform float u_radius;
uniform float u_waveR;

void main() {
	float d = sqrt( (gl_FragCoord.x-u_pos.x)*(gl_FragCoord.x-u_pos.x) + (gl_FragCoord.y-u_pos.y)*(gl_FragCoord.y-u_pos.y) );
	float n_dToWave = abs(d-u_waveR)/u_radius;
	color = u_color*n_dToWave + vec4(1-n_dToWave);
	color.a = 1 - d/u_radius; ;
}
