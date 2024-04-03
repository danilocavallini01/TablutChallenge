#ifndef HEURISTIC
#define HEURISTIC

#pragma once

#include "Tablut.h"

#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>

#include <boost/sort/spreadsort/spreadsort.hpp>
#include <boost/bind/bind.hpp>
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


/*
    Negative values mean black advantage
    Otherwise white advantage

    weightsS
    0 : white pieces alive count weights
    1 : black pieces alive count weights
    2 : king distances from escape positions
    3 : kills white on this round weights
    4 : kills black on this round weights

*/
const Weights fixedWeights = {1, -2, 10, 1, -1};
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

    int evaluate(const Tablut &t);
    static int kingPos(const Tablut &t) ;
    bool compare(const Tablut &t1, const Tablut &t2);

    void sortMoves(std::vector<Tablut> &moves);
};

#endif