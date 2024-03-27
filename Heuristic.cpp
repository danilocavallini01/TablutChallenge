#include "Heuristic.h"

Heuristic::Heuristic()
{
    weights = {};
    weights[0] = 2;
    weights[1] = 1;
    weights[2] = 10;
    weights[3] = 2;
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

/*
    Sorting moves algorithm
*/

void Heuristic::sortMoves(std::vector<Tablut> &moves)
{
    std::sort(moves.begin(), moves.end(), Heuristic::compare);
}

int Heuristic::evaluateS(Tablut & t)
{
    return 2 * t.blackCheckersCount + 1 * t.whiteCheckersCount + 10 * 6 - kingPosHeuristic[t.kingX][t.kingY] + 2 * t.killFeedIndex * (t.isWhiteTurn ? 1 : -1);
}

bool Heuristic::compare(Tablut & t1, Tablut & t2)
{
    int score1;
    int score2;

    return evaluateS(t1) - evaluateS(t2);
}