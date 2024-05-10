#ifndef PLAYER
#define PLAYER

#include <stdexcept>

#include "Connection.hpp"
#include "../Tablut.h"
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

    const int _maxDepth = 6;
    const int _timeout = 60000;

    const Weights _whiteWeight = {280, -297, 73, -160, 259, 150, 17, 117, 62, 107, 28, -31, 95, 13, 54, 26, -8, 29, -32, -77, -55, 41, 6, -43, -16, -24, -24, 47, 27, 46, 4, 15, 80, -21, -7, -15, -15, 69};
    const Weights _blackWeight = {300, -300, 73, -104, 259, 132, 17, 117, 65, 69, 28, -13, 79, 13, 55, 16, 5, 29, -24, -77, -45, 57, 10, -51, -18, -24, -28, 38, 31, 62, -11, 33, -61, -36, 17, -15, -6, 69};

    const Heuristic _whiteH{_whiteWeight};
    const Heuristic _blackH{_blackWeight};

    class Player
    {
        Connection _socket;
        COLOR _color;

        NegaScoutEngine _engine;
        Zobrist _hasher;

    public:
        Player(Connection __socket, COLOR __color, Zobrist __hasher, NegaScoutEngine __engine) : _socket(__socket),
                                                                                                 _color(__color),
                                                                                                 _hasher(__hasher),
                                                                                                 _engine(__engine){};

        Player(Connection __socket, COLOR __color) : Player(__socket, __color, Zobrist(), NegaScoutEngine(Heuristic(_color == COLOR::WHITE ? _whiteH : _blackH), _hasher, _maxDepth))
        {
        }

        ~Player()
        {
            _socket.socketClose();
        };

        static Player of(std::string __ipAddress, std::string __color)
        {
            std::transform(__color.begin(), __color.end(), __color.begin(), ::toupper);

            if (__color == "WHITE")
            {
                return Player(Connection(name, __ipAddress, true), COLOR::WHITE);
            }

            if (__color == "BLACK")
            {
                return Player(Connection(name, __ipAddress, false), COLOR::BLACK);
            }

            throw std::runtime_error("Color parameter must be either WHITE or BLACK");
        }

        void play()
        {
            bool gameContinue = true;

            Tablut board = Tablut::getStartingPosition();
            std::array<ZobristKey, MAX_DRAW_LOG> hashes = {};
            int hashesIndex = 0;

            _hasher.addHash(board);
            hashes = board.getPastMoves();
            hashesIndex = board.getPastMovesIndex();

            int turn = 0;

            while (gameContinue)
            {
                // RECEIVE GAME STATE VIA JSON VALUE
                board = Tablut::fromJson(_socket.receiveString());
                _hasher.addHash(board);
                board._turn++;

                if (_color == COLOR::WHITE)
                {
                    std::cout << "FROM MOVE" << std::endl;
                    board.print();
                    sendMove(board);
                }

                board = Tablut::fromJson(_socket.receiveString());
                _hasher.addHash(board);
                board._turn++;

                if (_color == COLOR::BLACK)
                {
                    std::cout << "FROM MOVE" << std::endl;
                    board.print();
                    sendMove(board);
                }
            }
        }

        void sendMove(Tablut &board)
        {
            StopWatch timer = StopWatch(_timeout);

            // BEST MOVE SEARCH
            timer.start();
            Tablut bestMove = _engine.ParallelSearch(board);

            bestMove.print();
            _engine.print();

            // MOVE PARSING AND SEND
            std::string parsedMove = Player::toStandardMove(bestMove.getMove());
            std::cout << parsedMove << std::endl;
            _socket.send(parsedMove);
        }

        std::string toStandardMove(std::tuple<int, int, int, int> _move)
        {
            std::string from = "", to = "";

            from += char(std::get<1>(_move) + 97);
            from += char(std::get<0>(_move) + 49);

            to += char(std::get<3>(_move) + 97);
            to += char(std::get<2>(_move) + 49);

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