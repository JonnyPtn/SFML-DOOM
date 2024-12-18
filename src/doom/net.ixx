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
//	DOOM Network game communication and protocol,
//	all OS independend parts.
//
//-----------------------------------------------------------------------------
module;

#include "g_game.h"
#include "i_video.h"

export module net;

import engine;
import engine;
import menu;
import doom;
import app;

// For some odd reason...
#ifndef ntohl
#define ntohl( x )                                                                                                                                             \
    ( (unsigned long int)( ( ( (unsigned long int)( x ) & 0x000000ffU ) << 24 ) | ( ( (unsigned long int)( x ) & 0x0000ff00U ) << 8 ) |                        \
                           ( ( (unsigned long int)( x ) & 0x00ff0000U ) >> 8 ) | ( ( (unsigned long int)( x ) & 0xff000000U ) >> 24 ) ) )
#endif
#ifndef ntohs
#define ntohs( x ) ( (unsigned short int)( ( ( (unsigned short int)( x ) & 0x00ff ) << 8 ) | ( ( (unsigned short int)( x ) & 0xff00 ) >> 8 ) ) )

#endif
#ifndef htonl
#define htonl( x ) ntohl( x )
#endif
#ifndef htons
#define htons( x ) ntohs( x )
#endif

export constexpr auto DOOMCOM_ID = 0x12345678l;

// Networking and tick handling related.
export constexpr auto BACKUPTICS = 12;

typedef enum
{
    CMD_SEND = 1,
    CMD_GET = 2

} command_t;

//
// Network packet data.
//
export struct doomdata_t
{
    // High bit is retransmit request.
    unsigned checksum;
    // Only valid if NCMD_RETRANSMIT.
    std::byte retransmitfrom;

    std::byte starttic;
    std::byte player;
    std::byte numtics;
    ticcmd_t cmds[BACKUPTICS];
};

export struct doomcom_t
{
    // Supposed to be DOOMCOM_ID?
    long id;

    // DOOM executes an int to execute commands.
    short intnum;
    // Communication between DOOM and the driver.
    // Is CMD_SEND or CMD_GET.
    short command;
    // Is dest for send, set by get (-1 = no packet).
    short remotenode;

    // Number of bytes in doomdata to be sent
    short datalength;

    // Info common to all nodes.
    // Console is allways node 0.
    short numnodes;
    // Flag: 1 = no duplication, 2-5 = dup for slow nets.
    short ticdup;
    // Flag: 1 = send a backup tic in every packet.
    short extratics;
    // Flag: 1 = deathmatch.
    short deathmatch;
    // Flag: -1 = new game, 0-5 = load savegame
    short savegame;
    short episode; // 1-3
    short map;     // 1-9
    short skill;   // 1-5

    // Info specific to this node.
    short consoleplayer;
    short numplayers;

    // These are related to the 3-display mode,
    //  in which two drones looking left and right
    //  were used to render two additional views
    //  on two additional computers.
    // Probably not operational anymore.
    // 1 = left, 0 = center, -1 = right
    short angleoffset;
    // 1 = drone
    short drone;

    // The packet data to be sent.
    doomdata_t data;
};

export doomcom_t doomcom;
export doomdata_t *netbuffer; // points inside doomcom

//
// NETWORKING
//

// int	DOOMPORT =	(IPPORT_USERRESERVED +0x1d );

int sendsocket;
int insocket;

// struct	sockaddr_in	sendaddress[MAXNETNODES];

void ( *netget )( void );

void ( *netsend )( void );

//
// UDPsocket
//
int UDPsocket( void )
{
    int s{};

    // allocate a socket
    //    s = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    //    if (s<0)
    logger::error( "can't create socket: {}", strerror( errno ) );

    return s;
}

//
// BindToLocalPort
//
void BindToLocalPort( int s, int port )
{
    //    struct sockaddr_in	address;

    //    memset (&address, 0, sizeof(address));
    //    address.sin_family = AF_INET;
    //    address.sin_addr.s_addr = INADDR_ANY;
    //    address.sin_port = port;
    // JONNY TODO
    // v = bind (s, (void *)&address, sizeof(address));
    // if (v == -1)
    logger::error( "BindToPort: bind: %s", strerror( errno ) );
}

