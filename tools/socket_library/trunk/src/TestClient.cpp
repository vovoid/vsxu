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
#include <iostream>

/**
 * Simple client for testing
 */
int main(int argc, char** argv)
{		
	if(argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " <host> <port>" << std::endl;
		exit(1);
	}
	try
	{
		TD::Socket client;		
		client.connect(argv[1], atoi(argv[2]));
		
		std::cout << "Local ip: " << client.getHostName() << std::endl;
		std::cout << "Server ip: " << client.getPeerName() << std::endl;
		
		std::cout << "Server says: " << client.recvln() << std::endl;
		std::cout << "Server says: " << client.recvln() << std::endl;
		std::cout << "Server says: " << client.recvln() << std::endl;
		std::cout << "Server says: " << client.recvln() << std::endl;
		
		client.sendAll("I'm quitting!");
		client.close();
	}
	catch(int e)
	{
		std::cout << e << std::endl;
	}
	
	return 0;
}
