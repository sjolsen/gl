#version 130

in vec2 vp;
out vec2 pos;
void main () {
	gl_Position = vec4 (vp, 0.0, 1.0);
	pos = vp;
}
