#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#ifdef _WIN32
#include "unistd.hpp"
#else
#include "unistd.h"
#endif

#include "i_system.hpp"
#include "d_event.hpp"
#include "d_net.hpp"
#include "m_argv.hpp"

#include "doomstat.hpp"

#ifndef _WIN32
#include <errno.h>
#endif

#include "i_net.hpp"

//SFML networking
#include <SFML/Network.hpp>

// For some odd reason...
#define ntohl(x) \
        ((unsigned long int)((((unsigned long int)(x) & 0x000000ffU) << 24) | \
                             (((unsigned long int)(x) & 0x0000ff00U) <<  8) | \
                             (((unsigned long int)(x) & 0x00ff0000U) >>  8) | \
                             (((unsigned long int)(x) & 0xff000000U) >> 24)))

#define ntohs(x) \
        ((unsigned short int)((((unsigned short int)(x) & 0x00ff) << 8) | \
                              (((unsigned short int)(x) & 0xff00) >> 8))) \
	  
#define htonl(x) ntohl(x)
#define htons(x) ntohs(x)

void	NetSend (void);
bool NetListen (void);


//
// NETWORKING
//

int	DOOMPORT = 53005;

sf::UdpSocket	sendsocket;
sf::UdpSocket	insocket;

sf::IpAddress	sendaddress[MAXNETNODES];

void	(*netget) (void);
void	(*netsend) (void);


//
// BindToLocalPort
//
void BindToLocalPort( sf::UdpSocket socket, int port )
{			
	auto result = socket.bind(port);
	if (result != sf::UdpSocket::Done)
			I_Error ("BindToPort: bind: %s", strerror(errno));
}


//
// PacketSend
//
void PacketSend (void)
{
    int		c;
    doomdata_t	sw;
				
    // unsigned char swap
    sw.checksum = htonl(netbuffer->checksum);
    sw.player = netbuffer->player;
    sw.retransmitfrom = netbuffer->retransmitfrom;
    sw.starttic = netbuffer->starttic;
    sw.numtics = netbuffer->numtics;
    for (c=0 ; c< netbuffer->numtics ; c++)
    {
		sw.cmds[c].forwardmove = netbuffer->cmds[c].forwardmove;
		sw.cmds[c].sidemove = netbuffer->cmds[c].sidemove;
		sw.cmds[c].angleturn = htons(netbuffer->cmds[c].angleturn);
		sw.cmds[c].consistancy = htons(netbuffer->cmds[c].consistancy);
		sw.cmds[c].chatchar = netbuffer->cmds[c].chatchar;
		sw.cmds[c].buttons = netbuffer->cmds[c].buttons;
    }
		
	printf ("sending %s\n",sendaddress[doomcom->remotenode].toString().c_str());		
    auto result = sendsocket.send(static_cast<void*>(&sw), doomcom->datalength,
		sendaddress[doomcom->remotenode], DOOMPORT);
    	if (result != sf::Socket::Done)
    		I_Error ("SendPacket error: %s",strerror(errno));
}


//
// PacketGet
//
void PacketGet (void)
{
	doomdata_t		sw;
	size_t			received;
	sf::IpAddress	fromAddress;
	unsigned short	port = DOOMPORT;

	auto result = insocket.receive(&sw, sizeof(sw),received
	  , fromAddress, port);
    if (result != sf::Socket::Done)
    {
		if (insocket.isBlocking())
			I_Error ("GetPacket: %i",result);
		doomcom->remotenode = -1;		// no packet
		return;
    }

    {
		static int first=1;
		if (first)
			printf("received len=%d:p=[0x%x 0x%x] \n", received, *(int*)&sw, *((int*)&sw+1));
		first = 0;
    }

    // find remote node number
	int i;
    for (i=0 ; i<doomcom->numnodes ; i++)
		if ( fromAddress == sendaddress[i])
			break;

    if (i == doomcom->numnodes)
    {
		// packet is not from one of the players (new game broadcast)
		doomcom->remotenode = -1;		// no packet
		return;
    }
	
    doomcom->remotenode = i;			// good packet from a game player
    doomcom->datalength = received;
	
    // unsigned char swap
    netbuffer->checksum = ntohl(sw.checksum);
    netbuffer->player = sw.player;
    netbuffer->retransmitfrom = sw.retransmitfrom;
    netbuffer->starttic = sw.starttic;
    netbuffer->numtics = sw.numtics;

    for (int c=0 ; c< netbuffer->numtics ; c++)
    {
		netbuffer->cmds[c].forwardmove = sw.cmds[c].forwardmove;
		netbuffer->cmds[c].sidemove = sw.cmds[c].sidemove;
		netbuffer->cmds[c].angleturn = ntohs(sw.cmds[c].angleturn);
		netbuffer->cmds[c].consistancy = ntohs(sw.cmds[c].consistancy);
		netbuffer->cmds[c].chatchar = sw.cmds[c].chatchar;
		netbuffer->cmds[c].buttons = sw.cmds[c].buttons;
    }
}



