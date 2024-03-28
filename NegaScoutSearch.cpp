#include <cstring>

#include "NegaScoutSearch.h"
#include "Heuristic.h"
#include "MoveGenerator.h"

NegaScoutSearch::NegaScoutSearch()
{
    heuristic = Heuristic();
    zobrist = Zobrist();
    history = TranspositionTable();
}

NegaScoutSearch::~NegaScoutSearch(){};

int NegaScoutSearch::NegaScout(Tablut t, int depth, int alpha, int beta)
{
    int score;
    int b;
    int v;
    std::vector<Tablut> moves;
    Tablut move;

    ZobristKey hash = zobrist.hash(t);
    // history.get(hash);

    if (depth == 0 || t.isGameOver())
    {
        return heuristic.evaluate(t);
    }

    score = BOTTOM_SCORE;
    b = beta;

    MoveGenerator::generateLegalMoves(t, moves);
    heuristic.sortMoves(moves);

    for (int i = 0; i < moves.size(); i++)
    {
        move = moves[i];
        v = -NegaScoutSearch::NegaScout(move, depth - 1, -b, -alpha);

        if (v > alpha && v < beta && i > 0)
        {
            v = -NegaScoutSearch::NegaScout(move, depth - 1, -beta, -v);
        }

        if (v > score)
        {
            score = v;
            bestmove[depth] = move;
        }

        alpha = std::max(alpha, v);

        if (alpha >= beta)
        {
            return alpha;
        }

        b = alpha + 1;
    }

    return score;
}