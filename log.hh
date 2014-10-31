#ifndef ERROR_HH
#define ERROR_HH

#include <iostream>
#include <cstdlib>

static inline
std::ostream& print (std::ostream& os)
{
	return os;
}

template <typename First, typename... Rest>
static inline
std::ostream& print (std::ostream& os, First&& first, Rest&&... rest)
{
	return print (os << std::forward <First> (first), std::forward <Rest> (rest)...);
}

template <typename... Args>
[[noreturn]]
static inline
void error (Args&&... args)
{
	print (std::cerr, "Error: ", std::forward <Args> (args)...);
	std::cerr << std::endl;
	std::exit (EXIT_FAILURE);
}

template <typename... Args>
static inline
void log (Args&&... args)
{
	print (std::clog, std::forward <Args> (args)...);
	std::clog << std::endl;
}

#endif
