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
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: m_bbox.c,v 1.1 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <errno.h>

#include "i_system.hpp"
#include "d_event.hpp"
#include "d_net.hpp"
#include "m_argv.hpp"

#include "doomstat.hpp"

#include "i_net.hpp"
#include <assert.h>




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

void	NetSend(void);
bool    NetListen(void);


//
// NETWORKING
//

int	DOOMPORT = (3074);

sf::UdpSocket			sendsocket;
sf::UdpSocket			insocket;

sf::IpAddress	sendaddress[MAXNETNODES];

void(*netget) (void);
void(*netsend) (void);


//
// PacketSend
//
void PacketSend(void)
{
    int		c;
    doomdata_t	sw;

    // byte swap
    sw.checksum = htonl(netbuffer->checksum);
    sw.player = netbuffer->player;
    sw.retransmitfrom = netbuffer->retransmitfrom;
    sw.starttic = netbuffer->starttic;
    sw.numtics = netbuffer->numtics;
    for (c = 0; c< netbuffer->numtics; c++)
    {
        sw.cmds[c].forwardmove = netbuffer->cmds[c].forwardmove;
        sw.cmds[c].sidemove = netbuffer->cmds[c].sidemove;
        sw.cmds[c].angleturn = htons(netbuffer->cmds[c].angleturn);
        sw.cmds[c].consistancy = htons(netbuffer->cmds[c].consistancy);
        sw.cmds[c].chatchar = netbuffer->cmds[c].chatchar;
        sw.cmds[c].buttons = netbuffer->cmds[c].buttons;
    }

    //printf ("sending %i\n",gametic);		
    c = sendsocket.send( &sw, doomcom->datalength
        , sendaddress[doomcom->remotenode],
        DOOMPORT);

    //	if (c == -1)
    //		I_Error ("SendPacket error: %s",strerror(errno));
}


//
// PacketGet
//
void PacketGet(void)
{
    int			    i;
    sf::IpAddress	fromaddress;
    size_t			fromlen;
    doomdata_t		sw;
    unsigned short  fromport;

    fromlen = sizeof(fromaddress);
    auto c = insocket.receive(&sw, sizeof(sw), fromlen
        , fromaddress, fromport);
    if (c != sf::Socket::Done)
    {
        //if we've received something on a blocking socket, but the status isn't done, something's gone awry
        if (insocket.isBlocking())
            I_Error("GetPacket: %s", strerror(errno));
        doomcom->remotenode = -1;		// no packet
        return;
    }

    {
        static int first = 1;
        if (first)
            printf("len=%d:p=[0x%x 0x%x] \n", c, *(int*)&sw, *((int*)&sw + 1));
        first = 0;
    }

    // find remote node number
    for (i = 0; i<doomcom->numnodes; i++)
        if (fromaddress == sendaddress[i])
            break;

    if (i == doomcom->numnodes)
    {
        // packet is not from one of the players (new game broadcast)
        doomcom->remotenode = -1;		// no packet
        return;
    }

    doomcom->remotenode = i;			// good packet from a game player
    doomcom->datalength = fromlen;

    // byte swap
    netbuffer->checksum = ntohl(sw.checksum);
    netbuffer->player = sw.player;
    netbuffer->retransmitfrom = sw.retransmitfrom;
    netbuffer->starttic = sw.starttic;
    netbuffer->numtics = sw.numtics;

    for (fromlen = 0; fromlen< netbuffer->numtics; fromlen++)
    {
        netbuffer->cmds[fromlen].forwardmove = sw.cmds[fromlen].forwardmove;
        netbuffer->cmds[fromlen].sidemove = sw.cmds[fromlen].sidemove;
        netbuffer->cmds[fromlen].angleturn = ntohs(sw.cmds[fromlen].angleturn);
        netbuffer->cmds[fromlen].consistancy = ntohs(sw.cmds[fromlen].consistancy);
        netbuffer->cmds[fromlen].chatchar = sw.cmds[fromlen].chatchar;
        netbuffer->cmds[fromlen].buttons = sw.cmds[fromlen].buttons;
    }
}



sf::IpAddress GetLocalAddress(void)
{
    return sf::IpAddress::getLocalAddress();
}


//
// I_InitNetwork
//
void I_InitNetwork(void)
{
    bool		trueval = true;
    int			i;
    int			p;
    struct hostent*	hostentry;	// host information entry

    doomcom = static_cast<doomcom_t*>(malloc(sizeof(*doomcom)));
    memset(doomcom, 0, sizeof(*doomcom));

    // set up for network
    i = CmdParameters::M_CheckParm("-dup");
    if (i && i< CmdParameters::myargc - 1)
    {
        doomcom->ticdup = CmdParameters::myargv[i + 1][0] - '0';
        if (doomcom->ticdup < 1)
            doomcom->ticdup = 1;
        if (doomcom->ticdup > 9)
            doomcom->ticdup = 9;
    }
    else
        doomcom->ticdup = 1;

    if (CmdParameters::M_CheckParm("-extratic"))
        doomcom->extratics = 1;
    else
        doomcom->extratics = 0;

    p = CmdParameters::M_CheckParm("-port");
    if (p && p<CmdParameters::myargc - 1)
    {
        DOOMPORT = atoi(CmdParameters::myargv[p + 1].c_str());
        printf("using alternate port %i\n", DOOMPORT);
    }

    // parse network game options,
    //  -net <consoleplayer> <host> <host> ...
    i = CmdParameters::M_CheckParm("-net");
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
    doomcom->consoleplayer = CmdParameters::myargv[i + 1][0] - '1';

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
    insocket.setBlocking(false);
    sendsocket.setBlocking(false);
}


void I_NetCmd(void)
{
    if (doomcom->command == CMD_SEND)
    {
        netsend();
    }
    else if (doomcom->command == CMD_GET)
    {
        netget();
    }
    else
        I_Error("Bad net cmd: %i\n", doomcom->command);
}
