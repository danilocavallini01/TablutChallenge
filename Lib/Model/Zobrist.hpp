#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "../Interfaces/IZobrist.hpp"
#include "Tablut.hpp"

#include <cstdint>
#include <random>
#include <ctime>

// Forward Declaration
class Tablut;

using namespace AI::Interface;

class Zobrist : public IZobrist<Tablut, ZobristKey>
{
private:
    // Table containing all random bitstring for all positions DIM x DIM and for all type of pieces: 3 total ( WHITE, BLACK, KING )
    ZobristKey _hashesTable[DIM][DIM][3];

    // Single bitstring containing the board turn
    ZobristKey _whiteColorHash;
    ZobristKey _blackColorHash;

public:
    Zobrist()
    {
        std::random_device rd{"/dev/urandom"};
        std::srand(std::time(nullptr));

        std::uniform_int_distribution<ZobristKey> distribution(1, std::numeric_limits<uint64_t>::max());

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                for (int x = 0; x < 3; x++)
                {
                    _hashesTable[i][j][x] = distribution(rd);
                }
            }
        }

        _whiteColorHash = distribution(rd);
        _blackColorHash = distribution(rd);
    }

    ~Zobrist() {}

    // Give hash by calculating XOR of all
    ZobristKey hash(const Tablut &__t, bool __colored = false) const override
    {
        ZobristKey hashKey = 0;

        std::pair<Pos, Pos> position;
        Pos x, y;

        if (__colored)
        {
            if (__t._isWhiteTurn)
            {
                hashKey ^= _whiteColorHash;
            }
            else
            {
                hashKey ^= _blackColorHash;
            }
        }

        for (int i = 0; i < __t._checkerPositionIndex; i++)
        {
            position = __t._checkerPositions[i];
            x = position.first;
            y = position.second;

            if (__t._board[x][y] == C::WHITE)
            {
                hashKey ^= _hashesTable[x][y][0];
            }
            else
            {
                hashKey ^= _hashesTable[x][y][1];
            }
        }

        if (__t._kingX != KDEADPOSITION)
        {
            hashKey ^= _hashesTable[__t._kingX][__t._kingY][2];
        }

        return hashKey;
    }

    /**
     * @brief Add hash computed by this Zobrist function and add the computed kyes to the GameState
        If the Search Algorithm also consider current turn into keys then compute the 2 different values
        Specified by the @var colored
     * 
     * @param __t the gamestate that need the hash
     */
    void addHash(Tablut &__t)
    {
        ZobristKey hash = Zobrist::hash(__t, false);

        __t._hash = hash;

        __t._pastHashes[__t._pastHashesIndex++] = __t._hash;
        if (__t._pastHashesIndex == MAX_DRAW_LOG)
        {
            __t._pastHashesIndex = 0;
        }
    }
};

#endif
