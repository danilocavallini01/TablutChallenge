#include "Heuristic.h"

Heuristic::Heuristic()
{
    weights = fixedWeights;
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
int Heuristic::evaluate(const Tablut &t)
{
    int score;

    if (t.isGameOver())
    {
        if (t.gameState == WIN::DRAW)
        {
            return 0;
        }
        score = t.gameState == WIN::WHITEWIN ? HEURISTIC::winWeight - t.turn : -HEURISTIC::winWeight + t.turn;
    }
    else
    {
        score = weights[0] * t.whiteCheckersCount + weights[1] * t.blackCheckersCount + weights[2] * (kingPosHeuristic[t.kingX][t.kingY]) + t.killFeedIndex * (t.isWhiteTurn ? weights[3] : weights[4]);
    }

    return t.isWhiteTurn ? score : -score;
}

// Compare function between two Tabluts
bool Heuristic::compare(const Tablut &t1, const Tablut &t2)
{
    return Heuristic::evaluate(t1) < Heuristic::evaluate(t2);
}

/*
    Sorting moves algorithm
*/

void Heuristic::sortMoves(std::vector<Tablut> &moves)
{
    std::sort(moves.begin(), moves.end(), std::bind(&Heuristic::compare, std::ref(*this), std::placeholders::_1, std::placeholders::_2));
}