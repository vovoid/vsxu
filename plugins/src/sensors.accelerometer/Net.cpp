/*
        Sending and Receiving Packets Example
        From "Networking for Game Programmers" - http://www.gaffer.org/networking-for-game-programmers
        Author: Glenn Fiedler <gaffer@gaffer.org>
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include "Net.h"

using namespace std;
using namespace Net;

int main( int argc, char * argv[] )
{
        // initialize socket layer

        if ( !InitializeSockets() )
        {
                printf( "failed to initialize sockets\n" );
                return 1;
        }

        // create socket

        int port = 30000;

        if ( argc == 2 )
                port = atoi( argv[1] );

        printf( "creating socket on port %d\n", port );

        Socket socket;
        if ( !socket.Open( port ) )
        {
                printf( "failed to create socket!\n" );
                return 1;
        }

        // read in addresses.txt to get the set of addresses we will send packets to

        vector<Address> addresses;

        string line;
        ifstream file( "addresses.txt" );
        if ( !file.is_open() )
        {
                printf( "failed to open 'addresses.txt'\n" );
                return 1;
        }

        while ( !file.eof() )
        {
                getline( file, line );
                int a,b,c,d,port;
                if ( sscanf( line.c_str(), "%d.%d.%d.%d:%d", &a, &b, &c, &d, &port ) == 5 )
                        addresses.push_back( Address( a,b,c,d,port ) );
        }

        file.close();

        // send and receive packets until the user ctrl-breaks...

        while ( true )
        {
                const char data[] = "hello world!";
                for ( int i = 0; i < (int) addresses.size(); ++i )
                        socket.Send( addresses[i], data, sizeof( data ) );

                while ( true )
                {
                        Address sender;
                        unsigned char buffer[256];
                        int bytes_read = socket.Receive( sender, buffer, sizeof( buffer ) );
                        if ( !bytes_read )
                                break;

                        printf( "received packet from %d.%d.%d.%d:%d (%d bytes)\n", sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(), sender.GetPort(), bytes_read );
                }

               // wait( 1.0f );
        }

        // shutdown socket layer

        ShutdownSockets();

        return 0;
}

