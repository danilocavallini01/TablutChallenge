#ifndef HEURISTIC
#define HEURISTIC

#pragma once

#include "Tablut.h"

#include <array>
#include <vector>
#include <algorithm>
#include <iostream>

// Matrix representing the distance ( in number of moves ) in every cell from the nearest camp
/*--0 1 2 3 4 5 6 7 8
 |
 0  - E E C C C E E -
 1  E - - - C - - - E
 2  E - - - - - - - E
 3  C - - - - - - - C
 4  C C - - T - - C C
 5  C - - - - - - - C
 6  E - - - - - - - E
 7  E - - - C - - - E
 8  - E E C C C E E -

*/

class Tablut;

typedef std::array<int, 5> Weights;

const Weights fixedWeights = {-1, 2, 2, 1};
const int winWeight = 10000;

const std::array<std::array<int16_t, 9>, 9> kingPosHeuristic = {{
    {1, 0, 0, 9, 9, 9, 0, 0, 1},
    {0, 1, 1, 2, 9, 2, 1, 1, 0},
    {0, 1, 2, 3, 4, 3, 2, 1, 0},
    {9, 2, 3, 4, 5, 4, 3, 2, 9},
    {9, 9, 4, 5, 6, 5, 4, 9, 9},
    {9, 2, 3, 4, 5, 4, 3, 2, 9},
    {0, 1, 2, 3, 4, 3, 2, 1, 0},
    {0, 1, 1, 2, 9, 2, 1, 1, 0},
    {1, 0, 0, 9, 9, 9, 0, 0, 1},
}};

class Heuristic
{
public:
    Weights weights;

    Heuristic();
    ~Heuristic();

    int evaluate(Tablut &t);
    static int kingPos(const Tablut &t);

    void sortMoves(std::vector<Tablut> &moves);
    static int evaluateS(const Tablut &t);
};

#endif