#pragma once

#include "d_englsh.hpp"

#include <string>
#include <array>

// Misc. other strings.
#define SAVEGAMENAME	"doomsav"

// QuitDOOM messages
#define NUM_QUITMESSAGES   22
const std::array<std::string, NUM_QUITMESSAGES + 1> endmsg =
{
    // DOOM1
    QUITMSG,
    "please don't leave, there's more\ndemons to toast!",
    "let's beat it -- this is turning\ninto a bloodbath!",
    "i wouldn't leave if i were you.\ndos is much worse.",
    "you're trying to say you like dos\nbetter than me, right?",
    "don't leave yet -- there's a\ndemon around that corner!",
    "ya know, next time you come in here\ni'm gonna toast ya.",
    "go ahead and leave. see if i care.",

    // QuitDOOM II messages
    "you want to quit?\nthen, thou hast lost an eighth!",
    "don't go now, there's a \ndimensional shambler waiting\nat the dos prompt!",
    "get outta here and go back\nto your boring programs.",
    "if i were your boss, i'd \n deathmatch ya in a minute!",
    "look, bud. you leave now\nand you forfeit your body count!",
    "just leave. when you come\nback, i'll be waiting with a bat.",
    "you're lucky i don't smack\nyou for thinking about leaving.",

    // FinalDOOM?
    "fuck you, pussy!\nget the fuck out!",
    "you quit and i'll jizz\nin your cystholes!",
    "if you leave, i'll make\nthe lord drink my jizz.",
    "hey, ron! can we say\n'fuck' in the game?",
    "i'd leave: this is just\nmore monsters and levels.\nwhat a load.",
    "suck it down, asshole!\nyou're a fucking wimp!",
    "don't quit now! we're \nstill spending your money!",

    // Internal debug. Different style, too.
    "THIS IS NO MESSAGE!\nPage intentionally left blank."
};