#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glfw_wrappers.hh"
#include "glew_wrappers.hh"

void gl_info ()
{
	auto renderer = gl_get_string (GL_RENDERER);
	auto version = gl_get_string (GL_VERSION);
	log ("Renderer: ", renderer);
	log ("OpenGL version supported: ", version);
}

int main ()
{
	glfw_manager glfw;
	glfw_window window (640, 480, "Hello Triangle", NULL, NULL);
	::glfwMakeContextCurrent (window);
	glew_manager glew;

	gl_info ();

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	::glEnable (GL_DEPTH_TEST);
	::glDepthFunc (GL_LESS);

	/* OTHER STUFF GOES HERE NEXT */

	return EXIT_SUCCESS;
}
