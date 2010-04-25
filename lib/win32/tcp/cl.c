#ifndef VSX_NO_CLIENT
/********************************************************************/
/*                    Connection Library                            */
/*    version: 0.6                                                  */  
/*                                                                  */
/*    file: cl.c                                                    */
/*    Copyright: (c) 1998, 1999 by Ugo Varetto                      */
/*    e-mail: varetto@computer.org                                  */
/********************************************************************/

/* 
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  See the README file for more information.
 */

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include "../pthread/pthread.h"
#include "cl.h"



/* globals */

static int errCode=CL_ERR_NOERROR;
static char errMsg[ERRMSG_SIZE]="No error";
static int srDelay=1000; /* 1 second */
static void (*errCallback)(char *msg,int errcode) = NULL;



#ifndef UNIX
static struct WSAData wsa;
#endif


/* functions */


/* Error handling stuff */

void clPrintError() {

	if(errCallback==NULL)
		fprintf(stderr,"\nERROR - %s\n", errMsg);
	else
		errCallback(errMsg,errCode);
}


int clGetErrorCode() {

	return errCode;
 
}

void clGetErrorMessage(char *msg) {

	strcpy(msg,errMsg);

}

extern void clSetError(char *msg,int code) {
	strcpy(errMsg,msg);	
	errCode=code;
}


void clResetError() {
	strcpy(errMsg,"No error");	
	errCode=CL_ERR_NOERROR;
}

void clSetErrorCallback(void (*callback)(char *err_msg,int err_code)) {

	errCallback=callback;

}
/* end of error handling */

/* init & reset */
/* Under win32 any call to clBegin should be matched with a correspondig clEnd call */
extern int clBegin() {
	int w=0;
	clResetError();
#ifndef	UNIX
	memset(&wsa,0,sizeof(struct WSAData));
	w=WSAStartup(MAKEWORD(2,0),&wsa);
	if(w)
		clSetError("Can't initialize winsock library",CL_ERR_INIT_FAILED);
#endif
	return w;
}

extern int clEnd() {
	int c=0;
	/*if(SmtpBody != NULL)
		free(SmtpBody);*/
#ifndef	UNIX
	c=WSACleanup();
	if(c)
		clSetError("Winsock library cleanup error",CL_ERR_LIBCLOSE_FAILED);
#endif
	return c;
}

/* end init & reset */


/* Utility functions */

int clClose(SOCKET s) {
	shutdown(s,SD_BOTH);
	return closesocket(s);
}


int clIsNumber(char *str) {
	int s=strlen(str);
	int i;
	if(str==NULL)
		return 0;
	for(i=0;i<s;++i)
		if(!isdigit(str[i]))
			return 0;
	return 1;
}

unsigned short clGetPortByService(char *service,char *proto) {
	struct servent *sp=NULL;
	sp=getservbyname(service,proto);
	if(sp==NULL) {
		clSetError("Can't find service",CL_ERR_UNKNOWN_SERVICE);
		return 0;
	}
	return  ntohs(sp->s_port);
}



int clAsyncSocket(SOCKET s) {

	int block=1;

	return ioctlsocket(s,FIONBIO,(IOCTL_TYPE *) &block);	

}

int clSyncSocket(SOCKET s) {

	int block=0;

	return ioctlsocket(s,FIONBIO,(IOCTL_TYPE *) &block);	

}

int clGetRemoteName(SOCKET s,char * hostname,char *hostaddr) {

	struct hostent *hp=NULL;
	struct sockaddr_in remote_addr;
	int err=0;
	int len=sizeof(struct sockaddr);
	memset(&remote_addr,0,sizeof(struct sockaddr));

	
	err=getpeername(s,(struct sockaddr *) &remote_addr,&len);
	
	if(err) {
		clSetError("getpeername",CL_ERR);
		return CL_ERR;
	}
	
	hp=gethostbyaddr((char *) &(remote_addr.sin_addr),sizeof(remote_addr.sin_addr),AF_INET);

	if(hp==NULL) {
			clSetError("gethostbyaddr",CL_ERR);
			return CL_ERR;
	}

	strcpy(hostname,hp->h_name);
	sprintf(hostaddr,"%d.%d.%d.%d",hp->h_addr_list[0][0],hp->h_addr_list[0][1],
		hp->h_addr_list[0][2],hp->h_addr_list[0][2]);

	return 0;
}

