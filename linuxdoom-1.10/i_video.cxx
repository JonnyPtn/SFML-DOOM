//-----------------------------------------------------------------------------
//
//
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
//
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------


#include <stdlib.h>
// Had to dig up XShm.c for this one.
// It is in the libXext, but not in the XFree86 headers.
#ifdef LINUX
int XShmGetEventBase(Display *dpy); // problems with g++?
#endif

#include <stdarg.h>
#include <sys/types.h>

#include <signal.h>

#include "d_main.h"
#include "doomstat.h"
#include "i_system.h"
#include "i_video.h"
#include "m_argv.h"
#include "v_video.h"

#include "doomdef.h"

#include <SFML/Graphics.hpp>

#define POINTER_WARP_COUNTDOWN 1

sf::RenderWindow X_mainWindow;
std::vector<uint8_t> pixelBuffer;
sf::Texture texture;
int X_screen;
unsigned int X_width;
unsigned int X_height;

int X_shmeventtype;

// Fake mouse handling.
// This cannot work properly w/o DGA.
// Needs an invisible mouse cursor at least.
boolean grabMouse;
int doPointerWarp = POINTER_WARP_COUNTDOWN;

// Blocky mode,
// replace each 320x200 pixel with multiply*multiply pixels.
// According to Dave Taylor, it still is a bonehead thing
// to use ....
static int multiply = 3;

//
// I_StartFrame
//
void I_StartFrame(void)
{
    // er?
}

int sfKeyToDoom(sf::Keyboard::Key key)
{
    using enum sf::Keyboard::Key;
    switch (key)
    {
    case Left:
        return KEY_LEFTARROW;
    case Right:
        return KEY_RIGHTARROW;
    case Down:
        return KEY_DOWNARROW;
    case Up:
        return KEY_UPARROW;
    case Escape:
        return KEY_ESCAPE;
    case Enter:
        return KEY_ENTER;
    case Tab:
        return KEY_TAB;
    case F1:
        return KEY_F1;
    case F2:
        return KEY_F2;
    case F3:
        return KEY_F3;
    case F4:
        return KEY_F4;
    case F5:
        return KEY_F5;
    case F6:
        return KEY_F6;
    case F7:
        return KEY_F7;
    case F8:
        return KEY_F8;
    case F9:
        return KEY_F9;
    case F10:
        return KEY_F10;
    case F11:
        return KEY_F11;
    case F12:
        return KEY_F12;
    case Delete:
    case Backspace:
        return KEY_BACKSPACE;
    case Pause:
        return KEY_PAUSE;
    case Equal:
        return KEY_EQUALS;
    case Subtract:
        return KEY_MINUS;
    case LShift:
    case RShift:
        return KEY_RSHIFT;
    case LControl:
    case RControl:
        return KEY_RCTRL;
    case LAlt:
    case RAlt:
        return KEY_RALT;
    }
    return 'a' + static_cast<int>(key);
}

static int lastmousex = 0;
static int lastmousey = 0;
boolean mousemoved = false;
boolean shmFinished;

