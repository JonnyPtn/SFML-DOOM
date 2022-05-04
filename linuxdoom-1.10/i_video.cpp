// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <errno.h>
#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#define POINTER_WARP_COUNTDOWN	1

sf::RenderWindow	mainWindow;
int			        screen;
sf::Texture         texture;
sf::Sprite          sprite;
int			        width;
int			        height;

// Fake mouse handling.
// This cannot work properly w/o DGA.
// Needs an invisible mouse cursor at least.
boolean		grabMouse;
int		doPointerWarp = POINTER_WARP_COUNTDOWN;

// Blocky mode,
// replace each 320x200 pixel with multiply*multiply pixels.
// According to Dave Taylor, it still is a bonehead thing
// to use ....
static int	multiply=1;

//
// I_StartFrame
//
void I_StartFrame (void)
{
    // er?

}

static int	lastmousex = 0;
static int	lastmousey = 0;
boolean		mousemoved = false;
boolean		shmFinished;

void I_GetEvent(void)
{
	sf::Event event;

    // put event-grabbing stuff in here
	while(mainWindow.pollEvent(event))
	{
		D_PostEvent(event);
	}
}

// JONNY TODO
//sf::Cursor createnullcursor( Display*	display, Window	root )
//{
//    Pixmap cursormask;
//    XGCValues xgc;
//    GC gc;
//    XColor dummycolour;
//    Cursor cursor;
//
//    cursormask = XCreatePixmap(display, root, 1, 1, 1/*depth*/);
//    xgc.function = GXclear;
//    gc =  XCreateGC(display, cursormask, GCFunction, &xgc);
//    XFillRectangle(display, cursormask, gc, 0, 0, 1, 1);
//    dummycolour.pixel = 0;
//    dummycolour.red = 0;
//    dummycolour.flags = 04;
//    cursor = XCreatePixmapCursor(display, cursormask, cursormask,
//				 &dummycolour,&dummycolour, 0,0);
//    XFreePixmap(display,cursormask);
//    XFreeGC(display,gc);
//    return cursor;
//}

//
// I_StartTic
//
void I_StartTic (void)
{
	I_GetEvent();

    // Warp the pointer back to the middle of the window
    //  or it will wander off - that is, the game will
    //  loose input focus within X11.
	// JONNY TODO
    //if (grabMouse)
    //{
	//if (!--doPointerWarp)
	//{
	//    XWarpPointer( X_display,
	//		  None,
	//		  mainWindow,
	//		  0, 0,
	//		  0, 0,
	//		  X_width/2, X_height/2);
//
	//    doPointerWarp = POINTER_WARP_COUNTDOWN;
	//}
    //}

    mousemoved = false;

}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// Palette stuff.
//
static std::array<sf::Color,256>    colours;

//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
    for (int i=0 ; i<256 ; i++)
    {
        auto c = gammatable[usegamma][*palette++];
        colours[i].r = (c<<8) + c;
        c = gammatable[usegamma][*palette++];
        colours[i].g = (c<<8) + c;
        c = gammatable[usegamma][*palette++];
        colours[i].b = (c<<8) + c;
    }
}

//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{

    static int	lasttic;
    int		tics;
    int		i;
    // UNUSED static unsigned char *bigscreen=0;

    // draws little dots on the bottom of the screen
    if (devparm)
    {

	i = I_GetTime();
	tics = i - lasttic;
	lasttic = i;
	if (tics > 20) tics = 20;

	for (i=0 ; i<tics*2 ; i+=2)
	    screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0xff;
	for ( ; i<20*2 ; i+=2)
	    screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0x0;
    
    }

    sf::Uint8 colouredPixels[SCREENHEIGHT*SCREENWIDTH * 4] = { 0 };
    for (int i = 0; i < SCREENHEIGHT*SCREENWIDTH; i++)
    {
        colouredPixels[i * 4] = colours[screens[0][i]].r;
        colouredPixels[i * 4 + 1] = colours[screens[0][i]].g;
        colouredPixels[i * 4 + 2] = colours[screens[0][i]].b;
        colouredPixels[i * 4 + 3] = 255;
    }
    
    texture.update(colouredPixels);
    sprite.setTexture(texture);
    
    mainWindow.clear();
    mainWindow.draw(sprite);
    mainWindow.display();
}


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}