/* end of utility functions */


void clSetSRDelay(int d) {
	if(d<0)
		return;
	srDelay=d;

}

int clGetSRDelay() {
	return srDelay;
}


/*** sockets creation functions ***/

SOCKET clBroadcastSocket() {
	SOCKET s;
	int optval=1;
	s=socket(AF_INET,SOCK_DGRAM,0);
	if(s==INVALID_SOCKET) {
		clSetError("Can't create socket",CL_ERR_INVALID_SOCKET);
		return INVALID_SOCKET;
	}


	
	if(setsockopt(s,SOL_SOCKET,SO_BROADCAST,(char *) &optval,sizeof(int))) {
		clSetError("Can't change socket attribute",CL_ERR_SOCKET_ATTR);
		return INVALID_SOCKET;
	}

	
	return s;

}

extern SOCKET clTcpSyncSocket() {
	
	SOCKET s;
	s=socket(AF_INET,SOCK_STREAM,0);
	if(s==INVALID_SOCKET) {
		clSetError("Can't create TCP socket",CL_ERR_INVALID_SOCKET);
		return INVALID_SOCKET;
	}

	if(clSyncSocket(s)) {
		closesocket(s);
		clSetError("Can't change socket attribute",CL_ERR_SOCKET_ATTR);
		return INVALID_SOCKET;
	}

	return s;

}

extern SOCKET clUdpSyncSocket() {
	SOCKET s;
	int optval=1;
	s=socket(AF_INET,SOCK_DGRAM,0);
	if(s==INVALID_SOCKET) {
		clSetError("Can't create UDP socket",CL_ERR_INVALID_SOCKET);
		return INVALID_SOCKET;
	}


	if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(char *) &optval,sizeof(int))) {
		closesocket(s);
		clSetError("Can't change socket attribute",CL_ERR_SOCKET_ATTR);
		return INVALID_SOCKET;
	}

	if(clSyncSocket(s)) {
		closesocket(s);
		clSetError("Can't change socket attribute",CL_ERR_SOCKET_ATTR);
		return INVALID_SOCKET;
	}
	
	return s;

}


extern SOCKET clTcpAsyncSocket() {
	
	SOCKET s;
	s=socket(AF_INET,SOCK_STREAM,0);
	if(s==INVALID_SOCKET) {
		clSetError("Can't create TCP async socket",CL_ERR_INVALID_SOCKET);
		return INVALID_SOCKET;
	}

	
	if(clAsyncSocket(s)) {
		closesocket(s);
		clSetError("Can't change socket attribute",CL_ERR_SOCKET_ATTR);
		return INVALID_SOCKET;
	}
	
	return s;

}

extern SOCKET clUdpAsyncSocket() {
	SOCKET s;
	int optval=1;
	s=socket(AF_INET,SOCK_DGRAM,0);
	if(s==INVALID_SOCKET) {
		clSetError("Can't create UDP async socket",CL_ERR_INVALID_SOCKET);
		return INVALID_SOCKET;
	}


	if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(char *) &optval,sizeof(int))) {
		closesocket(s);
		clSetError("Can't change socket attribute",CL_ERR_SOCKET_ATTR);
		return INVALID_SOCKET;
	}
	
	if(clAsyncSocket(s)) {
		closesocket(s);
		clSetError("Can't change socket attribute",CL_ERR_SOCKET_ATTR);
		return INVALID_SOCKET;
	}
	
	return s;

}


/*** end of sockets creation functions ***/