void I_GetEvent(void)
{
    event_t event;

    // put event-grabbing stuff in here
    while (auto sfEvent = X_mainWindow.pollEvent())
    {
        if (auto keyEvent = sfEvent->getIf<sf::Event::KeyPressed>())
        {
            event.type = ev_keydown;
            event.data1 = sfKeyToDoom(keyEvent->code);
            D_PostEvent(&event);
        }
        else if (auto keyEvent = sfEvent->getIf<sf::Event::KeyReleased>())
        {
            event.type = ev_keyup;
            event.data1 = sfKeyToDoom(keyEvent->code);
            D_PostEvent(&event);
        }
        else if (auto mouseEvent = sfEvent->getIf<sf::Event::MouseButtonPressed>())
        {
            event.type = ev_mouse;
            using enum sf::Mouse::Button;
            switch (mouseEvent->button)
            {
            case Left:
                event.data1 = 1;
            case Right:
                event.data1 = 2;
            case Middle:
                event.data1 = 4;
            }
            D_PostEvent(&event);
        }
        else if (auto mouseEvent = sfEvent->getIf<sf::Event::MouseButtonReleased>())
        {
            event.type = ev_mouse;
            using enum sf::Mouse::Button;
            switch (mouseEvent->button)
            {
            case Left:
                event.data1 = 1;
            case Right:
                event.data1 = 2;
            case Middle:
                event.data1 = 4;
            }
            D_PostEvent(&event);
        }
        else if (auto mouseEvent = sfEvent->getIf<sf::Event::MouseMoved>())
        {
            event.type = ev_mouse;
            event.data2 = mouseEvent->position.x;
            event.data3 = mouseEvent->position.y;
            // JONNY TODO mouse movement a bit wacky currently D_PostEvent( &event );
        }
        else if (sfEvent->is<sf::Event::Closed>())
        {
            exit(0);
        }
    }

    // TODO JONNY - needed?
    /*
            event.data2 = (X_event.xmotion.x - lastmousex) << 2;
    event.data3 = (lastmousey - X_event.xmotion.y) << 2;
    if ( event.data2 || event.data3 )
    {
            lastmousex = X_event.xmotion.x;
            lastmousey = X_event.xmotion.y;
            if (X_event.xmotion.x != X_width/2 &&
            X_event.xmotion.y != X_height/2)
            {
            D_PostEvent(&event);
            // fprintf(stderr, "m");
            mousemoved = false;
            } else
            {
            mousemoved = true;
            }
    }*/
}

//
// I_StartTic
//
void I_StartTic(void)
{
    I_GetEvent();
    X_mainWindow.clear();
}

//
// I_UpdateNoBlit
//
void I_UpdateNoBlit(void)
{
    // what is this?
}

//
// I_FinishUpdate
//
void I_FinishUpdate(void)
{

    static int lasttic;
    int tics;
    int i;
    // UNUSED static unsigned char *bigscreen=0;

    // draws little dots on the bottom of the screen
    if (devparm)
    {

        i = I_GetTime();
        tics = i - lasttic;
        lasttic = i;
        if (tics > 20)
            tics = 20;

        for (i = 0; i < tics * 2; i += 2)
            screens[0][(SCREENHEIGHT - 1) * SCREENWIDTH + i] = 0xff;
        for (; i < 20 * 2; i += 2)
            screens[0][(SCREENHEIGHT - 1) * SCREENWIDTH + i] = 0x0;
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

        ilineptr = (unsigned int *)(screens[0]);
        for (i = 0; i < 2; i++)
            olineptrs[i] = (unsigned int *)&pixelBuffer[i * X_width];

        y = SCREENHEIGHT;
        while (y--)
        {
            x = SCREENWIDTH;
            do
            {
                fouripixels = *ilineptr++;
                twoopixels = (fouripixels & 0xff000000) | ((fouripixels >> 8) & 0xffff00) | ((fouripixels >> 16) & 0xff);
                twomoreopixels = ((fouripixels << 16) & 0xff000000) | ((fouripixels << 8) & 0xffff00) | (fouripixels & 0xff);
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
            } while (x -= 4);
            olineptrs[0] += X_width / 4;
            olineptrs[1] += X_width / 4;
        }
    }
    else if (multiply == 3)
    {
        unsigned int *olineptrs[3];
        unsigned int *ilineptr;
        int x, y, i;
        unsigned int fouropixels[3];
        unsigned int fouripixels;

        ilineptr = (unsigned int *)(screens[0]);
        for (i = 0; i < 3; i++)
            olineptrs[i] = (unsigned int *)&pixelBuffer[i * X_width];

        y = SCREENHEIGHT;
        while (y--)
        {
            x = SCREENWIDTH;
            do
            {
                fouripixels = *ilineptr++;
                fouropixels[0] = (fouripixels & 0xff000000) | ((fouripixels >> 8) & 0xff0000) | ((fouripixels >> 16) & 0xffff);
                fouropixels[1] = ((fouripixels << 8) & 0xff000000) | (fouripixels & 0xffff00) | ((fouripixels >> 8) & 0xff);
                fouropixels[2] = ((fouripixels << 16) & 0xffff0000) | ((fouripixels << 8) & 0xff00) | (fouripixels & 0xff);
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
            } while (x -= 4);
            olineptrs[0] += 2 * X_width / 4;
            olineptrs[1] += 2 * X_width / 4;
            olineptrs[2] += 2 * X_width / 4;
        }
    }
    else if (multiply == 4)
    {
        // Broken. Gotta fix this some day.
        void Expand4(unsigned *, double *);
        Expand4((unsigned *)(screens[0]), (double *)(pixelBuffer.data()));
    }

    std::vector<uint8_t> colouredPixels(pixelBuffer.size() * 4);
    for (int i = 0; i < pixelBuffer.size(); i++)
    {
        colouredPixels[i * 4] = colors[static_cast<int>(pixelBuffer[i])].r;
        colouredPixels[i * 4 + 1] = colors[static_cast<int>(pixelBuffer[i])].g;
        colouredPixels[i * 4 + 2] = colors[static_cast<int>(pixelBuffer[i])].b;
        colouredPixels[i * 4 + 3] = 255;
    }

    // draw the image
    texture.update(colouredPixels.data());
    sf::Sprite sprite(texture);
    X_mainWindow.draw(sprite);
    X_mainWindow.display();
}