//
// PacketSend
//
void PacketSend( void )
{
    int c;
    doomdata_t sw;

    // byte swap
    sw.checksum = htonl( netbuffer->checksum );
    sw.player = netbuffer->player;
    sw.retransmitfrom = netbuffer->retransmitfrom;
    sw.starttic = netbuffer->starttic;
    sw.numtics = netbuffer->numtics;
    for ( c = 0; c < static_cast<int>( netbuffer->numtics ); c++ )
    {
        sw.cmds[c].forwardmove = netbuffer->cmds[c].forwardmove;
        sw.cmds[c].sidemove = netbuffer->cmds[c].sidemove;
        sw.cmds[c].angleturn = htons( netbuffer->cmds[c].angleturn );
        sw.cmds[c].consistancy = htons( netbuffer->cmds[c].consistancy );
        sw.cmds[c].chatchar = netbuffer->cmds[c].chatchar;
        sw.cmds[c].buttons = netbuffer->cmds[c].buttons;
    }

    // printf ("sending {}\n",gametic);
    //  JONNY TODO
    // c = sendto (sendsocket , &sw, doomcom.datalength
    //	,0,(void *)&sendaddress[doomcom.remotenode]
    //	,sizeof(sendaddress[doomcom.remotenode]));

    //	if (c == -1)
    //		logger::error ("SendPacket error: %s",strerror(errno));
}

//
// PacketGet
//
void PacketGet( void )
{
    int i;
    int c{ -1 };
    //    struct sockaddr_in	fromaddress;
    doomdata_t sw{};

    // JONNY TODO
    // fromlen = sizeof(fromaddress);
    // c = recvfrom (insocket, &sw, sizeof(sw), 0
    //	  , (struct sockaddr *)&fromaddress, &fromlen );
    if ( c == -1 )
    {
        if ( errno != EWOULDBLOCK )
            logger::error( "GetPacket: %s", strerror( errno ) );
        doomcom.remotenode = -1; // no packet
        return;
    }

    {
        static int first = 1;
        if ( first )
            //	    printf("len=%d:p=[0x%x 0x%x] \n", c, *(int*)&sw,
            //*((int*)&sw+1));
            first = 0;
    }

    // find remote node number
    for ( i = 0; i < doomcom.numnodes; i++ )
        //	if ( fromaddress.sin_addr.s_addr ==
        // sendaddress[i].sin_addr.s_addr )
        break;

    if ( i == doomcom.numnodes )
    {
        // packet is not from one of the players (new game broadcast)
        doomcom.remotenode = -1; // no packet
        return;
    }

    doomcom.remotenode = i; // good packet from a game player
    doomcom.datalength = c;

    // byte swap
    netbuffer->checksum = ntohl( sw.checksum );
    netbuffer->player = sw.player;
    netbuffer->retransmitfrom = sw.retransmitfrom;
    netbuffer->starttic = sw.starttic;
    netbuffer->numtics = sw.numtics;

    for ( c = 0; c < static_cast<int>( netbuffer->numtics ); c++ )
    {
        netbuffer->cmds[c].forwardmove = sw.cmds[c].forwardmove;
        netbuffer->cmds[c].sidemove = sw.cmds[c].sidemove;
        netbuffer->cmds[c].angleturn = ntohs( sw.cmds[c].angleturn );
        netbuffer->cmds[c].consistancy = ntohs( sw.cmds[c].consistancy );
        netbuffer->cmds[c].chatchar = sw.cmds[c].chatchar;
        netbuffer->cmds[c].buttons = sw.cmds[c].buttons;
    }
}

int GetLocalAddress( void )
{
    char hostname[1024];
    struct hostent *hostentry; // host information entry
    int v;

    // get local address
    //    v = gethostname (hostname, sizeof(hostname));
    //    if (v == -1)
    logger::error( "GetLocalAddress : gethostname: errno %d", errno );

    //    hostentry = gethostbyname (hostname);
    //   if (!hostentry)
    logger::error( "GetLocalAddress : gethostbyname: couldn't get local host" );

    //    return *(int *)hostentry->h_addr_list[0];
    return {};
}

