#include "m_cheat.hpp"

//
// CHEAT SEQUENCE PACKAGE
//

//
// Called in st_stuff module, which handles the input.
// Returns a 1 if the cheat was successful, 0 if failed.
//
bool	cht_CheckCheat(cheatseq_t& sequence, sf::Event* ev)
{
	//first append the new char to the sequence so far
	sequence.soFar += ev->text.unicode;
	auto sequenceLength(sequence.sequence.length());
	if (sequence.soFar.getSize() > sequenceLength)
		sequence.soFar.erase(0);

	//do they match?
	if (std::string(sequence.soFar) == sequence.sequence)
		return true;
	else
		return false;
}

void
cht_GetParam
( cheatseq_t*	cht,
  char*		buffer )
{
    char *p, c;

    p = const_cast<char*>(cht->sequence.data());
    while (*(p++) != 1);
    
    do
    {
	c = *p;
	*(buffer++) = c;
	*(p++) = 0;
    }
    while (c && *p!=0xff );

    if (*p==0xff)
	    *buffer = 0;
}


