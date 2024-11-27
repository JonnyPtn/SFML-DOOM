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
// DESCRIPTION:  Heads-up displays
//
//-----------------------------------------------------------------------------
module;
#include "g_game.h"
#include "r_defs.h"
#include "r_draw.h"
#include "r_main.h"
#include <ctype.h>
export module hud;

import engine;
import wad;
import doom.map;
import doom;
import video;

//
// Globally visible constants.
//
export constexpr auto HU_FONTSTART = '!'; // the first font characters
export constexpr auto HU_FONTEND = '_';   // the last font characters

// Calculate # of glyphs in font.
export constexpr auto HU_FONTSIZE = ( HU_FONTEND - HU_FONTSTART + 1 );

#define HU_BROADCAST 5

#define HU_MSGREFRESH KEY_ENTER
#define HU_MSGX 0
#define HU_MSGY 0
#define HU_MSGWIDTH 64 // in characters
#define HU_MSGHEIGHT 1 // in lines

#define HU_MSGTIMEOUT ( 4 * TICRATE )

// background and foreground screen numbers
// different from other modules.
#define BG 1
#define FG 0

// font stuff
#define HU_CHARERASE KEY_BACKSPACE

#define HU_MAXLINES 4
#define HU_MAXLINELENGTH 80

//
// Locally used constants, shortcuts.
//
#define HU_TITLE ( mapnames[( gameepisode - 1 ) * 9 + gamemap - 1] )
#define HU_TITLE2 ( mapnames2[gamemap - 1] )
#define HU_TITLEP ( mapnamesp[gamemap - 1] )
#define HU_TITLET ( mapnamest[gamemap - 1] )
#define HU_TITLEHEIGHT 1
#define HU_TITLEX 0
#define HU_TITLEY ( 167 - hu_font[0]->height )

#define HU_INPUTTOGGLE 't'
#define HU_INPUTX HU_MSGX
#define HU_INPUTY ( HU_MSGY + HU_MSGHEIGHT * ( hu_font[0]->height + 1 ) )
#define HU_INPUTWIDTH 64
#define HU_INPUTHEIGHT 1

//
// Typedefs of widgets
//

// Text Line widget
//  (parent of Scrolling Text and Input Text widgets)
typedef struct
{
    // left-justified position of scrolling text window
    int x;
    int y;

    patch_t **f;                  // font
    int sc;                       // start character
    char l[HU_MAXLINELENGTH + 1]; // line of text
    int len;                      // current line length

    // whether this line needs to be udpated
    int needsupdate;

} hu_textline_t;

// Scrolling Text window widget
//  (child of Text Line widget)
typedef struct
{
    hu_textline_t l[HU_MAXLINES]; // text lines to draw
    int h;                        // height in lines
    int cl;                       // current line number

    // pointer to bool stating whether to update window
    bool *on;
    bool laston; // last value of *->on.

} hu_stext_t;

// Input Text Line widget
//  (child of Text Line widget)
typedef struct
{
    hu_textline_t l; // text line to input on

    // left margin past which I am not to delete characters
    int lm;

    // pointer to bool stating whether to update window
    bool *on;
    bool laston; // last value of *->on;

} hu_itext_t;

std::array chat_macros = { HUSTR_CHATMACRO0, HUSTR_CHATMACRO1, HUSTR_CHATMACRO2, HUSTR_CHATMACRO3, HUSTR_CHATMACRO4,
                           HUSTR_CHATMACRO5, HUSTR_CHATMACRO6, HUSTR_CHATMACRO7, HUSTR_CHATMACRO8, HUSTR_CHATMACRO9 };

export std::array player_names = { HUSTR_PLRGREEN, HUSTR_PLRINDIGO, HUSTR_PLRBROWN, HUSTR_PLRRED };

char chat_char; // remove later.
inline player_t *plr;
inline hu_textline_t w_title;
bool chat_on;
inline hu_itext_t w_chat;
inline bool always_off = false;
inline char chat_dest[MAXPLAYERS];
inline hu_itext_t w_inputbuffer[MAXPLAYERS];

