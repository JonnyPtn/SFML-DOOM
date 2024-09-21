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
//
// $Log:$
//
// DESCRIPTION:
//	Main loop menu stuff.
//	Default Config File.
//	PCX Screenshots.
//
//-----------------------------------------------------------------------------
module;

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <ctype.h>

#include "doomdef.h"

#include "z_zone.h"

#include "m_argv.h"
#include "m_swap.h"

#include "w_wad.h"

#include "i_video.h"
#include "v_video.h"

#include "hu_stuff.h"

// State.
#include "doomstat.h"

// Data.
#include "dstrings.h"

#include <spdlog/spdlog.h>

#include <fstream>
#include <map>
export module m_misc;

export patch_t *hu_font[HU_FONTSIZE];

//
// M_DrawText
// Returns the final X coordinate
// HU_Init must have been called to init the font
//

int M_DrawText(int x, int y, bool direct, char *string) {
  int c;
  int w;

  while (*string) {
    c = toupper(*string) - HU_FONTSTART;
    string++;
    if (c < 0 || c > HU_FONTSIZE) {
      x += 4;
      continue;
    }

    w = SHORT(hu_font[c]->width);
    if (x + w > SCREENWIDTH)
      break;
    if (direct)
      V_DrawPatchDirect(x, y, 0, hu_font[c]);
    else
      V_DrawPatch(x, y, 0, hu_font[c]);
    x += w;
  }

  return x;
}

//
// DEFAULTS
//
int usemouse = 1;
int usejoystick = 0;

export int viewwidth;
export int viewheight;

export int mouseSensitivity = 5;

// Show messages has default, 0 = off, 1 = on
export int showMessages = 1;

// Blocky mode, has default, 0 = high, 1 = normal
export int detailLevel = 0;

export int screenblocks = 9;

// machine-independent sound params
export int numChannels = 3;

#ifdef LINUX
char *mousetype;
char *mousedev;
#endif

extern char *chat_macros[];

std::map<std::string,int&> defaults = {
    {"mouse_sensitivity", mouseSensitivity},
    //{"sfx_volume", &snd_SfxVolume},
    //{"music_volume", &snd_MusicVolume},
    {"show_messages", showMessages},
    //{"key_right", &key_right},
    //{"key_left", &key_left},
    //{"key_up", &key_up},
    //{"key_down", &key_down},
    //{"key_strafeleft", &key_strafeleft},
    //{"key_straferight", &key_straferight},
    //{"key_fire", &key_fire},
    //{"key_use", &key_use},
    //{"key_strafe", &key_strafe},
    //{"key_speed", &key_speed},
    {"use_mouse", usemouse},
    //{"mouseb_fire", &mousebfire, 0},
    //{"mouseb_strafe", &mousebstrafe, 1},
    //{"mouseb_forward", &mousebforward, 2},

    //{"use_joystick", &usejoystick, 0},
    //{"joyb_fire", &joybfire, 0},
    //{"joyb_strafe", &joybstrafe, 1},
    //{"joyb_use", &joybuse, 3},
    //{"joyb_speed", &joybspeed, 2},

    {"screenblocks", screenblocks},
    {"detaillevel", detailLevel},

    {"snd_channels", numChannels},

    {"usegamma", usegamma},
    /*
    {"chatmacro0", (int *) &chat_macros[0],
    reinterpret_cast<intptr_t>(HUSTR_CHATMACRO0) },
    {"chatmacro1", (int *) &chat_macros[1],
    reinterpret_cast<intptr_t>(HUSTR_CHATMACRO1) },
    {"chatmacro2", (int *) &chat_macros[2],
    reinterpret_cast<intptr_t>(HUSTR_CHATMACRO2) },
    {"chatmacro3", (int *) &chat_macros[3],
    reinterpret_cast<intptr_t>(HUSTR_CHATMACRO3) },
    {"chatmacro4", (int *) &chat_macros[4],
    reinterpret_cast<intptr_t>(HUSTR_CHATMACRO4) },
    {"chatmacro5", (int *) &chat_macros[5],
    reinterpret_cast<intptr_t>(HUSTR_CHATMACRO5) },
    {"chatmacro6", (int *) &chat_macros[6],
    reinterpret_cast<intptr_t>(HUSTR_CHATMACRO6) },
    {"chatmacro7", (int *) &chat_macros[7],
    reinterpret_cast<intptr_t>(HUSTR_CHATMACRO7) },
    {"chatmacro8", (int *) &chat_macros[8],
    reinterpret_cast<intptr_t>(HUSTR_CHATMACRO8) },
    {"chatmacro9", (int *) &chat_macros[9],
    reinterpret_cast<intptr_t>(HUSTR_CHATMACRO9) }*/

};

// Defaults are stored in a file with this name
constexpr auto defaults_file_name = ".doomrc";

// Path can be set via command line, or found in the home directory
std::filesystem::path defaults_file_path;

//
// M_SaveDefaults
//
export void M_SaveDefaults(void) {
  std::ofstream file(defaults_file_path);
  if (file)
  {
    for (const auto& [key, value] : defaults)
    {
      file << key << " " << value << std::endl;
    }
  }
}

//
// M_LoadDefaults
//
extern std::byte scantokey[128];

export void M_LoadDefaults(void) {

  // check for a custom default file
  const auto config_index = M_CheckParm("-config");
  if (config_index)
  {
    defaults_file_path = myargv[config_index + 1];
    spdlog::info("Defaults config set on command line: {}", defaults_file_path.string());
  }
  else if (const auto home_var = getenv("HOME"); home_var)
  {
    const std::filesystem::path home_path = home_var;
    defaults_file_path = home_path / defaults_file_name;
  }

  if (std::filesystem::exists(defaults_file_path))
  {
    spdlog::info("Defaults config found at: {}", defaults_file_path.string());

    if (std::ifstream config(defaults_file_path); config)
    {
      std::string key, value;
      while(config >> key >> value)
      {
        spdlog::info("Read default from config: {} = {}", key, value);
        assert(defaults.contains(key));
        defaults.find(key)->second = std::stoi(value);
      }
    }
    else
    {
      spdlog::error("Failed to open config: {}", defaults_file_path.string());
    }
  }
  else
  {
    spdlog::info("No default config file found");
  }
}

//
// SCREEN SHOTS
//

typedef struct {
  char manufacturer;
  char version;
  char encoding;
  char bits_per_pixel;

  unsigned short xmin;
  unsigned short ymin;
  unsigned short xmax;
  unsigned short ymax;

  unsigned short hres;
  unsigned short vres;

  unsigned char palette[48];

  char reserved;
  char color_planes;
  unsigned short bytes_per_line;
  unsigned short palette_type;

  char filler[58];
  std::byte data; // unbounded
} pcx_t;

//
// M_ScreenShot
//
export void M_ScreenShot(void) {
  // JONNY TODO
}
