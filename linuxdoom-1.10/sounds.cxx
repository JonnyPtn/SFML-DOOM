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
//	Created by a sound utility.
//	Kept as a sample, DOOM2 sounds.
//
//-----------------------------------------------------------------------------


#include "sounds.h"
#include "doomtype.h"

//
// Information about all the music
//

musicinfo_t S_music[] = {{0},           {"e1m1", 0},   {"e1m2", 0},   {"e1m3", 0},   {"e1m4", 0},   {"e1m5", 0},   {"e1m6", 0},   {"e1m7", 0},   {"e1m8", 0},
                         {"e1m9", 0},   {"e2m1", 0},   {"e2m2", 0},   {"e2m3", 0},   {"e2m4", 0},   {"e2m5", 0},   {"e2m6", 0},   {"e2m7", 0},   {"e2m8", 0},
                         {"e2m9", 0},   {"e3m1", 0},   {"e3m2", 0},   {"e3m3", 0},   {"e3m4", 0},   {"e3m5", 0},   {"e3m6", 0},   {"e3m7", 0},   {"e3m8", 0},
                         {"e3m9", 0},   {"inter", 0},  {"intro", 0},  {"bunny", 0},  {"victor", 0}, {"introa", 0}, {"runnin", 0}, {"stalks", 0}, {"countd", 0},
                         {"betwee", 0}, {"doom", 0},   {"the_da", 0}, {"shawn", 0},  {"ddtblu", 0}, {"in_cit", 0}, {"dead", 0},   {"stlks2", 0}, {"theda2", 0},
                         {"doom2", 0},  {"ddtbl2", 0}, {"runni2", 0}, {"dead2", 0},  {"stlks3", 0}, {"romero", 0}, {"shawn2", 0}, {"messag", 0}, {"count2", 0},
                         {"ddtbl3", 0}, {"ampie", 0},  {"theda3", 0}, {"adrian", 0}, {"messg2", 0}, {"romer2", 0}, {"tense", 0},  {"shawn3", 0}, {"openin", 0},
                         {"evil", 0},   {"ultima", 0}, {"read_m", 0}, {"dm2ttl", 0}, {"dm2int", 0}};

//
// Information about all the sfx
//

sfxinfo_t S_sfx[] = {
    // S_sfx[0] needs to be a dummy for odd reasons.
    {"none", false},

    {"pistol", false},  {"shotgn", false},
    {"sgcock", false},  {"dshtgn", false},
    {"dbopn", false},   {"dbcls", false},
    {"dbload", false},  {"plasma", false},
    {"bfg", false},     {"sawup", false},
    {"sawidl", false}, {"sawful", false},
    {"sawhit", false},  {"rlaunc", false},
    {"rxplod", false},  {"firsht", false},
    {"firxpl", false},  {"pstart", false},
    {"pstop", false},  {"doropn", false},
    {"dorcls", false,}, {"stnmov", false},
    {"swtchn", false},  {"swtchx", false},
    {"plpain", false},  {"dmpain", false},
    {"popain", false},  {"vipain", false},
    {"mnpain", false},  {"pepain", false},
    {"slop", false},    {"itemup", true},
    {"wpnup", true},    {"oof", false},
    {"telept", false},  {"posit1", true},
    {"posit2", true},   {"posit3", true},
    {"bgsit1", true},   {"bgsit2", true},
    {"sgtsit", true},   {"cacsit", true},
    {"brssit", true},   {"cybsit", true},
    {"spisit", true},   {"bspsit", true},
    {"kntsit", true},   {"vilsit", true},
    {"mansit", true},   {"pesit", true},
    {"sklatk", false},  {"sgtatk", false},
    {"skepch", false},  {"vilatk", false},
    {"claw", false},    {"skeswg", false},
    {"pldeth", false},  {"pdiehi", false},
    {"podth1", false},  {"podth2", false},
    {"podth3", false},  {"bgdth1", false},
    {"bgdth2", false},  {"sgtdth", false},
    {"cacdth", false},  {"skldth", false},
    {"brsdth", false},  {"cybdth", false},
    {"spidth", false},  {"bspdth", false},
    {"vildth", false},  {"kntdth", false},
    {"pedth", false},   {"skedth", false},
    {"posact", true},  {"bgact", true},
    {"dmact", true},   {"bspact", true},
    {"bspwlk", true},  {"vilact", true},
    {"noway", false},   {"barexp", false},
    {"punch", false},   {"hoof", false},
    {"metal", false},   {"chgun", false},
    {"tink", false},    {"bdopn", false},
    {"bdcls", false},  {"itmbk", false},
    {"flame", false},   {"flamst", false},
    {"getpow", false},  {"bospit", false},
    {"boscub", false},  {"bossit", false},
    {"bospn", false},   {"bosdth", false},
    {"manatk", false},  {"mandth", false},
    {"sssit", false},   {"ssdth", false},
    {"keenpn", false},  {"keendt", false},
    {"skeact", false},  {"skesit", false},
    {"skeatk", false},  {"radio", false}};