/********* Main connection functions **********/
/* clConnect can use either a port number or a service name */
SOCKET clConnect(char *hostname,char *service,char *protocol) {
/* This function will return a SOCKET to use for communication */
/* if something goes wrong INVALID_SOCKET will be returned and */
/* an error message and code wil be set */
/* To get the message error and or code use the functions: */
/* clGetErrorMsg(char *msg,int maxsize) and clGetErrorCode(int *errcode) */
/* If service is a number then it will be interpreted as a port number */
/* If service is a string then it will interpreted as service name */
/* protocol can be "tcp", "udp" or NULL, in the last case "tcp" will be*/
/* assigned as default*/
/* If hostname  is NULL "localhost" will be used. */
/* WARNING: under win32 this function works only if clBegin has been*/ 
/* called before. */ 
	
	
	struct hostent *hp;
	struct servent *sp;
	SOCKET client_socket;
	struct sockaddr_in hostaddr_in;
	
	char remote_host[HOSTNAME_SIZE]="localhost";
	/*unsigned short port;*/
	char proto[]="tcp";
	
	if(protocol!=NULL)
		strcpy(proto,protocol);
		
	if(hostname!=NULL)
		strcpy(remote_host,hostname);

	/*clear host address */ 
	memset((char *) &hostaddr_in,0,sizeof(struct sockaddr_in));
	hostaddr_in.sin_family=AF_INET; /* always */

	/*find remote host information*/
	hp=gethostbyname(remote_host);
	
	if(hp==NULL) {
		clSetError("Can't find host",CL_ERR_UNKNOWN_HOST);
		return INVALID_SOCKET;
	}

	/*assign a host address to the address structure*/
	hostaddr_in.sin_addr.s_addr=((struct in_addr *)(hp->h_addr))->s_addr;

	/*assign a communication port for the address*/
	if(clIsNumber(service))
		hostaddr_in.sin_port=htons((unsigned short) atoi(service));/*convert port number to network byte ordering*/
	else {

		/*get port number given the service name*/ 
		sp=getservbyname(service,proto);
		if(sp==NULL) {
			clSetError("Can't find service",CL_ERR_UNKNOWN_SERVICE);
			return INVALID_SOCKET;
			
		}
		hostaddr_in.sin_port=sp->s_port;
	}

	/*create a socket*/
	if(strcmp(proto,"udp")==0)
		client_socket=socket(AF_INET,SOCK_DGRAM,0);
	else
		client_socket=socket(AF_INET,SOCK_STREAM,0);
	 

	if(client_socket==INVALID_SOCKET) {
		clSetError("Can't create socket",CL_ERR_INVALID_SOCKET);
		return INVALID_SOCKET;
	}

  char tcp_nodelay = 1;
  setsockopt(client_socket,IPPROTO_TCP,TCP_NODELAY,(char*)&tcp_nodelay,sizeof(char));


	if(connect(client_socket,(struct sockaddr *) &hostaddr_in,sizeof(struct sockaddr_in)))
	{	
		clSetError("Can't connect to server",CL_ERR_CONNECTION_UNAVAIL);
		return INVALID_SOCKET;
	}
	
	
	/***** OK we are connected *****/	 

	return client_socket;
	
	/*
	  NOTE: for UDP (connectionless) sockets this function has the effect
	  of establishing a default address to which packets will be sent.
    */
}


