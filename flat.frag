#version 130

uniform sampler2D texmap;
in vec2 pos;
out vec4 frag_color;
void main () {
	frag_color = texture (texmap, pos);
}
