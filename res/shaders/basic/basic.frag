#version 440 core

layout(location = 0) out vec4 color;

uniform vec4 u_color;
uniform vec4 tintColour ;

void main() {
	color = u_color;
	color = vec4( color.rgb*(1-tintColour.a) + tintColour.rgb*tintColour.a , color.a );
}
