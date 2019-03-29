#version 440 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2D u_texture;

void main() {
	// TODO Better way to handle transparency 
	// https://learnopengl.com/Advanced-OpenGL/Blending
	vec4 texColor = texture(u_texture, v_texCoord);
	if (texColor.a < 0.1) {
		discard;
	}
	color = texColor;
}