inline bool message_on;
inline bool message_nottobefuckedwith;

inline hu_stext_t w_message;
inline int message_counter;

inline bool headsupactive = false;

export patch_t *hu_font[HU_FONTSIZE];

//
// Builtin map names.
// The actual names can be found in DStrings.h.
//

std::array mapnames = // DOOM shareware/registered/retail (Ultimate) names.
    {

        HUSTR_E1M1, HUSTR_E1M2, HUSTR_E1M3, HUSTR_E1M4, HUSTR_E1M5, HUSTR_E1M6, HUSTR_E1M7, HUSTR_E1M8, HUSTR_E1M9,

        HUSTR_E2M1, HUSTR_E2M2, HUSTR_E2M3, HUSTR_E2M4, HUSTR_E2M5, HUSTR_E2M6, HUSTR_E2M7, HUSTR_E2M8, HUSTR_E2M9,

        HUSTR_E3M1, HUSTR_E3M2, HUSTR_E3M3, HUSTR_E3M4, HUSTR_E3M5, HUSTR_E3M6, HUSTR_E3M7, HUSTR_E3M8, HUSTR_E3M9,

        HUSTR_E4M1, HUSTR_E4M2, HUSTR_E4M3, HUSTR_E4M4, HUSTR_E4M5, HUSTR_E4M6, HUSTR_E4M7, HUSTR_E4M8, HUSTR_E4M9,

        "NEWLEVEL", "NEWLEVEL", "NEWLEVEL", "NEWLEVEL", "NEWLEVEL", "NEWLEVEL", "NEWLEVEL", "NEWLEVEL", "NEWLEVEL" };

std::array mapnames2 = // DOOM 2 map names.
    { HUSTR_1,  HUSTR_2,  HUSTR_3,  HUSTR_4,  HUSTR_5,  HUSTR_6,  HUSTR_7,  HUSTR_8,  HUSTR_9,  HUSTR_10, HUSTR_11,

      HUSTR_12, HUSTR_13, HUSTR_14, HUSTR_15, HUSTR_16, HUSTR_17, HUSTR_18, HUSTR_19, HUSTR_20,

      HUSTR_21, HUSTR_22, HUSTR_23, HUSTR_24, HUSTR_25, HUSTR_26, HUSTR_27, HUSTR_28, HUSTR_29, HUSTR_30, HUSTR_31, HUSTR_32 };

std::array mapnamesp = // Plutonia WAD map names.
    { PHUSTR_1,  PHUSTR_2,  PHUSTR_3,  PHUSTR_4,  PHUSTR_5,  PHUSTR_6,  PHUSTR_7,  PHUSTR_8,  PHUSTR_9,  PHUSTR_10, PHUSTR_11,

      PHUSTR_12, PHUSTR_13, PHUSTR_14, PHUSTR_15, PHUSTR_16, PHUSTR_17, PHUSTR_18, PHUSTR_19, PHUSTR_20,

      PHUSTR_21, PHUSTR_22, PHUSTR_23, PHUSTR_24, PHUSTR_25, PHUSTR_26, PHUSTR_27, PHUSTR_28, PHUSTR_29, PHUSTR_30, PHUSTR_31, PHUSTR_32 };

std::array mapnamest = // TNT WAD map names.
    { THUSTR_1,  THUSTR_2,  THUSTR_3,  THUSTR_4,  THUSTR_5,  THUSTR_6,  THUSTR_7,  THUSTR_8,  THUSTR_9,  THUSTR_10, THUSTR_11,

      THUSTR_12, THUSTR_13, THUSTR_14, THUSTR_15, THUSTR_16, THUSTR_17, THUSTR_18, THUSTR_19, THUSTR_20,

      THUSTR_21, THUSTR_22, THUSTR_23, THUSTR_24, THUSTR_25, THUSTR_26, THUSTR_27, THUSTR_28, THUSTR_29, THUSTR_30, THUSTR_31, THUSTR_32 };

const char *shiftxform;

