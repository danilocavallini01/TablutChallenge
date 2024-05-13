#ifndef PLAYER
#define PLAYER

#include <stdexcept>

#include "Connection.hpp"
#include "../Tablut.hpp"
#include "../Heuristic.hpp"
#include "../NegaScoutEngine.hpp"
#include "../Zobrist.hpp"
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

    const int _maxDepth = 7;
    const int _qDepth = 1;

    const Weights _whiteWeight = {256, -297, 46, -161, 257, 150, 14, 109, 25, 101, 47, -24, 61, 39, 54, 7, -26, 33, -32, -77, -53, 36, 16, -51, -18, -22, -24, 32, 31, 52, -14, 40, -63, -18, -18, -12, -12, 69, -22, 14};
    const Weights _blackWeight = {278, -183, 158, -195, 165, 59, 53, 52, 68, -63, 23, 51, 15, 76, -26, -49, 40, -49, 57, -79, 30, -12, 4, 29, 12, -42, -44, 24, 62, 71, 2, -41, -39, -39, -3, -79, -65, -78, 0, 33};

    const Heuristic _whiteH{_whiteWeight};
    const Heuristic _blackH{_blackWeight};

    const bool _verbose = true;

    class Player
    {
        Connection _socket;
        COLOR _color;

        NegaScoutEngine _engine;
        Zobrist _hasher;

        int _timeout;

    public:
        Player(Connection __socket, COLOR __color, int __timeout, Zobrist __hasher, NegaScoutEngine __engine) : _socket(__socket),
                                                                                                                _color(__color),
                                                                                                                _hasher(__hasher),
                                                                                                                _engine(__engine)
        {
            std::cout << std::endl
                      << "LOADED " << (__color == COLOR::WHITE ? "WHITE" : "BLACK") << " HEURISTIC " << std::endl;

            std::cout << "[";
            if (__color == COLOR::WHITE)
            {
                for (int weight : _whiteWeight)
                {
                    std::cout << weight << ",";
                }
            }
            else
            {
                for (int weight : _blackWeight)
                {
                    std::cout << weight << ",";
                }
            }
            std::cout << "]" << std::endl;
        };

        Player(Connection __socket, COLOR __color, int __timeout) : Player(__socket, __color, __timeout - 500, Zobrist(),
                                                                           NegaScoutEngine(Heuristic(_color == COLOR::WHITE ? _whiteH : _blackH), _hasher, _maxDepth, _qDepth))
        {
        }

        ~Player()
        {
            _socket.socketClose();
        };

        /*
            Construct a Player instance based on the ip and player color given as parameter
        */

        static Player of(std::string __ipAddress, std::string __color, std::string __timeout)
        {
            std::transform(__color.begin(), __color.end(), __color.begin(), ::toupper);

            if (__color == "WHITE")
            {
                return Player(Connection(name, __ipAddress, true), COLOR::WHITE, stoi(__timeout) * 1000);
            }

            if (__color == "BLACK")
            {
                return Player(Connection(name, __ipAddress, false), COLOR::BLACK, stoi(__timeout) * 1000);
            }

            throw std::runtime_error("Color parameter must be either WHITE or BLACK");
        }

        /*
            Start the game loop
            every loop cycle read the json string given from the server, parse the string into a Tablut
            and compute the move with the SearchEngine if the turn is the current player turn
        */
        void play()
        {
            std::cout << "WAITING FOR SERVER INPUT" << std::endl;
            bool gameContinue = true;

            Tablut board = Tablut::getStartingPosition();
            std::array<ZobristKey, MAX_DRAW_LOG> hashes = {};
            int hashesIndex = 0;
            int turn = 0;

            while (gameContinue)
            {
                // RECEIVE GAME STATE VIA JSON VALUE
                board = Tablut::fromJson(_socket.receiveString());
                updateGameBoard(board, hashes, hashesIndex++, turn++);

                if (_color == COLOR::WHITE)
                {
                    if (_verbose)
                    {
                        std::cout << "FROM MOVE" << std::endl;
                        board.print();
                    }

                    std::cout << "TURN: " << turn << std::endl;
                    sendMove(board);
                }

                board = Tablut::fromJson(_socket.receiveString());
                updateGameBoard(board, hashes, hashesIndex++, turn++);

                if (_color == COLOR::BLACK)
                {
                    if (_verbose)
                    {
                        std::cout << "FROM MOVE" << std::endl;
                        board.print();
                    }
                    sendMove(board);
                }
            }
        }

        void updateGameBoard(Tablut &__board, std::array<ZobristKey, MAX_DRAW_LOG> &__hashes, int __hashesIndex, int __turn)
        {
            // RESET HASHES INDEX IF EXCEED MAX_DRAW_LOG LENGTH
            __hashesIndex = __hashesIndex % MAX_DRAW_LOG;
            __hashes[__hashesIndex] = _hasher.hash(__board, false);
            __board._turn = __turn;
            __board._pastHashes = __hashes;
        }

        /*
            Compute the move with the search engine
            Send a single move by parsing mine implmentation of move so <fromx,fromy,tox,toy> all integers
            into a standard Tablut move ( es. e5 e3 )
        */
        void sendMove(Tablut &__board)
        {
            StopWatch timer = StopWatch(_timeout);

            // BEST MOVE SEARCH
            timer.start();
            Tablut bestMove = _engine.TimeLimitedSearch(__board, timer);

            if (_verbose)
            {
                std::cout << "TIME TAKEN: " << _timeout - timer.getRemainingTime() << std::endl;
                bestMove.print();
                _engine.print();
            }

            timer.reset();

            // MOVE PARSING AND SEND
            std::string parsedMove = Player::toStandardMove(bestMove.getMove());
            if (_verbose)
            {
                std::cout << parsedMove << std::endl;
            }
            _socket.send(parsedMove);
        }

        std::string toStandardMove(std::tuple<int, int, int, int> __move)
        {
            std::string from = "", to = "";

            from += char(std::get<1>(__move) + 97);
            from += char(std::get<0>(__move) + 49);

            to += char(std::get<3>(__move) + 97);
            to += char(std::get<2>(__move) + 49);

            json JSON = {
                {"from", from},
                {"to", to},
                {"turn", _color == WHITE ? "WHITE" : "BLACK"}};

            return JSON.dump();
        }

        void declareName()
        {
            _socket.declareName();
        }
    };
}

#endif