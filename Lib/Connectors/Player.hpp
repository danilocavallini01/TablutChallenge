#ifndef PLAYER
#define PLAYER

#include <stdexcept>

#include "Connection.hpp"

#include "Lib/Tablut/TablutNegaScoutEngine.hpp"

// Forward Declaration
class Connection;

using namespace AI::Engine;

namespace Connectors
{
    enum COLOR : int
    {
        WHITE = 0,
        BLACK = 1
    };

    const std::string name = "IMPOSTOR";

    const int _maxWDepth = 6;
    const int _qWDepth = 1;

    const int _maxBDepth = 6;
    const int _qBDepth = 1;

    const Weights _whiteWeight = {247, -297, 114, -160, 260, 116, 21, 115, 64, 102, 67, -15, 116, 26, 44, 7, -6, 27, -27, -84, -39, 38, 18, -44, -23, -17, -24, 34, 62, 48, -29, 53, -69, -19, -14, -14, -27, 63, -15, 5};
    const Weights _blackWeight = {299, -160, 191, -213, 260, 61, 57, 38, 53, -82, 38, 27, 12, 71, -31, -46, 19, -52, 57, -79, 26, -2, 1, 43, 22, -54, -38, 24, 69, 85, 7, -41, -24, -18, 18, -68, -83, -81, -1, 25};

    const Heuristic _whiteH{_whiteWeight};
    const Heuristic _blackH{_blackWeight};

    const bool _verbose = true;

    class Player
    {
        Connection _socket;
        COLOR _color;

        TNegaScoutEngine _engine;
        Zobrist _hasher;

        int _timeout;

    public:
        Player(Connection __socket, COLOR __color, int __timeout, Zobrist __hasher) : _socket(__socket),
                                                                                      _color(__color),
                                                                                      _hasher(__hasher),
                                                                                      _timeout(__timeout),
                                                                                      _engine(_color == COLOR::WHITE ? TNegaScoutEngine(_maxWDepth, _qWDepth, Heuristic(_whiteH), _hasher) : TNegaScoutEngine(_maxBDepth, _qBDepth, Heuristic(_blackH), _hasher))
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

        Player(Connection __socket, COLOR __color, int __timeout) : Player(__socket, __color, __timeout - 500, Zobrist()) {}

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

                    std::cout << "TURN: " << turn << std::endl;
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

            _engine = {_color == COLOR::WHITE ? TNegaScoutEngine(_maxWDepth, _qWDepth, Heuristic(_whiteH), _hasher)
                                              : TNegaScoutEngine(_maxBDepth, _qBDepth, Heuristic(_blackH), _hasher)};

            // BEST MOVE SEARCH
            timer.start();
            Tablut bestMove = _engine.TimeLimitedSlicedSearch(__board, timer);

            std::cout << "------------------------" << std::endl;
            std::cout << "TIME TAKEN: " << _timeout - timer.getRemainingTime() << std::endl;
            std::cout << "------------------------" << std::endl;
            if (_verbose)
            {
                bestMove.print();
                _engine.print();
            }

            timer.reset();

            // MOVE PARSING AND SEND
            std::string parsedMove = Tablut::toStandardMove(bestMove.getMove(), _color == COLOR::WHITE);
            std::cout << parsedMove << std::endl;
            _socket.send(parsedMove);
        }

        void declareName()
        {
            _socket.declareName();
        }
    };
}

#endif
