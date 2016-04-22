static const char
rcsid[] = "$Id: m_bbox.c,v 1.1 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//JONNY//#include <sys/socket.h>
//JONNY//#include <netinet/in.h>
//JONNY//#include <arpa/inet.h>
//JONNY//#include <errno.h>
#include "unistd.h"
//JONNY//#include <netdb.h>
//JONNY//#include <sys/ioctl.h>

#include "i_system.hpp"
#include "d_event.hpp"
#include "d_net.hpp"
#include "m_argv.hpp"

#include "doomstat.hpp"

#ifndef _WIN32
#include <errno.h>
#endif

#ifdef __GNUG__
#pragma implementation "i_net.hpp"
#endif
#include "i_net.hpp"





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

//JONNY//int	DOOMPORT =	(IPPORT_USERRESERVED +0x1d );

int			sendsocket;
int			insocket;

//JONNY//struct	sockaddr_in	sendaddress[MAXNETNODES];

void	(*netget) (void);
void	(*netsend) (void);


//
// UDPsocket
//
int UDPsocket (void)
{
    int	s(0);
	
    // allocate a socket
	//JONNY//	   s = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s<0)
			I_Error ("can't create socket: %s",strerror(errno));
		
    return s;
	return 0;
}

//
// BindToLocalPort
//
void
BindToLocalPort
( int	s,
  int	port )
{
    int			v(0);
	//JONNY//    struct sockaddr_in	address;
	
   //JONNY// memset (&address, 0, sizeof(address));
   //JONNY// address.sin_family = AF_INET;
   //JONNY// address.sin_addr.s_addr = INADDR_ANY;
   //JONNY// address.sin_port = port;
			
   //JONNY// v = bind (s, (void *)&address, sizeof(address));
	if (v == -1);
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
		
    //printf ("sending %i\n",gametic);		
//JONNY//    c = sendto (sendsocket , &sw, doomcom->datalength
//JONNY//		,0,(void *)&sendaddress[doomcom->remotenode]
//JONNY//		,sizeof(sendaddress[doomcom->remotenode]));
	
    //	if (c == -1)
    //		I_Error ("SendPacket error: %s",strerror(errno));
}


//
// PacketGet
//
void PacketGet (void)
{
    int			i(0);
    int			c(0);
//JONNY//	    struct sockaddr_in	fromaddress;
    int			fromlen;
	//JONNY// doomdata_t		sw;
				
	//JONNY//	  fromlen = sizeof(fromaddress);
	  //JONNY//  c = recvfrom (insocket, &sw, sizeof(sw), 0
	  //JONNY//, (struct sockaddr *)&fromaddress, &fromlen );
    if (c == -1 )
    {
	if (errno != EWOULDBLOCK)
			    I_Error ("GetPacket: %s",strerror(errno));
	doomcom->remotenode = -1;		// no packet
	return;
    }

    {
	static int first=1;
	if (first)
		//JONNY//			    printf("len=%d:p=[0x%x 0x%x] \n", c, *(int*)&sw, *((int*)&sw+1));
	first = 0;
    }

    // find remote node number
    for (i=0 ; i<doomcom->numnodes ; i++)
		//JONNY//			if ( fromaddress.sin_addr.s_addr == sendaddress[i].sin_addr.s_addr )
	    break;

    if (i == doomcom->numnodes)
    {
	// packet is not from one of the players (new game broadcast)
	doomcom->remotenode = -1;		// no packet
	return;
    }
	
    doomcom->remotenode = i;			// good packet from a game player
    doomcom->datalength = c;
	
    // unsigned char swap
//JONNY//	    netbuffer->checksum = ntohl(sw.checksum);
//JONNY//	    netbuffer->player = sw.player;
//JONNY//    netbuffer->retransmitfrom = sw.retransmitfrom;
//JONNY//    netbuffer->starttic = sw.starttic;
//JONNY//    netbuffer->numtics = sw.numtics;

    for (c=0 ; c< netbuffer->numtics ; c++)
    {
//JONNY//	netbuffer->cmds[c].forwardmove = sw.cmds[c].forwardmove;
//JONNY//	netbuffer->cmds[c].sidemove = sw.cmds[c].sidemove;
//JONNY//	netbuffer->cmds[c].angleturn = ntohs(sw.cmds[c].angleturn);
//JONNY//	netbuffer->cmds[c].consistancy = ntohs(sw.cmds[c].consistancy);
//JONNY//	netbuffer->cmds[c].chatchar = sw.cmds[c].chatchar;
//JONNY//	netbuffer->cmds[c].buttons = sw.cmds[c].buttons;
    }
}



int GetLocalAddress (void)
{
    char		hostname[1024];
    struct hostent*	hostentry(NULL);	// host information entry
    int			v(0);

    // get local address
	//JONNY//    v = gethostname (hostname, sizeof(hostname));
  if (v == -1)
	I_Error ("GetLocalAddress : gethostname: errno %d",errno);
	
  //JONNY// hostentry = gethostbyname (hostname);
    if (!hostentry)
	I_Error ("GetLocalAddress : gethostbyname: couldn't get local host");
		
	//JONNY//	  return *(int *)hostentry->h_addr_list[0];
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
    i = M_CheckParm ("-dup");
    if (i && i< myargc-1)
    {
	doomcom->ticdup = myargv[i+1][0]-'0';
	if (doomcom->ticdup < 1)
	    doomcom->ticdup = 1;
	if (doomcom->ticdup > 9)
	    doomcom->ticdup = 9;
    }
    else
	doomcom-> ticdup = 1;
	
    if (M_CheckParm ("-extratic"))
	doomcom-> extratics = 1;
    else
	doomcom-> extratics = 0;
		
    p = M_CheckParm ("-port");
    if (p && p<myargc-1)
    {
		//JONNY//		DOOMPORT = atoi (myargv[p+1]);
		//JONNY//			printf ("using alternate port %i\n",DOOMPORT);
    }
    
    // parse network game options,
    //  -net <consoleplayer> <host> <host> ...
    i = M_CheckParm ("-net");
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
    doomcom->consoleplayer = myargv[i+1][0]-'1';

    doomcom->numnodes = 1;	// this node for sure
	
    i++;
    while (++i < myargc && myargv[i][0] != '-')
    {
		//JONNY//			sendaddress[doomcom->numnodes].sin_family = AF_INET;
		//JONNY//	sendaddress[doomcom->numnodes].sin_port = htons(DOOMPORT);
	if (myargv[i][0] == '.')
	{
		//JONNY//sendaddress[doomcom->numnodes].sin_addr.s_addr 
					//JONNY//	= inet_addr (myargv[i]+1);
	}
	else
	{
		//JONNY//   hostentry = gethostbyname (myargv[i]);
	    if (!hostentry)
		I_Error ("gethostbyname: couldn't find %s", myargv[i]);
		//JONNY//   sendaddress[doomcom->numnodes].sin_addr.s_addr 
		//JONNY//	= *(int *)hostentry->h_addr_list[0];
	}
	doomcom->numnodes++;
    }
	
    doomcom->id = DOOMCOM_ID;
    doomcom->numplayers = doomcom->numnodes;
    
    // build message to receive
    insocket = UDPsocket ();
	//JONNY//	 BindToLocalPort (insocket,htons(DOOMPORT));
	 //JONNY//	   ioctl (insocket, FIONBIO, &trueval);

    sendsocket = UDPsocket ();
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