/* clConnectPort can use port number only     */
SOCKET clConnectPort(char *hostname,unsigned short port,char *protocol) {
/* This function will return a SOCKET to use for communication */
/* if something goes wrong INVALID_SOCKET will be returned and */
/* an error message and code wil be set */
/* To get the message error and or code use the functions:*/
/* clGetErrorMsg(char *msg,int maxsize) and clGetErrorCode(int *errcode) */
/* protocol can be "tcp", "udp" or NULL, in the last case "tcp" will be*/
/* assigned as default*/
/* If hostname  is NULL "localhost" will be used. */
/* WARNING: under win32 this function works only if clBegin has been*/ 
/* called before. */ 
	
	
	struct hostent *hp;
	SOCKET client_socket;
	struct sockaddr_in hostaddr_in;
	
	char remote_host[HOSTNAME_SIZE]="localhost";
	char proto[]="tcp";
	
	if(protocol!=NULL)
		strcpy(proto,protocol);
		
	if(hostname!=NULL)
		strcpy(remote_host,hostname);

	/*clear host address */ 
	memset((char *) &hostaddr_in,0,sizeof(struct sockaddr_in));
	hostaddr_in.sin_family=AF_INET; /* always */

	/*find remote host information*/
	hp=gethostbyname(remote_host);
	
	if(hp==NULL) {
			clSetError("Can't find host",CL_ERR_UNKNOWN_HOST);
			return INVALID_SOCKET;
			
	}

	/*assign a host address to the address structure*/
	hostaddr_in.sin_addr.s_addr=((struct in_addr *)(hp->h_addr))->s_addr;

	/*assign a communication port for the address*/
	hostaddr_in.sin_port=htons(port);/*convert port number to network byte ordering*/
	
	/*create a socket*/
	if(strcmp(proto,"udp")==0)
		client_socket=socket(AF_INET,SOCK_DGRAM,0);
	else
		client_socket=socket(AF_INET,SOCK_STREAM,0);
	 

	if(client_socket==INVALID_SOCKET) {
		clSetError("Can't create socket",CL_ERR_INVALID_SOCKET);
		return INVALID_SOCKET;
	}

  char tcp_nodelay = 1;
  int sopt = setsockopt(client_socket,IPPROTO_TCP,TCP_NODELAY,(char*)&tcp_nodelay,sizeof(char));
  printf("setsockopt nodelay returned: %d",sopt);

	if(connect(client_socket,(struct sockaddr *) &hostaddr_in,sizeof(struct sockaddr_in)))
	{	
		clSetError("Can't connect to server",CL_ERR_CONNECTION_UNAVAIL);
		return INVALID_SOCKET;
	}
	
	
	/***** OK we are connected *****/	 

	return client_socket;
	
	/*
	  NOTE: for UDP (connectionless) sockets this function has the effect
	  of establishing a default address to which packets will be sent.
    */
}


/******* end of main connection functions ********/


int clDisconnect(SOCKET s) {
	int c;
	c=closesocket(s);
	if(c)
		clSetError("Error on closing socket",CL_ERR_CLOSE_SOCKET);
	return c;
}


/* send and recv */
int clSend(SOCKET s,char *msg,int msg_size) {
	return send(s,msg,msg_size,0);
}

int clRecv(SOCKET s,char *msg,int msg_size) {
	return recv(s,msg,msg_size,0);
}
int clAsyncRecv(SOCKET s,char *msg,int msg_size,int timeout /*milliseconds*/) {

	clock_t now;
	int rec;
	clock_t endtime;
	int block=1;
	endtime=(clock_t) ((((double)timeout)/1000.0f)*(double)CLOCKS_PER_SEC + (double)clock());
	
	
	/*ioctlsocket(s,FIONBIO,(IOCTL_TYPE *) &block);*/
	
	do {
		rec=clRecv(s,msg,msg_size);
		now=clock();
//		printf("%d\n",now);
		if(!rec)
			break; /* remote end disconnected */
	} while(rec==-1 && endtime>now);
	
	if(endtime > now)
		clSetError("Timeout on receive",CL_ERR_TIMEOUT);

	block=0;
	/*ioctlsocket(s,FIONBIO,(IOCTL_TYPE *) &block);*/
	
	return rec;

}

int clAsyncSend(SOCKET s,char *msg,int msg_size,int timeout /*milliseconds*/) {

	clock_t now;
	int sent;
	clock_t endtime;
	int block=1;
	endtime=(clock_t) (((double) timeout)/1000.0)*CLOCKS_PER_SEC + clock();
	
	
	/*ioctlsocket(s,FIONBIO,(IOCTL_TYPE *) &block);*/
	
	do {
		sent=clSend(s,msg,msg_size);
		now=clock();
		if(!sent)
			break; /* remote end disconnected */
		
	} while(sent==-1 && endtime>now);
	
	if(endtime > now)
		clSetError("Timeout on send",CL_ERR_TIMEOUT);

	block=0;
	/*ioctlsocket(s,FIONBIO,(IOCTL_TYPE *) &block);*/
	
	return sent;

}


