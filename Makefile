.PHONY: all test clean
all: gl
test: gl
	@./gl
clean:
	rm -f gl

gl: gl.cc glfw_wrappers.hh glew_wrappers.hh log.hh read_file.hh
	g++ -std=c++14 -O3 ${CXXFLAGS} $< -o $@ `pkg-config --libs --cflags glfw3 glew`
