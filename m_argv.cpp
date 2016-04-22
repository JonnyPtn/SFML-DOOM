

static const char
rcsid[] = "$Id: m_argv.c,v 1.1 1997/02/03 22:45:10 b1 Exp $";


#include <string.h>

int		myargc;
char**		myargv;

#ifndef _WIN32
#define _strcmpi strcasecmp
#endif


//
// M_CheckParm
// Checks for the given parameter
// in the program's command line arguments.
// Returns the argument number (1 to argc-1)
// or 0 if not present
int M_CheckParm (char *check)
{
    int		i;

    for (i = 1;i<myargc;i++)
    {
	if ( !_strcmpi(check, myargv[i]) )
	    return i;
    }

    return 0;
}