//
// I_InitNetwork
//
void I_InitNetwork( void )
{
    bool trueval = true;
    int i;
    int p;
    struct hostent *hostentry; // host information entry

    // set up for network
    i = arguments::has( "-dup" );
    if ( i && i < arguments::count() - 1 )
    {
        doomcom.ticdup = arguments::at( i + 1 )[0] - '0';
        if ( doomcom.ticdup < 1 )
            doomcom.ticdup = 1;
        if ( doomcom.ticdup > 9 )
            doomcom.ticdup = 9;
    }
    else
        doomcom.ticdup = 1;

    if ( arguments::has( "-extratic" ) )
        doomcom.extratics = 1;
    else
        doomcom.extratics = 0;

    p = arguments::has( "-port" );
    if ( p && p < arguments::count() - 1 )
    {
        //	DOOMPORT = atoi (arguments::at(p+1).c_str());
        //	printf ("using alternate port {}\n",DOOMPORT);
    }

    // parse network game options,
    //  -net <consoleplayer> <host> <host> ...
    i = arguments::has( "-net" );
    if ( !i )
    {
        // single player game
        netgame = false;
        doomcom.id = DOOMCOM_ID;
        doomcom.numplayers = doomcom.numnodes = 1;
        doomcom.deathmatch = false;
        doomcom.consoleplayer = 0;
        return;
    }

    netsend = PacketSend;
    netget = PacketGet;
    netgame = true;

    // parse player number and host list
    doomcom.consoleplayer = arguments::at( i + 1 )[0] - '1';

    doomcom.numnodes = 1; // this node for sure

    i++;
    while ( ++i < arguments::count() && arguments::at( i )[0] != '-' )
    {
        //	sendaddress[doomcom.numnodes].sin_family = AF_INET;
        //	sendaddress[doomcom.numnodes].sin_port = htons(DOOMPORT);
        if ( arguments::at( i )[0] == '.' )
        {
            //	    sendaddress[doomcom.numnodes].sin_addr.s_addr
            //		= inet_addr (arguments::at(i+1).c_str());
        }
        else
        {
            //	    hostentry = gethostbyname (arguments::at(i).c_str());
            //	    if (!hostentry)
            logger::error( "gethostbyname: couldn't find %s", arguments::at( i ).data() );
            //	    sendaddress[doomcom.numnodes].sin_addr.s_addr
            //		= *(int *)hostentry->h_addr_list[0];
        }
        doomcom.numnodes++;
    }

    doomcom.id = DOOMCOM_ID;
    doomcom.numplayers = doomcom.numnodes;

    // build message to receive
    insocket = UDPsocket();
    //    BindToLocalPort (insocket,htons(DOOMPORT));
    //    ioctl (insocket, FIONBIO, &trueval);

    sendsocket = UDPsocket();
}

void I_NetCmd( void )
{
    if ( doomcom.command == CMD_SEND )
    {
        netsend();
    }
    else if ( doomcom.command == CMD_GET )
    {
        netget();
    }
    else
        logger::error( "Bad net cmd: {}\n", doomcom.command );
}

//
// Network play related stuff.
// There is a data struct that stores network
//  communication related stuff, and another
//  one that defines the actual packets to
//  be transmitted.
//

// Max computers/players in a game.
#define MAXNETNODES 8

#define NCMD_EXIT 0x80000000
#define NCMD_RETRANSMIT 0x40000000
#define NCMD_SETUP 0x20000000
#define NCMD_KILL 0x10000000 // kill game
#define NCMD_CHECKSUM 0x0fffffff

//
// NETWORKING
//
// gametic is the tic about to (or currently being) run
// maketic is the tick that hasn't had control made for it yet
// nettics[] has the maketics for all players
//
// a gametic cannot be run until nettics[] > gametic for all players
//
#define RESENDCOUNT 10
#define PL_DRONE 0x80 // bit flag in doomdata->player

ticcmd_t localcmds[BACKUPTICS];

export inline ticcmd_t netcmds[MAXPLAYERS][BACKUPTICS];
int nettics[MAXNETNODES];
bool nodeingame[MAXNETNODES];   // set false as nodes leave game
bool remoteresend[MAXNETNODES]; // set when local needs tics
int resendto[MAXNETNODES];      // set when remote needs tics
int resendcount[MAXNETNODES];

