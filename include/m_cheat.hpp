#pragma once

#include <string>
#include <SFML/Window/Event.hpp>

//
// CHEAT SEQUENCE PACKAGE
//

#define SCRAMBLE(a) \
((((a)&1)<<7) + (((a)&2)<<5) + ((a)&4) + (((a)&8)<<1) \
 + (((a)&16)>>1) + ((a)&32) + (((a)&64)>>5) + (((a)&128)>>7))

typedef struct
{
   std::string	sequence;
   sf::String	soFar;
    
} cheatseq_t;

bool cht_CheckCheat(cheatseq_t& sequence, sf::Event* ev);

// Needs refactor
void
cht_GetParam
( cheatseq_t*		cht,
  char*			buffer );
