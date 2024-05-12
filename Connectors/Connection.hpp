#ifndef CONNECTOR
#define CONNECTOR

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

namespace Connectors
{
    enum PORT
    {
        WHITE_PORT = 5800,
        BLACK_PORT = 5801
    };

    class Connection
    {
    private:
        int _sd;
        std::string _ipAddress;

        std::string _name;
        bool _isWhite;

        bool _readAll(char *buffer, unsigned int x)
        {
            int bytesRead = 0;
            int result;
            while (bytesRead < x)
            {
                result = read(_sd, buffer + bytesRead, x - bytesRead);
                if (result < 0)
                {
                    return false;
                }
                bytesRead += result;
            }
            return true;
        }

    public:
        Connection(std::string __name, std::string __ipAddress = "127.0.0.1", bool __isWhite = true) : _name(__name),
                                                                                                       _ipAddress(__ipAddress),
                                                                                                       _isWhite(__isWhite)
        {
            struct hostent *host;
            struct sockaddr_in serverAddress;

            // SOCKET SETUP
            memset((char *)&serverAddress, 0, sizeof(struct sockaddr_in));
            serverAddress.sin_family = AF_INET;
            host = gethostbyname(__ipAddress.c_str());

            serverAddress.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
            serverAddress.sin_port = htons(__isWhite ? PORT::WHITE_PORT : PORT::BLACK_PORT);

            // END SOCKET SETUP

            // INIZIALIZING CLIENT SOCKET
            _sd = socket(AF_INET, SOCK_STREAM, 0);

            if (_sd < 0)
            {
                perror("SOCKET INITIALIZE ERROR (BAD DESCRIPTOR)");
                exit(1);
            }

            // Effettuo la connect
            if (connect(_sd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr)) < 0)
            {
                perror("CONNECT ERROR");
                exit(1);
            }

            declareName();
        };

        ~Connection(){};

        void socketClose()
        {
            close(_sd);
        }

        // DECLARE PLAYER NAME TO SERVER
        void declareName()
        {
            std::string msg = {'"' + _name + '"'};
            send(msg);
            std::cout << "CONNECTED TO SERVER AS " << _name << std::endl;
        }

        /*
            SEND MESSAGE TO SERVER
            First compute length of message and send it to the server
            than send the whole message encoded in UTF-8 standard
        */
        void send(std::string &__msg)
        {
            int n;

            // MESSAGE LENGTH
            int msgLength{static_cast<int>(htonl(static_cast<uint32_t>(__msg.length())))};

            // SEND MESSAGE LENGTH
            n = write(_sd, &msgLength, sizeof(msgLength));
            if (n < 0)
            {
                perror("ERROR WHILE WRITING SIZE OF MESSAGE TO SOCKET");
            }

            // SEND MESSAGE
            n = write(_sd, __msg.c_str(), sizeof(char) * __msg.length());
            if (n < 0)
            {
                perror("ERROR WHILE WRITING MESSAGE TO SOCKET");
            }
        }

        /*
            RECEIVE MESSAGE FROM SERVER
            First receive length of message than receive the whole message
        */
        std::string receiveString()
        {
            int strlen;

            // READ MESSAGE LENGTH
            if (!_readAll(reinterpret_cast<char *>(&strlen), sizeof(strlen)))
            {
                perror("ERROR WHILE READING LENGTH OF MESSAGE");
                exit(1);
            }
            strlen = ntohl(strlen);

            std::vector<char> buffer;
            buffer.reserve(strlen);

            // READ MESSAGE
            if (!_readAll(buffer.data(), sizeof(char) * strlen))
            {
                perror("ERROR WHILE READING MESSAGE");
                exit(1);
            }

            std::string output(buffer.data());

            return std::move(output);
        }
    };
}

#endif