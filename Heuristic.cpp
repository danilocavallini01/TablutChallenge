#include "Heuristic.h"

#include "Tablut.h"
#include <vector>
#include <functional>

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
    3 : kills on this round weights
*/

int Heuristic::evaluate(Tablut &t)
{
    return weights[0] * t.blackCheckersCount + weights[1] * t.whiteCheckersCount + weights[2] * kingPos(t) + weights[3] * t.killFeedIndex * (t.isWhiteTurn ? 1 : -1);
}

int Heuristic::kingPos(Tablut &t)
{
    return 6 - kingPosHeuristic[t.kingX][t.kingY];
}

// Compare function between two Tabluts
int Heuristic::compare(Tablut &t1, Tablut &t2)
{
    return Heuristic::evaluate(t1) - Heuristic::evaluate(t2);
}

/*
    Sorting moves algorithm
*/

void Heuristic::sortMoves(std::vector<Tablut> &moves)
{
    std::sort(moves.begin(), moves.end(), std::bind(&Heuristic::compare, this, std::placeholders::_1, std::placeholders::_2));
}