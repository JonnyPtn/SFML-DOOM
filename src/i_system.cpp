#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdarg.h>
#ifdef _WIN32
#include "unistd.hpp"
#else
#include "unistd.h"
#endif

#include "doomdef.hpp"
#include "m_misc.hpp"
#include "i_video.hpp"
#include "i_sound.hpp"

#include "d_net.hpp"
#include "g_game.hpp"

#include "i_system.hpp"

//JONNY//
#include <chrono>
#include <thread>

int	mb_used = 6;


void I_Tactile( int on, int off, int total )
{
  // UNUSED.
  on = off = total = 0;
}

ticcmd_t	emptycmd;
ticcmd_t*	I_BaseTiccmd(void)
{
    return &emptycmd;
}


int  I_GetHeapSize (void)
{
    return mb_used*2048*2048;
}

unsigned char* I_ZoneBase (int*	size)
{
    *size = mb_used*2048*2048;
    return (unsigned char *) malloc (*size);
}



//
// I_GetTime
// returns time in 1/70th second tics
//
int  I_GetTime (void)
{
	auto currentTime(std::chrono::system_clock::now());
	static long basetime = 0;

	if (!basetime)
		basetime = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(currentTime.time_since_epoch()).count());

	int newtics = static_cast<int>((std::chrono::duration_cast<std::chrono::seconds>(currentTime.time_since_epoch()).count() - basetime)*TICRATE +
		(std::chrono::duration_cast<std::chrono::microseconds>(currentTime.time_since_epoch()).count() - std::chrono::duration_cast<std::chrono::seconds>(currentTime.time_since_epoch()).count() * 1000000)*TICRATE / 1000000);

	//OLD
    /*struct timeval	tp;
    struct timezone	tzp;
    int			newtics;
    static int		basetime=0;
  
    gettimeofday(&tp, &tzp);
    if (!basetime)
	basetime = tp.tv_sec;
    newtics = (tp.tv_sec-basetime)*TICRATE + tp.tv_usec*TICRATE/1000000;*/
    return newtics;
}



//
// 
// I_Init
//
void I_Init (void)
{
    I_InitGraphics();
}

//
// I_Quit
//
void I_Quit (void)
{
    D_QuitNetGame ();
    I_Sound::shutdown();
    M_SaveDefaults ();
    I_ShutdownGraphics();
    exit(0);
}

void I_WaitVBL(int count)
{
#ifdef SGI
    sginap(1);                                           
#else
#ifdef SUN
    sleep(0);
#else
    //JONNY//usleep (count * (1000000/70) );  
	std::this_thread::sleep_for(std::chrono::microseconds(count*(1000000 / 70)));
#endif
#endif
}

unsigned char*	I_AllocLow(int length)
{
    unsigned char*	mem;
        
    mem = (unsigned char *)malloc (length);
    memset (mem,0,length);
    return mem;
}


//
// I_Error
//
extern bool demorecording;

void I_Error (const char *error, ...)
{
    va_list	argptr;

    // Message first.
    va_start (argptr,error);
    fprintf (stderr, "Error: ");
    vfprintf (stderr,error,argptr);
    fprintf (stderr, "\n");
    va_end (argptr);

    fflush( stderr );

    // Shutdown. Here might be other errors.
    if (demorecording)
	G_CheckDemoStatus();

    D_QuitNetGame ();
    I_ShutdownGraphics();
    
    exit(-1);
}
