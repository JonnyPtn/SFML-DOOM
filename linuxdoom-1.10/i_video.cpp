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

// Had to dig up XShm.c for this one.
// It is in the libXext, but not in the XFree86 headers.
#ifdef LINUX
int XShmGetEventBase( Display* dpy ); // problems with g++?
#endif

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

sf::Window	mainWindow;
sf::Event	event;
int			screen;
sf::Image	image;
int			width;
int			height;

// MIT SHared Memory extension.
boolean		doShm;

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
//  Translates the event key into a doom key
//

int xlatekey(const sf::Event& event)
{

    int rc;

    switch(rc = event.key.code)
    {
      case sf::Keyboard::Key::Left:	rc = KEY_LEFTARROW;	break;
      case sf::Keyboard::Key::Right:	rc = KEY_RIGHTARROW;	break;
      case sf::Keyboard::Key::Down:	rc = KEY_DOWNARROW;	break;
      case sf::Keyboard::Key::Up:	rc = KEY_UPARROW;	break;
      case sf::Keyboard::Key::Escape:	rc = KEY_ESCAPE;	break;
      case sf::Keyboard::Key::Enter:	rc = KEY_ENTER;		break;
      case sf::Keyboard::Key::Tab:	rc = KEY_TAB;		break;
      case sf::Keyboard::Key::F1:	rc = KEY_F1;		break;
      case sf::Keyboard::Key::F2:	rc = KEY_F2;		break;
      case sf::Keyboard::Key::F3:	rc = KEY_F3;		break;
      case sf::Keyboard::Key::F4:	rc = KEY_F4;		break;
      case sf::Keyboard::Key::F5:	rc = KEY_F5;		break;
      case sf::Keyboard::Key::F6:	rc = KEY_F6;		break;
      case sf::Keyboard::Key::F7:	rc = KEY_F7;		break;
      case sf::Keyboard::Key::F8:	rc = KEY_F8;		break;
      case sf::Keyboard::Key::F9:	rc = KEY_F9;		break;
      case sf::Keyboard::Key::F10:	rc = KEY_F10;		break;
      case sf::Keyboard::Key::F11:	rc = KEY_F11;		break;
      case sf::Keyboard::Key::F12:	rc = KEY_F12;		break;
	
      case sf::Keyboard::Key::Backspace:
      case sf::Keyboard::Key::Delete:	rc = KEY_BACKSPACE;	break;

      case sf::Keyboard::Key::Pause:	rc = KEY_PAUSE;		break;

      case sf::Keyboard::Key::Equal:	rc = KEY_EQUALS;	break;

      case sf::Keyboard::Key::Subtract:	rc = KEY_MINUS;		break;

      case sf::Keyboard::Key::LShift:
      case sf::Keyboard::Key::RShift:
	rc = KEY_RSHIFT;
	break;
	
      case sf::Keyboard::Key::LControl:
      case sf::Keyboard::Key::RControl:
	rc = KEY_RCTRL;
	break;
	
      case sf::Keyboard::Key::LAlt:
      case sf::Keyboard::Key::LSystem:
      case sf::Keyboard::Key::RAlt:
      case sf::Keyboard::Key::RSystem:
	rc = KEY_RALT;
	break;
	
      default:
	if (rc >= sf::Keyboard::Key::Space && rc <= sf::Keyboard::Key::Tilde)
	    rc = rc - sf::Keyboard::Key::Space + ' ';
	if (rc >= 'A' && rc <= 'Z')
	    rc = rc - 'A' + 'a';
	break;
    }

    return rc;

}


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

    // scales the screen size before blitting it
    if (multiply == 2)
    {
	unsigned int *olineptrs[2];
	unsigned int *ilineptr;
	int x, y, i;
	unsigned int twoopixels;
	unsigned int twomoreopixels;
	unsigned int fouripixels;

	ilineptr = (unsigned int *) (screens[0]);

	// TODO JONNY
	//for (i=0 ; i<2 ; i++)
	//    olineptrs[i] = (unsigned int *) &image->data[i*X_width];

	y = SCREENHEIGHT;
	while (y--)
	{
	    x = SCREENWIDTH;
	    do
	    {
		fouripixels = *ilineptr++;
		twoopixels =	(fouripixels & 0xff000000)
		    |	((fouripixels>>8) & 0xffff00)
		    |	((fouripixels>>16) & 0xff);
		twomoreopixels =	((fouripixels<<16) & 0xff000000)
		    |	((fouripixels<<8) & 0xffff00)
		    |	(fouripixels & 0xff);
#ifdef __BIG_ENDIAN__
		*olineptrs[0]++ = twoopixels;
		*olineptrs[1]++ = twoopixels;
		*olineptrs[0]++ = twomoreopixels;
		*olineptrs[1]++ = twomoreopixels;
#else
		*olineptrs[0]++ = twomoreopixels;
		*olineptrs[1]++ = twomoreopixels;
		*olineptrs[0]++ = twoopixels;
		*olineptrs[1]++ = twoopixels;
#endif
	    } while (x-=4);
	    olineptrs[0] += width/4;
	    olineptrs[1] += width/4;
	}

    }
    else if (multiply == 3)
    {
	unsigned int *olineptrs[3];
	unsigned int *ilineptr;
	int x, y, i;
	unsigned int fouropixels[3];
	unsigned int fouripixels;

	ilineptr = (unsigned int *) (screens[0]);

	// JONNY TODO
	//for (i=0 ; i<3 ; i++)
	//    olineptrs[i] = (unsigned int *) &image->data[i*X_width];

	y = SCREENHEIGHT;
	while (y--)
	{
	    x = SCREENWIDTH;
	    do
	    {
		fouripixels = *ilineptr++;
		fouropixels[0] = (fouripixels & 0xff000000)
		    |	((fouripixels>>8) & 0xff0000)
		    |	((fouripixels>>16) & 0xffff);
		fouropixels[1] = ((fouripixels<<8) & 0xff000000)
		    |	(fouripixels & 0xffff00)
		    |	((fouripixels>>8) & 0xff);
		fouropixels[2] = ((fouripixels<<16) & 0xffff0000)
		    |	((fouripixels<<8) & 0xff00)
		    |	(fouripixels & 0xff);
#ifdef __BIG_ENDIAN__
		*olineptrs[0]++ = fouropixels[0];
		*olineptrs[1]++ = fouropixels[0];
		*olineptrs[2]++ = fouropixels[0];
		*olineptrs[0]++ = fouropixels[1];
		*olineptrs[1]++ = fouropixels[1];
		*olineptrs[2]++ = fouropixels[1];
		*olineptrs[0]++ = fouropixels[2];
		*olineptrs[1]++ = fouropixels[2];
		*olineptrs[2]++ = fouropixels[2];
#else
		*olineptrs[0]++ = fouropixels[2];
		*olineptrs[1]++ = fouropixels[2];
		*olineptrs[2]++ = fouropixels[2];
		*olineptrs[0]++ = fouropixels[1];
		*olineptrs[1]++ = fouropixels[1];
		*olineptrs[2]++ = fouropixels[1];
		*olineptrs[0]++ = fouropixels[0];
		*olineptrs[1]++ = fouropixels[0];
		*olineptrs[2]++ = fouropixels[0];
#endif
	    } while (x-=4);
	    olineptrs[0] += 2*width/4;
	    olineptrs[1] += 2*width/4;
	    olineptrs[2] += 2*width/4;
	}

    }
    else if (multiply == 4)
    {
	// Broken. Gotta fix this some day.
	// TODO JONNY
	// void Expand4(unsigned *, double *);
  	// Expand4 ((unsigned *)(screens[0]), (double *) (image->data));
    }

    if (doShm)
    {

	// TODO JONNY
	//if (!XShmPutImage(	X_display,
	//			mainWindow,
	//			X_gc,
	//			image,
	//			0, 0,
	//			0, 0,
	//			X_width, X_height,
	//			True ))
	//    I_Error("XShmPutImage() failed\n");

	// wait for it to finish and processes all input events
	shmFinished = false;
	do
	{
	    I_GetEvent();
	} while (!shmFinished);

    }
    else
    {

	// draw the image
	// TODO JONNY
	//XPutImage(	X_display,
	//		mainWindow,
	//		X_gc,
	//		image,
	//		0, 0,
	//		0, 0,
	//		X_width, X_height );
//
	//// sync up with server
	//XSync(X_display, False);

    }

}


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}