//
// I_ReadScreen
//
void I_ReadScreen(byte *scr) { memcpy(scr, screens[0], SCREENWIDTH * SCREENHEIGHT); }

//
// Palette stuff.
//

void UploadNewPalette(const byte *palette)
{

    int i;
    int c;
    static boolean firstcall = true;

    // initialize the colormap
    if (firstcall)
    {
        firstcall = false;
        for (i = 0; i < 256; i++)
        {
            colors[i] = sf::Color::White;
        }
    }

    // set the X colormap entries
    for (i = 0; i < 256; i++)
    {
        c = gammatable[usegamma][*palette++];
        colors[i].r = (c << 8) + c;
        c = gammatable[usegamma][*palette++];
        colors[i].g = (c << 8) + c;
        c = gammatable[usegamma][*palette++];
        colors[i].b = (c << 8) + c;
    }
}

//
// I_SetPalette
//
void I_SetPalette(const byte *palette) { UploadNewPalette(palette); }

void I_InitGraphics(void)
{

    char *displayname;
    char *d;
    int n;
    int pnum;
    int x = 0;
    int y = 0;

    // warning: char format, different type arg
    char xsign = ' ';
    char ysign = ' ';

    int oktodraw;
    unsigned long attribmask;
    // JONNY TODO   XSetWindowAttributes attribs;
    // JONNY TODO  XGCValues		xgcvalues;
    int valuemask;
    static int firsttime = 1;

    if (!firsttime)
        return;
    firsttime = 0;

    signal(SIGINT, (void (*)(int))I_Quit);

    if (M_CheckParm("-2"))
        multiply = 2;

    if (M_CheckParm("-3"))
        multiply = 3;

    if (M_CheckParm("-4"))
        multiply = 4;

    X_width = SCREENWIDTH * multiply;
    X_height = SCREENHEIGHT * multiply;

    // check for command-line display name
    if ((pnum = M_CheckParm("-disp"))) // suggest parentheses around assignment
        displayname = myargv[pnum + 1];
    else
        displayname = 0;

    // check if the user wants to grab the mouse (quite unnice)
    grabMouse = !!M_CheckParm("-grabmouse");

    // check for command-line geometry
    if ((pnum = M_CheckParm("-geom"))) // suggest parentheses around assignment
    {
        // warning: char format, different type arg 3,5
        n = sscanf(myargv[pnum + 1], "%c%d%c%d", &xsign, &x, &ysign, &y);

        if (n == 2)
            x = y = 0;
        else if (n == 6)
        {
            if (xsign == '-')
                x = -x;
            if (ysign == '-')
                y = -y;
        }
        else
            I_Error("bad -geom parameter");
    }

    // create the main window
    X_mainWindow.create(sf::VideoMode{{X_width, X_height}}, "SFML-DOOM");

    // grabs the pointer so it is restricted to this window
    X_mainWindow.setMouseCursorGrabbed(grabMouse);

    if (!texture.resize({X_width, X_height}))
    {
        I_Error("Failed to resize texture");
    }
    pixelBuffer.resize(X_width * X_height);

    if (multiply == 1)
    {
        screens[0] = pixelBuffer.data();
    }
    else
    {
        screens[0] = (unsigned char *)malloc(SCREENWIDTH * SCREENHEIGHT);
    }
}