int clSendAll(SOCKET s,char *msg,int msg_size) {
	int i=0;
	int sz=0;
	do {
		sz=send(s,&msg[i],msg_size-sz,0);
		if(sz<0)
			return sz;
		i+=sz;
	} while(i<msg_size);
	
	return i;
}

int clRecvAll(SOCKET s,char *msg,int msg_size) {
	int i=0;
	int r=0;
	do {
		r=recv(s,&msg[i],msg_size-r,0);
		if(r<=0)
			return r;
		i+=r;
	} while(i<msg_size);
	
	return i;
}

/* UDP */
int clSendTo(SOCKET s,char *msg,int msg_size,char *hostname,unsigned short port) {

	int err=0;
	struct hostent *hp;
	struct sockaddr_in hostaddr_in;
	char remote_host[HOSTNAME_SIZE]="localhost";
			
	if(hostname!=NULL)
		strcpy(remote_host,hostname);

	/*clear host address */ 
	memset((char *) &hostaddr_in,0,sizeof(struct sockaddr_in));
	hostaddr_in.sin_family=AF_INET; /* always */

	/*find remote host information*/
	hp=gethostbyname(remote_host);
	
	if(hp==NULL) {
			clSetError("Can't find host",CL_ERR_UNKNOWN_HOST);
			return -1;
			
	}

	/*assign a host address to the address structure*/
	hostaddr_in.sin_addr.s_addr=((struct in_addr *)(hp->h_addr))->s_addr;

	/*assign a communication port for the address*/
	hostaddr_in.sin_port=htons(port);/*convert port number to network byte ordering*/
	
	err=bind(s,(struct sockaddr *) &hostaddr_in,sizeof(struct sockaddr));
		if(err) {
			clSetError("Can't bind UDP socket",CL_ERR_SOCKET_BIND);
			return -1;
			
	}
	
	err=sendto(s,msg,msg_size,0,(struct sockaddr *) &hostaddr_in,sizeof(struct sockaddr));
	return err;
}	


int clRecvFrom(SOCKET s,char *msg,int msg_size,char *hostname,int size,unsigned short port) {
	int err=0;
	struct hostent *remote_host=NULL;
	struct hostent *hp=NULL;
	struct sockaddr_in myaddr_in, remote_addr;
	int fromlength=sizeof(struct sockaddr);
			
	
	/*clear local host address */ 
	memset((char *) &myaddr_in,0,sizeof(struct sockaddr_in));
	memset((char *) &remote_addr,0,sizeof(struct sockaddr_in));
	myaddr_in.sin_family=AF_INET; /* always */

	
	/*find local host information*/
	hp=gethostbyname("localhost");
	
	if(hp==NULL) {
			clSetError("Can't find host",CL_ERR_UNKNOWN_HOST);
			return -1;
			
	}

	/*assign the local host address to the address structure*/
	myaddr_in.sin_addr.s_addr=htonl ( INADDR_ANY );

	/*assign a communication port for the address*/
	myaddr_in.sin_port=htons(port);/*convert port number to network byte ordering*/
	
	/*getsockname(s,(struct sockaddr *) &myaddr_in,&mysize);*/
	err=bind(s,(struct sockaddr *) &myaddr_in,sizeof(struct sockaddr));
	if(err) {
			clSetError("Can't bind UDP socket",CL_ERR_SOCKET_BIND);
			return -1;
			
	}
	err=recvfrom(s,msg,msg_size,0,(struct sockaddr *) &remote_addr,&fromlength);

	remote_host=gethostbyaddr((char *) &(remote_addr.sin_addr),4,AF_INET);

	if(hostname != NULL && size > 1 && remote_host != NULL && err >0) 
		strncpy(hostname,remote_host->h_name,size);
	
	return err;
}



