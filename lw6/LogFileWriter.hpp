#pragma once

#include <fstream>
#include <string>

class LogFileWriter
{
public:
	LogFileWriter(std::string_view fileName)
		:m_output(fileName.data())
	{
	}

	void Log(std::string_view str)
	{
		m_output << str << std::endl;
	}

private:
	std::ofstream m_output;
};
