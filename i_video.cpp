
#include <stdlib.h>
#ifdef _WIN32
#include "unistd.hpp"
#else
#include "unistd.h"
#endif

#include <stdarg.h>
#include <sys/types.h>
#include <signal.h>

#include "doomstat.hpp"
#include "i_system.hpp"
#include "v_video.hpp"
#include "m_argv.hpp"
#include "d_main.hpp"

#include "doomdef.hpp"

#define POINTER_WARP_COUNTDOWN	1

//JONNY// SFML stuff for rendering
#include <SFML/Graphics.hpp>
#include <memory>

std::unique_ptr<sf::RenderWindow> window;

sf::Image			image;
std::unique_ptr<sf::Texture>			texture;
std::unique_ptr<sf::Sprite>			sprite;
sf::Color	colors[256];
int		X_screen;
int		X_width;
int		X_height;

void I_ShutdownGraphics(void)
{
	sprite.release();
	texture.release();
	window.release();
}

static int	lastmousex = 0;
static int	lastmousey = 0;
bool		mousemoved = false;
bool		shmFinished;

//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{
    static int	lasttic;
    int		tics;
	int		i;

	sf::Uint8 pixels[SCREENHEIGHT*SCREENWIDTH * 4] = { 0 };
	auto imagePixels = image.getPixelsPtr();
	for (int i = 0; i < SCREENHEIGHT*SCREENWIDTH; i++)
	{
		pixels[i * 4] = colors[imagePixels[i]].r;
		pixels[i * 4 + 1] = colors[imagePixels[i]].g;
		pixels[i * 4 + 2] = colors[imagePixels[i]].b;
		pixels[i * 4 + 3] = 255;
	}
	texture->update(pixels);
	window->clear();
	window->draw(*sprite);
	window->display();
}


//
// I_ReadScreen
//
void I_ReadScreen (unsigned char* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}


//
// Palette stuff.
//

void UploadNewPalette( unsigned char *palette)
{
	static bool	firstcall = true;
	
	// initialize the colormap
	if (firstcall)
	{
		firstcall = false;
		for (int i=0 ; i<256 ; i++)
		{
		    colors[i] = sf::Color::White;
		}
	}

	// set the X colormap entries
	int c;
	for (int i=0 ; i<256 ; i++)
	{
		c = gammatable[usegamma][*palette++];
		colors[i].r = (c<<8) + c;
		c = gammatable[usegamma][*palette++];
		colors[i].g = (c<<8) + c;
		c = gammatable[usegamma][*palette++];
		colors[i].b = (c<<8) + c;
	}
}

//
// I_SetPalette
//
void I_SetPalette (unsigned char* palette)
{
    UploadNewPalette(palette);
}

void I_InitGraphics(void)
{

    std::string	displayname;
    int			n;
    int			pnum;
    int			x=0;
    int			y=0;
    
    // warning: char format, different type arg
    char		xsign=' ';
    char		ysign=' ';
    static int		firsttime=1;

    if (!firsttime)
		return;
    firsttime = 0;

    signal(SIGINT, (void (*)(int)) I_Quit);

    X_width = SCREENWIDTH;
	X_height = SCREENHEIGHT;

    // check for command-line display name
    if ( (pnum= CmdParameters::M_CheckParm("-disp")) ) // suggest parentheses around assignment
		displayname = CmdParameters::myargv[pnum+1];
    else
		displayname = "";

    // check for command-line geometry
    if ( (pnum= CmdParameters::M_CheckParm("-geom")) ) // suggest parentheses around assignment
    {
		// warning: char format, different type arg 3,5
		n = sscanf(CmdParameters::myargv[pnum+1].c_str(), "%c%d%c%d", &xsign, &x, &ysign, &y);
		
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

	window.reset(new sf::RenderWindow());
	window->create(sf::VideoMode(X_width, X_height), displayname);
	window->setVerticalSyncEnabled(true);
	image.create(X_width, X_height);
	texture.reset(new sf::Texture);
	texture->create(X_width, X_height);
	texture->loadFromImage(image);
	sprite.reset(new sf::Sprite());
	sprite->setTexture(*texture);

	screens[0] = (unsigned char *) malloc (SCREENWIDTH * SCREENHEIGHT);
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
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[0] = dpixel;
	    xline[160] = dpixel;
	    xline[320] = dpixel;
	    xline[480] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[1] = dpixel;
	    xline[161] = dpixel;
	    xline[321] = dpixel;
	    xline[481] = dpixel;

	    fourpixels = lineptr[1];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[2] = dpixel;
	    xline[162] = dpixel;
	    xline[322] = dpixel;
	    xline[482] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[3] = dpixel;
	    xline[163] = dpixel;
	    xline[323] = dpixel;
	    xline[483] = dpixel;

	    fourpixels = lineptr[2];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[4] = dpixel;
	    xline[164] = dpixel;
	    xline[324] = dpixel;
	    xline[484] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[5] = dpixel;
	    xline[165] = dpixel;
	    xline[325] = dpixel;
	    xline[485] = dpixel;

	    fourpixels = lineptr[3];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[6] = dpixel;
	    xline[166] = dpixel;
	    xline[326] = dpixel;
	    xline[486] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
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

bool pollEvent(sf::Event& ev)
{
	return window->pollEvent(ev);
}