//
// Palette stuff.
//
static sf::Color	colors[256];

// JONNY TODO
//void UploadNewPalette(Colormap cmap, byte *palette)
//{
//
//    register int	i;
//    register int	c;
//    static boolean	firstcall = true;
//
//#ifdef __cplusplus
//    if (X_visualinfo.c_class == PseudoColor && X_visualinfo.depth == 8)
//#else
//    if (X_visualinfo.class == PseudoColor && X_visualinfo.depth == 8)
//#endif
//	{
//	    // initialize the colormap
//	    if (firstcall)
//	    {
//		firstcall = false;
//		for (i=0 ; i<256 ; i++)
//		{
//		    colors[i].pixel = i;
//		    colors[i].flags = DoRed|DoGreen|DoBlue;
//		}
//	    }
//
//	    // set the X colormap entries
//	    for (i=0 ; i<256 ; i++)
//	    {
//		c = gammatable[usegamma][*palette++];
//		colors[i].red = (c<<8) + c;
//		c = gammatable[usegamma][*palette++];
//		colors[i].green = (c<<8) + c;
//		c = gammatable[usegamma][*palette++];
//		colors[i].blue = (c<<8) + c;
//	    }
//
//	    // store the colors to the current colormap
//	    XStoreColors(X_display, cmap, colors, 256);
//
//	}
//}

//
// I_SetPalette
//
// JONNY TODO
//void I_SetPalette (byte* palette)
//{
//    UploadNewPalette(X_cmap, palette);
//}


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
    mainWindow.create({width,height},displayname);

	// TODO JONNY
    //XDefineCursor(X_display, mainWindow,
	//	  createnullcursor( X_display, mainWindow ) );

	mainWindow.setMouseCursorGrabbed(grabMouse);

	image.create(width,height);


	// JONNY TODO
    //if (multiply == 1)
	//screens[0] = (unsigned char *) (image->data);
    //else
	//screens[0] = (unsigned char *) malloc (SCREENWIDTH * SCREENHEIGHT);

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


