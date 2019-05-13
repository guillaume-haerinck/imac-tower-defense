#version 440 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2DArray u_texture;
uniform int u_activeTile;
uniform vec4 tintColour ;

void main() {
	vec4 texColor = texture(u_texture, vec3(v_texCoord, u_activeTile));
	texColor = vec4( texColor.rgb*(1-tintColour.a) + tintColour.rgb*tintColour.a , texColor.a );
	if (texColor.a < 0.1) {
		discard;
	}
	color = texColor;
}
