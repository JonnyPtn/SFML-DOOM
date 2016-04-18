
static const char
rcsid[] = "$Id: m_argv.c,v 1.1 1997/02/03 22:45:10 b1 Exp $";


#include <string>
#include <cctype>

int		myargc;
char**		myargv;


//
// M_CheckParm
// Checks for the given parameter
// in the program's command line arguments.
// Returns the argument number (1 to argc-1)
// or 0 if not present
int M_CheckParm(char *check)
{
	int		i;

	for (i = 1; i<myargc; i++)
	{
		std::string checkString(check);
		std::string parameter(myargv[i]);

		//convert both to upper case
		for (auto& character : checkString)
			character = std::toupper(character);

		for (auto& character : parameter)
			character = std::toupper(character);

		if (checkString == parameter)
			return i;
	}

	return 0;
}




