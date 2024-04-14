#include "Heuristic.h"

Heuristic::Heuristic(Weights __weights)
{
    _weights = __weights;
}
Heuristic::~Heuristic() {}

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
    6 : king no space Threshold weight
*/
int Heuristic::evaluate(const Tablut &__t)
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
        score = _weights[0] * __t._whiteCount + _weights[1] * __t._blackCount + _weights[2] * kingPosHeuristic[__t._kingX][__t._kingY] + __t._kills * (__t._isWhiteTurn ? _weights[3] : _weights[4]) + _weights[5] * (__t._kingMovements - _weights[6]);
    }

    return __t._isWhiteTurn ? score : -score;
}

// Compare function between two Tabluts
bool Heuristic::compare(const Tablut &__t1, const Tablut &__t2)
{
    return Heuristic::evaluate(__t1) < Heuristic::evaluate(__t2);
}

// Sorting moves algorithm
void Heuristic::sortMoves(std::vector<Tablut> &__moves)
{
    std::sort(__moves.begin(), __moves.end(), std::bind(&Heuristic::compare, std::ref(*this), std::placeholders::_1, std::placeholders::_2));
}