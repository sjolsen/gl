#version 130

#define pi 3.14159265358979

float vector_norm (vec2 x)
{
	if (x.x == 0.0 && x.y == 0.0)
		return 0.0;
	return sqrt (x.x*x.x + x.y*x.y);
}

vec2 unit_vector2 (vec2 x)
{
	float xnorm = vector_norm (x);
	vec2 result = {x.x / xnorm, x.y / xnorm};
	return result;
}

float dot_product (vec2 x, vec2 y)
{
	return x.x*y.x + x.y*y.y;
}

vec2 projection (vec2 x, vec2 y)
{
	vec2 uy = unit_vector2 (y);
	return dot_product (x, uy) * uy;
}

float theta (vec2 x)
{
	if (x.x == 0.0 && x.y == 0.0)
		return 0.0;
	vec2 y = unit_vector2 (x);
	if (y.x == 0.0) {
		if (y.y > 0)
			return pi / 2;
		return 3 * pi / 2;
	}
	if (y.y < 0.0)
		return 2*pi - acos (y.x);
	return acos (y.x);
}

vec2 untheta (float t)
{
	return vec2 (cos (t), sin (t));
}

in vec2 pos;
out vec4 frag_color;
void main () {
	vec2 red = untheta (0.0);
	vec2 green = untheta (2*pi / 3);
	vec2 blue = untheta (4*pi / 3);
	frag_color = vec4 (
		vector_norm (projection (pos, red)),
		vector_norm (projection (pos, green)),
		vector_norm (projection (pos, blue)),
		1.0
	);
}
