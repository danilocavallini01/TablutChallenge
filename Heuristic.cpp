#include "Heuristic.h"

#include "Tablut.h"
#include <vector>

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

int Heuristic::kingPos(const Tablut &t)
{
    return 6 - kingPosHeuristic[t.kingX][t.kingY];
}

/*
    Sorting moves algorithm
*/

void Heuristic::sortMoves(std::vector<Tablut> &moves)
{
    std::sort(moves.begin(), moves.end(), [](const Tablut &l, const Tablut &r)
              { return Heuristic::evaluateS(l) < Heuristic::evaluateS(r); });
}

int Heuristic::evaluateS(const Tablut &t)
{
    return fixedWeights[0] * t.blackCheckersCount + fixedWeights[1] * t.whiteCheckersCount + fixedWeights[2] * kingPos(t) + fixedWeights[3] * t.killFeedIndex * (t.isWhiteTurn ? 1 : -1);
}