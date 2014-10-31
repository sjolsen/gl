#ifndef READ_FILE_HH
#define READ_FILE_HH

#include <fstream>
#include "log.hh"

std::string read_file (const std::string& path)
{
	std::ifstream fin (path, std::ios::in | std::ios::binary | std::ios::ate);
	if (!fin.good ())
		error ("read_file (", path, ")");

	const auto length = fin.tellg ();
	fin.seekg (0, std::ios::beg);

	std::string contents;
	contents.resize (length);
	fin.read (&*contents.begin (), length);
	return contents;
}

#endif
