#include "NegaScoutSearch.h"
#include "Heuristic.h"
#include "MoveGenerator.h"

#include <cstring>

NegaScoutSearch::NegaScoutSearch() {
    heuristic = Heuristic();
}
NegaScoutSearch::~NegaScoutSearch() {};

int NegaScoutSearch::NegaScout(Tablut t, int depth, int alpha, int beta)
{
    int score;
    int n;
    int cur;
    std::vector<Tablut> moves;
    Tablut move;

    if (depth == 0 || t.isGameOver())
    {
        return heuristic.evaluate(t);
    }

    score = BOTTOM_SCORE;
    n = beta;

    MoveGenerator::generateLegalMoves(t, moves);
    //heuristic.sortMoves(moves);

    for (int i = 0; i < moves.size(); i++)
    {
        move = moves[i];
        cur = -NegaScoutSearch::NegaScout(move, depth - 1, -n, -alpha);

        if (cur > score)
        {
            if (n == beta || depth <= 2)
            {
                score = cur;
            }
            else
            {
                score = -NegaScoutSearch::NegaScout(move, depth - 1, -beta, -cur);
            }
        }

        if (score > alpha)
        {
            alpha = score;
        }

        if (alpha >= beta)
        {
            return alpha;
        }

        n = alpha + 1;
    }

    return score;
}