int clAsyncRecvFrom(SOCKET s,char *msg,int msg_size,char *hostname,int size,
					unsigned short port,int timeout) {
	clock_t now;
	int rec;
	clock_t endtime;
	int block=1;
	endtime=(clock_t) (((double) timeout)/1000.0)*CLOCKS_PER_SEC + clock();
	
	/*ioctlsocket(s,FIONBIO,(IOCTL_TYPE *) &block);*/
	do {
		rec=clRecvFrom(s,msg,msg_size,hostname,size,port);
		now=clock();
		if(!rec)
			break; /* remote end disconnected */
	} while(rec==-1  && endtime>now);
	
	
	if(endtime > now)
		clSetError("Timeout on receive",CL_ERR_TIMEOUT);
	
	block=0;
	/*ioctlsocket(s,FIONBIO,(IOCTL_TYPE *) &block);*/

	return rec;

}

int clAsyncSendTo(SOCKET s,char *msg,int msg_size,char *hostname,
					unsigned short port,int timeout) {
	clock_t now;
	int sent;
	clock_t endtime;
	int block=1;
	endtime=(clock_t) (((double) timeout)/1000.0)*CLOCKS_PER_SEC + clock();
	
	/*ioctlsocket(s,FIONBIO,(IOCTL_TYPE *) &block);*/
	do {
		sent=clSendTo(s,msg,msg_size,hostname,port);
		now=clock();
		if(!sent)
			break; /* remote end disconnected */
		
	} while(sent==-1 && endtime>now);
	
	if(endtime > now)
		clSetError("Timeout on send",CL_ERR_TIMEOUT);

	block=0;
	/*ioctlsocket(s,FIONBIO,(IOCTL_TYPE *) &block);*/

	return sent;

}

/*********************/

int clBroadcast(SOCKET s,char *msg,int msg_size,unsigned short port) {

	int err=0;
	struct sockaddr_in hostaddr_in;
	//char remote_host[HOSTNAME_SIZE]="localhost";
	int type=SOCK_STREAM;
	int typelen=sizeof(int);

	
	/* check SOCKET type */ 
	getsockopt(s,SOL_SOCKET,SO_TYPE,(char *) &type,&typelen); 

	if(type!=SOCK_DGRAM) {
		clSetError("Attempt to broadcast over a TCP socket",CL_ERR_SOCKET_BROADCAST);
		return -1;
	}
	
	/*clear host address */ 
	memset((char *) &hostaddr_in,0,sizeof(struct sockaddr_in));
	hostaddr_in.sin_family=AF_INET; /* always */

	/*assign a host address to the address structure*/
	hostaddr_in.sin_addr.s_addr=htonl(INADDR_BROADCAST);

	/*assign a communication port for the address*/
	hostaddr_in.sin_port=htons(port);/*convert port number to network byte ordering*/
	
	err=sendto(s,msg,msg_size,0,(struct sockaddr *) &hostaddr_in,sizeof(struct sockaddr));
	return err;

}

/* end of send and recv */

/* server functions */

#define MAXHOSTNAME 256
SOCKET clEstablish(unsigned short portnum,int backlog) { 

  char   myname[MAXHOSTNAME+1];
  SOCKET    s;
  struct sockaddr_in sa;
  struct hostent *hp;

  memset(&sa, 0, sizeof(struct sockaddr_in)); /* clear our address */
  gethostname(myname, MAXHOSTNAME);           /* who are we? */
  hp= gethostbyname(myname);                  /* get our address info */
  
  if (hp == NULL)                             /* we don't exist !? */
    return(INVALID_SOCKET);

  sa.sin_family= hp->h_addrtype;              /* this is our host address */
  sa.sin_port= htons(portnum);                /* this is our port number */
  
  if ((s= socket(AF_INET, SOCK_STREAM, 0)) < 0) /* create socket */
	return(INVALID_SOCKET);
  
  //char tcp_nodelay = 1;
  //int sopt = setsockopt(s,IPPROTO_TCP,TCP_NODELAY,(char*)&tcp_nodelay,sizeof(char));
  //printf("setsockopt nodelay returned: %d",sopt);

  if (bind(s,(struct sockaddr *) &sa,sizeof(struct sockaddr)) < 0) 
  {    closesocket(s);
    return(INVALID_SOCKET);                               /* bind address to socket */  
  }
 
  
  listen(s, backlog);                               /* max # of queued connects */
  return(s);
}



