#include "Zobrist.h"

Zobrist::Zobrist()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::srand(std::time(nullptr));

    std::uniform_int_distribution<ZobristKey> distribution(1, 9223372036854775807);

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            _hashesTable[i][j] = distribution(gen);
        }
    }
}

Zobrist::~Zobrist() {}

// Give hash by calculating XOR of all  
ZobristKey Zobrist::hash(const Tablut &__t) const
{
    ZobristKey hashKey = 0;
    
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (__t._board[i][j] > 0)
            {
                hashKey ^= _hashesTable[i][j];
            }
        }
    }
    return hashKey;
}
