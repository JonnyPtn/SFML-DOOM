#pragma once

#include <vector>
#include <string>

class CmdParameters
{
public:
	CmdParameters(int argc, char** argv);

	static  int							myargc;
	static  std::vector<std::string>	myargv;

	// Returns the position of the given parameter
	// in the arg list (0 if not found).
	static int M_CheckParm(const std::string& check);
};
