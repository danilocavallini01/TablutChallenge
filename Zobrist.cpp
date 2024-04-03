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
            table[i][j] = distribution(gen);  
        }
    }
}

Zobrist::~Zobrist() {}

ZobristKey Zobrist::hash(const Tablut &t) const
{
    ZobristKey h = 0;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (t.board[i][j] > 0) {
                h ^= table[i][j];
            }
        }
    }
    return h;
}