const char french_shiftxform[] = { 0,    1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
                                   20,   21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  ' ', '!', '"', '#', '$', '%', '&',
                                   '"', // shift-'
                                   '(',  ')', '*', '+',
                                   '?', // shift-,
                                   '_', // shift--
                                   '>', // shift-.
                                   '?', // shift-/
                                   '0', // shift-0
                                   '1', // shift-1
                                   '2', // shift-2
                                   '3', // shift-3
                                   '4', // shift-4
                                   '5', // shift-5
                                   '6', // shift-6
                                   '7', // shift-7
                                   '8', // shift-8
                                   '9', // shift-9
                                   '/',
                                   '.', // shift-;
                                   '<',
                                   '+', // shift-=
                                   '>',  '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
                                   'R',  'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                   '[', // shift-[
                                   '!', // shift-backslash - OH MY GOD DOES WATCOM SUCK
                                   ']', // shift-]
                                   '"',  '_',
                                   '\'', // shift-`
                                   'A',  'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                                   'U',  'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '~', 127

};

const char english_shiftxform[] = {

    0,    1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,
    29,   30,  31,  ' ', '!', '"', '#', '$', '%', '&',
    '"', // shift-'
    '(',  ')', '*', '+',
    '<', // shift-,
    '_', // shift--
    '>', // shift-.
    '?', // shift-/
    ')', // shift-0
    '!', // shift-1
    '@', // shift-2
    '#', // shift-3
    '$', // shift-4
    '%', // shift-5
    '^', // shift-6
    '&', // shift-7
    '*', // shift-8
    '(', // shift-9
    ':',
    ':', // shift-;
    '<',
    '+', // shift-=
    '>',  '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '[', // shift-[
    '!', // shift-backslash - OH MY GOD DOES WATCOM SUCK
    ']', // shift-]
    '"',  '_',
    '\'', // shift-`
    'A',  'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}',
    '~',  127 };

char frenchKeyMap[128] = { 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,   15,  16,  17,  18,  19,  20,   21,  22,  23,  24,  25,
                           26,  27,  28,  29,  30,  31,  ' ', '!', '"', '#', '$', '%', '&', '%', '(',  ')', '*', '+', ';', '-', ':',  '!', '0', '1', '2', '3',
                           '4', '5', '6', '7', '8', '9', ':', 'M', '<', '=', '>', '?', '@', 'Q', 'B',  'C', 'D', 'E', 'F', 'G', 'H',  'I', 'J', 'K', 'L', ',',
                           'N', 'O', 'P', 'A', 'R', 'S', 'T', 'U', 'V', 'Z', 'X', 'Y', 'W', '^', '\\', '$', '^', '_', '@', 'Q', 'B',  'C', 'D', 'E', 'F', 'G',
                           'H', 'I', 'J', 'K', 'L', ',', 'N', 'O', 'P', 'A', 'R', 'S', 'T', 'U', 'V',  'Z', 'X', 'Y', 'W', '^', '\\', '$', '^', 127 };

char ForeignTranslation( unsigned char ch ) { return ch < 128 ? frenchKeyMap[ch] : ch; }

// bool : whether the screen is always erased
#define noterased viewwindowx

void HUlib_init( void ) {}

void HUlib_clearTextLine( hu_textline_t *t )
{
    t->len = 0;
    t->l[0] = 0;
    t->needsupdate = true;
}

void HUlib_initTextLine( hu_textline_t *t, int x, int y, patch_t **f, int sc )
{
    t->x = x;
    t->y = y;
    t->f = f;
    t->sc = sc;
    HUlib_clearTextLine( t );
}

bool HUlib_addCharToTextLine( hu_textline_t *t, char ch )
{

    if ( t->len == HU_MAXLINELENGTH )
        return false;
    else
    {
        t->l[t->len++] = ch;
        t->l[t->len] = 0;
        t->needsupdate = 4;
        return true;
    }
}

bool HUlib_delCharFromTextLine( hu_textline_t *t )
{

    if ( !t->len )
        return false;
    else
    {
        t->l[--t->len] = 0;
        t->needsupdate = 4;
        return true;
    }
}

void HUlib_drawTextLine( hu_textline_t *l, bool drawcursor )
{

    int i;
    int w;
    int x;
    unsigned char c;

    // draw the new stuff
    x = l->x;
    for ( i = 0; i < l->len; i++ )
    {
        c = toupper( l->l[i] );
        if ( c != ' ' && c >= l->sc && c <= '_' )
        {
            w = l->f[c - l->sc]->width;
            if ( x + w > SCREENWIDTH )
                break;
            V_DrawPatchDirect( x, l->y, FG, l->f[c - l->sc] );
            x += w;
        }
        else
        {
            x += 4;
            if ( x >= SCREENWIDTH )
                break;
        }
    }

    // draw the cursor if requested
    if ( drawcursor && x + l->f['_' - l->sc]->width <= SCREENWIDTH )
    {
        V_DrawPatchDirect( x, l->y, FG, l->f['_' - l->sc] );
    }
}

// sorta called by HU_Erase and just better darn get things straight
void HUlib_eraseTextLine( hu_textline_t *l )
{
    int lh;
    int y;
    int yoffset;

    // Only erases when NOT in automap and the screen is reduced,
    // and the text must either need updating or refreshing
    // (because of a recent change back from the automap)

    if ( !automapactive && viewwindowx && l->needsupdate )
    {
        lh = l->f[0]->height + 1;
        for ( y = l->y, yoffset = y * SCREENWIDTH; y < l->y + lh; y++, yoffset += SCREENWIDTH )
        {
            if ( y < viewwindowy || y >= viewwindowy + viewheight )
                R_VideoErase( yoffset, SCREENWIDTH ); // erase entire line
            else
            {
                R_VideoErase( yoffset, viewwindowx ); // erase left border
                R_VideoErase( yoffset + viewwindowx + viewwidth, viewwindowx );
                // erase right border
            }
        }
    }
    if ( l->needsupdate )
        l->needsupdate--;
}

void HUlib_initSText( hu_stext_t *s, int x, int y, int h, patch_t **font, int startchar, bool *on )
{

    int i;

    s->h = h;
    s->on = on;
    s->laston = true;
    s->cl = 0;
    for ( i = 0; i < h; i++ )
        HUlib_initTextLine( &s->l[i], x, y - i * ( font[0]->height + 1 ), font, startchar );
}

void HUlib_addLineToSText( hu_stext_t *s )
{

    int i;

    // add a clear line
    if ( ++s->cl == s->h )
        s->cl = 0;
    HUlib_clearTextLine( &s->l[s->cl] );

    // everything needs updating
    for ( i = 0; i < s->h; i++ )
        s->l[i].needsupdate = 4;
}

void HUlib_addMessageToSText( hu_stext_t *s, char *prefix, const char *msg )
{
    HUlib_addLineToSText( s );
    if ( prefix )
        while ( *prefix )
            HUlib_addCharToTextLine( &s->l[s->cl], *( prefix++ ) );

    while ( *msg )
        HUlib_addCharToTextLine( &s->l[s->cl], *( msg++ ) );
}

void HUlib_drawSText( hu_stext_t *s )
{
    int i, idx;
    hu_textline_t *l;

    if ( !*s->on )
        return; // if not on, don't draw

    // draw everything
    for ( i = 0; i < s->h; i++ )
    {
        idx = s->cl - i;
        if ( idx < 0 )
            idx += s->h; // handle queue of lines

        l = &s->l[idx];

        // need a decision made here on whether to skip the draw
        HUlib_drawTextLine( l, false ); // no cursor, please
    }
}

void HUlib_eraseSText( hu_stext_t *s )
{

    int i;

    for ( i = 0; i < s->h; i++ )
    {
        if ( s->laston && !*s->on )
            s->l[i].needsupdate = 4;
        HUlib_eraseTextLine( &s->l[i] );
    }
    s->laston = *s->on;
}

void HUlib_initIText( hu_itext_t *it, int x, int y, patch_t **font, int startchar, bool *on )
{
    it->lm = 0; // default left margin is start of text
    it->on = on;
    it->laston = true;
    HUlib_initTextLine( &it->l, x, y, font, startchar );
}

// The following deletion routines adhere to the left margin restriction
void HUlib_delCharFromIText( hu_itext_t *it )
{
    if ( it->l.len != it->lm )
        HUlib_delCharFromTextLine( &it->l );
}

void HUlib_eraseLineFromIText( hu_itext_t *it )
{
    while ( it->lm != it->l.len )
        HUlib_delCharFromTextLine( &it->l );
}

// Resets left margin as well
void HUlib_resetIText( hu_itext_t *it )
{
    it->lm = 0;
    HUlib_clearTextLine( &it->l );
}

void HUlib_addPrefixToIText( hu_itext_t *it, char *str )
{
    while ( *str )
        HUlib_addCharToTextLine( &it->l, *( str++ ) );
    it->lm = it->l.len;
}

// wrapper function for handling general keyed input.
// returns true if it ate the key
bool HUlib_keyInIText( hu_itext_t *it, unsigned char ch )
{
    // JONNY TODO
    //    if (ch >= ' ' && ch <= '_')
    //  	HUlib_addCharToTextLine(&it->l, (char) ch);
    //    else
    //	if (ch == KEY_BACKSPACE)
    //	    HUlib_delCharFromIText(it);
    //	else
    //	    if (ch != KEY_ENTER)
    return false; // did not eat key

    return true; // ate the key
}

void HUlib_drawIText( hu_itext_t *it )
{

    hu_textline_t *l = &it->l;

    if ( !*it->on )
        return;
    HUlib_drawTextLine( l, true ); // draw the line w/ cursor
}

void HUlib_eraseIText( hu_itext_t *it )
{
    if ( it->laston && !*it->on )
        it->l.needsupdate = 4;
    HUlib_eraseTextLine( &it->l );
    it->laston = *it->on;
}

export void HU_Init( void )
{

    int i;
    int j;
    char buffer[9];

    if ( french )
        shiftxform = french_shiftxform;
    else
        shiftxform = english_shiftxform;

    // load the heads-up font
    j = HU_FONTSTART;
    for ( i = 0; i < HU_FONTSIZE; i++ )
    {
        snprintf( buffer, 9, "STCFN%.3d", j++ );
        hu_font[i] = (patch_t *)wad::get( buffer );
    }
}

void HU_Stop( void ) { headsupactive = false; }

export void HU_Start( void )
{

    int i;
    const char *s;

    if ( headsupactive )
        HU_Stop();

    plr = &players[consoleplayer];
    message_on = false;
    // JONNY TODO CIRCULAR DEPENDENCY
    // message_dontfuckwithme = false;
    message_nottobefuckedwith = false;
    chat_on = false;

    // create the message widget
    HUlib_initSText( &w_message, HU_MSGX, HU_MSGY, HU_MSGHEIGHT, hu_font, HU_FONTSTART, &message_on );

    // create the map title widget
    HUlib_initTextLine( &w_title, HU_TITLEX, HU_TITLEY, hu_font, HU_FONTSTART );

    switch ( gamemode )
    {
        using enum game_mode;
    case shareware:
    case registered:
    case retail:
        s = HU_TITLE;
        break;

        /* FIXME
              case pack_plut:
                s = HU_TITLEP;
                break;
              case pack_tnt:
                s = HU_TITLET;
                break;
        */

    case commercial:
    default:
        s = HU_TITLE2;
        break;
    }

    while ( *s )
        HUlib_addCharToTextLine( &w_title, *( s++ ) );

    // create the chat widget
    HUlib_initIText( &w_chat, HU_INPUTX, HU_INPUTY, hu_font, HU_FONTSTART, &chat_on );

    // create the inputbuffer widgets
    for ( i = 0; i < MAXPLAYERS; i++ )
        HUlib_initIText( &w_inputbuffer[i], 0, 0, 0, 0, &always_off );

    headsupactive = true;
}

export void HU_Drawer( void )
{

    HUlib_drawSText( &w_message );
    HUlib_drawIText( &w_chat );
    if ( automapactive )
        HUlib_drawTextLine( &w_title, false );
}

export void HU_Erase( void )
{

    HUlib_eraseSText( &w_message );
    HUlib_eraseIText( &w_chat );
    HUlib_eraseTextLine( &w_title );
}

export void HU_Ticker( void )
{

    int i;
    char c;

    // tick down message counter if message is up
    if ( message_counter && !--message_counter )
    {
        message_on = false;
        message_nottobefuckedwith = false;
    }

    // JONNY TODO CIRCULAR DEPENDENCY
    if ( showMessages /* || message_dontfuckwithme*/ )
    {

        // display message if necessary
        if ( ( plr->message && !message_nottobefuckedwith ) || ( plr->message /*&& message_dontfuckwithme*/ ) )
        {
            HUlib_addMessageToSText( &w_message, 0, plr->message );
            plr->message = 0;
            message_on = true;
            message_counter = HU_MSGTIMEOUT;
            // JONNY TODO CIRCULAR DEPENDENCY
            /*message_nottobefuckedwith = message_dontfuckwithme;
            message_dontfuckwithme = 0;*/
        }

    } // else message_on = false;

    // check for incoming chat characters
    if ( netgame )
    {
        for ( i = 0; i < MAXPLAYERS; i++ )
        {
            if ( !playeringame[i] )
                continue;
            if ( i != consoleplayer && ( c = players[i].cmd.chatchar ) )
            {
                if ( c <= HU_BROADCAST )
                    chat_dest[i] = c;
                else
                {
                    if ( c >= 'a' && c <= 'z' )
                        c = (char)shiftxform[(unsigned char)c];
                    // rc = HUlib_keyInIText(&w_inputbuffer[i], c);
                    //  JONNY TODO
                    //		    if (rc && c == KEY_ENTER)
                    //		    {
                    //			if (w_inputbuffer[i].l.len
                    //			    && (chat_dest[i] == consoleplayer+1
                    //				|| chat_dest[i] ==
                    // HU_BROADCAST))
                    //			{
                    //			    HUlib_addMessageToSText(&w_message,
                    //						    player_names[i],
                    //						    w_inputbuffer[i].l.l);
                    //
                    //			    message_nottobefuckedwith = true;
                    //			    message_on = true;
                    //			    message_counter = HU_MSGTIMEOUT;
                    //			    if ( gamemode == commercial )
                    //			      S_StartSound(0, sfx_radio);
                    //			    else
                    //			      S_StartSound(0, sfx_tink);
                    //			}
                    //			HUlib_resetIText(&w_inputbuffer[i]);
                    //		    }
                }
                players[i].cmd.chatchar = 0;
            }
        }
    }
}

#define QUEUESIZE 128

static char chatchars[QUEUESIZE];
static int head = 0;
static int tail = 0;

void HU_queueChatChar( char c )
{
    if ( ( ( head + 1 ) & ( QUEUESIZE - 1 ) ) == tail )
    {
        plr->message = HUSTR_MSGU;
    }
    else
    {
        chatchars[head] = c;
        head = ( head + 1 ) & ( QUEUESIZE - 1 );
    }
}

export char HU_dequeueChatChar( void )
{
    char c;

    if ( head != tail )
    {
        c = chatchars[tail];
        tail = ( tail + 1 ) & ( QUEUESIZE - 1 );
    }
    else
    {
        c = 0;
    }

    return c;
}

export bool HU_Responder( const sf::Event &ev )
{

    static char lastmessage[HU_MAXLINELENGTH + 1];
    const char *macromessage;
    bool eatkey = false;
    static bool shiftdown = false;
    static bool altdown = false;
    unsigned char c;
    int i;
    int numplayers;

    std::array destination_keys = { HUSTR_KEYGREEN, HUSTR_KEYINDIGO, HUSTR_KEYBROWN, HUSTR_KEYRED };

    static int num_nobrainers = 0;

    numplayers = 0;
    for ( i = 0; i < MAXPLAYERS; i++ )
        numplayers += playeringame[i];

    if ( auto key_release = ev.getIf<sf::Event::KeyPressed>(); key_release )
    {
        switch ( key_release->code )
        {
        case sf::Keyboard::Key::RShift:
        case sf::Keyboard::Key::LShift:
        {
            shiftdown = false;
            return false;
        }
        case sf::Keyboard::Key::LAlt:
        case sf::Keyboard::Key::RAlt:
        {
            altdown = false;
            return false;
        }
        default:
            break;
        }
    }

    if ( auto key_press = ev.getIf<sf::Event::KeyPressed>(); key_press )
    {
        switch ( key_press->code )
        {
        case sf::Keyboard::Key::RShift:
        case sf::Keyboard::Key::LShift:
        {
            shiftdown = true;
            return false;
        }
        case sf::Keyboard::Key::LAlt:
        case sf::Keyboard::Key::RAlt:
        {
            altdown = true;
            return false;
        }
        default:
            break;
        }

        if ( !chat_on )
        {
            if ( key_press->code == sf::Keyboard::Key::Enter )
            {
                message_on = true;
                message_counter = HU_MSGTIMEOUT;
                eatkey = true;
            }
            else if ( netgame && key_press->code == sf::Keyboard::Key::T )
            {
                eatkey = chat_on = true;
                HUlib_resetIText( &w_chat );
                HU_queueChatChar( HU_BROADCAST );
            }
            else if ( netgame && numplayers > 2 )
            {
                for ( i = 0; i < MAXPLAYERS; i++ )
                {
                    if ( key_press->code == destination_keys[i] )
                    {
                        if ( playeringame[i] && i != consoleplayer )
                        {
                            eatkey = chat_on = true;
                            HUlib_resetIText( &w_chat );
                            HU_queueChatChar( i + 1 );
                            break;
                        }
                        else if ( i == consoleplayer )
                        {
                            num_nobrainers++;
                            if ( num_nobrainers < 3 )
                                plr->message = HUSTR_TALKTOSELF1;
                            else if ( num_nobrainers < 6 )
                                plr->message = HUSTR_TALKTOSELF2;
                            else if ( num_nobrainers < 9 )
                                plr->message = HUSTR_TALKTOSELF3;
                            else if ( num_nobrainers < 32 )
                                plr->message = HUSTR_TALKTOSELF4;
                            else
                                plr->message = HUSTR_TALKTOSELF5;
                        }
                    }
                }
            }
        }
        else
        {
            c = static_cast<char>( key_press->code );
            // send a macro
            if ( altdown )
            {
                c = c - '0';
                if ( c > 9 )
                    return false;
                // fprintf(stderr, "got here\n");
                macromessage = chat_macros[c];

                // kill last message with a '\n'
                // JONNY TODO
                // HU_queueChatChar(KEY_ENTER); // DEBUG!!!

                // send the macro message
                while ( *macromessage )
                    HU_queueChatChar( *macromessage++ );
                // JONNY TODO
                // HU_queueChatChar(KEY_ENTER);

                // leave chat mode and notify that it was sent
                chat_on = false;
                strcpy( lastmessage, chat_macros[c] );
                plr->message = lastmessage;
                eatkey = true;
            }
            else
            {
                if ( french )
                    c = ForeignTranslation( c );
                if ( shiftdown || ( c >= 'a' && c <= 'z' ) )
                    c = shiftxform[c];
                eatkey = HUlib_keyInIText( &w_chat, c );
                if ( eatkey )
                {
                    // static unsigned char buf[20]; // DEBUG
                    HU_queueChatChar( c );

                    // sprintf(buf, "KEY: %d => %d", ev->data1, c);
                    //      plr->message = buf;
                }
                // JONNY TODO
                //	    if (c == KEY_ENTER)
                //	    {
                //		chat_on = false;
                //		if (w_chat.l.len)
                //		{
                //		    strcpy(lastmessage, w_chat.l.l);
                //		    plr->message = lastmessage;
                //		}
                //	    }
                //	    else if (c == KEY_ESCAPE)
                //		chat_on = false;
            }
        }
    }
    else
    {
        return false;
    }

    return eatkey;
}