SOCKET get_connection(SOCKET s) { 
	
	return accept(s,NULL,NULL);   /* accept connection if there is one */
}


#ifdef UNIX

void clChild(int sig) {  
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
}

int clServerByPortX(unsigned short port,int backlog,
				   void (*threadFun)(struct clThreadData *dat),struct clThreadData *tdata) {

	SOCKET s, t;
	
	if ((s= clEstablish(port,backlog)) < 0) {  /* plug in the phone */
		clSetError("Can't create socket",CL_ERR_SOCKET_CREATE);
		return CL_ERR_SOCKET_CREATE;  
	}
	signal(SIGCHLD,clChild);           /* this eliminates zombies */
	for (;;) {                          /* loop for phone calls */
		if ((t= get_connection(s)) < 0) { /* get a connection */
			clSetError("accept",CL_ERR_ACCEPT);               /* bad */     
			return CL_ERR_ACCEPT;    
		}
		switch(fork()) {                  /* try to handle connection */
		case -1 :                         /* bad news.  scream and die */
			clSetError("fork",CL_ERRX_FORK);
			closesocket(s);
			closesocket(t);
			return CL_ERRX_FORK;
		case 0 :                          /* we're the child, do something */
			closesocket(s);
			if(tdata != NULL) {
				tdata->sock=t;
			}
			threadFun(tdata);
			return 0;
						
		default :                         /* we're the parent so look for */
			closesocket(t);                       /* another connection */
			continue;
		}  
	}
}/* as children die we should get catch their returns or else we get
  zombies, A Bad Thing.  fireman() catches falling children. */

#endif



#ifndef UNIX
pthread_t					tcps_t;
pthread_attr_t		tcps_t_attr;

int clServerByPortW(unsigned short port,int backlog,
				   void *(*threadFun)(void *),struct clThreadData *tdata) {

	SOCKET s, t;
	
		
	clBegin();
	if ((s= clEstablish((unsigned short)port,backlog)) < 0) {  /* plug in the phone */
		clSetError("Can't create socket",CL_ERR_SOCKET_CREATE);
		return CL_ERR_SOCKET_CREATE;  
	}
							           /* this eliminates zombies */
	for (;;) {    
		
				/* loop for phone calls */
		if ((t= get_connection(s)) < 0) { /* get a connection */
			clSetError("accept",CL_ERR_ACCEPT);               /* bad */     
			return CL_ERR_ACCEPT;    
		}
		
		if(tdata != NULL)
			tdata->sock=t;
	
		pthread_attr_init(&tcps_t_attr);
		if (pthread_create(&tcps_t, &tcps_t_attr, threadFun, (void *) tdata) != 0) {
		  //printf("thread failed to create\n");


// 		if(_beginthread(threadFun,0,(void *) tdata)==-1) {
			clSetError("_beginthread",CL_ERRW_THREAD);
			closesocket(s);
			closesocket(t);
			return CL_ERRW_THREAD;
		}
					  
	}
}

#endif



int clServerByPort(unsigned short port,int backlog,void *(*threadFun)(void *),struct clThreadData *tdata) {

#ifdef UNIX
	return clServerByPortX(port,backlog,threadFun,tdata);
#endif
#ifndef UNIX
	return clServerByPortW(port,backlog,threadFun,tdata);
#endif

}

int clKillParent(int retval) {

#ifndef UNIX	
	return TerminateProcess(GetCurrentProcess(),retval);
#endif
#ifdef UNIX
	return kill(getppid(),SIGKILL);
#endif
	
}


/***** end of source code *****/

#endif