int nodeforplayer[MAXPLAYERS];

export int maketic;
int lastnettic;
int skiptics;
export int ticdup;
int maxsend; // BACKUPTICS/(2*ticdup)-1

bool reboundpacket;
doomdata_t reboundstore;

//
//
//
int NetbufferSize( void )
{
    // JONNY TODO
    // return (int)&(((doomdata_t *)0)->cmds[netbuffer->numtics]);
    return {};
}

//
// Checksum
//
unsigned NetbufferChecksum( void )
{
    unsigned c;

    c = 0x1234567;

    // FIXME -endianess?
#ifdef NORMALUNIX
    return 0; // byte order problems
#endif

    // JONNY TODO
    // l = (NetbufferSize () - (int)&(((doomdata_t *)0)->retransmitfrom))/4;
    // for (i=0 ; i<l ; i++)
    // c += ((unsigned *)&netbuffer->retransmitfrom)[i] * (i+1);

    return c & NCMD_CHECKSUM;
}

//
//
//
int ExpandTics( int low )
{
    int delta;

    delta = low - ( maketic & 0xff );

    if ( delta >= -64 && delta <= 64 )
        return ( maketic & ~0xff ) + low;
    if ( delta > 64 )
        return ( maketic & ~0xff ) - 256 + low;
    if ( delta < -64 )
        return ( maketic & ~0xff ) + 256 + low;

    logger::error( "ExpandTics: strange value {} at maketic {}", low, maketic );
    return 0;
}

//
// HSendPacket
//
void HSendPacket( int node, int flags )
{
    netbuffer->checksum = NetbufferChecksum() | flags;

    if ( !node )
    {
        reboundstore = *netbuffer;
        reboundpacket = true;
        return;
    }

    if ( demoplayback )
        return;

    if ( !netgame )
        logger::error( "Tried to transmit to another node" );

    doomcom.command = CMD_SEND;
    doomcom.remotenode = node;
    doomcom.datalength = NetbufferSize();

    I_NetCmd();
}

//
// HGetPacket
// Returns false if no packet is waiting
//
bool HGetPacket( void )
{
    if ( reboundpacket )
    {
        *netbuffer = reboundstore;
        doomcom.remotenode = 0;
        reboundpacket = false;
        return true;
    }

    if ( !netgame )
        return false;

    if ( demoplayback )
        return false;

    doomcom.command = CMD_GET;
    I_NetCmd();

    if ( doomcom.remotenode == -1 )
        return false;

    if ( doomcom.datalength != NetbufferSize() )
    {
        return false;
    }

    if ( NetbufferChecksum() != ( netbuffer->checksum & NCMD_CHECKSUM ) )
    {
        return false;
    }
    return true;
}

//
// GetPackets
//
char exitmsg[80];

void GetPackets( void )
{
    int netconsole;
    int netnode;
    ticcmd_t *src, *dest;
    int realend;
    int realstart;

    while ( HGetPacket() )
    {
        if ( netbuffer->checksum & NCMD_SETUP )
            continue; // extra setup packet

        netconsole = static_cast<int>( netbuffer->player ) & ~PL_DRONE;
        netnode = doomcom.remotenode;

        // to save bytes, only the low byte of tic numbers are sent
        // Figure out what the rest of the bytes are
        realstart = ExpandTics( static_cast<int>( netbuffer->starttic ) );
        realend = ( realstart + static_cast<int>( netbuffer->numtics ) );

        // check for exiting the game
        if ( netbuffer->checksum & NCMD_EXIT )
        {
            if ( !nodeingame[netnode] )
                continue;
            nodeingame[netnode] = false;
            playeringame[netconsole] = false;
            strcpy( exitmsg, "Player 1 left the game" );
            exitmsg[7] += netconsole;
            players[consoleplayer].message = exitmsg;
            // TODO JONNY circular dependency
            // if (demorecording)
            // G_CheckDemoStatus();
            continue;
        }

        // check for a remote game kill
        if ( netbuffer->checksum & NCMD_KILL )
            logger::error( "Killed by network driver" );

        nodeforplayer[netconsole] = netnode;

        // check for retransmit request
        if ( resendcount[netnode] <= 0 && ( netbuffer->checksum & NCMD_RETRANSMIT ) )
        {
            resendto[netnode] = ExpandTics( static_cast<int>( netbuffer->retransmitfrom ) );
            resendcount[netnode] = RESENDCOUNT;
        }
        else
            resendcount[netnode]--;

        // check for out of order / duplicated packet
        if ( realend == nettics[netnode] )
            continue;

        if ( realend < nettics[netnode] )
        {
            continue;
        }

        // check for a missed packet
        if ( realstart > nettics[netnode] )
        {
            // stop processing until the other system resends the missed tics
            remoteresend[netnode] = true;
            continue;
        }

        // update command store from the packet
        {
            int start;

            remoteresend[netnode] = false;

            start = nettics[netnode] - realstart;
            src = &netbuffer->cmds[start];

            while ( nettics[netnode] < realend )
            {
                dest = &netcmds[netconsole][nettics[netnode] % BACKUPTICS];
                nettics[netnode]++;
                *dest = *src;
                src++;
            }
        }
    }
}

