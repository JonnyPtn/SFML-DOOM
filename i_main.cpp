
#include "doomdef.hpp"

#include "m_argv.hpp"
#include "d_main.hpp"

int
main
( int		argc,
  char**	argv ) 
{ 
    myargc = argc; 
    myargv = argv; 
 
    D_DoomMain (); 

    return 0;
} 