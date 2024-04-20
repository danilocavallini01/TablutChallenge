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

/* -------------------------------
        MUST IMPLEMENT QUICK HEURISTIC WHEN TRAINING IS COMPLETED
   -------------------------------
*/

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
const int POSITION_WEIGHT_START_INDEX = 5;
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
            {_weights[5], _weights[6], _weights[6], _weights[00], _weights[00], _weights[00], _weights[6], _weights[6], _weights[5]},
            {_weights[6], _weights[7], _weights[7], _weights[8], _weights[00], _weights[8], _weights[7], _weights[7], _weights[6]},
            {_weights[6], _weights[7], _weights[8], _weights[9], _weights[10], _weights[9], _weights[8], _weights[7], _weights[6]},
            {_weights[00], _weights[8], _weights[9], _weights[10], _weights[11], _weights[10], _weights[9], _weights[8], _weights[00]},
            {_weights[00], _weights[00], _weights[10], _weights[11], _weights[12], _weights[11], _weights[10], _weights[00], _weights[00]},
            {_weights[00], _weights[8], _weights[9], _weights[10], _weights[11], _weights[10], _weights[9], _weights[8], _weights[00]},
            {_weights[6], _weights[7], _weights[8], _weights[9], _weights[10], _weights[9], _weights[8], _weights[7], _weights[6]},
            {_weights[6], _weights[7], _weights[7], _weights[8], _weights[00], _weights[8], _weights[7], _weights[7], _weights[6]},
            {_weights[5], _weights[6], _weights[6], _weights[00], _weights[00], _weights[00], _weights[6], _weights[6], _weights[5]},
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
    int evaluate(Tablut &__t)
    {
        int score;

        if (__t.isGameOver())
        {
            if (__t._gameState == GAME_STATE::WHITEDRAW || __t._gameState == GAME_STATE::BLACKDRAW)
            {
                score = 0;
            }
            else
            {
                score = __t._gameState == GAME_STATE::WHITEWIN ? HEURISTIC::WIN_WEIGHT - __t._turn : -HEURISTIC::WIN_WEIGHT + __t._turn;
            }
        }
        else
        {
            score = _weights[0] * __t._whiteCount + _weights[1] * __t._blackCount + _kingPosHeuristic[__t._kingX][__t._kingY] + __t._kills * (__t._isWhiteTurn ? _weights[2] : _weights[3]) + _weights[4] * (__t._kingMovements) + positionsWeightSum(__t);
        }

        return __t._isWhiteTurn ? score : -score;
    }

    int positionsWeightSum(Tablut &__t)
    {
        int score = 0;

        std::pair<Pos, Pos> position;
        Pos x, y;

        for (int i = 0; i < __t._checkerPositionIndex; i++)
        {
            position = __t._checkerPositions[i];
            x = position.first;
            y = position.second;

            if (__t._board[x][y] == C::WHITE)
            {
                score += _whitePosHeuristic[x][y];
            }
            else if (__t._board[x][y] == C::BLACK)
            {
                score += _blackPosHeuristic[x][y];
            }
        }
        return score;
    }

    // Compare function between two Tabluts
    bool compare(Tablut &__t1, Tablut &__t2)
    {
        return __t1._score < __t2._score;
    }

    // Sorting moves algorithm, first evaluate the score of every Tablut then it proceed to Sort them
    void sortMoves(std::vector<Tablut> &__moves)
    {
        for (Tablut &move : __moves)
        {
            move._score = evaluate(move);
        }

        std::sort(__moves.begin(), __moves.end(), std::bind(&Heuristic::compare, std::ref(*this), std::placeholders::_1, std::placeholders::_2));
    }
};

#endif