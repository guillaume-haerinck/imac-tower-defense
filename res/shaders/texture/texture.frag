#version 440 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2D u_texture;
uniform vec4 tintColour ;

void main() {
	// TODO Better way to handle transparency 
	// https://learnopengl.com/Advanced-OpenGL/Blending
	vec4 texColor = vec4( texture(u_texture, v_texCoord).rgb*(1-tintColour.a) + tintColour.rgb*tintColour.a , texture(u_texture, v_texCoord).a );
	if (texColor.a < 0.001) {
		discard;
	}
	color = texColor;
}
