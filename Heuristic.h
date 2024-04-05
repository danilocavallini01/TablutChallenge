#ifndef HEURISTIC
#define HEURISTIC

#include "Tablut.h"

#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>

#include <boost/sort/spreadsort/spreadsort.hpp>
#include <boost/bind/bind.hpp>

// Forward Declaration
class Tablut;

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
typedef std::array<int, 6> Weights;

/*
    Negative values mean black advantage
    Otherwise white advantage

    weightsS
    0 : white pieces alive count weight
    1 : black pieces alive count weight
    2 : king distances from escape positions
    3 : kills white on this round weight
    4 : kills black on this round weight
    5 : king no space weight

*/
const Weights FIXED_WEIGHTS = {2, -1, 1, 2, -5, -15};
const int WIN_WEIGHT = 10000;

const std::array<std::array<int, 9>, 9> kingPosHeuristic = {{
    {50, 60, 60, 00, 00, 00, 60, 60, 50},
    {60, 50, 50, 40, 00, 40, 50, 50, 60},
    {60, 50, 40, 30, 20, 30, 40, 50, 60},
    {00, 40, 30, 20, 10, 20, 30, 40, 00},
    {00, 00, 20, 10, 0, 10, 20, 00, 00},
    {00, 40, 30, 20, 10, 20, 30, 40, 00},
    {60, 50, 40, 30, 20, 30, 40, 50, 60},
    {60, 50, 50, 40, 00, 40, 50, 50, 60},
    {50, 60, 60, 00, 00, 00, 60, 60, 50},
}};

class Heuristic
{
public:
    Weights _weights;

    Heuristic();
    ~Heuristic();

    int evaluate(const Tablut &__t);

    static int kingNoSpacePenality(const Tablut &__t);

    bool compare(const Tablut &__t1, const Tablut &__t2);

    void sortMoves(std::vector<Tablut> &__moves);
};

#endif