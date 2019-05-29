#version 440 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2D u_texture;
uniform vec4 tintColour ;
uniform float u_alpha ;
uniform float probaDisappear ;

float rand(){
	return 0 ;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	// TODO Better way to handle transparency 
	// https://learnopengl.com/Advanced-OpenGL/Blending
	vec4 texColor = vec4( texture(u_texture, v_texCoord).rgb*(1-tintColour.a) + tintColour.rgb*tintColour.a , texture(u_texture, v_texCoord).a*u_alpha );
	if( rand(gl_FragCoord.xy) < probaDisappear ){
		discard;
	}
	color = texColor;
}
