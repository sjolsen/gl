#ifndef GLEW_WRAPPERS_HH
#define GLEW_WRAPPERS_HH

#include <GL/glew.h>
#include <experimental/string_view>
#include "log.hh"

struct glew_manager
{
	glew_manager ()
	{
		if (::glewInit ())
			error ("glewInit");
	}

	glew_manager (glew_manager&&) = delete;
	glew_manager (const glew_manager&) = delete;
	glew_manager& operator = (glew_manager&&) = delete;
	glew_manager& operator = (const glew_manager&) = delete;
};

std::experimental::string_view gl_get_string (GLenum name)
{
	const GLubyte* s = ::glGetString (name);
	if (s == nullptr)
		error ("glGetString");
	return {reinterpret_cast <const char*> (s)};
}

#endif