//
// NetUpdate
// Builds ticcmds for console player,
// sends out a packet
//
int gametime;

export void NetUpdate( void )
{
    int nowtime;
    int newtics;
    int i, j;
    int realstart;
    int gameticdiv;

    // check time
    nowtime = get_current_tick() / ticdup;
    newtics = nowtime - gametime;
    gametime = nowtime;

    if ( newtics <= 0 ) // nothing new to update
        goto listen;

    if ( skiptics <= newtics )
    {
        newtics -= skiptics;
        skiptics = 0;
    }
    else
    {
        skiptics -= newtics;
        newtics = 0;
    }

    netbuffer->player = static_cast<std::byte>( consoleplayer );

    // build new ticcmds for console player
    gameticdiv = gametic / ticdup;
    for ( i = 0; i < newtics; i++ )
    {
        I_StartTic();
        if ( maketic - gameticdiv >= BACKUPTICS / 2 - 1 )
            break; // can't hold any more

        // printf ("mk:%i ",maketic);
        G_BuildTiccmd( &localcmds[maketic % BACKUPTICS] );
        maketic++;
    }

    // TODO JONNY circular dependency
    // if (singletics)
    // return; // singletic update is syncronous

    // send the packet to the other nodes
    for ( i = 0; i < doomcom.numnodes; i++ )
        if ( nodeingame[i] )
        {
            realstart = resendto[i];
            netbuffer->starttic = static_cast<std::byte>( realstart );
            netbuffer->numtics = static_cast<std::byte>( maketic - realstart );
            if ( static_cast<int>( netbuffer->numtics ) > BACKUPTICS )
                logger::error( "NetUpdate: netbuffer->numtics > BACKUPTICS" );

            resendto[i] = maketic - doomcom.extratics;

            for ( j = 0; j < static_cast<int>( netbuffer->numtics ); j++ )
                netbuffer->cmds[j] = localcmds[( realstart + j ) % BACKUPTICS];

            if ( remoteresend[i] )
            {
                netbuffer->retransmitfrom = static_cast<std::byte>( nettics[i] );
                HSendPacket( i, NCMD_RETRANSMIT );
            }
            else
            {
                netbuffer->retransmitfrom = std::byte{ 0 };
                HSendPacket( i, 0 );
            }
        }

// listen for other packets
listen:
    GetPackets();
}

//
// CheckAbort
//
void CheckAbort( void )
{
    int stoptic{};

    // TODO JONNY circular dependency
    // stoptic = get_current_tick() + 2;
    while ( get_current_tick() < stoptic )
        I_StartTic();

    I_StartTic();
    // TODO JONNY circular dependency
    // for (; eventtail != eventhead; eventtail = (++eventtail) & (MAXEVENTS -
    // 1)) {
    // JONNY TODO
    // ev = events[eventtail];
    // if (ev.type == sf::Event::KeyPressed && ev.key.code ==
    // sf::Keyboard::Key::Escape)
    //    logger::error ("Network game synchronization aborted.");
    //}
}