int GetLocalAddress (void)
{
	/*
    char		hostname[1024];
    struct hostent*	hostentry(NULL);	// host information entry
    int			v(0);

    // get local address
	v = gethostname (hostname, sizeof(hostname));
	if (v == -1)
		I_Error ("GetLocalAddress : gethostname: errno %d",errno);
	
   hostentry = gethostbyname (hostname);
    if (!hostentry)
	I_Error ("GetLocalAddress : gethostbyname: couldn't get local host");
		
	//JONNY//	  return *(int *)hostentry->h_addr_list[0];*/
	return 0;
}


//
// I_InitNetwork
//
void I_InitNetwork (void)
{
    bool		trueval = true;
    int			i(0);
    int			p(0);
    struct hostent*	hostentry(NULL);	// host information entry
	
    doomcom = (doomcom_t*)malloc (sizeof (*doomcom) );
    memset (doomcom, 0, sizeof(*doomcom) );
    
    // set up for network
    i = CmdParameters::M_CheckParm ("-dup");
    if (i && i< CmdParameters::myargc-1)
    {
		doomcom->ticdup = CmdParameters::myargv[i+1][0]-'0';
		if (doomcom->ticdup < 1)
		    doomcom->ticdup = 1;
		if (doomcom->ticdup > 9)
		    doomcom->ticdup = 9;
    }
    else
	doomcom-> ticdup = 1;
	
    if (CmdParameters::M_CheckParm ("-extratic"))
		doomcom-> extratics = 1;
    else
		doomcom-> extratics = 0;
		
    p = CmdParameters::M_CheckParm ("-port");
    if (p && p<CmdParameters::myargc-1)
    {
		DOOMPORT = atoi (CmdParameters::myargv[p+1].c_str());
		printf ("using alternate port %i\n",DOOMPORT);
    }
    
    // parse network game options,
    //  -net <consoleplayer> <host> <host> ...
    i = CmdParameters::M_CheckParm ("-net");
    if (!i)
    {
		// single player game
		netgame = false;
		doomcom->id = DOOMCOM_ID;
		doomcom->numplayers = doomcom->numnodes = 1;
		doomcom->deathmatch = false;
		doomcom->consoleplayer = 0;
		return;
    }

    netsend = PacketSend;
    netget = PacketGet;
    netgame = true;

    // parse player number and host list
	//doomcom->consoleplayer = CmdParameters::myargv[i+1][0]-'1';

	//cin instead, we still haven't opened the window yet
	std::cout << "Enter player number (0 is host):";
	std::cin >> doomcom->consoleplayer;

    doomcom->numnodes = 1;	// this node for sure
	
    i++;
    while (++i < CmdParameters::myargc && CmdParameters::myargv[i][0] != '-')
    {
		sendaddress[doomcom->numnodes] = CmdParameters::myargv[i];
		doomcom->numnodes++;
    }
	
    doomcom->id = DOOMCOM_ID;
    doomcom->numplayers = doomcom->numnodes;
    
    // build message to receive
	insocket.bind(DOOMPORT);
}


void I_NetCmd (void)
{
    if (doomcom->command == CMD_SEND)
    {
		netsend ();
    }
    else if (doomcom->command == CMD_GET)
    {
		netget ();
    }
    else
		I_Error ("Bad net cmd: %i\n",doomcom->command);
}
