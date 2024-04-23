#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "Tablut.h"
#include "Interfaces/IZobrist.hpp"
#include <cstdint>
#include <random>
#include <ctime>

// Forward Declaration
class Tablut;

typedef uint64_t ZobristKey;

class Zobrist
{
private:
    // Table containing all random bitstring for all positions DIM x DIM and for all type of pieces: 3 total ( WHITE, BLACK, KING )
    ZobristKey _hashesTable[DIM][DIM][3];

    // Single bitstring containing the board turn
    ZobristKey _colorHash;

public:
    Zobrist()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::srand(std::time(nullptr));

        std::uniform_int_distribution<ZobristKey> distribution(1, std::numeric_limits<uint64_t>::max());

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                for (int x = 0; x < 3; x++)
                {
                    _hashesTable[i][j][x] = distribution(gen);
                }
            }
        }

        _colorHash = distribution(gen);
    }

    ~Zobrist() {}

    // Give hash by calculating XOR of all
    ZobristKey hash(const Tablut &__t) const
    {
        ZobristKey hashKey = 0;

        if (!__t._isWhiteTurn)
        {
            hashKey ^= _colorHash;
        }

        for (int i = 0; i < DIM; i++)
        {
            for (int j = 0; j < DIM; j++)
            {
                if (__t._board[i][j] != C::EMPTY)
                {
                    if (__t._board[i][j] == C::WHITE)
                    {
                        hashKey ^= _hashesTable[i][j][0];
                    }
                    else if (__t._board[i][j] == C::BLACK)
                    {
                        hashKey ^= _hashesTable[i][j][1];
                    }
                    else
                    {
                        hashKey ^= _hashesTable[i][j][2];
                    }
                }
            }
        }
        return hashKey;
    }
};

#endif