//
// D_ArbitrateNetStart
//
void D_ArbitrateNetStart( void )
{
    int i;
    bool gotinfo[MAXNETNODES];

    // TODO JONNY circular dependency
    // autostart = true;
    memset( gotinfo, 0, sizeof( gotinfo ) );

    if ( doomcom.consoleplayer )
    {
        // listen for setup info from key player
        printf( "listening for network start info...\n" );
        while ( 1 )
        {
            CheckAbort();
            if ( !HGetPacket() )
                continue;
            if ( netbuffer->checksum & NCMD_SETUP )
            {
                if ( netbuffer->player != static_cast<std::byte>( VERSION ) )
                    logger::error( "Different DOOM versions cannot play a net game!" );
                // TODO JONNY circular dependency
                // startskill = static_cast<skill_t>(netbuffer->retransmitfrom &
                //                                   static_cast
                //                                   <std::byte>(15));
                // deathmatch = static_cast<uint8_t>((netbuffer->retransmitfrom
                // & std::byte{0xc0}) >> 6); nomonsters =
                // int(netbuffer->retransmitfrom & std::byte{0x20}) > 0;
                // respawnparm = int(netbuffer->retransmitfrom &
                // std::byte{0x10}) > 0; startmap =
                // static_cast<int>(netbuffer->starttic) & 0x3f; startepisode =
                // static_cast<int>(netbuffer->starttic) >> 6;
                return;
            }
        }
    }
    else
    {
        // key player, send the setup info
        printf( "sending network start info...\n" );
        do
        {
            CheckAbort();
            for ( i = 0; i < doomcom.numnodes; i++ )
            {
                // TODO JONNY circular dependency
                // netbuffer->retransmitfrom =
                // static_cast<std::byte>(startskill); if (deathmatch)
                //  netbuffer->retransmitfrom |=
                //  static_cast<std::byte>(deathmatch << 6);
                // if (nomonsters)
                //  netbuffer->retransmitfrom |= std::byte{0x20};
                // if (respawnparm)
                //  netbuffer->retransmitfrom |= std::byte{0x10};
                // netbuffer->starttic = static_cast<std::byte>(startepisode *
                // 64 + startmap); netbuffer->player =
                // static_cast<std::byte>(VERSION); netbuffer->numtics =
                // std::byte{0};
                HSendPacket( i, NCMD_SETUP );
            }

#if 1
            for ( i = 10; i && HGetPacket(); --i )
            {
                if ( int( netbuffer->player & std::byte{ 0x7f } ) < MAXNETNODES )
                    gotinfo[static_cast<int>( netbuffer->player ) & 0x7f] = true;
            }
#else
            while ( HGetPacket() )
            {
                gotinfo[netbuffer->player & 0x7f] = true;
            }
#endif

            for ( i = 1; i < doomcom.numnodes; i++ )
                if ( !gotinfo[i] )
                    break;
        } while ( i < doomcom.numnodes );
    }
}

//
// D_CheckNetGame
// Works out player numbers among the net participants
//

export void D_CheckNetGame( void )
{
    int i;

    for ( i = 0; i < MAXNETNODES; i++ )
    {
        nodeingame[i] = false;
        nettics[i] = 0;
        remoteresend[i] = false; // set when local needs tics
        resendto[i] = 0;         // which tic to start sending
    }

    // I_InitNetwork sets doomcom and netgame
    I_InitNetwork();
    if ( doomcom.id != DOOMCOM_ID )
        logger::error( "Doomcom buffer invalid!" );

    netbuffer = &doomcom.data;
    consoleplayer = displayplayer = doomcom.consoleplayer;
    if ( netgame )
        D_ArbitrateNetStart();

    // TODO JONNY circular dependency
    // printf("startskill %i  deathmatch: %i  startmap: %i  startepisode: %i\n",
    //     startskill, deathmatch, startmap, startepisode);

    // read values out of doomcom
    ticdup = doomcom.ticdup;
    maxsend = BACKUPTICS / ( 2 * ticdup ) - 1;
    if ( maxsend < 1 )
        maxsend = 1;

    for ( i = 0; i < doomcom.numplayers; i++ )
        playeringame[i] = true;
    for ( i = 0; i < doomcom.numnodes; i++ )
        nodeingame[i] = true;

    printf( "player %i of %i (%i nodes)\n", consoleplayer + 1, doomcom.numplayers, doomcom.numnodes );
}

