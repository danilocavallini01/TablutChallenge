#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <cstdint>
#include "Tablut.h"

typedef uint64_t ZobristKey;

class Zobrist
{
public:
    uint64_t table[DIM][DIM];

    Zobrist();
    ~Zobrist();

    uint64_t hash(const Tablut &t) const;
};


#endif