.PHONY: all test
all: gl
test: gl
	@./gl

gl: gl.cc glfw_wrappers.hh glew_wrappers.hh log.hh
	g++ -std=c++14 $< -o $@ `pkg-config --libs --cflags glfw3 glew`
