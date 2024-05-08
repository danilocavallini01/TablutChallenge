#ifndef PLAYER
#define PLAYER

#include <stdexcept>

#include "Connection.hpp"
#include "../Tablut.h"
#include "../Heuristic.hpp"

// Forward Declaration
class Connection;

namespace Connectors
{
    enum COLOR : int
    {
        WHITE = 0,
        BLACK = 1
    };

    const std::string name = "IMPOSTOR";

    class Player
    {
        Connection _socket;
        COLOR _color;

        const Heuristic _whiteH;
        const Heuristic _blackH;

    public:
        Player(Connection __socket, COLOR __color) : _socket(__socket),
                                                     _color(__color){};
        ~Player(){};

        static Player of(std::string __ipAddress, std::string __color)
        {
            std::transform(__color.begin(), __color.end(), __color.begin(), ::toupper);

            if (__color == "WHITE")
            {
                return Player(Connection{name, __ipAddress, true}, COLOR::WHITE);
            }

            if (__color == "BLACK")
            {
                return Player(Connection{name, __ipAddress, false}, COLOR::BLACK);
            }

            throw std::runtime_error("Color parameter must be either WHITE or BLACK");
        }

        void play()
        {
            bool gameContinue = true;
            Tablut board;

            while (gameContinue)
            {
                // RECEIVE GAME STATE VIA JSON VALUE
                board = Tablut::fromJson(_socket.receiveString());
            }
        }

        void declareName() {
            _socket.declareName();
        }
    };
}

#endif