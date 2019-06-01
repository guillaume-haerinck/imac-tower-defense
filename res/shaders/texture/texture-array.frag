#version 440 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2DArray u_texture;
uniform int u_activeTile;
uniform vec4 tintColour ;
uniform float u_alpha ;
uniform float probaDisappear ;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	vec4 texColor = texture(u_texture, vec3(v_texCoord, u_activeTile));
	texColor = vec4( texColor.rgb*(1-tintColour.a) + tintColour.rgb*tintColour.a , texColor.a*u_alpha );
	if( rand(gl_FragCoord.xy) < probaDisappear ){
		discard;
	}
	color = texColor;
}
