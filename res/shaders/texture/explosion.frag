#version 440 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2DArray u_texture;
uniform int u_activeTile;

void main() {
	float gradStart = 0.4 ;
	float gradEnd = 0.7 ;
	vec4 texColor = texture(u_texture, vec3(v_texCoord, u_activeTile));
	if (texColor.a < gradStart) {
		discard;
	}
	else if( gradStart < texColor.a && texColor.a < gradEnd ){
		texColor *= (texColor.a-gradStart)/(gradEnd-gradStart);
		texColor.a = 1 ;
	}
	color = texColor;
}
