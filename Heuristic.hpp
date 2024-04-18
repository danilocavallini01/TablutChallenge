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
const int TOTAL_WEIGHTS = 38;
typedef std::array<int, TOTAL_WEIGHTS> Weights;
typedef std::array<std::array<int, DIM>, DIM> BoardWeights;

/*
    Negative values mean black advantage
    Otherwise white advantage

    weightsS
    0 : white pieces alive count weight
    1 : black pieces alive count weight
    2 : kills white on this round weight
    3 : kills black on this round weight
    4 : king no space weight
    5-12: king position weight
    13-23: white pieces position weight
    24-37: black pieces position weight

*/

const int WIN_WEIGHT = 100000;

class Heuristic
{
private:
    Weights _weights;
    BoardWeights _kingPosHeuristic;
    BoardWeights _whitePosHeuristic;
    BoardWeights _blackPosHeuristic;

public:
    Heuristic(Weights __weights)
    {
        _weights = __weights;
        setupPositionsWeights();
    }
    
    Heuristic()
    {
        for (int i = 0; i < TOTAL_WEIGHTS; i++)
        {
            _weights[i] = 0;
        }
        setupPositionsWeights();
    }

    ~Heuristic() {}

    void setupPositionsWeights()
    {
        _kingPosHeuristic = {{
            {_weights[5], _weights[6], _weights[6], _weights[0], _weights[0], _weights[0], _weights[6], _weights[6], _weights[5]},
            {_weights[6], _weights[7], _weights[7], _weights[8], _weights[0], _weights[8], _weights[7], _weights[7], _weights[6]},
            {_weights[6], _weights[7], _weights[8], _weights[9], _weights[10], _weights[9], _weights[8], _weights[7], _weights[6]},
            {_weights[0], _weights[8], _weights[9], _weights[10], _weights[11], _weights[10], _weights[9], _weights[8], _weights[0]},
            {_weights[0], _weights[0], _weights[10], _weights[11], _weights[12], _weights[11], _weights[10], _weights[0], _weights[0]},
            {_weights[0], _weights[8], _weights[9], _weights[10], _weights[11], _weights[10], _weights[9], _weights[8], _weights[0]},
            {_weights[6], _weights[7], _weights[8], _weights[9], _weights[10], _weights[9], _weights[8], _weights[7], _weights[6]},
            {_weights[6], _weights[7], _weights[7], _weights[8], _weights[0], _weights[8], _weights[7], _weights[7], _weights[6]},
            {_weights[5], _weights[6], _weights[6], _weights[0], _weights[0], _weights[0], _weights[6], _weights[6], _weights[5]},
        }};

        _whitePosHeuristic = {{
            {_weights[13], _weights[14], _weights[26], _weights[00], _weights[00], _weights[00], _weights[16], _weights[14], _weights[13]},
            {_weights[14], _weights[15], _weights[17], _weights[19], _weights[00], _weights[19], _weights[17], _weights[15], _weights[14]},
            {_weights[16], _weights[17], _weights[18], _weights[20], _weights[22], _weights[20], _weights[18], _weights[17], _weights[16]},
            {_weights[00], _weights[19], _weights[20], _weights[21], _weights[23], _weights[21], _weights[20], _weights[19], _weights[00]},
            {_weights[00], _weights[00], _weights[22], _weights[23], _weights[00], _weights[23], _weights[22], _weights[00], _weights[00]},
            {_weights[00], _weights[19], _weights[20], _weights[21], _weights[23], _weights[21], _weights[20], _weights[19], _weights[00]},
            {_weights[16], _weights[17], _weights[18], _weights[20], _weights[22], _weights[20], _weights[18], _weights[17], _weights[16]},
            {_weights[14], _weights[15], _weights[17], _weights[19], _weights[00], _weights[19], _weights[17], _weights[15], _weights[14]},
            {_weights[13], _weights[14], _weights[16], _weights[00], _weights[00], _weights[00], _weights[16], _weights[14], _weights[13]},
        }};

        _blackPosHeuristic = {{
            {_weights[24], _weights[25], _weights[26], _weights[35], _weights[36], _weights[35], _weights[26], _weights[25], _weights[24]},
            {_weights[25], _weights[27], _weights[28], _weights[30], _weights[37], _weights[30], _weights[28], _weights[27], _weights[25]},
            {_weights[26], _weights[28], _weights[29], _weights[31], _weights[33], _weights[31], _weights[29], _weights[28], _weights[26]},
            {_weights[35], _weights[30], _weights[31], _weights[32], _weights[34], _weights[32], _weights[31], _weights[30], _weights[35]},
            {_weights[36], _weights[37], _weights[33], _weights[34], _weights[00], _weights[34], _weights[33], _weights[37], _weights[36]},
            {_weights[35], _weights[30], _weights[31], _weights[32], _weights[34], _weights[32], _weights[31], _weights[30], _weights[35]},
            {_weights[26], _weights[28], _weights[29], _weights[31], _weights[33], _weights[31], _weights[29], _weights[28], _weights[26]},
            {_weights[25], _weights[27], _weights[28], _weights[30], _weights[37], _weights[30], _weights[28], _weights[27], _weights[25]},
            {_weights[24], _weights[25], _weights[26], _weights[35], _weights[36], _weights[35], _weights[26], _weights[25], _weights[24]},
        }};
    }

    // Evaluate a tablut with all weights defined in this classes
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
            score = _weights[0] * __t._whiteCount + _weights[1] * __t._blackCount + _kingPosHeuristic[__t._kingX][__t._kingY] + __t._kills * (__t._isWhiteTurn ? _weights[3] : _weights[4]) + _weights[5] * (__t._kingMovements) + positionsWeightSum(__t);
        }

        return __t._isWhiteTurn ? score : -score;
    }

    int positionsWeightSum(const Tablut &__t)
    {
        int score = 0;
        for (int i = 0; i < DIM; i++)
        {
            for (int j = 0; j < DIM; j++)
            {
                if (__t._board[i][j] == C::WHITE)
                {
                    score += _whitePosHeuristic[i][j];
                }
                else if (__t._board[i][j] == C::BLACK)
                {
                    score += _blackPosHeuristic[i][j];
                }
            }
        }
        return score;
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