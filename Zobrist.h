#pragma once

#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "Tablut.h"
#include <cstdint>
#include <random>
#include <ctime>

// Forward Declaration
class Tablut;

typedef uint64_t ZobristKey;

class Zobrist
{
private:
    uint64_t _hashesTable[DIM][DIM];

public:
    Zobrist();
    ~Zobrist();

    uint64_t hash(const Tablut &__t) const;
};

#endif