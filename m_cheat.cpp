#include "m_cheat.hpp"

//
// CHEAT SEQUENCE PACKAGE
//

static int		firsttime = 1;
static unsigned char	cheat_xlate_table[256];


//
// Called in st_stuff module, which handles the input.
// Returns a 1 if the cheat was successful, 0 if failed.
//
bool	cht_CheckCheat(cheatseq_t& sequence, sf::Event* ev)
{
	//first append the new char to the sequence so far
	sequence.soFar += ev->text.unicode;

	//typed enough characters?
	if (sequence.soFar.getSize() >= sequence.sequence.length())
	{
		//do they match?
		if (std::string(sequence.soFar) == sequence.sequence)
		{
			//match
			sequence.soFar.clear();
			return true;
		}
		else
		{
			//no match
			sequence.soFar.clear();
			return false;
		}
	}
	return false;
}

void
cht_GetParam
( cheatseq_t*	cht,
  char*		buffer )
{

	//JONNY// TODO
  /*  unsigned char *p, c;

    p = cht->sequence;
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
	*/
}


