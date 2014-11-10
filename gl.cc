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

void check_shader (GLuint shader)
{
	GLint compile_status = GL_FALSE;
	::glGetShaderiv (shader, GL_COMPILE_STATUS, &compile_status);

	if (compile_status != GL_TRUE)
	{
		GLint log_length = 0;
		::glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &log_length);

		if (log_length != 0)
		{
			std::string compile_log;
			compile_log.resize (log_length);
			::glGetShaderInfoLog (shader, log_length, &log_length, &(compile_log [0]));
			compile_log.resize (log_length);
			log (compile_log);
		}
		error ("Compiling failed");
	}
}

void check_program (GLuint program)
{
	GLint link_status = GL_FALSE;
	::glGetProgramiv (program, GL_LINK_STATUS, &link_status);

	if (link_status != GL_TRUE)
	{
		GLint log_length = 0;
		::glGetProgramiv (program, GL_INFO_LOG_LENGTH, &log_length);

		if (log_length != 0)
		{
			std::string link_log;
			link_log.resize (log_length);
			::glGetProgramInfoLog (program, log_length, &log_length, &(link_log [0]));
			link_log.resize (log_length);
			log (link_log);
		}
		error ("Linking failed");
	}
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

	GLuint vertex_buffer = 0;
	::glGenBuffers (1, &vertex_buffer);
	::glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
	::glBufferData (GL_ARRAY_BUFFER, sizeof (points), points, GL_STATIC_DRAW);
	::glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint vertex_shader = ::glCreateShader (GL_VERTEX_SHADER);
	{
		const auto vs_source = read_file ("triangle.vert");
		const char* source [] = {vs_source.data ()};
		::glShaderSource (vertex_shader, 1, source, NULL);
		::glCompileShader (vertex_shader);
		check_shader (vertex_shader);
	}

	GLuint fragment_shader = ::glCreateShader (GL_FRAGMENT_SHADER);
	{
		const auto fs_source = read_file ("triangle.frag");
		const char* source [] = {fs_source.data ()};
		::glShaderSource (fragment_shader, 1, source, NULL);
		::glCompileShader (fragment_shader);
		check_shader (fragment_shader);
	}

	GLuint program = ::glCreateProgram ();
	::glAttachShader (program, vertex_shader);
	::glAttachShader (program, fragment_shader);
	::glLinkProgram (program);
	check_program (program);

	::glUseProgram (program);
	check_gl ("Use Program");
	::glBindVertexArray (vertex_attributes);
	check_gl ("Bind Vertex Array");

	while (!::glfwWindowShouldClose (window))
	{
		::glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		check_gl ("Clear");
		::glDrawArrays (GL_TRIANGLES, 0, 3);
		check_gl ("Draw Arrays");
		::glfwPollEvents ();
		check_gl ("Poll Events");
		::glfwSwapBuffers (window);
		check_gl ("Swap Buffers");
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
