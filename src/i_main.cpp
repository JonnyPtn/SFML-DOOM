
#include "doomdef.hpp"

#include "m_argv.hpp"
#include "d_main.hpp"

#include <iostream>

#ifdef SFML_DOOM_IOS
#include <SFML/Main.hpp>
int main()
{

#else
int main( int argc, char** argv )
{ 
	CmdParameters(argc, argv);
#endif
 
    D_DoomMain ();

    return 0;
} 
