#version 130

in vec3 vp;
out vec2 pos;
void main () {
	gl_Position = vec4 (vp, 1.0);
	pos = vec2 (vp.x, vp.y);
}
