//-----------------------------------------------------------------------------
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
// DESCRIPTION:
//	System specific interface stuff.
//
//-----------------------------------------------------------------------------
module;
#include "i_video.h"
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <map>

// For debug break on error
#if WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <signal.h>
#endif
export module system;

import argv;

//
// I_GetTime
// returns time in 1/70th second tics
//
export int I_GetTime( void ) {
	using namespace std::chrono;
	using tic = duration<int, std::ratio<1, 70>>;
	const auto now = steady_clock::now();
	static const auto basetime = now;
	const auto tics = duration_cast<tic>(now - basetime).count();
	return tics;
}

//
// I_Error
//
export template <typename... Args>
void I_Error( spdlog::format_string_t<Args...> fmt, Args &&...args ) {
	spdlog::error( fmt, std::forward<Args>( args )... );

#if WIN32
	DebugBreak();
#else
	raise( SIGTRAP );
#endif

	exit( -1 );
}

//
// I_Debug
//
export template <typename... Args>
void I_Log( spdlog::format_string_t<Args...> fmt, Args &&...args ) {
	spdlog::info( fmt, std::forward<Args>( args )... );
}

//
// I_Debug
//
export template <typename... Args>
void I_Debug( spdlog::format_string_t<Args...> fmt, Args &&...args ) {
	spdlog::debug( fmt, std::forward<Args>( args )... );
}

//
// I_Init
//
export void I_Init( void ) {
	//I_InitSound();
	I_InitGraphics();
}

//
// I_Quit
//
export void I_Quit( void ) {
	// TODO JONNY circular dependency
	//D_QuitNetGame();
	//I_ShutdownSound();
	//I_ShutdownMusic();
	// @TODO JONNY circular dependency
	//M_SaveDefaults();
	exit( 0 );
}



//
// DEFAULTS
//
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

extern char *chat_macros[];

std::map<std::string, int&> defaults = {
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

	//{"usegamma", usegamma},
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
export void M_SaveDefaults( void ) {
	std::ofstream file( defaults_file_path );
	if ( file )
	{
		for ( const auto& [key, value] : defaults )
		{
			file << key << " " << value << std::endl;
		}
	}
}

//
// M_LoadDefaults
//
extern std::byte scantokey[128];

export void M_LoadDefaults( void ) {

	// check for a custom default file
	const auto config_index = M_CheckParm( "-config" );
	if ( config_index )
	{
		defaults_file_path = myargv[config_index + 1];
		spdlog::info( "Defaults config set on command line: {}", defaults_file_path.string() );
	}
	else if ( const auto home_var = getenv( "HOME" ); home_var )
	{
		const std::filesystem::path home_path = home_var;
		defaults_file_path = home_path / defaults_file_name;
	}

	if ( std::filesystem::exists( defaults_file_path ) )
	{
		spdlog::info( "Defaults config found at: {}", defaults_file_path.string() );

		if ( std::ifstream config( defaults_file_path ); config )
		{
			std::string key, value;
			while ( config >> key >> value )
			{
				spdlog::info( "Read default from config: {} = {}", key, value );
				assert( defaults.contains( key ) );
				defaults.find( key )->second = std::stoi( value );
			}
		}
		else
		{
			spdlog::error( "Failed to open config: {}", defaults_file_path.string() );
		}
	}
	else
	{
		spdlog::info( "No default config file found" );
	}
}