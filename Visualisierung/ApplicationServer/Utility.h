#include <fstream>
#include <sstream>

std::string get_file_as_string(const char* filename)
{
	std::ifstream file(filename);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}