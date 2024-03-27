#include "Tablut.h"
#include "MoveGenerator.h"
#include "Heuristic.h"
#include "NegaScoutSearch.h"
#include <vector>

int main(int argc, char *argv[])
{
    Tablut t = Tablut::newGame();
    std::vector<Tablut> moves{};
    NegaScoutSearch searchEngine;
    Heuristic hh = Heuristic();
    int best_score;

    for (int i = 0; i < 200; i++)
    {
        searchEngine = NegaScoutSearch(hh);
        best_score = searchEngine.NegaScout(t, 4, BOTTOM_SCORE, TOP_SCORE);
        std::cout << "score = " << best_score << std::endl;

        MoveGenerator::generateLegalMoves(t, moves);

        for (int i = 0; i < moves.size(); i++)
        {
            std::cout << "score" << i << " = " << hh.evaluate(moves[i]) << std::endl;
            if (hh.evaluate(moves[i]) == best_score)
            {
                t = moves[i];
                break;
            }
        }

        t.print();

        if (t.isGameOver())
        {
            std::cout << "########################" << std::endl;
            std::cout << (t.isWinState() == WIN::BLACKWIN ? " BLACK WON " : " WHITE WON ") << std::endl;
            std::cout << "########################" << std::endl;
            break;
        }

        moves = {};
    }

    return EXIT_SUCCESS;
}