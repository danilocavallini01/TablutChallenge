#include "Heuristic.h"

Heuristic::Heuristic()
{
    _weights = FIXED_WEIGHTS;
}
Heuristic::~Heuristic() {}

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
int Heuristic::evaluate(const Tablut &__t)
{
    int _score;

    if (__t.isGameOver())
    {
        if (__t._gameState == GAME_STATE::DRAW)
        {
            return 0;
        }
        _score = __t._gameState == GAME_STATE::WHITEWIN ? HEURISTIC::WIN_WEIGHT - __t._turn : -HEURISTIC::WIN_WEIGHT + __t._turn;
    }
    else
    {
        _score = _weights[0] * __t._whiteCount + _weights[1] * __t._blackCount + _weights[2] * kingPosHeuristic[__t._kingX][__t._kingY] + __t._kills * (__t._isWhiteTurn ? _weights[3] : _weights[4]) + _weights[5] * Heuristic::kingNoSpacePenality(__t);
    }

    return __t._isWhiteTurn ? _score : -_score;
}

int Heuristic::kingNoSpacePenality(const Tablut &__t)
{
    int kingSurrounded = 0;
    // Left Check
    if (__t._board[__t._kingX][__t._kingY - 1] != C::EMPTY)
    {
        kingSurrounded++;
    }

    // Right Check
    if (__t._board[__t._kingX][__t._kingY + 1] != C::EMPTY)
    {
        kingSurrounded++;
    }

    // Down Check
    if (__t._board[__t._kingX - 1][__t._kingY] != C::EMPTY)
    {
        kingSurrounded++;
    }

    // Up Check
    if (__t._board[__t._kingX + 1][__t._kingY] != C::EMPTY)
    {
        kingSurrounded++;
    }
    return kingSurrounded;
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