#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glfw_wrappers.hh"
#include "glew_wrappers.hh"
#include "read_file.hh"

void gl_info ()
{
	auto renderer = gl_get_string (GL_RENDERER);
	auto version = gl_get_string (GL_VERSION);
	log ("Renderer: ", renderer);
	log ("OpenGL version supported: ", version);
}

void check_gl (const std::string& where)
{
	const auto e = ::glGetError ();
	if (e == GL_NO_ERROR)
		return;

	const auto es = ::gluErrorString (e);
	if (es == nullptr)
		error ("invalid error enum");
	error ("GL (", where, "): ", es);
}

void hello_triangle (GLFWwindow* window)
{
	const float points [] = {
		0.0f,  0.5f,  0.0f,
		0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f
	};

	GLuint vertex_attributes = 0;
	::glGenVertexArrays (1, &vertex_attributes);
	::glBindVertexArray (vertex_attributes);
	::glEnableVertexAttribArray (0);
	check_gl ("VAO");

	GLuint vertex_buffer = 0;
	::glGenBuffers (1, &vertex_buffer);
	::glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
	::glBufferData (GL_ARRAY_BUFFER, sizeof (points), points, GL_STATIC_DRAW);
	::glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	check_gl ("VBO");

	GLuint vs = ::glCreateShader (GL_VERTEX_SHADER);
	{
		const auto vs_source = read_file ("triangle.vert");
		const char* source [] = {vs_source.data ()};
		::glShaderSource (vs, 1, source, NULL);
		::glCompileShader (vs);
	}
	check_gl ("VS");

	GLuint fs = ::glCreateShader (GL_FRAGMENT_SHADER);
	{
		const auto fs_source = read_file ("triangle.frag");
		const char* source [] = {fs_source.data ()};
		::glShaderSource (fs, 1, source, NULL);
		::glCompileShader (fs);
	}
	check_gl ("FS");

	GLuint shader = ::glCreateProgram ();
	::glAttachShader (shader, vs);
	::glAttachShader (shader, fs);
	::glLinkProgram (shader);
	check_gl ("Compile");

	while (!::glfwWindowShouldClose (window))
	{
		::glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		check_gl ("Clear");
		::glUseProgram (shader);
		::glBindVertexArray (vertex_attributes);
		::glDrawArrays (GL_TRIANGLES, 0, 3);
		::glfwPollEvents ();
		::glfwSwapBuffers (window);
	}
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
	check_gl ("Init");

	hello_triangle (window);

	return EXIT_SUCCESS;
}
