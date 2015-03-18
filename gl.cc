#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glfw_wrappers.hh"
#include "glew_wrappers.hh"
#include "read_file.hh"
#include <cmath>
#include <thread>
#include <chrono>
#include <cstring>
#include <random>

#define N 512

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

union RGBA
{
	struct {
		std::uint8_t red;
		std::uint8_t green;
		std::uint8_t blue;
		std::uint8_t alpha;
	};
	std::uint32_t value;
};

bool lifestate [N] [N] = {};
bool newlifestate [N] [N] = {};

void init_life ()
{
	std::mt19937 gen {};
	std::uniform_int_distribution <unsigned short> dist (0, 1);
	for (int row = 0; row < N; ++row)
		for (int col = 0; col < N; ++col)
			lifestate [row] [col] = (dist (gen) != 0);
}

bool safe_state (int row, int col)
{
	while (row < 0) row += N;
	while (col < 0) col += N;
	row %= N;
	col %= N;
	return lifestate [row] [col];
}

void step_life ()
{
	for (int row = 0; row < N; ++row)
		for (int col = 0; col < N; ++col)
		{
			int neighbours = 0;
			if (safe_state (row - 1, col - 1)) ++neighbours;
			if (safe_state (row - 1, col))     ++neighbours;
			if (safe_state (row - 1, col + 1)) ++neighbours;
			if (safe_state (row,     col - 1)) ++neighbours;
			if (safe_state (row,     col + 1)) ++neighbours;
			if (safe_state (row + 1, col - 1)) ++neighbours;
			if (safe_state (row + 1, col))     ++neighbours;
			if (safe_state (row + 1, col + 1)) ++neighbours;

			if (lifestate [row] [col])
				newlifestate [row] [col] = (neighbours == 2 || neighbours == 3);
			else
				newlifestate [row] [col] = (neighbours == 3);
		}
	memcpy (lifestate, newlifestate, sizeof (lifestate));
}

RGBA texdata [N] [N];
void fill_texdata ()
{
	for (int row = 0; row < N; ++row)
		for (int col = 0; col < N; ++col)
		{
			std::uint8_t value = lifestate [row] [col] ? 0 : 0xFF;
			texdata [row] [col] = {
				value,
				value,
				value,
				static_cast <std::uint8_t> (0xFF)
			};
		}
}

void hello_texture (GLFWwindow* window)
{
	float points [3 * 4] = {
		-1.0f, -1.0f, // Lower left
		 1.0f, -1.0f, // Lower right
		 1.0f,  1.0f, // Upper right
		-1.0f,  1.0f  // Upper left
	};
	float texpoints [3 * 4] = {
		0.0f, 0.0f, // Lower left
		1.0f, 0.0f, // Lower right
		1.0f, 1.0f, // Upper right
		0.0f, 1.0f  // Upper left
	};

	GLuint vertex_attributes = 0;
	::glGenVertexArrays (1, &vertex_attributes);
	::glBindVertexArray (vertex_attributes);
	::glEnableVertexAttribArray (0);
	::glEnableVertexAttribArray (1);

	GLuint vertex_buffers [2] = {};
	::glGenBuffers (2, vertex_buffers);
	::glBindBuffer (GL_ARRAY_BUFFER, vertex_buffers [0]);
	::glBufferData (GL_ARRAY_BUFFER, sizeof (points), points, GL_STATIC_DRAW);
	::glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	::glBindBuffer (GL_ARRAY_BUFFER, vertex_buffers [1]);
	::glBufferData (GL_ARRAY_BUFFER, sizeof (texpoints), texpoints, GL_STATIC_DRAW);
	::glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint vertex_shader = ::glCreateShader (GL_VERTEX_SHADER);
	{
		const auto vs_source = read_file ("flat.vert");
		const char* source [] = {vs_source.data ()};
		::glShaderSource (vertex_shader, 1, source, NULL);
		::glCompileShader (vertex_shader);
		check_shader (vertex_shader);
	}

	GLuint fragment_shader = ::glCreateShader (GL_FRAGMENT_SHADER);
	{
		const auto fs_source = read_file ("flat.frag");
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
	::glBindVertexArray (vertex_attributes);

	GLuint texname = 0;
	::glGenTextures (1, &texname);
	::glActiveTexture (GL_TEXTURE0);
	::glBindTexture (GL_TEXTURE_2D, texname);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GLint texmap = ::glGetUniformLocation (program, "texmap");
	::glUniform1i (texmap, 0);

	init_life ();

	while (!::glfwWindowShouldClose (window))
	{
		auto time = std::chrono::steady_clock::now ();
		fill_texdata ();
		::glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, N, N, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata);
		::glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		::glDrawArrays (GL_TRIANGLE_FAN, 0, 4);
		::glfwPollEvents ();
		::glfwSwapBuffers (window);
		step_life ();
		std::this_thread::sleep_until (time + std::chrono::duration <int, std::ratio <1, 60>> (1));
	}
}

int main ()
{
	glfw_manager glfw;
	glfw_window window (N, N, "Hello Texture", NULL, NULL);
	::glfwSetWindowSizeCallback (window, [] (GLFWwindow*, int width, int height) {
		::glViewport (0, 0, width, height);
	});
	::glfwMakeContextCurrent (window);
	glew_manager glew;

	gl_info ();
	check_gl ("Init");
	hello_texture (window);

	return EXIT_SUCCESS;
}
