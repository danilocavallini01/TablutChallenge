#ifndef HEURISTIC
#define HEURISTIC

#include "Tablut.h"

#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>

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
const int TOTAL_WEIGHTS = 14;
typedef std::array<int, TOTAL_WEIGHTS> Weights;

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
    6-13: king position weight

*/
//                             0  1   2   3   4    5  6  7  8  9 10 11 12 13
const Weights FIXED_WEIGHTS = {5, -2, 10, 10, -20, 5, 5, 5, 5, 5, 5, 5, 5, 5};
const int WIN_WEIGHT = std::numeric_limits<int>::max()/2;
const int DRAW_WEIGHT = -2000;

class Heuristic
{
private:
    Weights _weights;
    std::array<std::array<int, 9>, 9> _kingPosHeuristic;

public:
    Heuristic(Weights __weights = FIXED_WEIGHTS)
    {
        _weights = __weights;
        std::array<std::array<int, 9>, 9> _kingPosHeuristic = {{
            {_weights[6], _weights[7], _weights[7], _weights[0], _weights[0], _weights[0], _weights[7], _weights[7], _weights[6]},
            {_weights[7], _weights[8], _weights[8], _weights[9], _weights[0], _weights[9], _weights[8], _weights[8], _weights[7]},
            {_weights[7], _weights[8], _weights[9], _weights[10], _weights[11], _weights[10], _weights[9], _weights[8], _weights[7]},
            {_weights[0], _weights[9], _weights[10], _weights[11], _weights[12], _weights[11], _weights[10], _weights[9], _weights[0]},
            {_weights[0], _weights[0], _weights[11], _weights[12], _weights[13], _weights[12], _weights[11], _weights[0], _weights[0]},
            {_weights[0], _weights[9], _weights[10], _weights[11], _weights[12], _weights[11], _weights[10], _weights[9], _weights[0]},
            {_weights[7], _weights[8], _weights[9], _weights[10], _weights[11], _weights[10], _weights[9], _weights[8], _weights[7]},
            {_weights[7], _weights[8], _weights[8], _weights[9], _weights[0], _weights[9], _weights[8], _weights[8], _weights[7]},
            {_weights[6], _weights[7], _weights[7], _weights[0], _weights[0], _weights[0], _weights[7], _weights[7], _weights[6]},
        }};
    }
    ~Heuristic() {}

    int evaluate(const Tablut &__t)
    {
        int score;

        if (__t.isGameOver())
        {
            if (__t._gameState == GAME_STATE::WHITEDRAW || __t._gameState == GAME_STATE::BLACKDRAW)
            {
                return 0;
            }
            else
            {
                score = __t._gameState == GAME_STATE::WHITEWIN ? HEURISTIC::WIN_WEIGHT - __t._turn : -HEURISTIC::WIN_WEIGHT + __t._turn;
            }
        }
        else
        {
            score = _weights[0] * __t._whiteCount + _weights[1] * __t._blackCount + _weights[2] * _kingPosHeuristic[__t._kingX][__t._kingY] + __t._kills * (__t._isWhiteTurn ? _weights[3] : _weights[4]) + _weights[5] * (__t._kingMovements);
        }

        return __t._isWhiteTurn ? score : -score;
    }

    // Compare function between two Tabluts
    bool compare(const Tablut &__t1, const Tablut &__t2)
    {
        return evaluate(__t1) < evaluate(__t2);
    }

    // Sorting moves algorithm
    void sortMoves(std::vector<Tablut> &__moves)
    {
        std::sort(__moves.begin(), __moves.end(), std::bind(&Heuristic::compare, std::ref(*this), std::placeholders::_1, std::placeholders::_2));
    }
};

#endif