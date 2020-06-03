#pragma once
#include <string>
#include <sstream>
#include <fstream>

std::string ReadFromFiles(const std::string& filepath, int& levelWidth, int& levelHeight)
{
	std::ifstream stream(filepath);
	std::string line;
	std::stringstream ss;

	int lineCount = 0;
	while (getline(stream, line))
	{
		levelWidth = line.size();
		ss << line;
		++lineCount;
	}

	levelHeight = lineCount;

	return ss.str();
}
