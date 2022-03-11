#pragma once

#include <fstream>
#include <sstream>
#include <iostream>

std::string ReadFile(const std::string& filepath)
{
	std::ifstream file(filepath, std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		std::cerr << "Error opening file " << filepath << "\n";
		return "";
	}

	std::ostringstream stream;
	stream << file.rdbuf();
	return stream.str();
}