//
// D_QuitNetGame
// Called before quitting to leave a net game
// without hanging the other players
//
void D_QuitNetGame( void )
{
    int i, j;

    if ( !netgame || !usergame || consoleplayer == -1 || demoplayback )
        return;

    // send a bunch of packets for security
    netbuffer->player = static_cast<std::byte>( consoleplayer );
    netbuffer->numtics = std::byte{ 0 };
    for ( i = 0; i < 4; i++ )
    {
        for ( j = 1; j < doomcom.numnodes; j++ )
            if ( nodeingame[j] )
                HSendPacket( j, NCMD_EXIT );
    }
}

//
// TryRunTics
//
int frametics[4];
int frameon;
int frameskip[4];
int oldnettics;

export void TryRunTics( void )
{
    int i;
    int lowtic;
    int entertic;
    static int oldentertics;
    int realtics;
    int availabletics;
    int counts;

    // get real tics
    entertic = get_current_tick() / ticdup;
    realtics = entertic - oldentertics;
    oldentertics = entertic;

    // get available tics
    NetUpdate();

    lowtic = std::numeric_limits<int>::max();
    for ( i = 0; i < doomcom.numnodes; i++ )
    {
        if ( nodeingame[i] )
        {
            if ( nettics[i] < lowtic )
                lowtic = nettics[i];
        }
    }
    availabletics = lowtic - gametic / ticdup;

    // decide how many tics to run
    if ( realtics < availabletics - 1 )
        counts = realtics + 1;
    else if ( realtics < availabletics )
        counts = realtics;
    else
        counts = availabletics;

    if ( counts < 1 )
        counts = 1;

    frameon++;

    if ( !demoplayback )
    {
        // ideally nettics[0] should be 1 - 3 tics above lowtic
        // if we are consistantly slower, speed up time
        for ( i = 0; i < MAXPLAYERS; i++ )
            if ( playeringame[i] )
                break;
        if ( consoleplayer == i )
        {
            // the key player does not adapt
        }
        else
        {
            if ( nettics[0] <= nettics[nodeforplayer[i]] )
            {
                gametime--;
                // printf ("-");
            }
            frameskip[frameon & 3] = ( oldnettics > nettics[nodeforplayer[i]] );
            oldnettics = nettics[0];
            if ( frameskip[0] && frameskip[1] && frameskip[2] && frameskip[3] )
            {
                skiptics = 1;
                // printf ("+");
            }
        }
    } // demoplayback

    // wait for new tics if needed
    while ( lowtic < gametic / ticdup + counts )
    {
        NetUpdate();
        lowtic = std::numeric_limits<int>::max();

        for ( i = 0; i < doomcom.numnodes; i++ )
            if ( nodeingame[i] && nettics[i] < lowtic )
                lowtic = nettics[i];

        if ( lowtic < gametic / ticdup )
            logger::error( "TryRunTics: lowtic < gametic" );

        // don't stay in here forever -- give the menu a chance to work
        if ( get_current_tick() / ticdup - entertic >= 20 )
        {
            M_Ticker();
            return;
        }
    }

    // run the count * ticdup dics
    while ( counts-- )
    {
        for ( i = 0; i < ticdup; i++ )
        {
            if ( gametic / ticdup > lowtic )
                logger::error( "gametic>lowtic" );
            M_Ticker();
            G_Ticker();
            gametic++;

            // modify command for duplicated tics
            if ( i != ticdup - 1 )
            {
                ticcmd_t *cmd;
                int buf;
                int j;

                buf = ( gametic / ticdup ) % BACKUPTICS;
                for ( j = 0; j < MAXPLAYERS; j++ )
                {
                    cmd = &netcmds[j][buf];
                    cmd->chatchar = 0;
                    if ( cmd->buttons & BT_SPECIAL )
                        cmd->buttons = {};
                }
            }
        }
        NetUpdate(); // check for new console commands
    }
}
