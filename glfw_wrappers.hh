#ifndef GLFW_WRAPPERS_HH
#define GLFW_WRAPPERS_HH

#include <GLFW/glfw3.h>
#include "log.hh"

struct glfw_manager
{
	glfw_manager ()
	{
		if (!::glfwInit ())
			error ("glfwInit");
	}

	glfw_manager (glfw_manager&&) = delete;
	glfw_manager (const glfw_manager&) = delete;
	glfw_manager& operator = (glfw_manager&&) = delete;
	glfw_manager& operator = (const glfw_manager&) = delete;

	~glfw_manager ()
	{
		::glfwTerminate ();
	}
};

class glfw_window
{
	GLFWwindow* _window;

public:
	glfw_window (int width, int height,
	             const char* title,
	             GLFWmonitor* monitor,
	             GLFWwindow* share)
		: _window {::glfwCreateWindow (width, height, title, monitor, share)}
	{
		if (_window == nullptr)
			error ("glfwCreateWindow");
	}

	glfw_window (glfw_window&& other) noexcept
		: _window {other._window}
	{
		other._window = nullptr;
	}
	glfw_window (const glfw_window&) = delete;

	glfw_window& operator = (glfw_window&& other) noexcept
	{
		_window = other._window;
		other._window = nullptr;
		return *this;
	}
	glfw_window& operator = (const glfw_window&) = delete;

	operator GLFWwindow* () const noexcept
	{
		return _window;
	}

	~glfw_window ()
	{
		if (_window != nullptr)
			::glfwDestroyWindow (_window);
	}
};

#endif
