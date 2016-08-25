#include <m_argv.hpp>

int							CmdParameters::myargc;
std::vector<std::string>	CmdParameters::myargv;

CmdParameters::CmdParameters(int argc, char** argv)
{
	myargc = 0;
	while (myargc < argc)
		myargv.emplace_back(argv[myargc++]);
}
//
// M_CheckParm
// Checks for the given parameter
// in the program's command line arguments.
// Returns the argument number (1 to argc-1)
// or 0 if not present
int CmdParameters::M_CheckParm(const std::string& check)
{
    int	index(0);
    for (auto& argv : myargv)
    {
		if (check == argv)
			return index;
		else
			index++;
    }
    return 0;
}