unsigned exptable[256];

void InitExpand(void)
{
    int i;

    for (i = 0; i < 256; i++)
        exptable[i] = i | (i << 8) | (i << 16) | (i << 24);
}

double exptable2[256 * 256];

void InitExpand2(void)
{
    int i;
    int j;
    // UNUSED unsigned	iexp, jexp;
    double *exp;
    union
    {
        double d;
        unsigned u[2];
    } pixel;

    printf("building exptable2...\n");
    exp = exptable2;
    for (i = 0; i < 256; i++)
    {
        pixel.u[0] = i | (i << 8) | (i << 16) | (i << 24);
        for (j = 0; j < 256; j++)
        {
            pixel.u[1] = j | (j << 8) | (j << 16) | (j << 24);
            *exp++ = pixel.d;
        }
    }
    printf("done.\n");
}

int inited;

void Expand4(unsigned *lineptr, double *xline)
{
    double dpixel;
    unsigned x;
    unsigned y;
    unsigned fourpixels;
    unsigned step;
    double *exp;

    exp = exptable2;
    if (!inited)
    {
        inited = 1;
        InitExpand2();
    }

    step = 3 * SCREENWIDTH / 2;

    y = SCREENHEIGHT - 1;
    do
    {
        x = SCREENWIDTH;

        do
        {
            fourpixels = lineptr[0];

            dpixel = *(double *)((intptr_t)exp + ((fourpixels & 0xffff0000) >> 13));
            xline[0] = dpixel;
            xline[160] = dpixel;
            xline[320] = dpixel;
            xline[480] = dpixel;

            dpixel = *(double *)((intptr_t)exp + ((fourpixels & 0xffff) << 3));
            xline[1] = dpixel;
            xline[161] = dpixel;
            xline[321] = dpixel;
            xline[481] = dpixel;

            fourpixels = lineptr[1];

            dpixel = *(double *)((intptr_t)exp + ((fourpixels & 0xffff0000) >> 13));
            xline[2] = dpixel;
            xline[162] = dpixel;
            xline[322] = dpixel;
            xline[482] = dpixel;

            dpixel = *(double *)((intptr_t)exp + ((fourpixels & 0xffff) << 3));
            xline[3] = dpixel;
            xline[163] = dpixel;
            xline[323] = dpixel;
            xline[483] = dpixel;

            fourpixels = lineptr[2];

            dpixel = *(double *)((intptr_t)exp + ((fourpixels & 0xffff0000) >> 13));
            xline[4] = dpixel;
            xline[164] = dpixel;
            xline[324] = dpixel;
            xline[484] = dpixel;

            dpixel = *(double *)((intptr_t)exp + ((fourpixels & 0xffff) << 3));
            xline[5] = dpixel;
            xline[165] = dpixel;
            xline[325] = dpixel;
            xline[485] = dpixel;

            fourpixels = lineptr[3];

            dpixel = *(double *)((intptr_t)exp + ((fourpixels & 0xffff0000) >> 13));
            xline[6] = dpixel;
            xline[166] = dpixel;
            xline[326] = dpixel;
            xline[486] = dpixel;

            dpixel = *(double *)((intptr_t)exp + ((fourpixels & 0xffff) << 3));
            xline[7] = dpixel;
            xline[167] = dpixel;
            xline[327] = dpixel;
            xline[487] = dpixel;

            lineptr += 4;
            xline += 8;
        } while (x -= 16);
        xline += step;
    } while (y--);
}