void I_InitGraphics(void)
{

    char*		displayname;
    char*		d;
    int			n;
    int			pnum;
    int			x=0;
    int			y=0;
    
    // warning: char format, different type arg
    char		xsign=' ';
    char		ysign=' ';
    
    int			oktodraw;
    unsigned long	attribmask;
    int			valuemask;
    static int		firsttime=1;

    if (!firsttime)
	return;
    firsttime = 0;

    signal(SIGINT, (void (*)(int)) I_Quit);

    if (M_CheckParm("-2"))
	multiply = 2;

    if (M_CheckParm("-3"))
	multiply = 3;

    if (M_CheckParm("-4"))
	multiply = 4;

    width = SCREENWIDTH * multiply;
    height = SCREENHEIGHT * multiply;

    // check for command-line display name
    if ( (pnum=M_CheckParm("-disp")) ) // suggest parentheses around assignment
	displayname = myargv[pnum+1];
    else
	displayname = 0;

    // check if the user wants to grab the mouse (quite unnice)
    grabMouse = !!M_CheckParm("-grabmouse");

    // check for command-line geometry
    if ( (pnum=M_CheckParm("-geom")) ) // suggest parentheses around assignment
    {
	// warning: char format, different type arg 3,5
	n = sscanf(myargv[pnum+1], "%c%d%c%d", &xsign, &x, &ysign, &y);
	
	if (n==2)
	    x = y = 0;
	else if (n==6)
	{
	    if (xsign == '-')
		x = -x;
	    if (ysign == '-')
		y = -y;
	}
	else
	    I_Error("bad -geom parameter");
    }

    // open the window
    mainWindow.create({width,height},displayname, sf::Style::Titlebar);
    mainWindow.setFramerateLimit(TICRATE);

    mainWindow.setMouseCursorVisible(false);
	mainWindow.setMouseCursorGrabbed(grabMouse);

    texture.create(SCREENWIDTH,SCREENHEIGHT);
    sprite.setScale(multiply, multiply);
    screens[0] = (unsigned char*)malloc(SCREENWIDTH*SCREENHEIGHT);
}


unsigned	exptable[256];

void InitExpand (void)
{
    int		i;
	
    for (i=0 ; i<256 ; i++)
	exptable[i] = i | (i<<8) | (i<<16) | (i<<24);
}

double		exptable2[256*256];

void InitExpand2 (void)
{
    int		i;
    int		j;
    // UNUSED unsigned	iexp, jexp;
    double*	exp;
    union
    {
	double 		d;
	unsigned	u[2];
    } pixel;
	
    printf ("building exptable2...\n");
    exp = exptable2;
    for (i=0 ; i<256 ; i++)
    {
	pixel.u[0] = i | (i<<8) | (i<<16) | (i<<24);
	for (j=0 ; j<256 ; j++)
	{
	    pixel.u[1] = j | (j<<8) | (j<<16) | (j<<24);
	    *exp++ = pixel.d;
	}
    }
    printf ("done.\n");
}

int	inited;

void
Expand4
( unsigned*	lineptr,
  double*	xline )
{
    double	dpixel;
    unsigned	x;
    unsigned 	y;
    unsigned	fourpixels;
    unsigned	step;
    double*	exp;
	
    exp = exptable2;
    if (!inited)
    {
	inited = 1;
	InitExpand2 ();
    }
		
		
    step = 3*SCREENWIDTH/2;
	
    y = SCREENHEIGHT-1;
    do
    {
	x = SCREENWIDTH;

	do
	{
	    fourpixels = lineptr[0];

		//JONNY TODO	
	    //dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[0] = dpixel;
	    xline[160] = dpixel;
	    xline[320] = dpixel;
	    xline[480] = dpixel;
		
		// JONNY TODO
	    //dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[1] = dpixel;
	    xline[161] = dpixel;
	    xline[321] = dpixel;
	    xline[481] = dpixel;

	    fourpixels = lineptr[1];
		
		// JONNY TODO
	    //dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[2] = dpixel;
	    xline[162] = dpixel;
	    xline[322] = dpixel;
	    xline[482] = dpixel;
			
		// JONNY TODO 
	    //dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[3] = dpixel;
	    xline[163] = dpixel;
	    xline[323] = dpixel;
	    xline[483] = dpixel;

	    fourpixels = lineptr[2];
		
		// JONNY TODO
	    //dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[4] = dpixel;
	    xline[164] = dpixel;
	    xline[324] = dpixel;
	    xline[484] = dpixel;
			
		// JONNY TODO 
	    //dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[5] = dpixel;
	    xline[165] = dpixel;
	    xline[325] = dpixel;
	    xline[485] = dpixel;

	    fourpixels = lineptr[3];

		// JONNY TODO	
	    //dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[6] = dpixel;
	    xline[166] = dpixel;
	    xline[326] = dpixel;
	    xline[486] = dpixel;

		// JONNY TODO			
	    //dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[7] = dpixel;
	    xline[167] = dpixel;
	    xline[327] = dpixel;
	    xline[487] = dpixel;

	    lineptr+=4;
	    xline+=8;
	} while (x-=16);
	xline += step;
    } while (y--);
}


