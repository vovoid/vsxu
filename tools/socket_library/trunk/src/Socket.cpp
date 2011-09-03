/*
	This file is part of TDSocket.

	TDSocket is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	TDSocket is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with TDSocket.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Socket.hpp"

#ifdef WIN32
	typedef int socklen_t;
#endif

namespace TD
{

Socket::Socket(int type, int domain, int protocol)
{	
	#ifdef WIN32 // Brain fuc*	
		int n= WSAStartup(MAKEWORD(2, 0), &wsa_data);
		if(n == -1) error(n);
	#endif
	
	sockfd = socket(domain, type, protocol);
	if(sockfd <= 0) error(sockfd);

	char yes = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	
	my_addr.sin_family = domain;
	buffer_size = 0;
	buffer = NULL;
}

Socket::Socket(int new_fd, bool flag)
{
	sockfd = new_fd;
	buffer_size = 0;
	buffer = NULL;
}

Socket::~Socket()
{
	free(buffer);
}

void Socket::bind(int port)
{
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);

    int n = ::bind(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if(n == -1) error(n);
}

void Socket::listen(int backlog)
{
	int n = ::listen(sockfd, backlog);
	if(n == -1) error(n);
}

Socket Socket::accept()
{
	int new_fd;
	struct sockaddr_in their_addr;
	
	socklen_t size = sizeof(their_addr);

	new_fd = ::accept(sockfd, (struct sockaddr*)&their_addr, &size);
	if(new_fd <= 0) error(new_fd);

    return Socket(new_fd, true);
}

struct in_addr* Socket::getHostByName(const char* server)
{
	struct hostent *h;
	h = gethostbyname(server);
	
	if(h == 0) error(-1);
	return (struct in_addr*)h->h_addr;	
}

const char* Socket::getPeerName()
{	
	struct sockaddr_in peer;
	socklen_t size = sizeof(peer);
	
	int n = getpeername(sockfd, (struct sockaddr*)&peer, &size);
	if(n == -1) throw(n);
	
	return inet_ntoa(peer.sin_addr);
}

const char* Socket::getHostName()
{
	char name[256];
	
	int n = gethostname(name, sizeof(name));
	if(n == -1) throw(n);
	
	return inet_ntoa(*getHostByName(name));
}

void Socket::connect(const char* server, int port)
{
    my_addr.sin_port = htons(port);
    my_addr.sin_addr = *getHostByName(server);
    memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));
    
    int n = ::connect(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    if(n == -1) error(n);
}

int Socket::send(std::string& msg)
{
	int ret;
 	ret = ::send(sockfd, msg.c_str(), msg.size(), 0);
	if(ret <= 0) error(ret);

    return ret;
}

void Socket::setTimeout(unsigned int timeout)
{
	struct linger lin;
	
	if(timeout == 0)
		lin.l_onoff = 0;
	else
		lin.l_onoff = 1;
	
	lin.l_linger = timeout;
	
	#ifdef WIN32
		setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin));
	#else
		setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin));
	#endif
}

int Socket::send(const char* msg)
{
	int ret;
 	ret = ::send(sockfd, msg, strlen(msg), 0);
	if(ret <= 0) error(ret);

    return ret;
}

int Socket::sendAll(std::string& msg)
{
	int total = 0;
	int len = msg.size();
    int bytesleft = len;
    int n = -1;

    while(total < len)
    {
    	n = ::send(sockfd, msg.c_str() + total, bytesleft, 0);
        if (n <= 0) { throw(n); }
        total += n;
        bytesleft -= n;
    }
    
    return len;
}

int Socket::sendAll(const char* msg)
{
	int total = 0;
	int len = strlen(msg);
    int bytesleft = len;
    int n = -1;

    while(total < len)
    {
    	n = ::send(sockfd, msg + total, bytesleft, 0);
        if (n <= 0) { throw(n); }
        total += n;
        bytesleft -= n;
    }

    return len;
}

int Socket::sendln(const char* msg)
{
	return (sendAll(msg) + sendAll("\n"));
}

int Socket::sendln(std::string& msg)
{
	return (sendAll(msg) + sendAll("\n"));
}

const char* Socket::recv(int len, int flags)
{
	if(len > buffer_size)
	{
		buffer = (char*) realloc(buffer, sizeof(char) * len);
		buffer_size = len;
	}
		
	int n = ::recv(sockfd, buffer, len, flags);
	if(n <= 0) error(n);

	return buffer;
}

const char* Socket::recvln(int flags)
{
	int n;
	
	for(int len = 0;; ++len)
	{
		if(len >= buffer_size)
		{
			int new_size = len * 2;
			buffer = (char*) realloc(buffer, new_size);
			buffer_size = new_size;
		}
		
		n = ::recv(sockfd, buffer + len, 1, flags);
		if(n <= 0) error(n);
		
		if(buffer[len] == '\n')
		{
			buffer[len] = '\0';
			break;
		}			
		 		
	}
	
	return buffer;
}

void Socket::close()
{
	#ifdef WIN32
		WSACleanup();
		closesocket(sockfd);		
	#else
		::close(sockfd);
	#endif
}

void Socket::shutdown(int type)
{
	int n;
	
	#ifdef WIN32
		WSACleanup();
	#endif

	n = ::shutdown(sockfd, type);	
	
	if(n != 0) error(n);
}

void Socket::error(int code)
{
	throw(code);
}

void Socket::clearBuffer()
{	
	if(buffer_size > 0)
	{
		free(buffer);
		buffer_size = 0;
	}
}

}

