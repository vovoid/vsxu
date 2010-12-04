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
 * Simple server for testing
 */
int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
		exit(1);
	}
	
	try
	{
		TD::Socket server;		
		server.bind(atoi(argv[1]));
		server.listen();		
		TD::Socket client = server.accept();		
		
		client.sendln("Hi, this is the server!");				
		client.sendln("Hi again!");
		client.sendln("Are you there?");	
		client.sendln("I will ignore you too...");				
		
		const char* res = client.recv();
		std::cout << res << std::endl;		
		server.close();
	}
	catch(int e)
	{
		std::cout << e << std::endl;
	}

	return 0;
}
